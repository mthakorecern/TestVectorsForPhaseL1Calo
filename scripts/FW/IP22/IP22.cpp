#include "IP22algo_top.h"


void processOutLinks(
   ecaltower ECALTowersSLR0[TOWERS_IN_ETA2 * TOWERS_IN_PHI],
   ecaltower ECALTowersSLR1[TOWERS_IN_ETA5 * TOWERS_IN_PHI],
   ecaltower ECALTowersSLR2[TOWERS_IN_ETA5 * TOWERS_IN_PHI],
   ecaltower ECALTowersSLR3[TOWERS_IN_ETA5 * TOWERS_IN_PHI],
   ap_uint < 576 > link_out[N_OUTPUT_LINKS]) {

   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR0 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR1 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR2 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR3 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = link_out complete dim = 0
   #pragma HLS PIPELINE II = 9
   #pragma HLS LATENCY min = 9

   ap_uint < 10 > start;
   ap_uint < 10 > end;
//packing towers for IP3
   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      start = i * 18;
      end = start + 17;
      link_out[1].range(end, start) = ECALTowersSLR3[i].getecaltower();
      link_out[2].range(end, start) = ECALTowersSLR2[i].getecaltower();
      link_out[3].range(end, start) = ECALTowersSLR1[i].getecaltower();
   }

   for (loop i = 0; i < TOWERS_IN_ETA2 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      start = i * 18;
      end = start + 17;
      link_out[4].range(end, start) = ECALTowersSLR0[i].getecaltower();
   }

}

void processInputLinks(ap_uint < 576 > link_in[N_INPUT_LINKS], ecaltower ECALTowersSLR3[TOWERS_IN_ETA5 * TOWERS_IN_PHI], ecaltower ECALTowersSLR2[TOWERS_IN_ETA5 * TOWERS_IN_PHI], ecaltower ECALTowersSLR1[TOWERS_IN_ETA5 * TOWERS_IN_PHI], ecaltower ECALTowersSLR0[TOWERS_IN_ETA2 * TOWERS_IN_PHI], ecalcluster ECALClustersRejected[27]) {
   #pragma HLS ARRAY_PARTITION variable = link_in complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALClustersRejected complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR3 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR2 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR1 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR0 complete dim = 0
   //#pragma HLS latency min=9

   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 10 > start = i * 18;
      ap_uint < 10 > end = start + 17;
      ECALTowersSLR3[i].fillecaltower(link_in[4].range(end, start));
   }
   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 10 > start = i * 18;
      ap_uint < 10 > end = start + 17;
      ECALTowersSLR2[i].fillecaltower(link_in[5].range(end, start));
   }
   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 10 > start = i * 18;
      ap_uint < 10 > end = start + 17;
      ECALTowersSLR1[i].fillecaltower(link_in[6].range(end, start));
   }

   for (loop i = 0; i < TOWERS_IN_ETA2 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 10 > start = i * 18;
      ap_uint < 10 > end = start + 17;
      ECALTowersSLR0[i].fillecaltower(link_in[7].range(end, start));
   }

//unpack rejected clusters received from IP21
   for (loop i = 0; i < 9; i++) {
      #pragma HLS UNROLL
      ap_uint < 10 > start = i * 64;
      ap_uint < 10 > end = start + 63;
      ECALClustersRejected[i].fillecalcluster(link_in[1].range(end, start));
      ECALClustersRejected[i+9].fillecalcluster(link_in[2].range(end, start));
      ECALClustersRejected[i + 18].fillecalcluster(link_in[3].range(end, start));
   }

}
void putbackintower2x6(ecalcluster ECALCluster, ecaltower ECALTowers[TOWERS_IN_ETA2 * TOWERS_IN_PHI]) {
   #pragma HLS ARRAY_PARTITION variable = ECALTowers complete dim = 0
   #pragma HLS INLINE
   loop id = (ECALCluster.eta) * 6 + (ECALCluster.phi);

   for (loop i = 0; i < TOWERS_IN_ETA2 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 12 > sum = ECALTowers[i].energy + ECALCluster.energy;
      if (id == i * 5) {
         ECALTowers[i].energy = sum;
      }
   }
}

void putbackintower(ecalcluster ECALCluster, ecaltower ECALTowers[TOWERS_IN_ETA5 * TOWERS_IN_PHI]) {
   #pragma HLS ARRAY_PARTITION variable = ECALTowers complete dim = 0
   #pragma HLS INLINE

   loop id = (ECALCluster.eta) * 6 + (ECALCluster.phi);

   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 12 > sum = ECALTowers[i].energy + ECALCluster.energy;
      if (id == i * 5) {
         ECALTowers[i].energy = sum;
      }
   }

}

void algo_top(ap_uint < 576 > link_in[N_INPUT_LINKS], ap_uint < 576 > link_out[N_OUTPUT_LINKS]) {

   #pragma HLS ARRAY_PARTITION variable = link_in complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = link_out complete dim = 0
   #pragma HLS PIPELINE II = 9
   #pragma HLS INTERFACE ap_ctrl_hs port = return
   //#pragma HLS latency min=145

   ecalcluster ECALClustersRejected[27];
   #pragma HLS ARRAY_PARTITION variable = ECALClustersRejected complete dim = 0
   ecaltower ECALTowersSLR3[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR2[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR1[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR0[TOWERS_IN_ETA2 * TOWERS_IN_PHI];
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR3 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR2 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR1 complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR0 complete dim = 0

   //reading clusters and towers

   processInputLinks(link_in, ECALTowersSLR3, ECALTowersSLR2, ECALTowersSLR1, ECALTowersSLR0, ECALClustersRejected);

   ecaltower ECALTowersSLR3C[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR2C[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR1C[TOWERS_IN_ETA5 * TOWERS_IN_PHI];
   ecaltower ECALTowersSLR0C[TOWERS_IN_ETA2 * TOWERS_IN_PHI];
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR3C complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR2C complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR1C complete dim = 0
   #pragma HLS ARRAY_PARTITION variable = ECALTowersSLR0C complete dim = 0
   for (loop i = 0; i < 21; i++) {
      #pragma HLS UNROLL

      if (ECALClustersRejected[i].spare == 0) putbackintower2x6(ECALClustersRejected[i], ECALTowersSLR0C);
      if (ECALClustersRejected[i].spare == 1) putbackintower(ECALClustersRejected[i], ECALTowersSLR1C);
      if (ECALClustersRejected[i].spare == 2) putbackintower(ECALClustersRejected[i], ECALTowersSLR2C);
      if (ECALClustersRejected[i].spare == 3) putbackintower(ECALClustersRejected[i], ECALTowersSLR3C);
   }

   for (loop i = 0; i < TOWERS_IN_ETA5 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 12 > energy3 = ECALTowersSLR3[i].energy + ECALTowersSLR3C[i].energy;
      ECALTowersSLR3[i].energy = energy3;
      ap_uint < 12 > energy2 = ECALTowersSLR2[i].energy + ECALTowersSLR2C[i].energy;
      ECALTowersSLR2[i].energy = energy2;
      ap_uint < 12 > energy1 = ECALTowersSLR1[i].energy + ECALTowersSLR1C[i].energy;
      ECALTowersSLR1[i].energy = energy1;
   }

   for (loop i = 0; i < TOWERS_IN_ETA2 * TOWERS_IN_PHI; i++) {
      #pragma HLS UNROLL
      ap_uint < 12 > energy = ECALTowersSLR0[i].energy + ECALTowersSLR0C[i].energy;
      ECALTowersSLR0[i].energy = energy;
   }

   /*---------------------------------link 0------------------------------------*/

   link_out[0] = 0;
   link_out[1] = 0;
   link_out[2] = 0;
   link_out[3] = 0;
   link_out[4] = 0;

   processOutLinks(ECALTowersSLR0, ECALTowersSLR1, ECALTowersSLR2, ECALTowersSLR3, link_out);
   link_out[0]=link_in[0];

}
