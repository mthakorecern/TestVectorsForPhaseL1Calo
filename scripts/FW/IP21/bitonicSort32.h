#ifndef _BITONICSORT32_H_
#define _BITONICSORT32_H_

#include <iostream>
#include "ap_int.h"
#include "IP21algo_top.h"

#define N 32

using namespace std;

class GreaterSmaller{
public:
    ecalcluster greater, smaller;
};

typedef ap_uint<6> dloop_t ;

void bitonicSort32(ecalcluster in[N], ecalcluster out[N]);

#endif
