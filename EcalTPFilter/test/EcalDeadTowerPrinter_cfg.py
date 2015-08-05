import FWCore.ParameterSet.Config as cms

process = cms.Process("RA1")

process.load('Configuration.StandardSequences.Services_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2015B/SingleMuon/AOD/PromptReco-v1/000/251/162/00000/C2C5E84D-4227-E511-8878-02163E01280D.root'
    	# "/store/data/Run2015B/SingleMuon/RECO/PromptReco-v1/000/252/126/00000/D878D71D-4231-E511-9699-02163E011AE0.root"
	)
)

# from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
# process.GlobalTag = GlobalTag(process.GlobalTag, '74X_dataRun2_Prompt_v1', '')
process.GlobalTag.globaltag = "74X_dataRun2_Prompt_v1"

from RA1Specific.EcalTPFilter.EcalDeadTowerPrinter_cfi import EcalPrinter
process.makeEcalMap = EcalPrinter 

process.p = cms.Path(process.makeEcalMap)
