#include "algo_top.h"
#include <cstdlib>

int main(){

  srand((unsigned)time(0));

ap_uint<576> link_in[N_INPUT_LINKS] ;
ap_uint<576> link_out[N_OUTPUT_LINKS] ;
ap_uint<16> crystal[5][6][25] = {0} ;
//ap_uint<16> crystal[3][6][25] = {0} ;

        for(loop i=0; i<N_INPUT_LINKS; i=i+6){
          for(loop k=0; k<6; k++){
                for(loop j=0; j<25; j++){
          crystal[i/6][k][j] = 0 ;
          if(i == 0 && k == 0 && j == 0) crystal[0][0][0] = 3;
          if(i == 0 && k == 0 && j == 1) crystal[0][0][1] = 5;
          if(i == 0 && k == 0 && j == 2) crystal[0][0][2] = 8;
          if(i == 0 && k == 0 && j == 3) crystal[0][0][3] = 5;
          if(i == 0 && k == 0 && j == 4) crystal[0][0][4] = 3;
          if(i == 0 && k == 1 && j == 0) crystal[0][1][0] = 2;
          if(i == 0 && k == 1 && j == 1) crystal[0][1][1] = 1;
          if(i == 0 && k == 1 && j == 2) crystal[0][1][2] = 0;
          if(i == 0 && k == 1 && j == 3) crystal[0][1][3] = 1;
          if(i == 0 && k == 1 && j == 4) crystal[0][1][4] = 1;
          if(i == 0 && k == 2 && j == 0) crystal[0][2][0] = 2;
          if(i == 0 && k == 2 && j == 1) crystal[0][2][1] = 1;
          if(i == 0 && k == 2 && j == 2) crystal[0][2][2] = 1;
          if(i == 0 && k == 2 && j == 3) crystal[0][2][3] = 0;
          if(i == 0 && k == 2 && j == 4) crystal[0][2][4] = 1;
          if(i == 0 && k == 3 && j == 0) crystal[0][3][0] = 2;
          if(i == 0 && k == 3 && j == 1) crystal[0][3][1] = 1;
          if(i == 0 && k == 3 && j == 2) crystal[0][3][2] = 0;
          if(i == 0 && k == 3 && j == 3) crystal[0][3][3] = 0;
          if(i == 0 && k == 3 && j == 4) crystal[0][3][4] = 0;
          if(i == 0 && k == 4 && j == 0) crystal[0][4][0] = 0;
          if(i == 0 && k == 4 && j == 1) crystal[0][4][1] = 0;
          if(i == 0 && k == 4 && j == 2) crystal[0][4][2] = 0;
          if(i == 0 && k == 4 && j == 3) crystal[0][4][3] = 1;
          if(i == 0 && k == 4 && j == 4) crystal[0][4][4] = 0;
          if(i == 0 && k == 5 && j == 0) crystal[0][5][0] = 0;
          if(i == 0 && k == 5 && j == 1) crystal[0][5][1] = 0;
          if(i == 0 && k == 5 && j == 2) crystal[0][5][2] = 0;
          if(i == 0 && k == 5 && j == 3) crystal[0][5][3] = 0;
          if(i == 0 && k == 5 && j == 4) crystal[0][5][4] = 0;
          if(i == 0 && k == 0 && j == 5) crystal[0][0][5] = 5;
          if(i == 0 && k == 0 && j == 6) crystal[0][0][6] = 8;
          if(i == 0 && k == 0 && j == 7) crystal[0][0][7] = 14;
          if(i == 0 && k == 0 && j == 8) crystal[0][0][8] = 8;
          if(i == 0 && k == 0 && j == 9) crystal[0][0][9] = 5;
          if(i == 0 && k == 1 && j == 5) crystal[0][1][5] = 3;
          if(i == 0 && k == 1 && j == 6) crystal[0][1][6] = 2;
          if(i == 0 && k == 1 && j == 7) crystal[0][1][7] = 1;
          if(i == 0 && k == 1 && j == 8) crystal[0][1][8] = 1;
          if(i == 0 && k == 1 && j == 9) crystal[0][1][9] = 2;
          if(i == 0 && k == 2 && j == 5) crystal[0][2][5] = 4;
          if(i == 0 && k == 2 && j == 6) crystal[0][2][6] = 2;
          if(i == 0 && k == 2 && j == 7) crystal[0][2][7] = 1;
          if(i == 0 && k == 2 && j == 8) crystal[0][2][8] = 1;
          if(i == 0 && k == 2 && j == 9) crystal[0][2][9] = 2;
          if(i == 0 && k == 3 && j == 5) crystal[0][3][5] = 3;
          if(i == 0 && k == 3 && j == 6) crystal[0][3][6] = 2;
          if(i == 0 && k == 3 && j == 7) crystal[0][3][7] = 1;
          if(i == 0 && k == 3 && j == 8) crystal[0][3][8] = 0;
          if(i == 0 && k == 3 && j == 9) crystal[0][3][9] = 0;
          if(i == 0 && k == 4 && j == 5) crystal[0][4][5] = 0;
          if(i == 0 && k == 4 && j == 6) crystal[0][4][6] = 0;
          if(i == 0 && k == 4 && j == 7) crystal[0][4][7] = 1;
          if(i == 0 && k == 4 && j == 8) crystal[0][4][8] = 2;
          if(i == 0 && k == 4 && j == 9) crystal[0][4][9] = 1;
          if(i == 0 && k == 5 && j == 5) crystal[0][5][5] = 0;
          if(i == 0 && k == 5 && j == 6) crystal[0][5][6] = 0;
          if(i == 0 && k == 5 && j == 7) crystal[0][5][7] = 0;
          if(i == 0 && k == 5 && j == 8) crystal[0][5][8] = 0;
          if(i == 0 && k == 5 && j == 9) crystal[0][5][9] = 0;
          if(i == 0 && k == 0 && j == 10) crystal[0][0][10] = 8;
          if(i == 0 && k == 0 && j == 11) crystal[0][0][11] = 14;
          if(i == 0 && k == 0 && j == 12) crystal[0][0][12] = 22;
          if(i == 0 && k == 0 && j == 13) crystal[0][0][13] = 14;
          if(i == 0 && k == 0 && j == 14) crystal[0][0][14] = 8;
          if(i == 0 && k == 1 && j == 10) crystal[0][1][10] = 5;
          if(i == 0 && k == 1 && j == 11) crystal[0][1][11] = 3;
          if(i == 0 && k == 1 && j == 12) crystal[0][1][12] = 2;
          if(i == 0 && k == 1 && j == 13) crystal[0][1][13] = 2;
          if(i == 0 && k == 1 && j == 14) crystal[0][1][14] = 4;
          if(i == 0 && k == 2 && j == 10) crystal[0][2][10] = 7;
          if(i == 0 && k == 2 && j == 11) crystal[0][2][11] = 4;
          if(i == 0 && k == 2 && j == 12) crystal[0][2][12] = 2;
          if(i == 0 && k == 2 && j == 13) crystal[0][2][13] = 2;
          if(i == 0 && k == 2 && j == 14) crystal[0][2][14] = 3;
          if(i == 0 && k == 3 && j == 10) crystal[0][3][10] = 5;
          if(i == 0 && k == 3 && j == 11) crystal[0][3][11] = 3;
          if(i == 0 && k == 3 && j == 12) crystal[0][3][12] = 2;
          if(i == 0 && k == 3 && j == 13) crystal[0][3][13] = 1;
          if(i == 0 && k == 3 && j == 14) crystal[0][3][14] = 0;
          if(i == 0 && k == 4 && j == 10) crystal[0][4][10] = 0;
          if(i == 0 && k == 4 && j == 11) crystal[0][4][11] = 1;
          if(i == 0 && k == 4 && j == 12) crystal[0][4][12] = 2;
          if(i == 0 && k == 4 && j == 13) crystal[0][4][13] = 3;
          if(i == 0 && k == 4 && j == 14) crystal[0][4][14] = 2;
          if(i == 0 && k == 5 && j == 10) crystal[0][5][10] = 1;
          if(i == 0 && k == 5 && j == 11) crystal[0][5][11] = 0;
          if(i == 0 && k == 5 && j == 12) crystal[0][5][12] = 0;
          if(i == 0 && k == 5 && j == 13) crystal[0][5][13] = 0;
          if(i == 0 && k == 5 && j == 14) crystal[0][5][14] = 0;
          if(i == 0 && k == 0 && j == 15) crystal[0][0][15] = 14;
          if(i == 0 && k == 0 && j == 16) crystal[0][0][16] = 22;
          if(i == 0 && k == 0 && j == 17) crystal[0][0][17] = 37;
          if(i == 0 && k == 0 && j == 18) crystal[0][0][18] = 22;
          if(i == 0 && k == 0 && j == 19) crystal[0][0][19] = 14;
          if(i == 0 && k == 1 && j == 15) crystal[0][1][15] = 8;
          if(i == 0 && k == 1 && j == 16) crystal[0][1][16] = 5;
          if(i == 0 && k == 1 && j == 17) crystal[0][1][17] = 3;
          if(i == 0 && k == 1 && j == 18) crystal[0][1][18] = 4;
          if(i == 0 && k == 1 && j == 19) crystal[0][1][19] = 7;
          if(i == 0 && k == 2 && j == 15) crystal[0][2][15] = 11;
          if(i == 0 && k == 2 && j == 16) crystal[0][2][16] = 7;
          if(i == 0 && k == 2 && j == 17) crystal[0][2][17] = 4;
          if(i == 0 && k == 2 && j == 18) crystal[0][2][18] = 3;
          if(i == 0 && k == 2 && j == 19) crystal[0][2][19] = 5;
          if(i == 0 && k == 3 && j == 15) crystal[0][3][15] = 9;
          if(i == 0 && k == 3 && j == 16) crystal[0][3][16] = 5;
          if(i == 0 && k == 3 && j == 17) crystal[0][3][17] = 3;
          if(i == 0 && k == 3 && j == 18) crystal[0][3][18] = 2;
          if(i == 0 && k == 3 && j == 19) crystal[0][3][19] = 1;
          if(i == 0 && k == 4 && j == 15) crystal[0][4][15] = 1;
          if(i == 0 && k == 4 && j == 16) crystal[0][4][16] = 2;
          if(i == 0 && k == 4 && j == 17) crystal[0][4][17] = 3;
          if(i == 0 && k == 4 && j == 18) crystal[0][4][18] = 5;
          if(i == 0 && k == 4 && j == 19) crystal[0][4][19] = 3;
          if(i == 0 && k == 5 && j == 15) crystal[0][5][15] = 2;
          if(i == 0 && k == 5 && j == 16) crystal[0][5][16] = 1;
          if(i == 0 && k == 5 && j == 17) crystal[0][5][17] = 0;
          if(i == 0 && k == 5 && j == 18) crystal[0][5][18] = 0;
          if(i == 0 && k == 5 && j == 19) crystal[0][5][19] = 0;
          if(i == 0 && k == 0 && j == 20) crystal[0][0][20] = 22;
          if(i == 0 && k == 0 && j == 21) crystal[0][0][21] = 37;
          if(i == 0 && k == 0 && j == 22) crystal[0][0][22] = 60;
          if(i == 0 && k == 0 && j == 23) crystal[0][0][23] = 37;
          if(i == 0 && k == 0 && j == 24) crystal[0][0][24] = 22;
          if(i == 0 && k == 1 && j == 20) crystal[0][1][20] = 14;
          if(i == 0 && k == 1 && j == 21) crystal[0][1][21] = 8;
          if(i == 0 && k == 1 && j == 22) crystal[0][1][22] = 5;
          if(i == 0 && k == 1 && j == 23) crystal[0][1][23] = 7;
          if(i == 0 && k == 1 && j == 24) crystal[0][1][24] = 11;
          if(i == 0 && k == 2 && j == 20) crystal[0][2][20] = 18;
          if(i == 0 && k == 2 && j == 21) crystal[0][2][21] = 11;
          if(i == 0 && k == 2 && j == 22) crystal[0][2][22] = 7;
          if(i == 0 && k == 2 && j == 23) crystal[0][2][23] = 5;
          if(i == 0 && k == 2 && j == 24) crystal[0][2][24] = 9;
          if(i == 0 && k == 3 && j == 20) crystal[0][3][20] = 14;
          if(i == 0 && k == 3 && j == 21) crystal[0][3][21] = 9;
          if(i == 0 && k == 3 && j == 22) crystal[0][3][22] = 5;
          if(i == 0 && k == 3 && j == 23) crystal[0][3][23] = 3;
          if(i == 0 && k == 3 && j == 24) crystal[0][3][24] = 2;
          if(i == 0 && k == 4 && j == 20) crystal[0][4][20] = 2;
          if(i == 0 && k == 4 && j == 21) crystal[0][4][21] = 3;
          if(i == 0 && k == 4 && j == 22) crystal[0][4][22] = 5;
          if(i == 0 && k == 4 && j == 23) crystal[0][4][23] = 9;
          if(i == 0 && k == 4 && j == 24) crystal[0][4][24] = 5;
          if(i == 0 && k == 5 && j == 20) crystal[0][5][20] = 3;
          if(i == 0 && k == 5 && j == 21) crystal[0][5][21] = 2;
          if(i == 0 && k == 5 && j == 22) crystal[0][5][22] = 1;
          if(i == 0 && k == 5 && j == 23) crystal[0][5][23] = 0;
          if(i == 0 && k == 5 && j == 24) crystal[0][5][24] = 0;
          if(i == 6 && k == 0 && j == 0) crystal[1][0][0] = 14;
          if(i == 6 && k == 0 && j == 1) crystal[1][0][1] = 22;
          if(i == 6 && k == 0 && j == 2) crystal[1][0][2] = 37;
          if(i == 6 && k == 0 && j == 3) crystal[1][0][3] = 22;
          if(i == 6 && k == 0 && j == 4) crystal[1][0][4] = 14;
          if(i == 6 && k == 1 && j == 0) crystal[1][1][0] = 8;
          if(i == 6 && k == 1 && j == 1) crystal[1][1][1] = 5;
          if(i == 6 && k == 1 && j == 2) crystal[1][1][2] = 7;
          if(i == 6 && k == 1 && j == 3) crystal[1][1][3] = 11;
          if(i == 6 && k == 1 && j == 4) crystal[1][1][4] = 18;
          if(i == 6 && k == 2 && j == 0) crystal[1][2][0] = 30;
          if(i == 6 && k == 2 && j == 1) crystal[1][2][1] = 18;
          if(i == 6 && k == 2 && j == 2) crystal[1][2][2] = 11;
          if(i == 6 && k == 2 && j == 3) crystal[1][2][3] = 9;
          if(i == 6 && k == 2 && j == 4) crystal[1][2][4] = 14;
          if(i == 6 && k == 3 && j == 0) crystal[1][3][0] = 23;
          if(i == 6 && k == 3 && j == 1) crystal[1][3][1] = 14;
          if(i == 6 && k == 3 && j == 2) crystal[1][3][2] = 9;
          if(i == 6 && k == 3 && j == 3) crystal[1][3][3] = 5;
          if(i == 6 && k == 3 && j == 4) crystal[1][3][4] = 3;
          if(i == 6 && k == 4 && j == 0) crystal[1][4][0] = 3;
          if(i == 6 && k == 4 && j == 1) crystal[1][4][1] = 5;
          if(i == 6 && k == 4 && j == 2) crystal[1][4][2] = 9;
          if(i == 6 && k == 4 && j == 3) crystal[1][4][3] = 15;
          if(i == 6 && k == 4 && j == 4) crystal[1][4][4] = 9;
          if(i == 6 && k == 5 && j == 0) crystal[1][5][0] = 5;
          if(i == 6 && k == 5 && j == 1) crystal[1][5][1] = 3;
          if(i == 6 && k == 5 && j == 2) crystal[1][5][2] = 2;
          if(i == 6 && k == 5 && j == 3) crystal[1][5][3] = 1;
          if(i == 6 && k == 5 && j == 4) crystal[1][5][4] = 0;
          if(i == 6 && k == 0 && j == 5) crystal[1][0][5] = 8;
          if(i == 6 && k == 0 && j == 6) crystal[1][0][6] = 14;
          if(i == 6 && k == 0 && j == 7) crystal[1][0][7] = 22;
          if(i == 6 && k == 0 && j == 8) crystal[1][0][8] = 14;
          if(i == 6 && k == 0 && j == 9) crystal[1][0][9] = 8;
          if(i == 6 && k == 1 && j == 5) crystal[1][1][5] = 5;
          if(i == 6 && k == 1 && j == 6) crystal[1][1][6] = 7;
          if(i == 6 && k == 1 && j == 7) crystal[1][1][7] = 11;
          if(i == 6 && k == 1 && j == 8) crystal[1][1][8] = 18;
          if(i == 6 && k == 1 && j == 9) crystal[1][1][9] = 30;
          if(i == 6 && k == 2 && j == 5) crystal[1][2][5] = 49;
          if(i == 6 && k == 2 && j == 6) crystal[1][2][6] = 30;
          if(i == 6 && k == 2 && j == 7) crystal[1][2][7] = 18;
          if(i == 6 && k == 2 && j == 8) crystal[1][2][8] = 14;
          if(i == 6 && k == 2 && j == 9) crystal[1][2][9] = 23;
          if(i == 6 && k == 3 && j == 5) crystal[1][3][5] = 38;
          if(i == 6 && k == 3 && j == 6) crystal[1][3][6] = 23;
          if(i == 6 && k == 3 && j == 7) crystal[1][3][7] = 14;
          if(i == 6 && k == 3 && j == 8) crystal[1][3][8] = 9;
          if(i == 6 && k == 3 && j == 9) crystal[1][3][9] = 5;
          if(i == 6 && k == 4 && j == 5) crystal[1][4][5] = 5;
          if(i == 6 && k == 4 && j == 6) crystal[1][4][6] = 9;
          if(i == 6 && k == 4 && j == 7) crystal[1][4][7] = 15;
          if(i == 6 && k == 4 && j == 8) crystal[1][4][8] = 24;
          if(i == 6 && k == 4 && j == 9) crystal[1][4][9] = 15;
          if(i == 6 && k == 5 && j == 5) crystal[1][5][5] = 9;
          if(i == 6 && k == 5 && j == 6) crystal[1][5][6] = 5;
          if(i == 6 && k == 5 && j == 7) crystal[1][5][7] = 3;
          if(i == 6 && k == 5 && j == 8) crystal[1][5][8] = 2;
          if(i == 6 && k == 5 && j == 9) crystal[1][5][9] = 1;
          if(i == 6 && k == 0 && j == 10) crystal[1][0][10] = 5;
          if(i == 6 && k == 0 && j == 11) crystal[1][0][11] = 8;
          if(i == 6 && k == 0 && j == 12) crystal[1][0][12] = 14;
          if(i == 6 && k == 0 && j == 13) crystal[1][0][13] = 8;
          if(i == 6 && k == 0 && j == 14) crystal[1][0][14] = 5;
          if(i == 6 && k == 1 && j == 10) crystal[1][1][10] = 7;
          if(i == 6 && k == 1 && j == 11) crystal[1][1][11] = 11;
          if(i == 6 && k == 1 && j == 12) crystal[1][1][12] = 18;
          if(i == 6 && k == 1 && j == 13) crystal[1][1][13] = 30;
          if(i == 6 && k == 1 && j == 14) crystal[1][1][14] = 49;
          if(i == 6 && k == 2 && j == 10) crystal[1][2][10] = 80;
          if(i == 6 && k == 2 && j == 11) crystal[1][2][11] = 49;
          if(i == 6 && k == 2 && j == 12) crystal[1][2][12] = 30;
          if(i == 6 && k == 2 && j == 13) crystal[1][2][13] = 23;
          if(i == 6 && k == 2 && j == 14) crystal[1][2][14] = 38;
          if(i == 6 && k == 3 && j == 10) crystal[1][3][10] = 61;
          if(i == 6 && k == 3 && j == 11) crystal[1][3][11] = 38;
          if(i == 6 && k == 3 && j == 12) crystal[1][3][12] = 23;
          if(i == 6 && k == 3 && j == 13) crystal[1][3][13] = 14;
          if(i == 6 && k == 3 && j == 14) crystal[1][3][14] = 9;
          if(i == 6 && k == 4 && j == 10) crystal[1][4][10] = 9;
          if(i == 6 && k == 4 && j == 11) crystal[1][4][11] = 15;
          if(i == 6 && k == 4 && j == 12) crystal[1][4][12] = 24;
          if(i == 6 && k == 4 && j == 13) crystal[1][4][13] = 40;
          if(i == 6 && k == 4 && j == 14) crystal[1][4][14] = 24;
          if(i == 6 && k == 5 && j == 10) crystal[1][5][10] = 15;
          if(i == 6 && k == 5 && j == 11) crystal[1][5][11] = 9;
          if(i == 6 && k == 5 && j == 12) crystal[1][5][12] = 5;
          if(i == 6 && k == 5 && j == 13) crystal[1][5][13] = 3;
          if(i == 6 && k == 5 && j == 14) crystal[1][5][14] = 2;
          if(i == 6 && k == 0 && j == 15) crystal[1][0][15] = 3;
          if(i == 6 && k == 0 && j == 16) crystal[1][0][16] = 5;
          if(i == 6 && k == 0 && j == 17) crystal[1][0][17] = 8;
          if(i == 6 && k == 0 && j == 18) crystal[1][0][18] = 5;
          if(i == 6 && k == 0 && j == 19) crystal[1][0][19] = 3;
          if(i == 6 && k == 1 && j == 15) crystal[1][1][15] = 4;
          if(i == 6 && k == 1 && j == 16) crystal[1][1][16] = 7;
          if(i == 6 && k == 1 && j == 17) crystal[1][1][17] = 11;
          if(i == 6 && k == 1 && j == 18) crystal[1][1][18] = 18;
          if(i == 6 && k == 1 && j == 19) crystal[1][1][19] = 30;
          if(i == 6 && k == 2 && j == 15) crystal[1][2][15] = 49;
          if(i == 6 && k == 2 && j == 16) crystal[1][2][16] = 30;
          if(i == 6 && k == 2 && j == 17) crystal[1][2][17] = 23;
          if(i == 6 && k == 2 && j == 18) crystal[1][2][18] = 38;
          if(i == 6 && k == 2 && j == 19) crystal[1][2][19] = 61;
          if(i == 6 && k == 3 && j == 15) crystal[1][3][15] = 100;
          if(i == 6 && k == 3 && j == 16) crystal[1][3][16] = 61;
          if(i == 6 && k == 3 && j == 17) crystal[1][3][17] = 38;
          if(i == 6 && k == 3 && j == 18) crystal[1][3][18] = 23;
          if(i == 6 && k == 3 && j == 19) crystal[1][3][19] = 14;
          if(i == 6 && k == 4 && j == 15) crystal[1][4][15] = 9;
          if(i == 6 && k == 4 && j == 16) crystal[1][4][16] = 9;
          if(i == 6 && k == 4 && j == 17) crystal[1][4][17] = 15;
          if(i == 6 && k == 4 && j == 18) crystal[1][4][18] = 24;
          if(i == 6 && k == 4 && j == 19) crystal[1][4][19] = 15;
          if(i == 6 && k == 5 && j == 15) crystal[1][5][15] = 9;
          if(i == 6 && k == 5 && j == 16) crystal[1][5][16] = 5;
          if(i == 6 && k == 5 && j == 17) crystal[1][5][17] = 3;
          if(i == 6 && k == 5 && j == 18) crystal[1][5][18] = 2;
          if(i == 6 && k == 5 && j == 19) crystal[1][5][19] = 1;
          if(i == 6 && k == 0 && j == 20) crystal[1][0][20] = 2;
          if(i == 6 && k == 0 && j == 21) crystal[1][0][21] = 3;
          if(i == 6 && k == 0 && j == 22) crystal[1][0][22] = 5;
          if(i == 6 && k == 0 && j == 23) crystal[1][0][23] = 3;
          if(i == 6 && k == 0 && j == 24) crystal[1][0][24] = 2;
          if(i == 6 && k == 1 && j == 20) crystal[1][1][20] = 2;
          if(i == 6 && k == 1 && j == 21) crystal[1][1][21] = 4;
          if(i == 6 && k == 1 && j == 22) crystal[1][1][22] = 7;
          if(i == 6 && k == 1 && j == 23) crystal[1][1][23] = 11;
          if(i == 6 && k == 1 && j == 24) crystal[1][1][24] = 18;
          if(i == 6 && k == 2 && j == 20) crystal[1][2][20] = 30;
          if(i == 6 && k == 2 && j == 21) crystal[1][2][21] = 18;
          if(i == 6 && k == 2 && j == 22) crystal[1][2][22] = 14;
          if(i == 6 && k == 2 && j == 23) crystal[1][2][23] = 23;
          if(i == 6 && k == 2 && j == 24) crystal[1][2][24] = 38;
          if(i == 6 && k == 3 && j == 20) crystal[1][3][20] = 61;
          if(i == 6 && k == 3 && j == 21) crystal[1][3][21] = 38;
          if(i == 6 && k == 3 && j == 22) crystal[1][3][22] = 23;
          if(i == 6 && k == 3 && j == 23) crystal[1][3][23] = 14;
          if(i == 6 && k == 3 && j == 24) crystal[1][3][24] = 9;
          if(i == 6 && k == 4 && j == 20) crystal[1][4][20] = 5;
          if(i == 6 && k == 4 && j == 21) crystal[1][4][21] = 5;
          if(i == 6 && k == 4 && j == 22) crystal[1][4][22] = 9;
          if(i == 6 && k == 4 && j == 23) crystal[1][4][23] = 15;
          if(i == 6 && k == 4 && j == 24) crystal[1][4][24] = 9;
          if(i == 6 && k == 5 && j == 20) crystal[1][5][20] = 5;
          if(i == 6 && k == 5 && j == 21) crystal[1][5][21] = 3;
          if(i == 6 && k == 5 && j == 22) crystal[1][5][22] = 2;
          if(i == 6 && k == 5 && j == 23) crystal[1][5][23] = 1;
          if(i == 6 && k == 5 && j == 24) crystal[1][5][24] = 0;

        }}}


        for(loop j=0; j<N_INPUT_LINKS; j=j+6){
        for(loop i=0; i<6; i++){
        ap_uint<10> start = 0;
        ap_uint<10> end = 571;
          link_in[i+j].range(end, start) = 0 ;
          for(loop k=0; k<25; k++){
          start = k*16;
          link_in[i+j].range(start+15, start) = crystal[j/6][i][k] ;
        }}}

        for(int i=0; i<N_INPUT_LINKS; i++){
                cout << hex << "link_in[" << i << "]: " << link_in[i] << endl;
        }

        cout << endl;

        algo_top(link_in, link_out);

        for(int i=0; i<N_OUTPUT_LINKS; i++){
                cout << hex << "link_out[" << i << "]: " << link_out[i] << endl;
        }

        ecalcluster cluster;

        for(loop i = 0; i <9; i++){
        ap_uint<10> start = i*64;
        ap_uint<10> end = start+63;
        cluster.fillecalcluster(((ap_uint<64>) link_out[0].range(end, start))) ;
        cout << " cluster Seed " << cluster.seedEnergy
         << " energy " << cluster.energy
         << " eta " << cluster.eta
         << " phi " << cluster.phi << endl ;
        }

        for(loop i = 0; i <30; i++){
        ap_uint<10> start = i*18;
        ap_uint<10> end = start + 17;
        cout << " tower " << i << " energy " <<  link_out[1].range(end, start) << endl ;
        }

  return 0;
}
