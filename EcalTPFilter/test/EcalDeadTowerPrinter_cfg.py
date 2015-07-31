import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2015B/SingleMuon/AOD/PromptReco-v1/000/251/162/00000/C2C5E84D-4227-E511-8878-02163E01280D.root'
    )
)

from RA1Specific.EcalTPFilter.EcalDeadTowerPrinter_cfi import EcalPrinter
process.makeEcalMap = EcalPrinter 

process.p = cms.Path(process.makeEcalMap)
