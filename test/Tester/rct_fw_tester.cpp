#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string quote(const std::string& s) {
  return "\"" + s + "\"";
}

void runCommand(const std::string& cmd) {
  std::cout << "[RUN] " << cmd << std::endl;
  int ret = std::system(cmd.c_str());
  if (ret != 0) {
    std::ostringstream os;
    os << "Command failed with exit code " << ret << ": " << cmd;
    throw std::runtime_error(os.str());
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 6) {
    std::cerr
        << "Usage: " << argv[0]
        << " <emu_dump_base_dir> <fw_output_base_dir> <card> <event> <bin_dir>\n\n"
        << "Arguments:\n"
        << "  emu_dump_base_dir    Emulator dump base directory for one card/event\n"
        << "                       Example: /path/to/rct_dumps/card_0_event_72692\n"
        << "  fw_output_base_dir   Base directory where FW outputs will be written\n"
        << "                       Example: /path/to/RCT_FW\n"
        << "  card                 Card number\n"
        << "  event                Event number\n"
        << "  bin_dir              Directory containing the tester executables\n"
        << "                       Example: .\n";
    return 1;
  }

  try {
    const std::string emuDumpBaseDir = argv[1];
    const std::string fwOutputBaseDir = argv[2];
    const std::string card = argv[3];
    const std::string event = argv[4];
    const std::string binDir = argv[5];

    const std::string ip1InputDir  = emuDumpBaseDir + "/input/IP1";
    const std::string ip3InputDir  = emuDumpBaseDir + "/input/IP3";

    const std::string ip1Exe  = binDir + "/ip1_tester";
    const std::string ip21Exe = binDir + "/ip21_tester";
    const std::string ip22Exe = binDir + "/ip22_tester";
    const std::string ip3Exe  = binDir + "/ip3_tester";

    const std::string ip1OutDir  = fwOutputBaseDir + "/output/IP1";
    const std::string ip21OutDir = fwOutputBaseDir + "/output/IP21";
    const std::string ip22OutDir = fwOutputBaseDir + "/output/IP22";

    runCommand(quote(ip1Exe) + " "
               + quote(ip1InputDir) + " "
               + quote(fwOutputBaseDir) + " "
               + card + " " + event);

    runCommand(quote(ip21Exe) + " "
               + quote(ip1OutDir) + " "
               + quote(fwOutputBaseDir) + " "
               + card + " " + event);

    runCommand(quote(ip22Exe) + " "
               + quote(ip21OutDir) + " "
               + quote(fwOutputBaseDir) + " "
               + card + " " + event);

    runCommand(quote(ip3Exe) + " "
               + quote(ip3InputDir) + " "
               + quote(fwOutputBaseDir) + " "
               + card + " " + event);

    std::cout << "\nAll FW stages completed successfully.\n";
    std::cout << "FW outputs written under: " << fwOutputBaseDir << std::endl;

  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << std::endl;
    return 2;
  }

  return 0;
}