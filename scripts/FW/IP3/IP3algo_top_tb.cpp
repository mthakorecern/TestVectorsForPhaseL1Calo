#include "IP3algo_top.h"
#include <cstdlib>

int main(){

ap_uint<576> link_in[N_INPUT_LINKS] ;
ap_uint<576> link_out[N_OUTPUT_LINKS] ;

for(int i=0; i<N_INPUT_LINKS; i++){
	link_in[i] = 0;
}

link_in[0]="0x04001C21FED1DC6438001C22ADCC406418001C22A14C406428001C22A78C406424001A124A48944614001A124408944634001A1250889446080018405FC8143C38001840EF47AC3C";
link_in[1]="0x0000000000005400AC002A4001900170000A00010004F001AC002800210005C00050006400128001900170000A0003C00160018C0004000040000001680003000040000000000000";
link_in[2]="0x0000000000005400AC002A4001900170000A00010004F001AC002800210005C00050006400128001900170000A0003C00160018C0004000040000001680003000040000000000000";
link_in[3]="0x0000000000005400AC002A4001900170000A00010004F001AC002800210005C00050006400128001900170000A0003C00160018C0004000040000001680003000040000000000000";
link_in[4]="0x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001800150001A40000001780091000280033001240071000780082";
link_in[5]  = 0;
link_in[6]  = 0;
link_in[7]  = 0;
link_in[8]  = 0;



for(int i=0; i<N_INPUT_LINKS; i++){
	cout << hex << "link_in[" << i << "]: " << link_in[i] << endl;
}

cout << endl;

algo_top(link_in, link_out);

for(int i=0; i<N_OUTPUT_LINKS; i++){
	cout << hex << "link_out[" << i << "]: " << link_out[i] << endl;
}

cout << endl;

/*        rctecalcluster rctcluster;

for(loop i = 0; i <9; i++){
	ap_uint<10> start = i*64;
    ap_uint<10> end = start+63;
    rctcluster.fillrctecalcluster(((ap_uint<64>) link_out[0].range(end, start))) ;
    cout << " cluster Seed " << rctcluster.seedEnergy << " energy " << rctcluster.energy
         << " eta " << rctcluster.eta
         << " phi " << rctcluster.phi << endl ;
}*/

return 0;
}
