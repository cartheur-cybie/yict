#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace yict {

struct InspectResult {
  bool ok = false;
  std::string message;
  std::size_t size_bytes = 0;
  std::uint16_t generation = 0;
  std::string rom_name;
  std::string base_name;
};

struct GeneralSettings {
  std::uint8_t stretch_count = 0;
  std::uint8_t wag_time = 0;
  std::uint16_t ssl_skip_count = 0;
  std::vector<std::uint8_t> mood_data;      // 10 bytes
  std::vector<std::uint8_t> ssl_new_data;   // 14 bytes
  std::uint8_t disable_charger_init = 0;
  std::uint8_t mute_init = 0;
  std::uint8_t idle_delay = 0;
};

InspectResult inspect_rom_halves(const std::string& base_name);
bool export_icb(const std::string& base_name, const std::string& icb_file, std::string* error);
bool import_icb(const std::string& in_base_name, const std::string& icb_file, const std::string& out_base_name,
                std::string* error);

}  // namespace yict
