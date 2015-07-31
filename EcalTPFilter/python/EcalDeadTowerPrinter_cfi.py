import FWCore.ParameterSet.Config as cms

EcalPrinter = cms.EDAnalyzer('EcalPrinter',
		maskedEcalChannelStatusThreshold = cms.int32( 1 ),
		makeFile = cms.bool(True),
		outPath = cms.string("EcalEtaPhiMap.txt"),
		tpDigiCollection = cms.InputTag("ecalTPSkimNA"),

)
