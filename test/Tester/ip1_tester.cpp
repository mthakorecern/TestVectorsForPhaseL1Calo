#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>
#include "ap_int.h"

extern "C" void run_ip1_5x6(const ap_uint<576>* in, ap_uint<576>* out);
extern "C" void run_ip1_2x6(const ap_uint<576>* in, ap_uint<576>* out);

namespace {

void ensureDir(const std::string& dir) {
  std::string cmd = "mkdir -p \"" + dir + "\"";
  int ret = std::system(cmd.c_str());
  if (ret != 0) {
    throw std::runtime_error("Failed to create directory: " + dir);
  }
}

struct DumpDirs {
  std::string base;
  std::string output;
  std::string decoded;
  std::string outputIP1;
  std::string decodedIP1;
};

DumpDirs makeDumpDirs(const std::string& base) {
  DumpDirs d;
  d.base = base;
  d.output = d.base + "/output";
  d.decoded = d.base + "/decoded";
  d.outputIP1 = d.output + "/IP1";
  d.decodedIP1 = d.decoded + "/IP1";
  return d;
}

void ensureFWOutputDirs(const DumpDirs& d) {
  ensureDir(d.output);
  ensureDir(d.decoded);
  ensureDir(d.outputIP1);
  ensureDir(d.decodedIP1);
}


std::string trim(const std::string& s) {
  size_t b = 0;
  while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
  size_t e = s.size();
  while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
  return s.substr(b, e - b);
}

std::string hexIndex(unsigned int i) {
  std::ostringstream os;
  os << std::hex << std::nouppercase << i;
  return os.str();
}

std::string formatApUint576(const ap_uint<576>& x) {
  if (x == 0) return "0";
  std::string s = x.to_string(16);
  if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) return s;
  return "0x" + s;
}

std::string formatApUint64(const ap_uint<64>& x) {
  if (x == 0) return "0";
  std::string s = x.to_string(16);
  if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) return s;
  return "0x" + s;
}

std::string formatApUint18(const ap_uint<18>& x) {
  if (x == 0) return "0";
  std::string s = x.to_string(16);
  if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) return s;
  return "0x" + s;
}

ap_uint<576> parseHex576(std::string s) {
  s = trim(s);
  while (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) {
    s = s.substr(2);
  }
  if (s.empty()) return ap_uint<576>(0);

  ap_uint<576> out = 0;
  for (char c : s) {
    ap_uint<576> nibble = 0;
    if (c >= '0' && c <= '9') nibble = c - '0';
    else if (c >= 'a' && c <= 'f') nibble = 10 + (c - 'a');
    else if (c >= 'A' && c <= 'F') nibble = 10 + (c - 'A');
    else throw std::runtime_error("Invalid hex character in input: " + std::string(1, c));
    out = (out << 4) | nibble;
  }
  return out;
}


std::vector<std::string> splitCSVLine(const std::string& line) {
  std::vector<std::string> out;
  std::stringstream ss(line);
  std::string item;
  while (std::getline(ss, item, ',')) {
    out.push_back(item);
  }
  return out;
}

std::vector<ap_uint<576>> parseRawLinkCSV(const std::string& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("Could not open CSV input file: " + path);
  }

  std::string headerLine;
  if (!std::getline(in, headerLine)) {
    throw std::runtime_error("Empty CSV file: " + path);
  }

  auto headers = splitCSVLine(headerLine);
  int rawHexCol = -1;
  int indexCol  = -1;

  for (int i = 0; i < (int)headers.size(); ++i) {
    std::string h = trim(headers[i]);
    if (h == "raw_hex") rawHexCol = i;
    if (h == "link_index_dec") indexCol = i;
  }

  if (rawHexCol < 0) {
    throw std::runtime_error("CSV missing raw_hex column: " + path);
  }

  std::vector<std::pair<int, ap_uint<576>>> indexed;
  std::string line;

  while (std::getline(in, line)) {
    if (trim(line).empty()) continue;
    auto cols = splitCSVLine(line);
    if ((int)cols.size() <= rawHexCol) continue;

    int idx = (int)indexed.size();
    if (indexCol >= 0 && indexCol < (int)cols.size()) {
      idx = std::stoi(trim(cols[indexCol]));
    }

    ap_uint<576> word = parseHex576(trim(cols[rawHexCol]));
    indexed.push_back({idx, word});
  }

  std::sort(indexed.begin(), indexed.end(),
          [](const std::pair<int, ap_uint<576>>& a,
             const std::pair<int, ap_uint<576>>& b) {
            return a.first < b.first;
          });

  std::vector<ap_uint<576>> out;
  out.reserve(indexed.size());
  for (const auto& x : indexed) out.push_back(x.second);
  return out;
}



void printRawOutputs(const std::string& label, const ap_uint<576>* out, int n) {
  std::cout << label << "\n";
  for (int i = 0; i < n; ++i) {
    std::cout << "link_out[" << hexIndex(i) << "]: " << formatApUint576(out[i]) << "\n";
  }
}

void printParsedInputs(const std::string& label,
                       const std::vector<ap_uint<576>>& in_words) {
  std::cout << label << " (" << in_words.size() << " links)\n";
  for (size_t i = 0; i < in_words.size(); ++i) {
    std::cout << "link_in[" << hexIndex(i) << "]: "
              << formatApUint576(in_words[i]) << "\n";
  }
}

struct FWCluster {
  ap_uint<10> seedEnergy;
  ap_uint<12> energy;
  ap_uint<5> eta;
  ap_uint<5> phi;
  ap_uint<10> et5x5;
  ap_uint<10> et2x5;
  ap_uint<5> timing;
  ap_uint<1> spike;
  ap_uint<1> satur;
  ap_uint<2> brems;
  ap_uint<3> spare;
};

struct FWTower {
  ap_uint<12> energy;
  ap_uint<5> timing;
  ap_uint<1> spike;
};

FWCluster unpackFWCluster(ap_uint<64> word) {
  FWCluster c;
  c.seedEnergy = word.range(9, 0);
  c.energy     = word.range(21, 10);
  c.eta        = word.range(26, 22);
  c.phi        = word.range(31, 27);
  c.et5x5      = word.range(41, 32);
  c.et2x5      = word.range(51, 42);
  c.timing     = word.range(56, 52);
  c.spike      = word.range(57, 57);
  c.satur      = word.range(58, 58);
  c.brems      = word.range(60, 59);
  c.spare      = word.range(63, 61);
  return c;
}

FWTower unpackFWTower(ap_uint<18> word) {
  FWTower t;
  t.energy = word.range(11, 0);
  t.timing = word.range(16, 12);
  t.spike  = word.range(17, 17);
  return t;
}

bool isNonZeroCluster(const FWCluster& c) {
  return (c.seedEnergy != 0) || (c.energy != 0) || (c.et5x5 != 0) ||
         (c.et2x5 != 0) || (c.timing != 0) || (c.spike != 0) ||
         (c.satur != 0) || (c.brems != 0) || (c.spare != 0);
}

bool isNonZeroTower(const FWTower& t) {
  return (t.energy != 0) || (t.timing != 0) || (t.spike != 0);
}

void printClusterDetailed(int idx, ap_uint<64> raw, const FWCluster& c) {
  std::cout << "  cluster[" << idx << "]"
            << " raw=" << formatApUint64(raw)
            << " seedEnergy=" << c.seedEnergy
            << " energy=" << c.energy
            << " eta=" << c.eta
            << " phi=" << c.phi
            << " et5x5=" << c.et5x5
            << " et2x5=" << c.et2x5
            << " timing=" << c.timing
            << " spike=" << c.spike
            << " satur=" << c.satur
            << " brems=" << c.brems
            << " spare=" << c.spare
            << "\n";
}

void printTowerDetailed(int idx, ap_uint<18> raw, const FWTower& t) {
  std::cout << "  tower[" << idx << "]"
            << " raw=" << formatApUint18(raw)
            << " energy=" << t.energy
            << " timing=" << t.timing
            << " spike=" << t.spike
            << "\n";
}

void decode5x6Outputs(const ap_uint<576>* out) {
  std::cout << "decoded FW clusters (9)\n";
  for (int i = 0; i < 9; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)out[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);
    printClusterDetailed(i, raw, c);
  }

  std::cout << "decoded FW towers (30)\n";
  for (int i = 0; i < 30; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)out[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);
    printTowerDetailed(i, raw, t);
  }
}

void decode2x6Outputs(const ap_uint<576>* out) {
  std::cout << "decoded FW clusters (3)\n";
  for (int i = 0; i < 3; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)out[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);
    printClusterDetailed(i, raw, c);
  }

  std::cout << "decoded FW towers (12)\n";
  for (int i = 0; i < 12; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)out[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);
    printTowerDetailed(i, raw, t);
  }
}

void printCompactSummary5x6(const ap_uint<576>* out) {
  std::cout << "nonzero FW clusters summary\n";
  bool found_cluster = false;
  for (int i = 0; i < 9; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)out[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);
    if (isNonZeroCluster(c)) {
      found_cluster = true;
      printClusterDetailed(i, raw, c);
    }
  }
  if (!found_cluster) {
    std::cout << "  none\n";
  }

  std::cout << "nonzero FW towers summary\n";
  bool found_tower = false;
  for (int i = 0; i < 30; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)out[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);
    if (isNonZeroTower(t)) {
      found_tower = true;
      printTowerDetailed(i, raw, t);
    }
  }
  if (!found_tower) {
    std::cout << "  none\n";
  }
}

void printCompactSummary2x6(const ap_uint<576>* out) {
  std::cout << "nonzero FW clusters summary\n";
  bool found_cluster = false;
  for (int i = 0; i < 3; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)out[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);
    if (isNonZeroCluster(c)) {
      found_cluster = true;
      printClusterDetailed(i, raw, c);
    }
  }
  if (!found_cluster) {
    std::cout << "  none\n";
  }

  std::cout << "nonzero FW towers summary\n";
  bool found_tower = false;
  for (int i = 0; i < 12; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)out[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);
    if (isNonZeroTower(t)) {
      found_tower = true;
      printTowerDetailed(i, raw, t);
    }
  }
  if (!found_tower) {
    std::cout << "  none\n";
  }
}

void writeDecodedCSVHeader(std::ofstream& out) {
  out << "slr,block_type,object_type,index,raw_hex,"
      << "seedEnergy,clusterEnergy,eta,phi,et5x5,et2x5,"
      << "timing,spike,satur,brems,spare,towerEnergy\n";
}

void dumpDecoded5x6CSV(std::ofstream& out,
                       const std::string& slrLabel,
                       const ap_uint<576>* links) {
  // clusters from links[0]
  for (int i = 0; i < 9; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)links[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);

    out << slrLabel << ","
        << "5x6" << ","
        << "cluster" << ","
        << i << ","
        << formatApUint64(raw) << ","
        << (unsigned)c.seedEnergy << ","
        << (unsigned)c.energy << ","
        << (unsigned)c.eta << ","
        << (unsigned)c.phi << ","
        << (unsigned)c.et5x5 << ","
        << (unsigned)c.et2x5 << ","
        << (unsigned)c.timing << ","
        << (unsigned)c.spike << ","
        << (unsigned)c.satur << ","
        << (unsigned)c.brems << ","
        << (unsigned)c.spare << ","
        << ""
        << "\n";
  }

  // towers from links[1]
  for (int i = 0; i < 30; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)links[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);

    int towerEta = i / 6;
    int towerPhi = i % 6;

    out << slrLabel << ","
        << "5x6" << ","
        << "tower" << ","
        << i << ","
        << formatApUint18(raw) << ","
        << "" << ","
        << "" << ","
        << towerEta << ","
        << towerPhi << ","
        << "" << ","
        << "" << ","
        << (unsigned)t.timing << ","
        << (unsigned)t.spike << ","
        << "" << ","
        << "" << ","
        << "" << ","
        << (unsigned)t.energy
        << "\n";
  }
}

void dumpDecoded2x6CSV(std::ofstream& out,
                       const std::string& slrLabel,
                       const ap_uint<576>* links) {
  // clusters from links[0]
  for (int i = 0; i < 3; ++i) {
    int start = i * 64;
    ap_uint<64> raw = (ap_uint<64>)links[0].range(start + 63, start);
    FWCluster c = unpackFWCluster(raw);

    out << slrLabel << ","
        << "2x6" << ","
        << "cluster" << ","
        << i << ","
        << formatApUint64(raw) << ","
        << (unsigned)c.seedEnergy << ","
        << (unsigned)c.energy << ","
        << (unsigned)c.eta << ","
        << (unsigned)c.phi << ","
        << (unsigned)c.et5x5 << ","
        << (unsigned)c.et2x5 << ","
        << (unsigned)c.timing << ","
        << (unsigned)c.spike << ","
        << (unsigned)c.satur << ","
        << (unsigned)c.brems << ","
        << (unsigned)c.spare << ","
        << ""
        << "\n";
  }

  // towers from links[1]
  for (int i = 0; i < 12; ++i) {
    int start = i * 18;
    ap_uint<18> raw = (ap_uint<18>)links[1].range(start + 17, start);
    FWTower t = unpackFWTower(raw);

    int towerEta = i / 6;
    int towerPhi = i % 6;

    out << slrLabel << ","
        << "2x6" << ","
        << "tower" << ","
        << i << ","
        << formatApUint18(raw) << ","
        << "" << ","
        << "" << ","
        << towerEta << ","
        << towerPhi << ","
        << "" << ","
        << "" << ","
        << (unsigned)t.timing << ","
        << (unsigned)t.spike << ","
        << "" << ","
        << "" << ","
        << "" << ","
        << (unsigned)t.energy
        << "\n";
  }
}

void dumpRawLinksCSVSimple(std::ofstream& out,
                           int card,
                           unsigned long long eventId,
                           const std::string& component,
                           const std::string& direction,
                           const ap_uint<576>* links,
                           int nLinks) {
  out << "card,event,component,direction,link_index_dec,link_index_hex,raw_hex\n";
  for (int i = 0; i < nLinks; ++i) {
    out << card << ","
        << eventId << ","
        << component << ","
        << direction << ","
        << i << ","
        << hexIndex(i) << ","
        << formatApUint576(links[i]) << "\n";
  }
}

void writeRawLinksCSVFile(const std::string& fileName,
                          int card,
                          unsigned long long eventId,
                          const std::string& component,
                          const std::string& direction,
                          const ap_uint<576>* links,
                          int nLinks) {
  std::ofstream out(fileName);
  if (!out.is_open()) {
    throw std::runtime_error("Could not open output file: " + fileName);
  }
  dumpRawLinksCSVSimple(out, card, eventId, component, direction, links, nLinks);
}

void runOne(const std::string& slrLabel,
            const std::string& component,
            bool is5x6,
            const std::vector<ap_uint<576>>& in_words,
            const DumpDirs& dirs,
            int expectedInputs,
            int card,
            unsigned long long eventId) {
  if ((int)in_words.size() != expectedInputs) {
    std::ostringstream os;
    os << "Component " << component << " expected " << expectedInputs
       << " input links but got " << in_words.size();
    throw std::runtime_error(os.str());
  }

  ap_uint<576> out[2] = {0, 0};

  std::cout << "========================================\n";
  std::cout << component << "\n";
  printParsedInputs("parsed FW inputs", in_words);

  if (is5x6) {
    run_ip1_5x6(in_words.data(), out);
  } else {
    run_ip1_2x6(in_words.data(), out);
  }

  printRawOutputs("raw outputs", out, 2);

  // raw FW output links in emulator-style raw-link CSV
  {
    std::string rawOutFile =
    dirs.outputIP1 + "/card_" + std::to_string(card) +
    "_event_" + std::to_string(eventId) +
    "_" + component + "_output_links.csv";

    writeRawLinksCSVFile(rawOutFile, card, eventId, component, "output", out, 2);
  }

  // decoded FW output CSV in emulator-style decoded format
  {
    std::string decodedFile = dirs.decodedIP1 + "/" + slrLabel + "_fw_decoded.csv";
    std::ofstream csv(decodedFile);
    writeDecodedCSVHeader(csv);

    if (is5x6) {
      dumpDecoded5x6CSV(csv, slrLabel, out);
    } else {
      dumpDecoded2x6CSV(csv, slrLabel, out);
    }
  }

  std::cout << "\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <input_ip1_dir> <output_base_dir> <card> <event>\n";
    return 1;
  }

  try {
    std::string inputIP1Dir = argv[1];
    std::string outputBaseDir = argv[2];
    int card = std::stoi(argv[3]);
    unsigned long long eventId = std::stoull(argv[4]);

    DumpDirs dirs = makeDumpDirs(outputBaseDir);
    ensureFWOutputDirs(dirs);

    struct JobSpec {
      std::string slrLabel;
      std::string component;
      int nInputs;
      bool is5x6;
    };

    std::vector<JobSpec> jobs = {
      {"SLR3", "IP1_SLR3_5x6", 30, true},
      {"SLR2", "IP1_SLR2_5x6", 30, true},
      {"SLR1", "IP1_SLR1_5x6", 30, true},
      {"SLR0", "IP1_SLR0_2x6", 12, false},
    };

    for (const auto& job : jobs) {
      std::string inputFile =
          inputIP1Dir + "/card_" + std::to_string(card) +
          "_event_" + std::to_string(eventId) +
          "_" + job.component + "_input_links.csv";

      auto in_words = parseRawLinkCSV(inputFile);

      runOne(job.slrLabel,
             job.component,
             job.is5x6,
             in_words,
             dirs,
             job.nInputs,
             card,
             eventId);
    }

  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << "\n";
    return 2;
  }

  return 0;
}