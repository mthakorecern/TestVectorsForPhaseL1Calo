
voms-proxy-init -voms cms -out /tmp/x509up_u10196 -valid 192:00

python3 TestVectorproducer.py \
    -i root://cms-xrd-global.cern.ch//store/mc/Phase2Spring24DIGIRECOMiniAOD/TTToSemileptonic_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/00000/22b4c974-c204-4495-8ec6-693a845710fd.root  \
    --range \
    --start 0   \
    --count 10   \
    -o TTToSemileptonic_TuneCP5_14TeV_693a845710fd_10events.root  \
    &> TTToSemileptonic_TuneCP5_14TeV_693a845710fd_10events.log &
