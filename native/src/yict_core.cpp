#include "yict_core.hpp"

#include "legacy/yictdata202.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace yict {
namespace fs = std::filesystem;
using legacy::addr_t;

namespace {

enum COND_TYPE { CT_PROBACTION, CT_ACTION };

struct CONDITION {
  COND_TYPE ct;
  int index;
  const char* szName;
  const char* szNewCategory;
};

#define SKIT_VECTOR(iProbAction) CT_PROBACTION, iProbAction
#define OTHER_VECTOR(iExtra) CT_ACTION, iExtra
#include "../../source/conditions.c_"
#undef OTHER_VECTOR
#undef SKIT_VECTOR

constexpr std::size_t kNumCond = sizeof(g_conditions) / sizeof(g_conditions[0]);

static std::string trim(std::string s) {
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
    s.pop_back();
  }
  std::size_t i = 0;
  while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
    ++i;
  }
  return s.substr(i);
}

static std::string c_field_to_string(const char* p, std::size_t n) {
  std::string s(p, p + n);
  const auto nul = s.find('\0');
  if (nul != std::string::npos) {
    s.resize(nul);
  }
  while (!s.empty() && s.back() == ' ') {
    s.pop_back();
  }
  return s;
}

static void string_to_c_field(const std::string& in, char* out, std::size_t n) {
  std::fill(out, out + n, 0);
  const auto copy_n = std::min(in.size(), n > 0 ? n - 1 : 0);
  std::copy(in.begin(), in.begin() + static_cast<std::ptrdiff_t>(copy_n), out);
}

static bool file_size_exact(const fs::path& p, std::uintmax_t expected) {
  std::error_code ec;
  const auto s = fs::file_size(p, ec);
  return !ec && s == expected;
}

static bool read_exact(const fs::path& p, std::vector<std::uint8_t>& out, std::size_t expected) {
  std::ifstream f(p, std::ios::binary);
  if (!f) {
    return false;
  }
  out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
  return out.size() == expected;
}

static bool write_exact(const fs::path& p, const std::vector<std::uint8_t>& data) {
  std::ofstream f(p, std::ios::binary | std::ios::trunc);
  if (!f) {
    return false;
  }
  f.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
  return static_cast<std::size_t>(f.tellp()) == data.size();
}

static std::uint32_t read_u32_le(const std::vector<std::uint8_t>& image, std::size_t offset) {
  return static_cast<std::uint32_t>(image[offset]) |
         (static_cast<std::uint32_t>(image[offset + 1]) << 8U) |
         (static_cast<std::uint32_t>(image[offset + 2]) << 16U) |
         (static_cast<std::uint32_t>(image[offset + 3]) << 24U);
}

static void write_u32_le(std::vector<std::uint8_t>& image, std::size_t offset, std::uint32_t v) {
  image[offset] = static_cast<std::uint8_t>(v & 0xFFU);
  image[offset + 1] = static_cast<std::uint8_t>((v >> 8U) & 0xFFU);
  image[offset + 2] = static_cast<std::uint8_t>((v >> 16U) & 0xFFU);
  image[offset + 3] = static_cast<std::uint8_t>((v >> 24U) & 0xFFU);
}

static std::size_t addr_to_offset(addr_t a) {
  if (a < legacy::CART_BASE || a >= legacy::CART_BASE + legacy::CART_SIZE) {
    return static_cast<std::size_t>(-1);
  }
  return static_cast<std::size_t>(a - legacy::CART_BASE);
}

struct Cartridge {
  std::vector<std::uint8_t> image;
  legacy::YICT_DATA* yd = nullptr;
  std::vector<addr_t> storage_to_addr;
  std::map<addr_t, std::uint16_t> addr_to_storage;
  std::map<int, std::vector<std::uint16_t>> probact_storage;  // key: cond index

  bool load(const std::string& base, std::string* err) {
    const fs::path low = base + "-l.bin";
    const fs::path high = base + "-h.bin";

    if (!fs::exists(low) || !fs::exists(high)) {
      if (err) *err = "Missing one or both ROM halves";
      return false;
    }
    if (!file_size_exact(low, legacy::ROM_HALF_SIZE) || !file_size_exact(high, legacy::ROM_HALF_SIZE)) {
      if (err) *err = "Unexpected ROM half size";
      return false;
    }

    std::vector<std::uint8_t> l;
    std::vector<std::uint8_t> h;
    if (!read_exact(low, l, legacy::ROM_HALF_SIZE) || !read_exact(high, h, legacy::ROM_HALF_SIZE)) {
      if (err) *err = "Failed to read ROM halves";
      return false;
    }

    image.assign(legacy::CART_SIZE, 0);
    for (std::size_t i = 0; i < legacy::ROM_HALF_SIZE; ++i) {
      image[i * 2] = l[i];
      image[i * 2 + 1] = h[i];
    }

    if (legacy::YICT_CUSTOMIZATION_OFFSET + sizeof(legacy::YICT_DATA) > image.size()) {
      if (err) *err = "YICT metadata offset out of bounds";
      return false;
    }

    yd = reinterpret_cast<legacy::YICT_DATA*>(image.data() + legacy::YICT_CUSTOMIZATION_OFFSET);
    if (std::memcmp(yd->yict_id, legacy::YICT_ID, legacy::YICTTAG_SIZE) != 0) {
      if (err) *err = "Missing YICT202 tag";
      return false;
    }

    if (std::search(image.begin() + legacy::YICT_CUSTOMIZATION_OFFSET,
                    image.begin() + legacy::YICT_CUSTOMIZATION_OFFSET + 512,
                    legacy::YICTEND_ID,
                    legacy::YICTEND_ID + legacy::YICTTAG_SIZE) ==
        image.begin() + legacy::YICT_CUSTOMIZATION_OFFSET + 512) {
      if (err) *err = "Missing YICTEND tag";
      return false;
    }

    if (!build_action_storage_map(err)) {
      return false;
    }
    if (!extract_probactions(err)) {
      return false;
    }
    return true;
  }

  bool save(const std::string& base, std::string* err) {
    if (!yd) {
      if (err) *err = "Image not loaded";
      return false;
    }
    if (!write_probactions(err)) {
      return false;
    }

    std::vector<std::uint8_t> l(legacy::ROM_HALF_SIZE);
    std::vector<std::uint8_t> h(legacy::ROM_HALF_SIZE);
    for (std::size_t i = 0; i < legacy::ROM_HALF_SIZE; ++i) {
      l[i] = image[i * 2];
      h[i] = image[i * 2 + 1];
    }

    const fs::path low = base + "-l.bin";
    const fs::path high = base + "-h.bin";
    if (!write_exact(low, l) || !write_exact(high, h)) {
      if (err) *err = "Failed writing output ROM halves";
      return false;
    }
    return true;
  }

  bool build_action_storage_map(std::string* err) {
    storage_to_addr.clear();
    addr_to_storage.clear();

    for (int chunk = 0; chunk < legacy::NUM_ACTION_CHUNK; ++chunk) {
      addr_t addr = yd->availableActions[chunk].start;
      int count = yd->availableActions[chunk].count;
      for (int i = 0; i < count; ++i) {
        const auto off = addr_to_offset(addr);
        if (off == static_cast<std::size_t>(-1) || off + 2 > image.size()) {
          if (err) {
            *err = "Action chunk address out of bounds (chunk=" + std::to_string(chunk) +
                   ", idx=" + std::to_string(i) + ", addr=0x" + [&]() {
                     std::ostringstream os;
                     os << std::hex << addr;
                     return os.str();
                   }() + ")";
          }
          return false;
        }

        storage_to_addr.push_back(addr);
        addr_to_storage[addr] = static_cast<std::uint16_t>(storage_to_addr.size() - 1);

        const std::uint8_t n_chunks = image[off];
        addr += static_cast<addr_t>(1 + 1 + n_chunks * 6);
      }
    }
    return true;
  }

  bool extract_probactions(std::string* err) {
    probact_storage.clear();

    for (std::size_t i = 0; i < kNumCond; ++i) {
      const CONDITION& c = g_conditions[i];
      if (c.ct != CT_PROBACTION) {
        continue;
      }

      const addr_t table_ptr_addr = yd->addrProbActionTable + static_cast<addr_t>(c.index * 4);
      const auto table_ptr_off = addr_to_offset(table_ptr_addr);
      if (table_ptr_off == static_cast<std::size_t>(-1) || table_ptr_off + 4 > image.size()) {
        if (err) *err = "ProbAction table pointer out of range";
        return false;
      }

      const addr_t list_addr = read_u32_le(image, table_ptr_off);
      if (list_addr == 0) {
        probact_storage[c.index] = {};
        continue;
      }

      auto off = addr_to_offset(list_addr);
      if (off == static_cast<std::size_t>(-1)) {
        if (err) *err = "ProbAction list address out of range";
        return false;
      }

      int last_percent = 0;
      std::vector<std::uint16_t> storages;
      while (true) {
        if (off + 5 > image.size()) {
          if (err) *err = "ProbAction list parse overflow";
          return false;
        }
        const int percent = image[off];
        if (percent < 1 || percent > 100 || percent <= last_percent) {
          if (err) *err = "Invalid ProbAction percent progression";
          return false;
        }
        const addr_t action_addr = read_u32_le(image, off + 1);
        const auto it = addr_to_storage.find(action_addr);
        if (it == addr_to_storage.end()) {
          if (err) *err = "ProbAction references unknown action address";
          return false;
        }
        storages.push_back(it->second);
        last_percent = percent;
        off += 5;
        if (percent == 100) {
          break;
        }
      }

      probact_storage[c.index] = std::move(storages);
    }
    return true;
  }

  bool write_probactions(std::string* err) {
    // Legacy behavior: serialize all PROBACTION blobs contiguously in pool #3,
    // after current audio allocation.
    const int iPool = legacy::NUM_AUDIO_POOLS - 1;
    addr_t base = yd->pools[iPool].start + yd->audio_sizes[iPool];
    std::uint32_t available = static_cast<std::uint32_t>(yd->pools[iPool].max_size - yd->audio_sizes[iPool]);

    const auto base_off = addr_to_offset(base);
    if (base_off == static_cast<std::size_t>(-1) || base_off + available > image.size()) {
      if (err) *err = "Pool3 allocation region out of range";
      return false;
    }
    std::fill(image.begin() + static_cast<std::ptrdiff_t>(base_off),
              image.begin() + static_cast<std::ptrdiff_t>(base_off + available), 0xFF);

    std::uint32_t used = 0;
    for (std::size_t i = 0; i < kNumCond; ++i) {
      const CONDITION& c = g_conditions[i];
      if (c.ct != CT_PROBACTION) {
        continue;
      }

      const auto vals_it = probact_storage.find(c.index);
      const std::vector<std::uint16_t> vals = (vals_it != probact_storage.end()) ? vals_it->second : std::vector<std::uint16_t>{};

      addr_t out_addr = 0;
      if (!vals.empty()) {
        const std::uint32_t needed = static_cast<std::uint32_t>(vals.size() * 5);
        if (used + needed > available) {
          if (err) *err = "Out of cartridge memory for PROBACTION data";
          return false;
        }

        out_addr = base + used;
        auto out_off = addr_to_offset(out_addr);
        for (std::size_t j = 0; j < vals.size(); ++j) {
          const int pct = static_cast<int>(((j + 1) * 100) / vals.size());
          image[out_off] = static_cast<std::uint8_t>(pct);
          const std::uint16_t storage = vals[j];
          if (storage >= storage_to_addr.size()) {
            if (err) *err = "Invalid storage number while writing PROBACTION";
            return false;
          }
          write_u32_le(image, out_off + 1, storage_to_addr[storage]);
          out_off += 5;
        }

        used += needed;
      }

      const addr_t table_ptr_addr = yd->addrProbActionTable + static_cast<addr_t>(c.index * 4);
      const auto table_ptr_off = addr_to_offset(table_ptr_addr);
      write_u32_le(image, table_ptr_off, out_addr);
    }

    yd->pool3_yictsize = static_cast<std::uint16_t>(used);
    return true;
  }

  bool export_icb_file(const std::string& icb_file, std::string* err) const {
    std::ofstream out(icb_file, std::ios::out | std::ios::trunc);
    if (!out) {
      if (err) *err = "Failed to open ICB output";
      return false;
    }

    out << "YICT_VERSION=202\n";
    out << "; General\n";
    out << "STRETCH=" << static_cast<int>(yd->bStretchCount) << "\n";
    out << "WAGTAIL=" << static_cast<int>(yd->bWagTime) << "\n";

    out << "MOODDATA=";
    for (int i = 0; i < 10; ++i) {
      static constexpr char hex[] = "0123456789ABCDEF";
      const std::uint8_t v = yd->mood_data[i];
      out << hex[(v >> 4U) & 0xFU] << hex[v & 0xFU];
    }
    out << "\n";

    out << "SSL_NEWDATA=";
    const auto ssl_off = addr_to_offset(yd->addrSitStandNewData);
    if (ssl_off == static_cast<std::size_t>(-1) || ssl_off + 14 > image.size()) {
      if (err) *err = "SSL_NEWDATA address out of range";
      return false;
    }
    for (int i = 0; i < 14; ++i) {
      static constexpr char hex[] = "0123456789ABCDEF";
      const std::uint8_t v = image[ssl_off + i];
      out << hex[(v >> 4U) & 0xFU] << hex[v & 0xFU];
    }
    out << "\n";

    out << "SSL_SKIPCOUNT=" << yd->wSslSkipCount << "\n";
    out << "DISABLE_CHARGER_INIT=" << static_cast<int>(yd->boolDisableCharger) << "\n";
    out << "MUTE_INIT=" << static_cast<int>(yd->boolMute) << "\n";
    out << "IDLE_DELAY=" << static_cast<int>(yd->bIdleDelay) << "\n";

    out << "; ProbAction\n";
    for (std::size_t i = 0; i < kNumCond; ++i) {
      const CONDITION& c = g_conditions[i];
      if (c.ct != CT_PROBACTION) {
        continue;
      }
      out << "PROBACT(" << c.index << ")=";
      const auto it = probact_storage.find(c.index);
      const std::vector<std::uint16_t> vals = (it != probact_storage.end()) ? it->second : std::vector<std::uint16_t>{};
      for (std::size_t j = 0; j < vals.size(); ++j) {
        if (j != 0) out << ",";
        out << vals[j];
      }
      out << "\n";
    }

    out << "; Action\n";
    for (std::size_t i = 0; i < kNumCond; ++i) {
      const CONDITION& c = g_conditions[i];
      if (c.ct != CT_ACTION) {
        continue;
      }

      const addr_t ptr_addr = yd->addrExtraActionTable + static_cast<addr_t>(c.index * 4);
      const auto ptr_off = addr_to_offset(ptr_addr);
      if (ptr_off == static_cast<std::size_t>(-1) || ptr_off + 4 > image.size()) {
        if (err) *err = "ACTION table pointer out of range";
        return false;
      }
      const addr_t action_addr = read_u32_le(image, ptr_off);
      const auto a_it = addr_to_storage.find(action_addr);
      if (a_it == addr_to_storage.end()) {
        if (err) *err = "ACTION references unknown address";
        return false;
      }
      out << "ACT(" << c.index << ")=" << a_it->second << "\n";
    }

    out << "; EOF\n";
    return true;
  }

  bool parse_hex(const std::string& s, std::vector<std::uint8_t>& out) {
    if ((s.size() % 2) != 0) return false;
    out.clear();
    out.reserve(s.size() / 2);
    for (std::size_t i = 0; i < s.size(); i += 2) {
      const auto h = s.substr(i, 2);
      char* end = nullptr;
      long v = std::strtol(h.c_str(), &end, 16);
      if (!end || *end != '\0' || v < 0 || v > 255) return false;
      out.push_back(static_cast<std::uint8_t>(v));
    }
    return true;
  }

  bool import_icb_file(const std::string& icb_file, std::string* err) {
    std::ifstream in(icb_file);
    if (!in) {
      if (err) *err = "Failed to open ICB input";
      return false;
    }

    std::string line;
    while (std::getline(in, line)) {
      line = trim(line);
      if (line.empty() || line[0] == ';') {
        continue;
      }
      const auto eq = line.find('=');
      if (eq == std::string::npos) {
        if (err) *err = "Malformed ICB line: " + line;
        return false;
      }

      std::string lhs = line.substr(0, eq);
      std::string rhs = line.substr(eq + 1);

      int index = -1;
      const auto lp = lhs.find('(');
      if (lp != std::string::npos) {
        const auto rp = lhs.find(')', lp + 1);
        if (rp == std::string::npos) {
          if (err) *err = "Malformed indexed key: " + lhs;
          return false;
        }
        index = std::atoi(lhs.substr(lp + 1, rp - lp - 1).c_str());
        lhs = lhs.substr(0, lp);
      }

      if (lhs == "YICT_VERSION") {
        const int v = std::atoi(rhs.c_str());
        if (v != 201 && v != 202) {
          if (err) *err = "Unsupported YICT_VERSION";
          return false;
        }
      } else if (lhs == "STRETCH") {
        yd->bStretchCount = static_cast<std::uint8_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "WAGTAIL") {
        yd->bWagTime = static_cast<std::uint8_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "MOODDATA") {
        std::vector<std::uint8_t> hex;
        if (!parse_hex(rhs, hex) || hex.size() != 10) {
          if (err) *err = "Invalid MOODDATA";
          return false;
        }
        std::copy(hex.begin(), hex.end(), yd->mood_data);
      } else if (lhs == "SSL_NEWDATA") {
        std::vector<std::uint8_t> hex;
        if (!parse_hex(rhs, hex) || hex.size() != 14) {
          if (err) *err = "Invalid SSL_NEWDATA";
          return false;
        }
        const auto off = addr_to_offset(yd->addrSitStandNewData);
        if (off == static_cast<std::size_t>(-1) || off + 14 > image.size()) {
          if (err) *err = "SSL_NEWDATA target out of range";
          return false;
        }
        std::copy(hex.begin(), hex.end(), image.begin() + static_cast<std::ptrdiff_t>(off));
      } else if (lhs == "SSL_SKIPCOUNT") {
        yd->wSslSkipCount = static_cast<std::uint16_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "DISABLE_CHARGER_INIT") {
        yd->boolDisableCharger = static_cast<std::uint8_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "MUTE_INIT") {
        yd->boolMute = static_cast<std::uint8_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "IDLE_DELAY") {
        yd->bIdleDelay = static_cast<std::uint8_t>(std::atoi(rhs.c_str()));
      } else if (lhs == "PROBACT") {
        if (index < 0) {
          if (err) *err = "PROBACT missing index";
          return false;
        }
        std::vector<std::uint16_t> vals;
        std::stringstream ss(rhs);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
          tok = trim(tok);
          if (tok.empty()) continue;
          const int s = std::atoi(tok.c_str());
          if (s < 0 || static_cast<std::size_t>(s) >= storage_to_addr.size()) {
            if (err) *err = "PROBACT references invalid storage number";
            return false;
          }
          vals.push_back(static_cast<std::uint16_t>(s));
        }
        std::sort(vals.begin(), vals.end());
        probact_storage[index] = std::move(vals);
      } else if (lhs == "ACT") {
        if (index < 0) {
          if (err) *err = "ACT missing index";
          return false;
        }
        const int s = std::atoi(rhs.c_str());
        if (s < 0 || static_cast<std::size_t>(s) >= storage_to_addr.size()) {
          if (err) *err = "ACT references invalid storage number";
          return false;
        }

        const addr_t ptr_addr = yd->addrExtraActionTable + static_cast<addr_t>(index * 4);
        const auto ptr_off = addr_to_offset(ptr_addr);
        if (ptr_off == static_cast<std::size_t>(-1) || ptr_off + 4 > image.size()) {
          if (err) *err = "ACT pointer out of range";
          return false;
        }
        write_u32_le(image, ptr_off, storage_to_addr[static_cast<std::size_t>(s)]);
      }
    }

    return true;
  }
};

}  // namespace

InspectResult inspect_rom_halves(const std::string& base_name) {
  Cartridge c;
  std::string err;
  if (!c.load(base_name, &err)) {
    return {false, err, 0, 0, "", ""};
  }

  return {true,
          "ROM halves and YICT metadata are valid",
          c.image.size(),
          c.yd->generation,
          c_field_to_string(c.yd->name, sizeof(c.yd->name)),
          c_field_to_string(c.yd->basename, sizeof(c.yd->basename))};
}

bool export_icb(const std::string& base_name, const std::string& icb_file, std::string* error) {
  Cartridge c;
  if (!c.load(base_name, error)) {
    return false;
  }
  return c.export_icb_file(icb_file, error);
}

bool import_icb(const std::string& in_base_name,
                const std::string& icb_file,
                const std::string& out_base_name,
                std::string* error) {
  Cartridge c;
  if (!c.load(in_base_name, error)) {
    return false;
  }
  if (!c.import_icb_file(icb_file, error)) {
    return false;
  }

  // Native workflow mirrors legacy non-original save behavior.
  c.yd->generation = static_cast<std::uint16_t>(c.yd->generation + 1);
  string_to_c_field(fs::path(out_base_name).filename().string(), c.yd->name, sizeof(c.yd->name));
  string_to_c_field(fs::path(out_base_name).filename().string(), c.yd->basename, sizeof(c.yd->basename));

  return c.save(out_base_name, error);
}

}  // namespace yict
