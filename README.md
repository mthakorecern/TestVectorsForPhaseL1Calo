# Test Vectors producer for Emulator and Firmware

This software package currently is capable for performing the following tasks:

1. Take a ```GEN-SIM-DIGI-RAW-MINIAOD``` file corresponding to a particular physics process and process it to retain the desired number of events, object collections, etc.

2. Take the same input corresponding to a particular RCT card, as provided to the RCT emulator and pass it to the FW code, sequentially through the different IPs.

3. Compare the outputs of the emulator and FW code after processing by each IP Stage


## Producing Physics Test Vectors using a MiniAOD file

Note: This part of the codebase needs to be run either in a CMSSW area or can be run anywhere after activating a CMSSW virtual environment (```cmsenv```). Also, a valid grid proxy is required for running this code - ```voms-proxy-init -voms cms -out /tmp/x509up_u10196 -valid 192:00```.



Let's say we want to produce a test vector with 10 events corresponding to a TTToSemileptonic process. Then inside the ```metadata``` folder, we can run the following command

```
python3 TestVectorproducer.py \
    -i root://cms-xrd-global.cern.ch//store/mc/Phase2Spring24DIGIRECOMiniAOD/TTToSemileptonic_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/00000/22b4c974-c204-4495-8ec6-693a845710fd.root  \
    --range \
    --start 0   \
    --count 10   \
    -o TTToSemileptonic_TuneCP5_14TeV_693a845710fd_10events.root
```

Note: One can find MINIAODs for various physics processes using the following query on the CMS Data Aggregration service

```dataset status=* dataset=/*/Phase2Spring24DIGIRECOMiniAOD-PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/GEN-SIM-DIGI-RAW-MINIAOD```

or at the following link: 
https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset%3D%2F*%2FPhase2Spring24DIGIRECOMiniAOD-PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2%2FGEN-SIM-DIGI-RAW-MINIAOD


## Testing the Emulator code

For testing the FW code and to ensure that its outputs align to that of the emulator, we first need to pass the test vector through the emulator. 

To run the emulator, we first need to set up a particular version of CMSSW. 

### Setting up the CMSSW
```
cmssw-el8
cmsrel CMSSW_15_0_0_pre3
cd CMSSW_15_0_0_pre3/src
cmsenv
git cms-init
git cms-addpkg L1Trigger/L1TCalorimeter
git cms-addpkg L1Trigger/L1CaloTrigger
git cms-addpkg DataFormats/L1TCalorimeterPhase2
```

Now we want to use a particular version of the Phase2 L1 Calo RCT emulator code (originally written by Ryan P. Simeon).

```
git remote add mitanshu-cmssw https://github.com/mthakorecern/cmssw.git
git fetch mitanshu-cmssw
git checkout -b singleRCTcard mitanshu-cmssw/singleRCTcard
git pull
```

We will also need to setup an analyzer area to test the emulator (also written by Ryan P. Simeon).
```
cd L1Trigger
git clone https://github.com/rpsimeon34/L1CaloPhase2Analyzer.git -b 15_0_0_pre3_calojet
cd ../
scram b -j 12
```
All of this is required to be done only once.

To get the Inputs/Outputs corresponding to a single RCT card in the form of csv files, run the following command
```
cd L1Trigger/L1CaloPhase2Analyzer/test/
cmsRun test-analyzer.py
```

Note: The input test vector will have to be specified in the ```test-analyzer.py``` script.

The I/O of corresponding to the physics test vector will be stored in the directory ```L1CaloPhase2Analyzer/test/rct_IO/<card>_<cardno.>_<event>_<event.no>```

Each event folder will have the following sub-directories:
- crystals (containing the csv file with input given to the crystals)
- decoded (containing the csv file with outputs corresponding to IP1 SLRs - 0, 1, 2 & 3)
- input (containing sub-directories (IP1, IP21, IP22 and IP3) each containing csv files with inputs given to each of the IPs of the emulator)
- output (containing sub-directories (IP1, IP21, IP22 and IP3) each containing csv files with outputs from each of the IPs of the emulator)

## Testing the FW code
Note: For now, the following part can be run only superbock machine

Now that we have the inputs and outputs from the RCT emulator, we can test the firmware code.

```
git clone https://github.com/mthakorecern/TestVectorsForPhase2L1Calo.git -b RCT_Final
cd TestVectorsForPhase2L1Calo/src/
source setup.sh
```

The FW code can either be tested for each IP separately or together sequentially.

### Sequential testing at once
```
cd TestVectorsForPhase2L1Calo/src/
make rct_fw_tester
```

This will compile all the FW source codes and produce executable binaries that can we use to test the physics test vectors. 

```
./rct_fw_tester \
    <path-to-a-single-RCT-card-IO-from-the-emulator> \
    <FW-Ouput-directory>    \
    <rct-card-no>   \
    <event-number>
```

This will generate generate and store the FW outputs in the specified directory containing the 
- decoded (containing csv files corresponding to the IP1 output)
- output (containing sub-directories (IP1, IP21, IP22 and IP3) each containing csv files with outputs from each of the IPs of the FW)








