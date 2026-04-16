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

tp_handle = Handle(
    "edm::SortedCollection<EcalEBTriggerPrimitiveDigi,edm::StrictWeakOrdering<EcalEBTriggerPrimitiveDigi> >"
)
tp_label = ("simEcalEBTriggerPrimitiveDigis", "", "HLT")

for iev, event in enumerate(events):
    event.getByLabel(tp_label, tp_handle)
    if not tp_handle.isValid():
        print("Handle invalid")
        continue

    prod = tp_handle.product()

    print(f"\n=== Event {iev} ===")
    print("Collection type:")
    print(type(prod))

    print("\nCollection attributes / methods:")
    for name in dir(prod):
        if not name.startswith("__"):
            print(name)

    if len(prod) > 0:
        digi = prod[0]

        print("\nElement type:")
        print(type(digi))

        print("\nElement attributes / methods:")
        for name in dir(digi):
            if not name.startswith("__"):
                print(name)

        print("\nType of digi.id():")
        print(type(digi.id()))
        print("digi.id() =", digi.id())

        print("\nAttributes / methods of digi.id():")
        for name in dir(digi.id()):
            if not name.startswith("__"):
                print(name)

        if digi.size() > 0:
            sample = digi.sample(0)

            print("\nType of digi.sample(0):")
            print(type(sample))

            print("\nAttributes / methods of digi.sample(0):")
            for name in dir(sample):
                if not name.startswith("__"):
                    print(name)

    break