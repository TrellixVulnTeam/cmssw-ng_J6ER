
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h"
#include "tdrstyle.C"
#include "TRandom3.h"
#include "TProfile.h"
#include "TDirectory.h"


class SiStripCluster;
namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}


#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/ProjectedSiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"

using namespace fwlite;
using namespace reco;
using namespace susybsm;
using namespace std;
using namespace edm;
using namespace trigger;
#endif


#include "Analysis_Global.h"
#include "Analysis_CommonFunction.h"
#include "Analysis_PlotFunction.h"
#include "Analysis_PlotStructure.h"
#include "Analysis_Samples.h"


std::vector<stSignal> signals;
std::vector<stMC>     MCsample;
std::vector<string>   DataFileName;
double CutMass;

double CutPt;
double CutI;
double CutTOF;

bool shapeSelection(const std::vector<unsigned char> & ampls)
{
  // ----------------  COMPTAGE DU NOMBRE DE MAXIMA   --------------------------
  //----------------------------------------------------------------------------
//	printf("ShapeTest \n");
	 Int_t NofMax=0; Int_t recur255=1; Int_t recur254=1;
	 bool MaxOnStart=false;bool MaxInMiddle=false, MaxOnEnd =false;
	 Int_t MaxPos=0;
	// D??but avec max
      	 if(ampls.size()!=1 && ((ampls[0]>ampls[1])
	    || (ampls.size()>2 && ampls[0]==ampls[1] && ampls[1]>ampls[2] && ampls[0]!=254 && ampls[0]!=255) 
	    || (ampls.size()==2 && ampls[0]==ampls[1] && ampls[0]!=254 && ampls[0]!=255)) ){
 	  NofMax=NofMax+1;  MaxOnStart=true;  }

	// Maximum entour??
         if(ampls.size()>2){
          for (unsigned int i =1; i < ampls.size()-1; i++) {
                if( (ampls[i]>ampls[i-1] && ampls[i]>ampls[i+1]) 
		    || (ampls.size()>3 && i>0 && i<ampls.size()-2 && ampls[i]==ampls[i+1] && ampls[i]>ampls[i-1] && ampls[i]>ampls[i+2] && ampls[i]!=254 && ampls[i]!=255) ){ 
		 NofMax=NofMax+1; MaxInMiddle=true;  MaxPos=i; 
		}
		if(ampls[i]==255 && ampls[i]==ampls[i-1]) {
			recur255=recur255+1;
			MaxPos=i-(recur255/2);
		 	if(ampls[i]>ampls[i+1]){NofMax=NofMax+1;MaxInMiddle=true;}
		}
		if(ampls[i]==254 && ampls[i]==ampls[i-1]) {
			recur254=recur254+1;
			MaxPos=i-(recur254/2);
		 	if(ampls[i]>ampls[i+1]){NofMax=NofMax+1;MaxInMiddle=true;}
		}
          }
	 }
	// Fin avec un max
         if(ampls.size()>1){
          if(ampls[ampls.size()-1]>ampls[ampls.size()-2]
	     || (ampls.size()>2 && ampls[ampls.size()-1]==ampls[ampls.size()-2] && ampls[ampls.size()-2]>ampls[ampls.size()-3] ) 
	     ||  ampls[ampls.size()-1]==255){
	   NofMax=NofMax+1;  MaxOnEnd=true;   }
         }
	// Si une seule strip touch??e
	if(ampls.size()==1){	NofMax=1;}

  // ---  SELECTION EN FONCTION DE LA FORME POUR LES MAXIMA UNIQUES ---------
  //------------------------------------------------------------------------
  /*
               ____
              |    |____
          ____|    |    |
         |    |    |    |____
     ____|    |    |    |    |
    |    |    |    |    |    |____
  __|____|____|____|____|____|____|__
    C_Mnn C_Mn C_M  C_D  C_Dn C_Dnn
  */
//   bool shapetest=true;
   bool shapecdtn=false;

//	Float_t C_M;	Float_t C_D;	Float_t C_Mn;	Float_t C_Dn;	Float_t C_Mnn;	Float_t C_Dnn;
	Float_t C_M=0.0;	Float_t C_D=0.0;	Float_t C_Mn=10000;	Float_t C_Dn=10000;	Float_t C_Mnn=10000;	Float_t C_Dnn=10000;
	Int_t CDPos;
	Float_t coeff1=1.7;	Float_t coeff2=2.0;
	Float_t coeffn=0.10;	Float_t coeffnn=0.02; Float_t noise=4.0;

        if(NofMax==1){

                if(MaxOnStart==true){
                        C_M=(Float_t)ampls[0]; C_D=(Float_t)ampls[1];
                                if(ampls.size()<3) shapecdtn=true ;
                                else if(ampls.size()==3){C_Dn=(Float_t)ampls[2] ; if(C_Dn<=coeff1*coeffn*C_D+coeff2*coeffnn*C_M+2*noise || C_D==255) shapecdtn=true;}
                                else if(ampls.size()>3){ C_Dn=(Float_t)ampls[2];  C_Dnn=(Float_t)ampls[3] ;
                                                        if((C_Dn<=coeff1*coeffn*C_D+coeff2*coeffnn*C_M+2*noise || C_D==255)
                                                           && C_Dnn<=coeff1*coeffn*C_Dn+coeff2*coeffnn*C_D+2*noise){
                                                         shapecdtn=true;}
                                }
                }

                if(MaxOnEnd==true){
                        C_M=(Float_t)ampls[ampls.size()-1]; C_D=(Float_t)ampls[ampls.size()-2];
                                if(ampls.size()<3) shapecdtn=true ;
                                else if(ampls.size()==3){C_Dn=(Float_t)ampls[0] ; if(C_Dn<=coeff1*coeffn*C_D+coeff2*coeffnn*C_M+2*noise || C_D==255) shapecdtn=true;}
                                else if(ampls.size()>3){C_Dn=(Float_t)ampls[ampls.size()-3] ; C_Dnn=(Float_t)ampls[ampls.size()-4] ;
                                                        if((C_Dn<=coeff1*coeffn*C_D+coeff2*coeffnn*C_M+2*noise || C_D==255)
                                                           && C_Dnn<=coeff1*coeffn*C_Dn+coeff2*coeffnn*C_D+2*noise){
                                                         shapecdtn=true;}
                                }
                }
                if(MaxInMiddle==true){
                        C_M=(Float_t)ampls[MaxPos];
                        int LeftOfMaxPos=MaxPos-1;if(LeftOfMaxPos<=0)LeftOfMaxPos=0;
                        int RightOfMaxPos=MaxPos+1;if(RightOfMaxPos>=(int)ampls.size())RightOfMaxPos=ampls.size()-1;
                        //int after = RightOfMaxPos; int before = LeftOfMaxPos; if (after>=(int)ampls.size() ||  before<0)  std::cout<<"invalid read MaxPos:"<<MaxPos <<"size:"<<ampls.size() <<std::endl; 
                        if(ampls[LeftOfMaxPos]<ampls[RightOfMaxPos]){ C_D=(Float_t)ampls[RightOfMaxPos]; C_Mn=(Float_t)ampls[LeftOfMaxPos];CDPos=RightOfMaxPos;} else{ C_D=(Float_t)ampls[LeftOfMaxPos]; C_Mn=(Float_t)ampls[RightOfMaxPos];CDPos=LeftOfMaxPos;}
                        if(C_Mn<coeff1*coeffn*C_M+coeff2*coeffnn*C_D+2*noise || C_M==255){
                                if(ampls.size()==3) shapecdtn=true ;
                                else if(ampls.size()>3){
                                        if(CDPos>MaxPos){
                                                if(ampls.size()-CDPos-1==0){
                                                        C_Dn=0; C_Dnn=0;
                                                }
                                                if(ampls.size()-CDPos-1==1){
                                                        C_Dn=(Float_t)ampls[CDPos+1];
                                                        C_Dnn=0;
                                                }
                                                if(ampls.size()-CDPos-1>1){
                                                        C_Dn=(Float_t)ampls[CDPos+1];
                                                        C_Dnn=(Float_t)ampls[CDPos+2];
                                                }
                                                if(MaxPos>=2){
                                                        C_Mnn=(Float_t)ampls[MaxPos-2];
                                                }
                                                else if(MaxPos<2) C_Mnn=0;
                                        }
                                        if(CDPos<MaxPos){
                                                if(CDPos==0){
                                                        C_Dn=0; C_Dnn=0;
                                                }
                                                if(CDPos==1){
                                                        C_Dn=(Float_t)ampls[0];
                                                        C_Dnn=0;
                                                }
                                                if(CDPos>1){
                                                        C_Dn=(Float_t)ampls[CDPos-1];
                                                        C_Dnn=(Float_t)ampls[CDPos-2];
                                                }
                                                if(ampls.size()-LeftOfMaxPos>1 && MaxPos+2<(int)(ampls.size())-1){
                                                        C_Mnn=(Float_t)ampls[MaxPos+2];
                                                }else C_Mnn=0;
                                        }
                                        if((C_Dn<=coeff1*coeffn*C_D+coeff2*coeffnn*C_M+2*noise || C_D==255)
                                           && C_Mnn<=coeff1*coeffn*C_Mn+coeff2*coeffnn*C_M+2*noise
                                           && C_Dnn<=coeff1*coeffn*C_Dn+coeff2*coeffnn*C_D+2*noise) {
                                                shapecdtn=true;
                                        }

                                }
                        }
                }
        }
	if(ampls.size()==1){shapecdtn=true;}

   return shapecdtn;
} 


void printCluster(FILE* pFile, const SiStripCluster*   Cluster)
{
//        const vector<uint8_t>&  Ampls       = Cluster->amplitudes();
        const vector<unsigned char>&  Ampls       = Cluster->amplitudes();
        uint32_t                DetId       = Cluster->geographicalId();

        int Charge=0;
        for(unsigned int i=0;i<Ampls.size();i++){Charge+=Ampls[i];}

        fprintf(pFile,"DetId = %7i --> %4i = %3i ",DetId,Charge,Ampls[0]);
        for(unsigned int i=1;i<Ampls.size();i++){
           fprintf(pFile,"%3i ",Ampls[i]);
        }
        bool isGood = true;
        if(!shapeSelection(Ampls)){isGood=false;}
           if(!isGood)fprintf(pFile,"   X");
           fprintf(pFile,"\n");
}


void DumpCandidateInfo(const susybsm::HSCParticle& hscp, const fwlite::ChainEvent& ev, FILE* pFile)
{
   reco::MuonRef  muon  = hscp.muonRef();
   reco::TrackRef track = hscp.trackRef();
   if(track.isNull())return;

   fwlite::Handle< std::vector<reco::Vertex> > vertexCollHandle;
   vertexCollHandle.getByLabel(ev,"offlinePrimaryVertices");
   if(!vertexCollHandle.isValid()){printf("Vertex Collection NotFound\n");return;}
   std::vector<reco::Vertex> vertexColl = *vertexCollHandle;
   if(vertexColl.size()<1){printf("NO VERTEX\n"); return;}
   const reco::Vertex& vertex = vertexColl[0];

   fwlite::Handle<DeDxDataValueMap> dEdxSCollH;
   dEdxSCollH.getByLabel(ev, dEdxS_Label.c_str());
   if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");return;}
   DeDxData dedxSObj  = dEdxSCollH->get(track.key());

   fwlite::Handle<DeDxDataValueMap> dEdxMCollH;
   dEdxMCollH.getByLabel(ev, dEdxM_Label.c_str());
   if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");return;}
   DeDxData dedxMObj  = dEdxMCollH->get(track.key());

   fwlite::Handle<DeDxDataValueMap> dEdxMNPCollH;
   dEdxMNPCollH.getByLabel(ev, "dedxNPHarm2");
   if(!dEdxMNPCollH.isValid()){printf("Invalid dEdx Mass collection\n");return;}
   DeDxData dedxMNPObj  = dEdxMNPCollH->get(track.key());

   fwlite::Handle<MuonTimeExtraMap> TOFDTCollH;
   TOFDTCollH.getByLabel(ev, "muontiming","dt");
   if(!TOFDTCollH.isValid()){printf("Invalid TOF DT collection\n");return;}

   fwlite::Handle<MuonTimeExtraMap> TOFCSCCollH;
   TOFCSCCollH.getByLabel(ev, "muontiming","csc");
   if(!TOFDTCollH.isValid()){printf("Invalid TOF CSC collection\n");return;}

   fwlite::Handle<MuonTimeExtraMap> TOFCombCollH;
   TOFCombCollH.getByLabel(ev, "muontiming","combined");
   if(!TOFCombCollH.isValid()){printf("Invalid TOF Combined collection\n");return;}
   const reco::MuonTimeExtra* tof = NULL;
   if(!hscp.muonRef().isNull()){ tof  = &TOFCombCollH->get(hscp.muonRef().key()); }


   if(track->pt()<=CutPt || dedxSObj.dEdx()<=CutI)return;
   if(CutTOF>-1 && tof && tof->inverseBeta()<=CutTOF)return;

   double Mass = GetMass(track->p(),dedxMObj.dEdx());   
   if(Mass<CutMass)return;

   double dz  = track->dz (vertex.position());
   double dxy = track->dxy(vertex.position());
   for(unsigned int i=1;i<vertexColl.size();i++){
      if(fabs(track->dz (vertexColl[i].position())) < fabs(dz) ){
         dz  = track->dz (vertexColl[i].position());
         dxy = track->dxy(vertexColl[i].position());
      }
   }
   double v3d = sqrt(dz*dz+dxy*dxy);

   fprintf(pFile,"\n");
   fprintf(pFile,"---------------------------------------------------------------------------------------------------\n");
   fprintf(pFile,"Candidate Type = %i --> Mass : %7.2f\n",hscp.type(),Mass);
   fprintf(pFile,"------------------------------------------ EVENT INFO ---------------------------------------------\n");
   fprintf(pFile,"Run=%i Lumi=%i Event=%i BX=%i  Orbit=%i Store=%i\n",ev.eventAuxiliary().run(),ev.eventAuxiliary().luminosityBlock(),ev.eventAuxiliary().event(),ev.eventAuxiliary().luminosityBlock(),ev.eventAuxiliary().orbitNumber(),ev.eventAuxiliary().storeNumber());
   //edm::TriggerResultsByName tr = ev.triggerResultsByName("Merge");
   //fprintf(pFile,"Trigger: SingleMu=%i  DoubleMu=%i  PFMHT=%i (CaloMET=%i)\n",(int)tr.accept(tr.triggerIndex("HscpPathSingleMu")), (int)tr.accept(tr.triggerIndex("HscpPathDoubleMu")), (int)tr.accept(tr.triggerIndex("HscpPathPFMet")), (int)tr.accept(tr.triggerIndex("HscpPathCaloMet")));
   fprintf(pFile,"------------------------------------------ INNER TRACKER ------------------------------------------\n");
   fprintf(pFile,"Quality = %i Chi2/NDF=%6.2f dz=+%6.2f dxy=%+6.2f V3D=%+6.2f charge:%+i\n",track->qualityMask(), track->chi2()/track->ndof(), dz, dxy, v3d, track->charge());
   fprintf(pFile,"P=%7.2f  Pt=%7.2f+-%6.2f (Cut=%6.2f) Eta=%+6.2f  Phi=%+6.2f  NOH=%2i\n",track->p(),track->pt(), track->ptError(), CutPt, track->eta(), track->phi(), track->found() );

   fprintf(pFile,"------------------------------------------ DEDX INFO ----------------------------------------------\n");
   fprintf(pFile,"dEdx for selection     :%6.2f (Cut=%6.2f) NOM %2i NOS %2i\n",dedxSObj.dEdx(),CutI,dedxSObj.numberOfMeasurements(),dedxSObj.numberOfSaturatedMeasurements());
   fprintf(pFile,"dEdx for mass reco     :%6.2f             NOM %2i NOS %2i  --> Beta dEdx = %6.2f\n",dedxMObj.dEdx(),dedxMObj.numberOfMeasurements(),dedxMObj.numberOfSaturatedMeasurements(), GetIBeta(dedxMObj.dEdx()) );
   fprintf(pFile,"dEdx for mass reco (NP):%6.2f             NOM %2i NOS %2i  --> Beta dEdx = %6.2f\n",dedxMNPObj.dEdx(),dedxMNPObj.numberOfMeasurements(),dedxMNPObj.numberOfSaturatedMeasurements(), GetIBeta(dedxMNPObj.dEdx()) );

   fprintf(pFile,"dEdx mass error     :%6.2f (1Sigma dEdx) or %6.2f (1Sigma P)\n",  GetMass(track->p(),0.95*dedxMObj.dEdx()),  GetMass(track->p()*(1-track->ptError()/track->pt()),dedxMObj.dEdx()) );

   for(unsigned int h=0;h<track->recHitsSize();h++){
        TrackingRecHit* recHit = (track->recHit(h))->clone();
        if(const SiStripMatchedRecHit2D* matchedHit=dynamic_cast<const SiStripMatchedRecHit2D*>(recHit)){
           fprintf(pFile,"Mono  Hit "); printCluster(pFile,(matchedHit->monoHit()->cluster()).get());
           fprintf(pFile,"StereoHit ");printCluster(pFile,(matchedHit->stereoHit()->cluster()).get());
       }else if(const SiStripRecHit2D* singleHit=dynamic_cast<const SiStripRecHit2D*>(recHit)){
           fprintf(pFile,"2D    Hit ");printCluster(pFile,(singleHit->cluster()).get());
       }else if(const SiStripRecHit1D* single1DHit=dynamic_cast<const SiStripRecHit1D*>(recHit)){
           fprintf(pFile,"1D    Hit ");printCluster(pFile,(single1DHit->cluster()).get());
       }else if(const SiPixelRecHit* pixelHit=dynamic_cast<const SiPixelRecHit*>(recHit)){
           fprintf(pFile,"Pixel Hit  --> Charge = %i\n",(int)pixelHit->cluster()->charge());
      }
   }

   if(!muon.isNull()){
      fprintf(pFile,"------------------------------------------ MUON INFO ----------------------------------------------\n");
      MuonTimeExtra tofDT      = TOFDTCollH->get(hscp.muonRef().key());
      MuonTimeExtra tofCSC      = TOFCSCCollH->get(hscp.muonRef().key());
      MuonTimeExtra tofComb      = TOFCombCollH->get(hscp.muonRef().key());

      fprintf(pFile,"MassTOF = %7.2fGeV\n",GetTOFMass(track->p(),tofComb.inverseBeta()));

      fprintf(pFile,"Quality=%i type=%i P=%7.2f  Pt=%7.2f Eta=%+6.2f Phi=%+6.2f #Chambers=%i\n" ,muon->isQualityValid(),muon->type(),muon->p(),muon->pt(),muon->eta(),muon->phi(),muon->numberOfChambers());
      fprintf(pFile,"muonTimeDT      : NDOF=%2i InvBeta=%6.2f+-%6.2f (Cut=%6.2f) --> beta=%6.2f FreeInvBeta=%6.2f+-%6.2f\n",tofDT  .nDof(),tofDT  .inverseBeta(), tofDT  .inverseBetaErr(), CutTOF, (1.0/tofDT  .inverseBeta()), tofDT  .freeInverseBeta(),tofDT  .freeInverseBetaErr());
      fprintf(pFile,"muonTimeCSC     : NDOF=%2i InvBeta=%6.2f+-%6.2f (Cut=%6.2f) --> beta=%6.2f FreeInvBeta=%6.2f+-%6.2f\n",tofCSC .nDof(),tofCSC .inverseBeta(), tofCSC .inverseBetaErr(), CutTOF, (1.0/tofCSC .inverseBeta()), tofCSC .freeInverseBeta(),tofCSC .freeInverseBetaErr());
      fprintf(pFile,"muonTimeCombined: NDOF=%2i InvBeta=%6.2f+-%6.2f (Cut=%6.2f) --> beta=%6.2f FreeInvBeta=%6.2f+-%6.2f\n",tofComb.nDof(),tofComb.inverseBeta(), tofComb.inverseBetaErr(), CutTOF, (1.0/tofComb.inverseBeta()), tofComb.freeInverseBeta(),tofComb.freeInverseBetaErr());
   }
   if(hscp.hasRpcInfo()){
      fprintf(pFile,"------------------------------------------ RPC INFO -----------------------------------------------\n");
      fprintf(pFile,"isCandidate %i Beta=%6.2f\n",hscp.rpc().isCandidate,hscp.rpc().beta);
   }
   if(hscp.hasCaloInfo() && hscp.caloInfoRef()->ecalTime!=-9999){
      fprintf(pFile,"------------------------------------------ CALO INFO ----------------------------------------------\n");
      fprintf(pFile,"HCAL: E=%6.2f E3x3=%6.2f E5x5=%6.2f HO E=%6.2f\n",hscp.caloInfoRef()->hcalCrossedEnergy,hscp.caloInfoRef()->hcal3by3dir, hscp.caloInfoRef()->hcal5by5dir, hscp.caloInfoRef()->hoCrossedEnergy);
      fprintf(pFile,"ECAL: E=%6.2f E3x3=%6.2f E5x5=%6.2f\n"           ,hscp.caloInfoRef()->ecalCrossedEnergy,hscp.caloInfoRef()->ecal3by3dir, hscp.caloInfoRef()->ecal5by5dir);
      fprintf(pFile,"ECAL: time=%6.2f beta=%6.2f trkisodr=%6.2f\n"    ,hscp.caloInfoRef()->ecalTime  ,hscp.caloInfoRef()->ecalBeta   , hscp.caloInfoRef()->trkIsoDr);
   }

   fprintf(pFile,"------------------------------------------ ISOL INFO ----------------------------------------------\n");
   fwlite::Handle<HSCPIsolationValueMap> IsolationH05;
   IsolationH05.getByLabel(ev, "HSCPIsolation05");
   if(!IsolationH05.isValid()){printf("Invalid IsolationH\n");return;}
   const ValueMap<HSCPIsolation>& IsolationMap05 = *IsolationH05.product();

   fwlite::Handle<HSCPIsolationValueMap> IsolationH03;
   IsolationH03.getByLabel(ev, "HSCPIsolation03");
   if(!IsolationH03.isValid()){printf("Invalid IsolationH\n");return;}
   const ValueMap<HSCPIsolation>& IsolationMap03 = *IsolationH03.product();

   fwlite::Handle<HSCPIsolationValueMap> IsolationH01;
   IsolationH01.getByLabel(ev, "HSCPIsolation01");
   if(!IsolationH01.isValid()){printf("Invalid IsolationH\n");return;}
   const ValueMap<HSCPIsolation>& IsolationMap01 = *IsolationH01.product();

   HSCPIsolation hscpIso05 = IsolationMap05.get((size_t)track.key());
   HSCPIsolation hscpIso03 = IsolationMap03.get((size_t)track.key());
   HSCPIsolation hscpIso01 = IsolationMap01.get((size_t)track.key());
   fprintf(pFile,"Isolation05 --> TkCount=%6.2f TkSumEt=%6.2f EcalE/P=%6.2f HcalE/P=%6.2f --> E/P=%6.2f\n",hscpIso05.Get_TK_Count(), hscpIso05.Get_TK_SumEt(), hscpIso05.Get_ECAL_Energy()/track->p(), hscpIso05.Get_HCAL_Energy()/track->p(), (hscpIso05.Get_ECAL_Energy()+hscpIso05.Get_HCAL_Energy())/track->p());
   fprintf(pFile,"Isolation03 --> TkCount=%6.2f TkSumEt=%6.2f EcalE/P=%6.2f HcalE/P=%6.2f --> E/P=%6.2f\n",hscpIso03.Get_TK_Count(), hscpIso03.Get_TK_SumEt(), hscpIso03.Get_ECAL_Energy()/track->p(), hscpIso03.Get_HCAL_Energy()/track->p(), (hscpIso03.Get_ECAL_Energy()+hscpIso03.Get_HCAL_Energy())/track->p());
   fprintf(pFile,"Isolation01 --> TkCount=%6.2f TkSumEt=%6.2f EcalE/P=%6.2f HcalE/P=%6.2f --> E/P=%6.2f\n",hscpIso01.Get_TK_Count(), hscpIso01.Get_TK_SumEt(), hscpIso01.Get_ECAL_Energy()/track->p(), hscpIso01.Get_HCAL_Energy()/track->p(), (hscpIso01.Get_ECAL_Energy()+hscpIso01.Get_HCAL_Energy())/track->p());
   fprintf(pFile,"\n");
}

bool PassTrigger(const fwlite::ChainEvent& ev)
{
      fwlite::Handle<edm::TriggerResults> hTriggerResults;
      hTriggerResults.getByLabel(ev, "TriggerResults", "", "Merge");
      if(!hTriggerResults.isValid()) return false;

      edm::TriggerResultsByName tr = ev.triggerResultsByName(*hTriggerResults);
      if(!tr.isValid())return false;
      if(tr.accept(tr.triggerIndex("HscpPathMu")))return true;
      if(tr.accept(tr.triggerIndex("HscpPathMet")))return true;
      return false;
}


void DumpInfo(string Pattern, int CutIndex=0, double MassMin=0)
{
   CutMass = MassMin;


   setTDRStyle();
   gStyle->SetPadTopMargin   (0.05);
   gStyle->SetPadBottomMargin(0.10);
   gStyle->SetPadRightMargin (0.18);
   gStyle->SetPadLeftMargin  (0.13);
   gStyle->SetTitleSize(0.04, "XYZ");
   gStyle->SetTitleXOffset(1.1);
   gStyle->SetTitleYOffset(1.35);
   gStyle->SetPalette(1);
   gStyle->SetNdivisions(505);
   TH1::AddDirectory(kTRUE);


   GetSignalDefinition(signals);
   GetMCDefinition(MCsample);
   GetInputFiles(DataFileName, "Data");

   TFile* InputFile      = new TFile((Pattern + "/Histos_Data.root").c_str());
   TH1D*  HCuts_Pt       = (TH1D*)GetObjectFromPath(InputFile, "HCuts_Pt");
   TH1D*  HCuts_I        = (TH1D*)GetObjectFromPath(InputFile, "HCuts_I");
   TH1D*  HCuts_TOF      = (TH1D*)GetObjectFromPath(InputFile, "HCuts_TOF");
   TH1D*  H_A            = (TH1D*)GetObjectFromPath(InputFile, "H_A");
   TH1D*  H_B            = (TH1D*)GetObjectFromPath(InputFile, "H_B");
   TH1D*  H_C            = (TH1D*)GetObjectFromPath(InputFile, "H_C");
   TH1D*  H_D            = (TH1D*)GetObjectFromPath(InputFile, "H_D");
   TH1D*  H_E            = (TH1D*)GetObjectFromPath(InputFile, "H_E");
   TH1D*  H_F            = (TH1D*)GetObjectFromPath(InputFile, "H_F");
   TH1D*  H_G            = (TH1D*)GetObjectFromPath(InputFile, "H_G");
   TH1D*  H_H            = (TH1D*)GetObjectFromPath(InputFile, "H_H");
   TH1D*  H_P            = (TH1D*)GetObjectFromPath(InputFile, "H_P");
   CutPt  = HCuts_Pt ->GetBinContent(CutIndex+1);
   CutI   = HCuts_I  ->GetBinContent(CutIndex+1);
   CutTOF = HCuts_TOF->GetBinContent(CutIndex+1);



   TTree* tree           = (TTree*)GetObjectFromPath(InputFile, "Data/HscpCandidates");
   printf("Tree Entries=%lli\n",tree->GetEntries());

   fwlite::ChainEvent ev(DataFileName);


   unsigned int    Run, Event, HscpI;
   float  Pt,  I,    TOF;

   tree->SetBranchAddress("Run"  ,&Run);
   tree->SetBranchAddress("Event",&Event);
   tree->SetBranchAddress("Hscp" ,&HscpI);
   tree->SetBranchAddress("Pt"   ,&Pt);
   tree->SetBranchAddress("I"    ,&I);
   tree->SetBranchAddress("TOF"  ,&TOF);

   FILE* pFile = fopen("DumpInfo.txt","w");
   fprintf(pFile, "A = %6.2E\n",H_A->GetBinContent(CutIndex+1));
   fprintf(pFile, "B = %6.2E\n",H_B->GetBinContent(CutIndex+1));
   fprintf(pFile, "C = %6.2E\n",H_C->GetBinContent(CutIndex+1));
   fprintf(pFile, "D = %6.2E\n",H_D->GetBinContent(CutIndex+1));
   fprintf(pFile, "E = %6.2E\n",H_E->GetBinContent(CutIndex+1));
   fprintf(pFile, "F = %6.2E\n",H_F->GetBinContent(CutIndex+1));
   fprintf(pFile, "G = %6.2E\n",H_G->GetBinContent(CutIndex+1));
   fprintf(pFile, "H = %6.2E\n",H_H->GetBinContent(CutIndex+1));
   fprintf(pFile, "OBSERVED  EVENTS = %6.2E\n",H_D->GetBinContent(CutIndex+1));
   fprintf(pFile, "PREDICTED EVENTS = %6.2E+-%6.2E\n",H_P->GetBinContent(CutIndex+1), H_P->GetBinError(CutIndex+1));


   printf("Progressing Bar              :0%%       20%%       40%%       60%%       80%%       100%%\n");
   printf("Scanning D                   :");
   int TreeStep = tree->GetEntries()/50;if(TreeStep==0)TreeStep=1;
   for (Int_t i=0;i<tree->GetEntries();i++){
      if(i%TreeStep==0){printf(".");fflush(stdout);}
      tree->GetEntry(i);
//      printf("%6i %9i %1i  %6.2f %6.2f %6.2f\n",Run,Event,HscpI,Pt,I,TOF);

      if(Pt<=CutPt || I<=CutI || (CutTOF>-1 && TOF<=CutTOF))continue;

      ev.to(Run, Event);
      fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
      hscpCollHandle.getByLabel(ev,"HSCParticleProducer");
      if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
      const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

      susybsm::HSCParticle hscp  = hscpColl[HscpI];
      DumpCandidateInfo(hscp, ev, pFile);

   }printf("\n");
   fclose(pFile);





/*
   fwlite::ChainEvent treeD(DataFileName);
   SetWeight(-1);
   printf("Progressing Bar              :0%%       20%%       40%%       60%%       80%%       100%%\n");
   printf("Scanning D                   :");
   TreeStep = treeD.size()/50;if(TreeStep==0)TreeStep=1;

   for(Long64_t ientry=0;ientry<treeD.size();ientry++){
      treeD.to(ientry);
      if(MaxEntry>0 && ientry>MaxEntry)break;
      if(ientry%TreeStep==0){printf(".");fflush(stdout);}

      DataPlots.TotalE->Fill(0.0,Event_Weight);  
      if(!PassTrigger(treeD) )continue;
      DataPlots.TotalTE->Fill(0.0,Event_Weight);

      fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
      hscpCollHandle.getByLabel(treeD,"HSCParticleProducer");
      if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
      const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

      fwlite::Handle<DeDxDataValueMap> dEdxSCollH;
      dEdxSCollH.getByLabel(treeD, dEdxS_Label.c_str());
      if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

      fwlite::Handle<DeDxDataValueMap> dEdxMCollH;
      dEdxMCollH.getByLabel(treeD, dEdxM_Label.c_str());
      if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

      fwlite::Handle<MuonTimeExtraMap> TOFCollH;
      TOFCollH.getByLabel(treeD, "muontiming",TOF_Label.c_str());
      if(!TOFCollH.isValid()){printf("Invalid TOF collection\n");return;}
      
      bool* HSCPTk = new bool[CutPt.size()]; for(unsigned int CutIndex=0;CutIndex<CutPt.size();CutIndex++){  HSCPTk[CutIndex] = false;   }
      for(unsigned int c=0;c<hscpColl.size();c++){
         susybsm::HSCParticle hscp  = hscpColl[c];
         reco::MuonRef  muon  = hscp.muonRef();
         reco::TrackRef track = hscp.trackRef();
         if(track.isNull())continue;

         const DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
         const DeDxData& dedxMObj  = dEdxMCollH->get(track.key());
         const reco::MuonTimeExtra* tof = NULL;
        if(TypeMode==2 && !hscp.muonRef().isNull()){ tof  = &TOFCollH->get(hscp.muonRef().key()); }


         double MuonTOF = GlobalMinTOF;
         if(tof){MuonTOF = tof->inverseBeta(); }
         if(track->pt()>40 && Mass>75)stPlots_FillTree(DataPlots, treeD.eventAuxiliary().run(),treeD.eventAuxiliary().event(), c, track->pt(), dedxSObj.dEdx(), tof ? tof->inverseBeta() : -1);
      } // end of Track Loop
      for(unsigned int CutIndex=0;CutIndex<CutPt.size();CutIndex++){  if(HSCPTk[CutIndex]){DataPlots.HSCPE->Fill(CutIndex,Event_Weight); }  }
   }// end of Event Loop
   //stPlots_CloseTree(DataPlots);
   printf("\n");
*/
}
