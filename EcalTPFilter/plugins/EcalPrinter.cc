// -*- C++ -*-
//
// Package:    RA1Specific/EcalPrinter
// Class:      EcalPrinter
// 
/**\class EcalPrinter EcalPrinter.cc RA1Specific/EcalPrinter/plugins/EcalPrinter.cc

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
#include "FWCore/Framework/interface/ESHandle.h"

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
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "CalibCalorimetry/EcalTPGTools/interface/EcalTPGScale.h"

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

class EcalPrinter : public edm::EDAnalyzer {
   public:
      explicit EcalPrinter(const edm::ParameterSet&);
      ~EcalPrinter();

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob();

      virtual void beginRun(const edm::Run &run, const edm::EventSetup& iSetup);
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      edm::ESHandle<EcalChannelStatus>  ecalStatus;
      edm::ESHandle<CaloGeometry>       geometry;
      int maskedEcalChannelStatusThreshold_;
      bool print_;
      std::string outPath_;

      std::map<DetId, std::vector<double> > EcalAllDeadChannelsValMap; 
      std::map<DetId, EcalTrigTowerDetId> EcalAllDeadChannelsTTMap;
      edm::ESHandle<EcalTrigTowerConstituentsMap> ttMap_;
      edm::Handle<EcalTrigPrimDigiCollection> pTPDigis;

      const edm::InputTag tpDigiCollection_;
      edm::EDGetTokenT<EcalTrigPrimDigiCollection> tpDigiCollectionToken_;

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
EcalPrinter::EcalPrinter(const edm::ParameterSet& iConfig)
: maskedEcalChannelStatusThreshold_ (iConfig.getParameter<int>("maskedEcalChannelStatusThreshold") ),
print_ (iConfig.getParameter<bool>("makeFile") ),
outPath_ (iConfig.getParameter<std::string>("outPath") ),
tpDigiCollection_ (iConfig.getParameter<edm::InputTag>("tpDigiCollection") ),
tpDigiCollectionToken_(consumes<EcalTrigPrimDigiCollection>(tpDigiCollection_))

{
   //now do what ever initialization is needed

}


EcalPrinter::~EcalPrinter()
{
} 

void
EcalPrinter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;




}

void
EcalPrinter::beginJob(){
};


// ------------ method called once each job just before starting event loop  ------------
void 
EcalPrinter::beginRun(const edm::Run &run, const edm::EventSetup& iSetup)
{
	
	iSetup.get<EcalChannelStatusRcd> ().get(ecalStatus);
	iSetup.get<CaloGeometryRecord>   ().get(geometry);
	iSetup.get<IdealGeometryRecord>().get(ttMap_);

	if( !ecalStatus.isValid() )  throw "Failed to get ECAL channel status!";
	if( !geometry.isValid()   )  throw "Failed to get the geometry!";	

	for (int ieta= -85; ieta <=85; ieta++){
		for (int iphi=0; iphi <= 360; iphi++){
			if(! EBDetId::validDetId( ieta, iphi ) )  continue;

			const EBDetId detid = EBDetId( ieta, iphi, EBDetId::ETAPHIMODE );
			EcalChannelStatus::const_iterator chit = ecalStatus->find( detid );
			int status = ( chit != ecalStatus->end() ) ? chit->getStatusCode() & 0x1F : -1;
			const CaloSubdetectorGeometry*  subGeom = geometry->getSubdetectorGeometry (detid);
			const CaloCellGeometry*        cellGeom = subGeom->getGeometry (detid);
			if(status >= maskedEcalChannelStatusThreshold_){
				double eta = cellGeom->getPosition ().eta ();
				double phi = cellGeom->getPosition ().phi ();
				double theta = cellGeom->getPosition().theta();

				std::vector<double> valVec;

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

	return;

}

void 
EcalPrinter::endJob() 
{
	FILE * f;
	f = fopen(outPath_.c_str(),"w");
	std::map<DetId,EcalTrigTowerDetId>::iterator it;
	std::cout << "TP Map size: " << EcalAllDeadChannelsTTMap.size() << std::endl;
	for (it = EcalAllDeadChannelsTTMap.begin(); it != EcalAllDeadChannelsTTMap.end(); ++it){
		fprintf(f,"%d %d \n",it->second.ieta(), it->second.iphi());
	};
	fclose(f);

}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalPrinter);

