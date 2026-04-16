#include "5x6algo_top.h"

void processOutputLinks(ecalcluster ECALClustersH1[N_CLUSTERS], ecalcluster ECALClustersH2[N_CLUSTERS], ecalcluster ECALClustersH3[N_CLUSTERS], ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI], ap_uint<576> link_out[N_OUTPUT_LINKS]){
#pragma HLS ARRAY_PARTITION variable=ECALClustersH1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH3 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=9
#pragma HLS LATENCY min=9

ap_uint<10> start;
ap_uint<10> end;

for(loop i=0; i<N_CLUSTERS; i++){
	start=i*64 ; end=start+63;
	link_out[0].range(end, start) = ECALClustersH1[i].getecalcluster() ;
	start=(3+i)*64 ; end=start+63;
	link_out[0].range(end, start) = ECALClustersH2[i].getecalcluster() ;
	start=(6+i)*64 ; end=start+63;
    link_out[0].range(end, start) = ECALClustersH3[i].getecalcluster() ;
}

for(loop i=0; i<TOWERS_IN_ETA*TOWERS_IN_PHI; i++){
	start=i*18 ; end=start+17;
	link_out[1].range(end, start) = ECALTowers[i].getecaltower() ;
}

}

void processInputLinks(ap_uint<576> link_in[N_INPUT_LINKS], ecalcrystal ECALCrystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI]){
#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
//#pragma HLS latency min=9

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
#pragma HLS latency min=1

ecalcrystal x;
x = (ecaltp0.energy > ecaltp1.energy)?ecaltp0:ecaltp1;
return x;

}

void getseedEta(ecalcrystal crystals[CRYSTALS_IN_PHI], ecalcrystal &EtaMax){
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline
 
ecalcrystal Step1[CRYSTALS_IN_PHI/2+1];
ecalcrystal Step2[CRYSTALS_IN_PHI/4+1];
ecalcrystal Step3[CRYSTALS_IN_PHI/8+1];
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

ecalcrystal Step1[CRYSTALS_IN_ETA23/2+1] ;
ecalcrystal Step2[CRYSTALS_IN_ETA23/4+1] ;
#pragma HLS ARRAY_PARTITION variable=Step1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=Step2 complete dim=0

for(loop k=0; k<CRYSTALS_IN_ETA23-1; k=k+2){
	#pragma HLS UNROLL
	Step1[k/2]  = bestOf2(crystals[k+1], crystals[k]) ;
}

Step1[5]=crystals[10] ;

for(loop k=0; k<CRYSTALS_IN_ETA23/2+1; k=k+2){
	#pragma HLS UNROLL
    Step2[k/2]  = bestOf2(Step1[k], Step1[k+1]) ;
}

ecalcrystal x1 = bestOf2(Step2[0], Step2[1]) ;

Seed  = bestOf2(x1, Step2[2]) ;

}

void getseedposition(ecalcrystal crystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcrystal &Seed){
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS latency min=11

ecalcrystal EtaSlices[CRYSTALS_IN_ETA23];
ecalcrystal crystals1D[CRYSTALS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=EtaSlices complete dim=0
#pragma HLS ARRAY_PARTITION variable=crystals1D complete dim=0

ecalcrystal tmp ;

int loop_var =0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var +=1;
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
        tmp = crystals[i][j] ;
        crystals1D[j] = tmp ;
	}
	getseedEta(crystals1D, EtaSlices[i]) ;
}

for(loop i=loop_var; i<CRYSTALS_IN_ETA23; i++){
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
#pragma HLS latency min=4

ap_uint<12> tmpValue = 0 ;

ecalcrystalmask mask[CRYSTALS_IN_PHI+4] ;
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0

for(loop j=0; j<CRYSTALS_IN_PHI; j++){
	#pragma HLS UNROLL
    mask[j].energy = 0 ;
}

int loop_var =0;
for(loop j=0; j<1; j++){
	#pragma HLS UNROLL
	loop_var +=1;
	if(j == Seed.phi){
		mask[j+0].energy = 1;
        mask[j+1].energy = 1;
        mask[j+2].energy = 1;
        mask[j+3].energy = 1;
        mask[j+4].energy = 1;

    }
else {}
}

for(loop j=loop_var; j<CRYSTALS_IN_PHI; j++){
	#pragma HLS UNROLL
	if(j == Seed.phi){
		mask[j+0].energy = 1;
        mask[j+1].energy = 1;
        mask[j+2].energy = 1;
        mask[j+3].energy = 1;
        mask[j+4].energy = 1;
    }
else {}
}

int loop_var_1=0;
for(loop j=0; j<1; j++){
	#pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy ;
    ap_uint<10> energy = energytmp *  mask[j].energy ;
    ap_uint<12> tmp = tmpValue + energy ;
    tmpValue = tmp  ;
    loop_var_1+=1;
}

for(loop j=loop_var_1; j<CRYSTALS_IN_PHI+4; j++){
	#pragma HLS UNROLL
    ap_uint<10> energytmp = crystals[j].energy ;
    ap_uint<10> energy = energytmp *  mask[j].energy ;
    ap_uint<12> tmp = tmpValue + energy ;
    tmpValue = tmp  ;
}
value = tmpValue ;
}

void zerrocrystals(ecalcrystal ECALCrystals[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI], ecalcrystal Seed, ap_uint<2> brems) {
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
#pragma HLS latency min=5

ecalcrystalmask mask[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskN[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskP[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
ecalcrystalmask maskI[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI] ;
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskN complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskP complete dim=0
#pragma HLS ARRAY_PARTITION variable=maskI complete dim=0

ap_uint<5> eta = Seed.eta ;

//*************" loop 1"*******************************


int loop_var_1=0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_1 +=0;
	if(i+1 >= eta && i <= eta+1){
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
    	if(j+7 >= Seed.phi && j+3 <= Seed.phi ) maskN[i][j].energy=1 ;
        }
	}
}


for(loop i=loop_var_1; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
    	if(j+7 >= Seed.phi && j+3 <= Seed.phi ) maskN[i][j].energy=1 ;
        }
	}
}

//*************" loop 2"*******************************


int loop_var_2 =0;
for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_2 += 1;
	if(i+1 >= eta && i <= eta+1){
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
		if(j+2 >= Seed.phi && j <= Seed.phi+2 ) mask[i][j].energy =1 ;
		}
	}
}

for(loop i=loop_var_2; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
		if(j+2 >= Seed.phi && j <= Seed.phi+2 ) mask[i][j].energy =1 ;
		}
	}
}


//*************" loop 3"*******************************

int loop_var_3 = 0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_3 += 1;
	if(i+1 >= eta && i <= eta+1){
        for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
        if(j >= Seed.phi+3 && j <= Seed.phi+7 ) maskP[i][j].energy=1 ;
        }
	}
}


for(loop i=loop_var_3; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i+1 >= eta && i <= eta+1){
        for(loop j=0; j<CRYSTALS_IN_PHI; j++){
        #pragma HLS UNROLL
        if(j >= Seed.phi+3 && j <= Seed.phi+7 ) maskP[i][j].energy=1 ;
        }
	}
}


//*************" loop 4"*******************************

int loop_var_4 =0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_4 += 1;
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
        maskI[i][j].energy = ((ap_uint<1>)1 - mask[i][j].energy) ;
    }
}

for(loop i=loop_var_4; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
        maskI[i][j].energy = ((ap_uint<1>)1 - mask[i][j].energy) ;
    }
}


//*************" loop 5"*******************************

if(brems == 1){

	int loop_var_5 =0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_5 += 1;
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskN[i][j].energy) ;
        maskI[i][j].energy = mask0 ;
       	}
	}

for(loop i=loop_var_5; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskN[i][j].energy) ;
        maskI[i][j].energy = mask0 ;
       	}
	}

}


//*************" loop 6"*******************************

if(brems == 2){

	int loop_var_6=0;


for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_6 += 1;
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskP[i][j].energy) ;
		maskI[i][j].energy = mask0 ;
	}
}

	for(loop i=loop_var_6; i<CRYSTALS_IN_ETA23; i++){
		#pragma HLS UNROLL
		for(loop j=0; j<CRYSTALS_IN_PHI; j++){
			#pragma HLS UNROLL
			ap_uint<1> mask0 = maskI[i][j].energy * ((ap_uint<1>)1 - maskP[i][j].energy) ;
			maskI[i][j].energy = mask0 ;
		}
	}
}


//*************" loop 7"*******************************
int loop_var_7 =0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_7 += 1;
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		ap_uint<10> tmp = ECALCrystals[i][j].energy ;
		ap_uint<10> energy = tmp * maskI[i][j].energy  ;
        ECALCrystals[i][j].energy = energy ;
	}
}

for(loop i=loop_var_7; i<CRYSTALS_IN_ETA23; i++){
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
#pragma HLS latency min=18

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



//************1st loop*********************
int loop_var_1=0;
for(loop i=0; i<1; i++){
	loop_var_1 += 1;
	#pragma HLS UNROLL
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
        extendedcrystals[i+2][j+2].energy = ECALCrystals[i][j].energy ;
	}
}

for(loop i=loop_var_1; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
    for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
        extendedcrystals[i+2][j+2].energy = ECALCrystals[i][j].energy ;
	}
}


//**************2nd loop**************
int loop_var_2=0;

for(loop i=0; i<1; i++){
	#pragma HLS UNROLL
	loop_var_2 += 1;
	if(i == Seed.eta){
		for(loop k=0; k<5; k++){
           #pragma HLS UNROLL
		   ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
           #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
		   	   for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
           	   	   #pragma HLS UNROLL
		   		   extendedcrystals1D[j].energy = extendedcrystals[i+k][j].energy ;
		   	   }
	       getslice(extendedcrystals1D, Seed, CntrSlice[k]) ;
         }
	}
	else { }
}

for(loop i=loop_var_2; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i == Seed.eta){
		for(loop k=0; k<5; k++){
           #pragma HLS UNROLL
		   ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
           #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
		   	   for(loop j=0; j<CRYSTALS_IN_PHI+4; j++){
           	   	   #pragma HLS UNROLL
		   		   extendedcrystals1D[j].energy = extendedcrystals[i+k][j].energy ;
		   	   }
	       getslice(extendedcrystals1D, Seed, CntrSlice[k]) ;
         }
	}
	else { }
}




//***************3rd loop*************

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i == Seed.eta){
		for(loop k=0; k<3; k++){
           #pragma HLS UNROLL
		   ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
           #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
		   for(loop j=0; j<CRYSTALS_IN_PHI-1; j++){
           		#pragma HLS UNROLL
        		extendedcrystals1D[j+5].energy = extendedcrystals[i+k+1][j].energy ;
			}
	        getslice(extendedcrystals1D, Seed, NegSlice[k+1]) ;
		}
	}
	else { }
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	if(i == Seed.eta){
		for(loop k=0; k<3; k++){
		#pragma HLS UNROLL
		ecalcrystal extendedcrystals1D[CRYSTALS_IN_PHI+4] ;
        #pragma HLS ARRAY_PARTITION variable=extendedcrystals1D complete dim=0
			for(loop j=0; j<CRYSTALS_IN_PHI-1; j++){
           		#pragma HLS UNROLL
        		extendedcrystals1D[j].energy = extendedcrystals[i+k+1][j+5].energy ;
			}
	    getslice(extendedcrystals1D, Seed, PosSlice[k+1]) ;
        }
	}
		else { }
}

CntrValue = CntrSlice[1] + CntrSlice[2] + CntrSlice[3] ;
NegValue = NegSlice[1] + NegSlice[2] + NegSlice[3] ;
PosValue = PosSlice[1] + PosSlice[2] + PosSlice[3] ;
s5x5 = CntrSlice[0] + CntrSlice[1] + CntrSlice[2] + CntrSlice[3] + CntrSlice[4] ;
s2x5n = CntrSlice[1] + CntrSlice[2] ;
s2x5p = CntrSlice[2] + CntrSlice[3] ;

ap_uint<12> clusterEnergyDiv8 = CntrValue >> 3;
ap_uint<12> Total = CntrValue ;

if(NegValue > clusterEnergyDiv8 && NegValue > PosValue) {
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

ecalcrystal Seed ;

int loop_var =0;

for(loop k=0; k<1; k++){
	loop_var += 1;
	getseedposition(crystals, Seed) ;
	getcluster(crystals,Seed,ECALClusters[k]) ;
	zerrocrystals(crystals, Seed, ECALClusters[k].brems) ;
}

for(loop k=loop_var; k<N_CLUSTERS; k++){
	getseedposition(crystals, Seed) ;
	getcluster(crystals,Seed,ECALClusters[k]) ;
	zerrocrystals(crystals, Seed, ECALClusters[k].brems) ;
}


}

void createTowers(ecalcrystal crystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI],ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI]) {
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=crystals complete dim=0
#pragma HLS inline

for(loop i=0; i<CRYSTALS_IN_ETA; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
   	   #pragma HLS UNROLL
	   ap_uint<12> energy = ECALTowers[i/5*6+j/5].energy + crystals[i][j].energy  ;
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
ecalcluster ECALClustersH2[N_CLUSTERS] ;
ecalcluster ECALClustersH3[N_CLUSTERS] ;
ecalcrystal ECALCrystals[CRYSTALS_IN_ETA][CRYSTALS_IN_PHI];
ecalcrystal ECALCrystalsH1[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI];
ecalcrystal ECALCrystalsH2[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI];
ecalcrystal ECALCrystalsH3[CRYSTALS_IN_ETA23][CRYSTALS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=ECALClusters complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALClustersH3 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystals complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystalsH1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystalsH2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ECALCrystalsH3 complete dim=0

ecaltower ECALTowers[TOWERS_IN_ETA*TOWERS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=ECALTowers complete dim=0

//creating 25x30 crystals matrix

processInputLinks(link_in, ECALCrystals) ;
          
// 25 crystals in eta are divided into 3 regions
// to create 3 regions, 11 eta each, with 2 eta overlap
//      9+2    -      2+7+2       -     2+9
//  0...8 9.10 - 7.8 9...15 16.17 - 14.15 16...24 
  
for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ECALCrystalsH1[i][j] = ECALCrystals[i][j];
		ECALCrystalsH1[i][j].eta = i;
		ECALCrystalsH1[i][j].phi = j;
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ECALCrystalsH2[i][j] = ECALCrystals[CRYSTALS_IN_ETA23+i-4][j];
		ECALCrystalsH2[i][j].eta = i;
		ECALCrystalsH2[i][j].phi = j;
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		ECALCrystalsH3[i][j] = ECALCrystals[CRYSTALS_IN_ETA23+i+3][j];
		ECALCrystalsH3[i][j].eta = i;
		ECALCrystalsH3[i][j].phi = j;
	}
}

// each region is treated in the same way and later we combine them back in one
//
createClusters1(ECALCrystalsH1, ECALClustersH1);
createClusters1(ECALCrystalsH2, ECALClustersH2);
createClusters1(ECALCrystalsH3, ECALClustersH3);

for(loop i=0; i<N_CLUSTERS; i++){
	#pragma HLS UNROLL
	ap_uint<5> eta = ECALClustersH2[i].eta + 7;
	ECALClustersH2[i].eta = eta ;
}

for(loop i=0; i<N_CLUSTERS; i++){
	#pragma HLS UNROLL
	ap_uint<5> eta = ECALClustersH3[i].eta + 14;
	ECALClustersH3[i].eta = eta ;
}

//  0...8 9.10 - 7.8 9...15 16.17 - 14.15 16...24 
//  clean double counted clusters 
//
for(loop i=0; i<N_CLUSTERS; i++){
	#pragma HLS UNROLL
	for(loop k=0; k<N_CLUSTERS; k++){
    	#pragma HLS UNROLL
		if(ECALClustersH1[i].eta == ECALClustersH2[k].eta && ECALClustersH1[i].phi == ECALClustersH2[k].phi){
		if(ECALClustersH1[i].eta >= 9) { ECALClustersH1[i].energy = 0 ; }
		else {ECALClustersH2[k].energy = 0 ;}
		}
	}
}

for(loop i=0; i<N_CLUSTERS; i++){
	#pragma HLS UNROLL
	for(loop k=0; k<N_CLUSTERS; k++){
    	#pragma HLS UNROLL
		if(ECALClustersH3[i].eta == ECALClustersH2[k].eta && ECALClustersH3[i].phi == ECALClustersH2[k].phi){
			if(ECALClustersH3[i].eta <= 15) { ECALClustersH3[i].energy = 0 ; }
		else {ECALClustersH2[k].energy = 0 ;}
		}
	}
}

// unique regions are combined 
//
for(loop i=0; i<CRYSTALS_IN_ETA23-2; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		ECALCrystals[i][j] = ECALCrystalsH1[i][j] ;
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23-4; i++){
 	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
    	#pragma HLS UNROLL
		ECALCrystals[CRYSTALS_IN_ETA23-2+i][j] = ECALCrystalsH2[i+2][j] ;
	}
}

for(loop i=0; i<CRYSTALS_IN_ETA23-2; i++){
	#pragma HLS UNROLL
	for(loop j=0; j<CRYSTALS_IN_PHI; j++){
		#pragma HLS UNROLL
		ECALCrystals[CRYSTALS_IN_ETA23+5+i][j] = ECALCrystalsH3[i+2][j] ;
	}
}

createTowers(ECALCrystals, ECALTowers) ;

/*---------------------------------link 0------------------------------------*/
        
link_out[0] = 0;
link_out[1] = 0;

processOutputLinks(ECALClustersH1, ECALClustersH2, ECALClustersH3, ECALTowers, link_out);

}
