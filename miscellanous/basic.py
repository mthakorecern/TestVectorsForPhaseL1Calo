# #!/usr/bin/env python3

# import ROOT
# from DataFormats.FWLite import Events, Handle

# ROOT.gSystem.Load("libFWCoreFWLite.so")
# ROOT.gSystem.Load("libDataFormatsFWLite.so")
# ROOT.gSystem.Load("libSimDataFormatsCaloHit.so")
# ROOT.gSystem.Load("libDataFormatsEcalDetId.so")
# ROOT.FWLiteEnabler.enable()

# events = Events("root://cms-xrd-global.cern.ch///store/mc/Phase2Spring24DIGIRECOMiniAOD/TTToSemileptonic_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/00000/22b4c974-c204-4495-8ec6-693a845710fd.root")
# eb_handle = Handle("std::vector<PCaloHit>")
# eb_label = ("g4SimHits", "EcalHitsEB")

# for iev, event in enumerate(events):
#     event.getByLabel(eb_label, eb_handle)
#     if not eb_handle.isValid():
#         print("Handle invalid")
#         continue

#     eb_hits = eb_handle.product()
#     print(f"\n=== Event {iev} ===")
#     print(f"n EB hits = {len(eb_hits)}")

#     for ihit, hit in enumerate(eb_hits):
#         detid = ROOT.EBDetId(hit.id())
#         print(
#             f"hit {ihit:4d} | "
#             f"ieta = {detid.ieta():4d} | "
#             f"iphi = {detid.iphi():4d} | "
#             f"energy = {hit.energy():12.6g} | "
#             f"time = {hit.time():12.6g}"
#         )
#     # break

#!/usr/bin/env python3

import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gSystem.Load("libFWCoreFWLite.so")
ROOT.gSystem.Load("libDataFormatsFWLite.so")
ROOT.gSystem.Load("libDataFormatsEcalDigi.so")
ROOT.gSystem.Load("libDataFormatsEcalDetId.so")
ROOT.FWLiteEnabler.enable()

events = Events(
    "root://cms-xrd-global.cern.ch///store/mc/Phase2Spring24DIGIRECOMiniAOD/"
    "TTToSemileptonic_TuneCP5_14TeV-powheg-pythia8/"
    "GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/"
    "00000/22b4c974-c204-4495-8ec6-693a845710fd.root"
)

# ECAL barrel trigger primitive digis
tp_handle = Handle("EcalEBTrigPrimDigiCollection")
tp_label  = ("simEcalEBTriggerPrimitiveDigis", "", "DIGI")

for iev, event in enumerate(events):
    event.getByLabel(tp_label, tp_handle)
    if not tp_handle.isValid():
        print(f"\n=== Event {iev} ===")
        print("Handle invalid for simEcalEBTriggerPrimitiveDigis")
        continue

    tp_digis = tp_handle.product()
    print(f"\n=== Event {iev} ===")
    print(f"n EB trigger primitive digis = {len(tp_digis)}")

    for itp, tp in enumerate(tp_digis):
        tower_id = ROOT.EcalTrigTowerDetId(tp.id())

        print(
            f"tp {itp:4d} | "
            f"ieta = {tower_id.ieta():4d} | "
            f"iphi = {tower_id.iphi():4d} | "
            f"rawId = {tower_id.rawId():10d} | "
            f"SOI = {tp.sampleOfInterest():2d} | "
            f"compressedEt = {tp.compressedEt():4d} | "
            f"fineGrain = {int(tp.fineGrain()):1d}"
        )

        # Print all time samples in this TP digi
        nsamples = tp.size()
        for isample in range(nsamples):
            s = tp.sample(isample)
            print(
                f"           sample {isample:2d} | "
                f"compressedEt = {s.compressedEt():4d} | "
                f"fineGrain = {int(s.fineGrain()):1d}"
            )

    # break