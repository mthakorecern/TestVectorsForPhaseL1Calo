#include "ap_int.h"
#include "../../scripts/FW/IP21/IP21algo_top.h"
#include "../../scripts/FW/IP21/bitonicSort32.h"
#include "../../scripts/FW/IP21/oe.h"


extern "C" void run_ip21(const ap_uint<576>* in, ap_uint<576>* out) {
  ap_uint<576> link_in[N_INPUT_LINKS];
  ap_uint<576> link_out[N_OUTPUT_LINKS];

  for (int i = 0; i < N_INPUT_LINKS; ++i) link_in[i] = in[i];

  algo_top(link_in, link_out,
           126, 125, 125, 124, 123, 123, 122, 122, 121, 121,
           121, 121, 121, 120, 120, 120, 120, 120, 120, 120,
           120, 120, 120, 120, 120);

  for (int i = 0; i < N_OUTPUT_LINKS; ++i) out[i] = link_out[i];
}