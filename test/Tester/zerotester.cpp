#include <iostream>
#include <string>
#include "ap_int.h"

extern "C" void run_ip1_2x6(const ap_uint<576>* in, ap_uint<576>* out);
extern "C" void run_ip1_5x6(const ap_uint<576>* in, ap_uint<576>* out);

std::string formatApUint576(const ap_uint<576>& x) {
  if (x == 0) return "0";
  std::string s = x.to_string(16);
  if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) return s;
  return "0x" + s;
}

int main() {
  // ---------------- 2x6 zero test ----------------
  ap_uint<576> zin2x6[12];
  ap_uint<576> zout2x6[2];

  for (int i = 0; i < 12; ++i) {
    zin2x6[i] = 0;
  }
  zout2x6[0] = 0;
  zout2x6[1] = 0;

  run_ip1_2x6(zin2x6, zout2x6);

  std::cout << "zero-test 2x6 out[0] = " << formatApUint576(zout2x6[0]) << "\n";
  std::cout << "zero-test 2x6 out[1] = " << formatApUint576(zout2x6[1]) << "\n";

  // ---------------- 5x6 zero test ----------------
  ap_uint<576> zin5x6[30];
  ap_uint<576> zout5x6[2];

  for (int i = 0; i < 30; ++i) {
    zin5x6[i] = 0;
  }
  zout5x6[0] = 0;
  zout5x6[1] = 0;

  run_ip1_5x6(zin5x6, zout5x6);

  std::cout << "zero-test 5x6 out[0] = " << formatApUint576(zout5x6[0]) << "\n";
  std::cout << "zero-test 5x6 out[1] = " << formatApUint576(zout5x6[1]) << "\n";

  return 0;
}