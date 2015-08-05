```shell
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src/
cmsenv
git clone git@github.com:lucien1011/RA1Specific.git
scram b -j 8
cmsRun RA1Specific/EcalTPFilter/test/EcalDeadTowerPrinter_cfg.py
```
