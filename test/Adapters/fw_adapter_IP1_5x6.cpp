#include "ap_int.h"

// First we remove every global function symbol from the 5x6 implementation
#define algo_top            algo_top_5x6
#define processOutputLinks  processOutputLinks_5x6
#define processInputLinks   processInputLinks_5x6
#define bestOf2             bestOf2_5x6
#define getseedEta          getseedEta_5x6
#define getseedMax          getseedMax_5x6
#define getseedposition     getseedposition_5x6
#define getslice            getslice_5x6
#define zerrocrystals       zerrocrystals_5x6
#define getcluster          getcluster_5x6
#define createClusters1     createClusters1_5x6
#define createTowers        createTowers_5x6

#include "../scripts/fw_IP1_5x6/5x6algo_top.h"
#include "../scripts/fw_IP1_5x6/5x6algo_top.cpp"

#undef algo_top
#undef processOutputLinks
#undef processInputLinks
#undef bestOf2
#undef getseedEta
#undef getseedMax
#undef getseedposition
#undef getslice
#undef zerrocrystals
#undef getcluster
#undef createClusters1
#undef createTowers

namespace {
std::string formatApUint576_dbg(const ap_uint<576>& x) {
  if (x == 0) return "0";
  std::string s = x.to_string(16);
  if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) return s;
  return "0x" + s;
}
}

extern "C" void run_ip1_5x6(const ap_uint<576>* in, ap_uint<576>* out) {
  ap_uint<576> local_in[30];
  ap_uint<576> local_out[2] = {0, 0};

  std::cout << "========== run_ip1_5x6 wrapper ==========\n";
  std::cout << "wrapper inputs:\n";
  for (int i = 0; i < 30; ++i) {
    local_in[i] = in[i];
    std::cout << "  in[" << i << "] = " << formatApUint576_dbg(local_in[i]) << "\n";
  }

  algo_top_5x6(local_in, local_out);

  std::cout << "wrapper outputs:\n";
  for (int i = 0; i < 2; ++i) {
    out[i] = local_out[i];
    std::cout << "  out[" << i << "] = " << formatApUint576_dbg(local_out[i]) << "\n";
  }
  std::cout << "=========================================\n";
}