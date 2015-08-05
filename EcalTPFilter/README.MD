This package prints out ieta vs iphi map of the trigger tower with dead/masked ECAL cells.
Recipe as follows:

```shell
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src/
cmsenv
git clone git@github.com:lucien1011/RA1Specific.git
scram b -j 8
cmsRun RA1Specific/EcalTPFilter/test/EcalDeadTowerPrinter_cfg.py
```
Works on CMSSW only.
