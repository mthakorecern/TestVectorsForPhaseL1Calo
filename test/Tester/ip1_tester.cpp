#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ap_int.h"

extern "C" void run_ip1_5x6(const ap_uint<576>* in, ap_uint<576>* out);
extern "C" void run_ip1_2x6(const ap_uint<576>* in, ap_uint<576>* out);

namespace {




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

struct SectionSpec {
  std::string name;
  int n_inputs;
  bool is_5x6;
};

const std::vector<SectionSpec> kSpecs = {
    {"SLR3_5x6_INPUT", 30, true},
    {"SLR2_5x6_INPUT", 30, true},
    {"SLR1_5x6_INPUT", 30, true},
    {"SLR0_2x6_INPUT", 12, false},
};


std::map<std::string, std::vector<ap_uint<576>>> parseInputFile(const std::string& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("Could not open input file: " + path);
  }

  std::map<std::string, std::vector<ap_uint<576>>> sections;
  std::string current;
  std::string line;

  while (std::getline(in, line)) {
    std::string t = trim(line);
    if (t.empty()) continue;

    bool matched_section = false;
    for (const auto& spec : kSpecs) {
      if (t == spec.name) {
        current = spec.name;
        matched_section = true;
        break;
      }
    }
    if (matched_section) continue;

    if (current.empty()) continue;
    if (t.find("link_in[") != 0) continue;

    std::size_t colon = t.find(':');
    if (colon == std::string::npos) continue;

    std::string value = trim(t.substr(colon + 1));
    if (value.empty()) continue;

    sections[current].push_back(parseHex576(value));
  }

  return sections;
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
                       ap_uint<576>* links) {
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
                       ap_uint<576>* links) {
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

std::string slrLabelFromSection(const std::string& sectionName) {
  if (sectionName.find("SLR3") != std::string::npos) return "SLR3";
  if (sectionName.find("SLR2") != std::string::npos) return "SLR2";
  if (sectionName.find("SLR1") != std::string::npos) return "SLR1";
  if (sectionName.find("SLR0") != std::string::npos) return "SLR0";
  return "UNKNOWN";
}

void runOne(const SectionSpec& spec, const std::vector<ap_uint<576>>& in_words) {
  if ((int)in_words.size() != spec.n_inputs) {
    std::ostringstream os;
    os << "Section " << spec.name << " expected " << spec.n_inputs
       << " input links but got " << in_words.size();
    throw std::runtime_error(os.str());
  }

  ap_uint<576> out[2] = {0, 0};

  std::cout << "========================================\n";
  std::cout << spec.name << "\n";
  
  printParsedInputs("parsed FW inputs", in_words);

  if (spec.is_5x6) {
    run_ip1_5x6(in_words.data(), out);
    printRawOutputs("raw outputs", out, 2);
    decode5x6Outputs(out);
    printCompactSummary5x6(out);
  } else {
    run_ip1_2x6(in_words.data(), out);
    printRawOutputs("raw outputs", out, 2);
    decode2x6Outputs(out);
    printCompactSummary2x6(out);
  }

  // write one CSV per section
  std::string slrLabel = slrLabelFromSection(spec.name);
  std::string csvName = slrLabel + "_fw_decoded.csv";

  std::ofstream csv(csvName);
  writeDecodedCSVHeader(csv);

  if (spec.is_5x6) {
    dumpDecoded5x6CSV(csv, slrLabel, out);
  } else {
    dumpDecoded2x6CSV(csv, slrLabel, out);
  }

  csv.close();
  std::cout << "Wrote " << csvName << "\n";

  std::cout << "\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " rct_card_X_ip1_io.txt\n";
    return 1;
  }

  try {
    auto sections = parseInputFile(argv[1]);

    for (const auto& spec : kSpecs) {
      auto it = sections.find(spec.name);
      if (it == sections.end()) {
        std::cout << "Skipping missing section: " << spec.name << "\n";
        continue;
      }
      runOne(spec, it->second);
    }
  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << "\n";
    return 2;
  }

  return 0;
}