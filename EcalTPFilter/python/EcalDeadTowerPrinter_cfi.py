import FWCore.ParameterSet.Config as cms

EcalPrinter = cms.EDAnalyzer('EcalTPFilter'
		maskedEcalChannelStatusThreshold = cms.int32( 1 ),

)
