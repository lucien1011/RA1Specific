// -*- C++ -*-
//
// Package:    RA1Specific/EcalTPFilter
// Class:      EcalTPFilter
// 
/**\class EcalTPFilter EcalTPFilter.cc RA1Specific/EcalTPFilter/plugins/EcalTPFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Kin Ho Lo
//         Created:  Fri, 31 Jul 2015 10:28:22 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// user include files
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "Geometry/CaloTopology/interface/EcalTrigTowerConstituentsMap.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

// Geometry
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "Geometry/CaloTopology/interface/CaloTowerConstituentsMap.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"



//
// class declaration
//

class EcalTPFilter : public edm::EDAnalyzer {
   public:
      explicit EcalTPFilter(const edm::ParameterSet&);
      ~EcalTPFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      edm::ESHandle<EcalChannelStatus>  ecalStatus;
      int maskedEcalChannelStatusThreshold_;

      std::map<DetId, std::vector<double> > EcalAllDeadChannelsValMap; 
      std::map<DetId, EcalTrigTowerDetId> EcalAllDeadChannelsTTMap;
      edm::ESHandle<EcalTrigTowerConstituentsMap> ttMap_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalTPFilter::EcalTPFilter(const edm::ParameterSet& iConfig)
: maskedEcalChannelStatusThreshold_ (iConfig.getParameter<int>("maskedEcalChannelStatusThreshold") ),

{
   //now do what ever initialization is needed

}


EcalTPFilter::~EcalTPFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
EcalTPFilter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;




}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalTPFilter::beginJob()
{
	iSetup.get<EcalChannelStatusRcd> ().get(ecalStatus);
	iSetup.get<CaloGeometryRecord>   ().get(geometry);
	iSetup.get<IdealGeometryRecord>().get(ttMap_);

	if( !ecalStatus.isValid() )  throw "Failed to get ECAL channel status!";
	if( !geometry.isValid()   )  throw "Failed to get the geometry!";	

// Loop over EB	
	for (int ieta= -85; ieta <=85; ieta++){
		for (int iphi=0; iphi <= 360; iphi++){
			if(! EBDetId::validDetId( ieta, iphi ) )  continue;

			const EBDetId detid = EBDetId( ieta, iphi, EBDetId::ETAPHIMODE );
			int status = ( chit != ecalStatus->end() ) ? chit->getStatusCode() & 0x1F : -1;
			const CaloSubdetectorGeometry*  subGeom = geometry->getSubdetectorGeometry (detid);
			const CaloCellGeometry*        cellGeom = subGeom->getGeometry (detid);
			if(status >= maskedEcalChannelStatusThreshold_){
				double eta = cellGeom->getPosition ().eta ();
				double phi = cellGeom->getPosition ().phi ();
				double theta = cellGeom->getPosition().theta();

				std::vector<int> valVec;

				valVec.push_back(eta); valVec.push_back(phi); valVec.push_back(theta);
				EcalAllDeadChannelsValMap.insert( std::make_pair(detid, valVec) );
			};
		};
	};

// Loop over EE
	for( int ix=0; ix<=100; ix++ ){
		for( int iy=0; iy<=100; iy++ ){
			for( int iz=-1; iz<=1; iz++ ){
				if(iz==0)  continue;
				if(! EEDetId::validDetId( ix, iy, iz ) )  continue;
				const EEDetId detid = EEDetId( ix, iy, iz, EEDetId::XYMODE );
				EcalChannelStatus::const_iterator chit = ecalStatus->find( detid );
				int status = ( chit != ecalStatus->end() ) ? chit->getStatusCode() & 0x1F : -1;
				const CaloSubdetectorGeometry* subGeom = geometry->getSubdetectorGeometry (detid);
				const CaloCellGeometry* cellGeom = subGeom->getGeometry (detid);
 
				if(status >= maskedEcalChannelStatusThreshold_){
					double eta = cellGeom->getPosition ().eta ();
					double phi = cellGeom->getPosition ().phi ();
					double theta = cellGeom->getPosition().theta();
					std::vector<double> valVec;
					valVec.push_back(eta); valVec.push_back(phi); valVec.push_back(theta);
					EcalAllDeadChannelsValMap.insert( std::make_pair(detid, valVec) );
		            };
		         }; // end loop iz
		      }; // end loop iy
		   }; // end loop ix

	std::map<DetId,std::vector<double>>::iterator itor;
	for (itor = EcalAllDeadChannelsValMap.begin(); itor != EcalAllDeadChannelsValMap.end(); itor++){
		const DetId id = itor -> first;
		EcalTrigTowerDetId ttDetId = ttMap_->towerOf(id);
		EcalAllDeadChannelsTTMap.insert(std::make_pair(id, ttDetId) );
	};

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalTPFilter::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
EcalTPFilter::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
EcalTPFilter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
EcalTPFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
EcalTPFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EcalTPFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalTPFilter);
