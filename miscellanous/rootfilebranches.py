import ROOT

file_name = "root://cms-xrd-global.cern.ch///store/mc/Phase2Spring24DIGIRECOMiniAOD/TTToSemileptonic_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/00000/22b4c974-c204-4495-8ec6-693a845710fd.root"

f = ROOT.TFile.Open(file_name)
if not f or f.IsZombie():
    raise RuntimeError(f"Could not open file: {file_name}")

tree = f.Get("Events")
if not tree:
    raise RuntimeError("Could not find tree 'Events'")

branches = tree.GetListOfBranches()

for branch in branches:
    bname = branch.GetName()
    class_name = branch.GetClassName()   # for object branches, e.g. vector<float>, edm::Wrapper<...>
    title = branch.GetTitle()            # often contains the leaflist/type info

    leaves = branch.GetListOfLeaves()

    if class_name:
        print(f"Branch: {bname}")
        print(f"  Object/Class type: {class_name}")
        print(f"  Title: {title}")
    elif leaves and leaves.GetEntries() > 0:
        print(f"Branch: {bname}")
        for leaf in leaves:
            leaf_name = leaf.GetName()
            leaf_type = leaf.GetTypeName()
            print(f"  Leaf: {leaf_name}  Type: {leaf_type}")
        print(f"  Title: {title}")
    else:
        print(f"Branch: {bname}")
        print(f"  Type info not directly available")
        print(f"  Title: {title}")

f.Close()