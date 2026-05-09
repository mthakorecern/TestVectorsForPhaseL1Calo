#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ap_int.h"

extern "C" void run_ip21(const ap_uint<576>* in, ap_uint<576>* out);

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
  std::string outputIP21;
};

DumpDirs makeDumpDirs(const std::string& base) {
  DumpDirs d;
  d.base = base;
  d.output = d.base + "/output";
  d.decoded = d.base + "/decoded";
  d.outputIP21 = d.output + "/IP21";
  return d;
}

void ensureFWOutputDirs(const DumpDirs& d) {
  ensureDir(d.output);
  ensureDir(d.decoded);
  ensureDir(d.outputIP21);
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
  int indexCol = -1;

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
    indexed.push_back(std::make_pair(idx, word));
  }

  std::sort(indexed.begin(), indexed.end(),
            [](const std::pair<int, ap_uint<576>>& a,
               const std::pair<int, ap_uint<576>>& b) {
              return a.first < b.first;
            });

  std::vector<ap_uint<576>> out;
  out.reserve(indexed.size());
  for (size_t i = 0; i < indexed.size(); ++i) {
    out.push_back(indexed[i].second);
  }
  return out;
}

void dumpRawLinksCSV(std::ofstream& out,
                     int card,
                     unsigned long long eventId,
                     const std::string& component,
                     const std::string& direction,
                     const ap_uint<576>* links,
                     int nLinks,
                     const std::vector<std::string>* sourceLabels = nullptr) {
  out << "card,event,component,direction,link_index_dec,link_index_hex";
  if (sourceLabels) out << ",source";
  out << ",raw_hex\n";

  for (int i = 0; i < nLinks; ++i) {
    out << card << ","
        << eventId << ","
        << component << ","
        << direction << ","
        << i << ","
        << hexIndex(i);

    if (sourceLabels) {
      if (i < (int)sourceLabels->size()) out << "," << (*sourceLabels)[i];
      else out << ",UNKNOWN";
    }

    out << "," << formatApUint576(links[i]) << "\n";
  }
}

void writeRawLinksCSVFile(const std::string& fileName,
                          int card,
                          unsigned long long eventId,
                          const std::string& component,
                          const std::string& direction,
                          const ap_uint<576>* links,
                          int nLinks,
                          const std::vector<std::string>* sourceLabels = nullptr) {
  std::ofstream out(fileName);
  if (!out.is_open()) {
    throw std::runtime_error("Could not open output file: " + fileName);
  }
  dumpRawLinksCSV(out, card, eventId, component, direction, links, nLinks, sourceLabels);
}

std::vector<std::string> makeIP21InputSources() {
  std::vector<std::string> src(8);
  src[0] = "SLR3_out0";
  src[1] = "SLR2_out0";
  src[2] = "SLR1_out0";
  src[3] = "SLR0_out0";
  src[4] = "SLR3_out1";
  src[5] = "SLR2_out1";
  src[6] = "SLR1_out1";
  src[7] = "SLR0_out1";
  return src;
}

void requireTwoLinks(const std::vector<ap_uint<576>>& v, const std::string& label) {
  if ((int)v.size() != 2) {
    std::ostringstream os;
    os << label << " expected 2 links but got " << v.size();
    throw std::runtime_error(os.str());
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <ip1_output_dir> <output_base_dir> <card> <event>\n";
    return 1;
  }

  try {
    std::string ip1OutputDir = argv[1];
    std::string outputBaseDir = argv[2];
    int card = std::stoi(argv[3]);
    unsigned long long eventId = std::stoull(argv[4]);

    DumpDirs dirs = makeDumpDirs(outputBaseDir);
    ensureFWOutputDirs(dirs);

    std::string slr3File =
        ip1OutputDir + "/card_" + std::to_string(card) +
        "_event_" + std::to_string(eventId) +
        "_IP1_SLR3_5x6_output_links.csv";

    std::string slr2File =
        ip1OutputDir + "/card_" + std::to_string(card) +
        "_event_" + std::to_string(eventId) +
        "_IP1_SLR2_5x6_output_links.csv";

    std::string slr1File =
        ip1OutputDir + "/card_" + std::to_string(card) +
        "_event_" + std::to_string(eventId) +
        "_IP1_SLR1_5x6_output_links.csv";

    std::string slr0File =
        ip1OutputDir + "/card_" + std::to_string(card) +
        "_event_" + std::to_string(eventId) +
        "_IP1_SLR0_2x6_output_links.csv";

    std::vector<ap_uint<576>> slr3 = parseRawLinkCSV(slr3File);
    std::vector<ap_uint<576>> slr2 = parseRawLinkCSV(slr2File);
    std::vector<ap_uint<576>> slr1 = parseRawLinkCSV(slr1File);
    std::vector<ap_uint<576>> slr0 = parseRawLinkCSV(slr0File);

    requireTwoLinks(slr3, "SLR3");
    requireTwoLinks(slr2, "SLR2");
    requireTwoLinks(slr1, "SLR1");
    requireTwoLinks(slr0, "SLR0");

    ap_uint<576> link_inIP21[8];
    link_inIP21[0] = slr3[0];
    link_inIP21[1] = slr2[0];
    link_inIP21[2] = slr1[0];
    link_inIP21[3] = slr0[0];
    link_inIP21[4] = slr3[1];
    link_inIP21[5] = slr2[1];
    link_inIP21[6] = slr1[1];
    link_inIP21[7] = slr0[1];

    ap_uint<576> link_outIP21[8] = {0,0,0,0,0,0,0,0};

    run_ip21(link_inIP21, link_outIP21);

    std::vector<std::string> srcLabels = makeIP21InputSources();

    // {
    //   std::string inFile =
    //       dirs.outputIP21 + "/card_" + std::to_string(card) +
    //       "_event_" + std::to_string(eventId) +
    //       "_IP21_input_links.csv";

    //   writeRawLinksCSVFile(inFile, card, eventId, "IP21", "input",
    //                        link_inIP21, 8, &srcLabels);
    // }

    {
      std::string outFile =
          dirs.outputIP21 + "/card_" + std::to_string(card) +
          "_event_" + std::to_string(eventId) +
          "_IP21_output_links.csv";

      writeRawLinksCSVFile(outFile, card, eventId, "IP21", "output",
                           link_outIP21, 8);
    }

    std::cout << "Wrote IP21 input/output CSVs under: " << dirs.outputIP21 << "\n";

  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << "\n";
    return 2;
  }

  return 0;
}