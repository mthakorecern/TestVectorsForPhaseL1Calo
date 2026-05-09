#include "ap_int.h"
#include "IP22algo_top.h"

extern "C" void run_ip22(const ap_uint<576>* in, ap_uint<576>* out) {
  ap_uint<576> link_in[N_INPUT_LINKS];
  ap_uint<576> link_out[N_OUTPUT_LINKS];

  for (int i = 0; i < N_INPUT_LINKS; ++i) {
    link_in[i] = in[i];
  }

  algo_top(link_in, link_out);

  for (int i = 0; i < N_OUTPUT_LINKS; ++i) {
    out[i] = link_out[i];
  }
}