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
#include "../../scripts/FW/IP3/IP3algo_top.h"

extern "C" void run_ip3(const ap_uint<576>* in, ap_uint<576>* out);

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
  std::string outputIP3;
};

DumpDirs makeDumpDirs(const std::string& base) {
  DumpDirs d;
  d.base = base;
  d.output = d.base + "/output";
  d.outputIP3 = d.output + "/IP3";
  return d;
}

void ensureFWOutputDirs(const DumpDirs& d) {
  ensureDir(d.output);
  ensureDir(d.outputIP3);
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
  while (std::getline(ss, item, ',')) out.push_back(item);
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
  for (size_t i = 0; i < indexed.size(); ++i) out.push_back(indexed[i].second);
  return out;
}

void dumpRawLinksCSV(std::ofstream& out,
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
  dumpRawLinksCSV(out, card, eventId, component, direction, links, nLinks);
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <ip3_input_dir> <output_base_dir> <card> <event>\n";
    return 1;
  }

  try {
    std::string ip3InputDir = argv[1];
    std::string outputBaseDir = argv[2];
    int card = std::stoi(argv[3]);
    unsigned long long eventId = std::stoull(argv[4]);

    DumpDirs dirs = makeDumpDirs(outputBaseDir);
    ensureFWOutputDirs(dirs);

    std::string ip3InputFile =
        ip3InputDir + "/card_" + std::to_string(card) +
        "_event_" + std::to_string(eventId) +
        "_IP3_input_links.csv";

    std::vector<ap_uint<576>> link_in = parseRawLinkCSV(ip3InputFile);

    if ((int)link_in.size() != N_INPUT_LINKS) {
      std::ostringstream os;
      os << "IP3 input should contain " << N_INPUT_LINKS
         << " links, got " << link_in.size();
      throw std::runtime_error(os.str());
    }

    ap_uint<576> link_out[N_OUTPUT_LINKS] = {0, 0, 0, 0};

    run_ip3(&link_in[0], link_out);

    // {
    //   std::string inFile =
    //       dirs.outputIP3 + "/card_" + std::to_string(card) +
    //       "_event_" + std::to_string(eventId) +
    //       "_IP3_input_links.csv";

    //   writeRawLinksCSVFile(inFile, card, eventId, "IP3", "input", &link_in[0], N_INPUT_LINKS);
    // }

    {
      std::string outFile =
          dirs.outputIP3 + "/card_" + std::to_string(card) +
          "_event_" + std::to_string(eventId) +
          "_IP3_output_links.csv";

      writeRawLinksCSVFile(outFile, card, eventId, "IP3", "output", link_out, N_OUTPUT_LINKS);
    }

    std::cout << "Wrote IP3 input/output CSVs under: " << dirs.outputIP3 << "\n";

  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << "\n";
    return 2;
  }

  return 0;
}