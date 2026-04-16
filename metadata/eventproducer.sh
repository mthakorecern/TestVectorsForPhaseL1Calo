
voms-proxy-init -voms cms -out /tmp/x509up_u10196 -valid 192:00

python3 TestVectorproducer.py \
    -i root://cmsxrootd.hep.wisc.edu//store/user/rsimeon/MCFiles/001ebf5f-b83c-43fc-997f-c2e5ecf1f9dd.root  \
    --range \
    --start 0   \
    --count 10   \
    -o OutputFile_10events.root  \
    &> Producer_10events.log &
