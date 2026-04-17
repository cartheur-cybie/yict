#include "yict_core.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
  if (argc >= 2 && std::string(argv[1]) == "inspect-yict" && argc == 3) {
    const auto result = yict::inspect_rom_halves(argv[2]);
    std::cout << (result.ok ? "OK: " : "ERROR: ") << result.message;
    if (result.ok) {
      std::cout << " (size=" << result.size_bytes << " bytes)"
                << ", generation=" << result.generation << ", name=\"" << result.rom_name
                << "\", basename=\"" << result.base_name << "\"";
    }
    std::cout << '\n';
    return result.ok ? 0 : 1;
  }

  if (argc >= 2 && std::string(argv[1]) == "export-icb" && argc == 4) {
    std::string error;
    if (!yict::export_icb(argv[2], argv[3], &error)) {
      std::cerr << "ERROR: " << error << "\n";
      return 1;
    }
    std::cout << "OK: exported " << argv[3] << "\n";
    return 0;
  }

  if (argc >= 2 && std::string(argv[1]) == "import-icb" && argc == 5) {
    std::string error;
    if (!yict::import_icb(argv[2], argv[3], argv[4], &error)) {
      std::cerr << "ERROR: " << error << "\n";
      return 1;
    }
    std::cout << "OK: wrote " << argv[4] << "-l.bin and " << argv[4] << "-h.bin\n";
    return 0;
  }

  std::cerr << "Usage:\n";
  std::cerr << "  yict_cli inspect-yict <base-name>\n";
  std::cerr << "  yict_cli export-icb <base-name> <out.icb>\n";
  std::cerr << "  yict_cli import-icb <in-base-name> <in.icb> <out-base-name>\n";
  std::cerr << "Example: yict_cli inspect-yict release/generic202\n";
  return 2;
}
