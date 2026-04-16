#include "2x6algo_top.h"

void processOutputLinks(ecalcluster ECALClustersH1[N_CLUSTERS], ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI], ap_uint<576> link_out[N_OUTPUT_LINKS]){
#pragma HLS ARRAY_PARTITION variable=ECALClustersH1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=9

ap_uint<10> start ;
ap_uint<10> end ;


for(loop i=0; i<N_CLUSTERS; i++){
#pragma HLS unroll
	start=i*64 ; end=start+63;
	link_out[0].range(end, start) = ECALClustersH1[i].getecalcluster() ;
}


for(loop i=0; i<TOWERS_IN_ETA*TOWERS_IN_PHI; i++){
#pragma HLS unroll
	start=i*18 ; end=start+17;
	link_out[1].range(end, start) = ECALTowers[i].getecaltower() ;
}

}


void processInputLinks(ap_uint<576> link_in[N_INPUT_LINKS], ecalcrystal ECALCrystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI]){
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0

ap_uint<6> wordId;
ap_uint<6> startId;

for(loop i=0; i<CRYSTALS_IN_ETA; i++){
    #pragma HLS UNROLL
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    #pragma HLS UNROLL
    wordId  = (i/5)*6+(j/5);
    startId = (i%5)*5+(j%5);
    ap_uint<10> start   = startId*16;
    ap_uint<10> end = start + 15;
    ECALCrystals[i][j] = ecalcrystal(link_in[wordId].range(end, start));
  }
}

}

ecalcrystal  bestOf2(const ecalcrystal& ecaltp0, const ecalcrystal& ecaltp1) {
//#pragma HLS latency min=1
#pragma HLS inline

ecalcrystal x;
    x = (ecaltp0.energy > ecaltp1.energy)?ecaltp0:ecaltp1;
    return x;
}

void getseedEta(ecalcrystal crystals[CRYSTALS_IN_PHI], ecalcrystal &EtaMax){
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline
//#pragma HLS latency min=5


ecalcrystal Step1[CRYSTALS_IN_PHI/2+1] ;
ecalcrystal Step2[CRYSTALS_IN_PHI/4+1] ;
ecalcrystal Step3[CRYSTALS_IN_PHI/8+1] ;
#pragma HLS ARRAY_PARTITION variable=Step1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=Step2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=Step3 complete dim=0

for(loop k=0; k<CRYSTALS_IN_PHI; k=k+2){
    #pragma HLS UNROLL
    Step1[k/2]  = bestOf2(crystals[k], crystals[k+1]) ;
}
	Step1[15] = Step1[14] ;

for(loop k=0; k<CRYSTALS_IN_PHI/2+1; k=k+2){
    #pragma HLS UNROLL
	Step2[k/2]  = bestOf2(Step1[k], Step1[k+1]) ;
}

for(loop k=0; k<CRYSTALS_IN_PHI/4+1; k=k+2){
    #pragma HLS UNROLL
    Step3[k/2]  = bestOf2(Step2[k], Step2[k+1]) ;
}

ecalcrystal x1 = bestOf2(Step3[0], Step3[1]) ;
ecalcrystal x2 = bestOf2(Step3[2], Step3[3]) ;

EtaMax  = bestOf2(x1,x2) ;

}

void getseedMax(ecalcrystal crystals[CRYSTALS_IN_ETA23], ecalcrystal &Seed) {
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline

ecalcrystal Step1[CRYSTALS_IN_ETA23/2] ;
ecalcrystal Step2[CRYSTALS_IN_ETA23/4] ;
#pragma HLS ARRAY_PARTITION variable=Step1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=Step2 complete dim=0

for(loop k=0; k<CRYSTALS_IN_ETA23; k=k+2){
    #pragma HLS UNROLL
    Step1[k/2]  = bestOf2(crystals[k+1], crystals[k]) ;
}

for(loop k=0; k<CRYSTALS_IN_ETA23/2-1; k=k+2){
    #pragma HLS UNROLL
    Step2[k/2]  = bestOf2(Step1[k], Step1[k+1]) ;
}

ecalcrystal x1 = bestOf2(Step2[0], Step2[1]);
Seed  = bestOf2(x1, Step1[4]);

}

void getseedposition(ecalcrystal crystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcrystal &Seed){
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline
//#pragma HLS latency min=11


ecalcrystal EtaSlices[CRYSTALS_IN_ETA23] ;
#pragma HLS ARRAY_PARTITION variable=EtaSlices complete dim=0

ecalcrystal crystals1D[CRYSTALS_IN_PHI] ;
#pragma HLS ARRAY_PARTITION variable=crystals1D complete dim=0

ecalcrystal tmp ;

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    #pragma HLS UNROLL
        tmp = crystals[i][j] ;
        crystals1D[j] = tmp ;
}
    getseedEta(crystals1D, EtaSlices[i]) ;
}

getseedMax(EtaSlices, Seed) ;

}

void getslice(ecalcrystal crystals[CRYSTALS_IN_PHI+4], const ecalcrystal& Seed, ap_uint<12> &value) {
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline
//#pragma HLS latency min=4

ecalcrystalmask mask[CRYSTALS_IN_PHI+4] ;
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0

for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
	#pragma HLS UNROLL
	mask[j].energy = 0 ;
}

for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    #pragma HLS UNROLL
	if(j == Seed.phi) {
        mask[j+0].energy = 1 ;
        mask[j+1].energy = 1 ;
        mask[j+2].energy = 1 ;
        mask[j+3].energy = 1 ;
        mask[j+4].energy = 1 ;
    }
	else { }
	}

ap_uint<12> sumA = 0;
for(loop j=0; j<9; j++){
    #pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy;
    ap_uint<10> energy = energytmp * mask[j].energy;
    sumA = sumA + energy;
}

ap_uint<12> sumB = 0;
for(loop j=9; j<18; j++){
    #pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy;
    ap_uint<10> energy = energytmp * mask[j].energy;
    sumB = sumB + energy;
}

ap_uint<12> sumC = 0;
for(loop j=18; j<27; j++){
    #pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy;
    ap_uint<10> energy = energytmp * mask[j].energy;
    sumC = sumC + energy;
}

ap_uint<12> sumD = 0;
for(loop j=27; j<34; j++){
    #pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy;
    ap_uint<10> energy = energytmp * mask[j].energy;
    sumD = sumD + energy;
}

ap_uint<13> sumAB = sumA + sumB;
ap_uint<13> sumCD = sumC + sumD;
value = sumAB + sumCD;

}

void zerrocrystals(ecalcrystal ECALCrystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcrystal Seed, ap_uint<2> brems) {
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
#pragma HLS inline
//#pragma HLS latency min=5

ecalcrystalmask mask[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskN[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskP[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskI[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskN complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskP complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskI complete dim=0

ap_uint<5> eta = Seed.eta ;

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
        for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
	    if(j+7 >= Seed.phi && j+3 <= Seed.phi ) maskN[i][j].energy=1 ;
	}
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
        for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
	    if(j+2 >= Seed.phi && j <= Seed.phi+2 ) mask[i][j].energy =1 ;
	}
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
    	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		if(j >= Seed.phi+3 && j <= Seed.phi+7 ) maskP[i][j].energy=1 ;
	}
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		maskI[i][j].energy = ((ap_uint<1>)1 - mask[i][j].energy) ;
	}
}

if(brems == 1){
for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskN[i][j].energy) ;
    	maskI[i][j].energy = mask0 ;
    }
	}
}

if(brems == 2){
    for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    	#pragma HLS UNROLL
        for(loop j=0; j<CRYSTALS_IN_PHI; j++){
      		#pragma HLS UNROLL
			ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskP[i][j].energy) ;
        	maskI[i][j].energy = mask0 ;
        }
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ap_uint<10> tmp = ECALCrystals[i][j].energy ;
		ap_uint<10> energy = tmp * maskI[i][j].energy  ;
    	ECALCrystals[i][j].energy = energy ;
	}
}

}

void getcluster(ecalcrystal ECALCrystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcrystal Seed, ecalcluster &output) {
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
#pragma HLS inline
//#pragma HLS latency min=18

ap_uint<2> brems = 0 ;
ap_uint<12> NegValue=0, CntrValue=0, PosValue=0 ;
ap_uint<12> s5x5=0, s2x5=0, s2x5n=0, s2x5p=0 ;
ap_uint<12> CntrSlice[5] ;
ap_uint<12> NegSlice[5] ;
ap_uint<12> PosSlice[5] ;
#pragma HLS ARRAY_PARTITION variable=CntrSlice complete dim=0
#pragma HLS ARRAY_PARTITION variable=NegSlice complete dim=0
#pragma HLS ARRAY_PARTITION variable=PosSlice complete dim=0

ecalcrystal extendedcrystals[CRYSTALS_IN_ETA23+4][CRYSTALS_IN_PHI+4] ;
    #pragma HLS ARRAY_PARTITION variable=extendedcrystals complete dim=0

for(loop ii=0; ii<CRYSTALS_IN_ETA23+4; ii++){
    #pragma HLS UNROLL
    for(loop jj=0; jj<CRYSTALS_IN_PHI+4; jj++){
        #pragma HLS UNROLL
        extendedcrystals[ii][jj].energy = 0;
        extendedcrystals[ii][jj].timing = 0;
        extendedcrystals[ii][jj].spike = 0;
    }
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    #pragma HLS UNROLL
    extendedcrystals[i+2][j+2].energy = ECALCrystals[i][j].energy ;
	}
}

ap_uint<5> seed_eta = Seed.eta;

// Compute CntrSlice
for(loop k=0; k<5; k++){
    #pragma HLS UNROLL
	ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
    #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
    #pragma HLS UNROLL
    extendedcrystals1D[j].energy = extendedcrystals[seed_eta + k][j].energy ;
}
	getslice(extendedcrystals1D, Seed, CntrSlice[k]) ;
}

// Compute NegSlice
NegSlice[0] = 0;
NegSlice[4] = 0;
for(loop k=0; k<3; k++){
    #pragma HLS UNROLL
ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
    #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
    for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
        #pragma HLS UNROLL
        extendedcrystals1D[j].energy = 0;
    }
	for(loop j=0; j<CRYSTALS_IN_PHI-1; j++){
       	#pragma HLS UNROLL
    	extendedcrystals1D[j+5].energy = extendedcrystals[seed_eta + k +1][j].energy ;
	}
    getslice(extendedcrystals1D, Seed, NegSlice[k+1]) ;
}

// Compute PosSlice
PosSlice[0] = 0;
PosSlice[4] = 0;
for(loop k=0; k<3; k++){
    #pragma HLS UNROLL
	ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
    #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
    for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
        #pragma HLS UNROLL
        extendedcrystals1D[j].energy = 0;
    }
	for(loop j=0; j<CRYSTALS_IN_PHI-1; j++){
        #pragma HLS UNROLL
        extendedcrystals1D[j].energy = extendedcrystals[seed_eta + k +1][j+5].energy ;
	}
	getslice(extendedcrystals1D, Seed, PosSlice[k+1]) ;
}

CntrValue = CntrSlice[1] + CntrSlice[2] + CntrSlice[3] ;
NegValue = NegSlice[1] + NegSlice[2] + NegSlice[3] ;
PosValue = PosSlice[1] + PosSlice[2] + PosSlice[3] ;
s5x5 = CntrSlice[0] + CntrSlice[1] + CntrSlice[2] + CntrSlice[3] + CntrSlice[4] ;
s2x5n = CntrSlice[1] + CntrSlice[2] ;
s2x5p = CntrSlice[2] + CntrSlice[3] ;

ap_uint<12> clusterEnergyDiv8 = CntrValue >> 3;
ap_uint<12> Total = CntrValue ;

if(NegValue > clusterEnergyDiv8 && NegValue > PosValue){
	Total = CntrValue + NegValue;
	brems = 1;
}
else if(PosValue > clusterEnergyDiv8){
    Total = CntrValue + PosValue;
brems  = 2;
}
else { brems = 0;}

s2x5 = (s2x5n > s2x5p) ? s2x5n:s2x5p ;

output.seedEnergy = Seed.energy ;
output.energy = Total ;
output.eta = Seed.eta ;
output.phi = Seed.phi ;
output.timing = Seed.timing ;
output.spike = Seed.spike ;
output.brems = brems ;
output.et2x5 = s2x5 ;
output.et5x5 = s5x5 ;

}

void createClusters1(ecalcrystal crystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcluster ECALClusters[N_CLUSTERS]) {
#pragma HLS ARRAY_PARTITION variable=ECALClusters complete dim=0
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline

ecalcrystal Seed ;

getseedposition(crystals, Seed);
getcluster(crystals, Seed, ECALClusters[0]) ;
zerrocrystals(crystals, Seed, ECALClusters[0].brems);

getseedposition(crystals, Seed);
getcluster(crystals, Seed, ECALClusters[1]) ;
zerrocrystals(crystals, Seed, ECALClusters[1].brems);

getseedposition(crystals, Seed);
getcluster(crystals, Seed, ECALClusters[2]) ;
zerrocrystals(crystals, Seed, ECALClusters[2].brems);

}

void createTowers(ecalcrystal crystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI],ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI]){
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS INLINE

for(loop i=0; i<CRYSTALS_IN_ETA; i++){
	#pragma HLS UNROLL
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ap_uint<12> energy = ECALTowers[i/5*6+j/5].energy + crystals[i][j].energy;
		ECALTowers[i/5*6+j/5].energy = energy  ;
		ECALTowers[i/5*6+j/5].timing = 0 ;
		ECALTowers[i/5*6+j/5].spike = 0 ;
	}
}
}

void algo_top(ap_uint<576> link_in[N_INPUT_LINKS], ap_uint<576> link_out[N_OUTPUT_LINKS]){
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=9
#pragma HLS INTERFACE ap_ctrl_hs port=return

ecalcluster ECALClusters[N_CLUSTERS] ;
ecalcluster ECALClustersH1[N_CLUSTERS] ;
ecalcrystal ECALCrystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI];
ecalcrystal ECALCrystalsH1[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=ECALClusters complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystalsH1 complete dim=0

ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0

//creating 25x30 crystals matrix

processInputLinks(link_in, ECALCrystals) ;

// 25 crystals in eta are divided into 3 regions
// to create 3 regions, 11 eta each, with 2 eta overlap
// ETA23 = 11
//       11             11               11
//      9+2    -      2+7+2       -     2+9
//  0...8 9.10 - 7.8 9...15 16.17 - 14.15 16...24

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
    #pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ECALCrystals[i][j].eta = i ;
		ECALCrystals[i][j].phi = j ;
	}
}

// each region 11x25 is treated in the same way and later we combine them back in one

createClusters1(ECALCrystals, ECALClustersH1) ;

createTowers(ECALCrystals, ECALTowers) ;

/*---------------------------------link 0------------------------------------*/

link_out[0] = 0;
link_out[1] = 0;

processOutputLinks(ECALClustersH1, ECALTowers, link_out);

}
