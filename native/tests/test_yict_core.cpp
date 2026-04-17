#include "yict_core.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main() {
  const fs::path root = fs::path(YICT_REPO_ROOT);
  const fs::path tmp = fs::temp_directory_path() / "yict_native_test";
  std::error_code ec;
  fs::remove_all(tmp, ec);
  fs::create_directories(tmp, ec);

  const std::string input_base = (root / "release/generic202").string();
  const std::string icb = (tmp / "roundtrip.icb").string();
  const std::string out_base = (tmp / "roundtrip_out").string();
  const std::string bad_icb = (tmp / "bad.icb").string();

  const auto inspect_in = yict::inspect_rom_halves(input_base);
  if (!inspect_in.ok) {
    std::cerr << "inspect input failed: " << inspect_in.message << "\n";
    return 1;
  }

  std::string error;
  if (!yict::export_icb(input_base, icb, &error)) {
    std::cerr << "export_icb failed: " << error << "\n";
    return 1;
  }

  if (!yict::import_icb(input_base, icb, out_base, &error)) {
    std::cerr << "import_icb failed: " << error << "\n";
    return 1;
  }

  const auto inspect_out = yict::inspect_rom_halves(out_base);
  if (!inspect_out.ok) {
    std::cerr << "inspect output failed: " << inspect_out.message << "\n";
    return 1;
  }

  if (inspect_out.generation != static_cast<std::uint16_t>(inspect_in.generation + 1)) {
    std::cerr << "unexpected generation bump\n";
    return 1;
  }

  // Failure path: missing ROM base should fail inspection.
  const auto missing = yict::inspect_rom_halves((tmp / "missing_base").string());
  if (missing.ok) {
    std::cerr << "missing base unexpectedly passed inspection\n";
    return 1;
  }

  // Failure path: invalid ICB line should fail import.
  {
    std::ofstream out(bad_icb, std::ios::out | std::ios::trunc);
    out << "YICT_VERSION=202\n";
    out << "BROKEN_LINE_WITHOUT_EQUALS\n";
  }
  std::string bad_error;
  if (yict::import_icb(input_base, bad_icb, (tmp / "bad_out").string(), &bad_error)) {
    std::cerr << "bad icb unexpectedly imported\n";
    return 1;
  }
  if (bad_error.find("Malformed ICB line") == std::string::npos) {
    std::cerr << "unexpected error for bad icb: " << bad_error << "\n";
    return 1;
  }

  std::cout << "yict_core_tests: ok\n";
  return 0;
}
