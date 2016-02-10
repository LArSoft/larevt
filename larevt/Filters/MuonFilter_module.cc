////////////////////////////////////////////////////////////////////////
//
// MuonFilter
//
// This event filter can act to identify events with only through-going tracks
//
// pagebri3@msu.edu
//
////////////////////////////////////////////////////////////////////////

#ifndef MUONFILTER_H
#define MUONFILTER_H

#include "TH2D.h"


extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "TMath.h"

//Framework Includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h" 
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "art/Persistency/Common/Ptr.h" 
#include "art/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 


//Larsoft Includes
#include "RecoBase/Hit.h"
#include "RecoBase/Cluster.h"
#include "Geometry/Geometry.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/WireGeo.h"
#include "Utilities/LArProperties.h"
#include "Utilities/DetectorProperties.h"
#include "Utilities/AssociationUtil.h"
 

namespace filter {

 class MuonFilter : public art::EDFilter  {
    
  public:
    
    explicit MuonFilter(fhicl::ParameterSet const& ); 
    virtual ~MuonFilter();      
    
    bool filter(art::Event& evt);
    void beginJob();
    void endJob();

    void reconfigure(fhicl::ParameterSet const& p);

  private:  
   
  protected:
 
    std::string fClusterModuleLabel;
    std::string fLineModuleLabel;
    std::vector<double>  fCuts; 
    double fDCenter; 
    double fDelay;
    double fTolerance;
    double fMaxIon;
    double fIonFactor;
    int    fDeltaWire; ///< allowed differences in wire number between 2 planes
  
  }; // class MuonFilter

  //-------------------------------------------------
  MuonFilter::MuonFilter(fhicl::ParameterSet const & pset) 
  {
    this->reconfigure(pset);   
  }

  //-------------------------------------------------
  MuonFilter::~MuonFilter()
  {
  }

  //------------------------------------------------
  void MuonFilter::reconfigure(fhicl::ParameterSet const& p)
  {
    fClusterModuleLabel = p.get< std::string         >("ClusterModuleLabel"); 
    fLineModuleLabel    = p.get< std::string  	     >("LineModuleLabel");    
    fTolerance          = p.get< double       	     >("Tolerance");	       
    fDelay              = p.get< double       	     >("Delay");	       
    fDCenter            = p.get< double       	     >("DCenter");	       
    fMaxIon             = p.get< double       	     >("MaxIon");	       
    fIonFactor          = p.get< double       	     >("IonFactor");          
    fCuts               = p.get< std::vector<double> >("Cuts");
    fDeltaWire          = p.get< int                 >("DeltaWire");
  }

  //-------------------------------------------------
  void MuonFilter::beginJob()
  {
  }

  //-------------------------------------------------
  void MuonFilter::endJob()
  {
  }

  //-------------------------------------------------
  bool MuonFilter::filter(art::Event &evt)
  { 
    art::ServiceHandle<geo::Geometry> geom;
    art::ServiceHandle<util::LArProperties> larprop;
    art::ServiceHandle<util::DetectorProperties> detprop;

    //Drift Velocity in cm/us Sampling rate in ns
    double drift = larprop->DriftVelocity(larprop->Efield(), larprop->Temperature())*detprop->SamplingRate()/1000.0; 

    //This code only works comparing 2 planes so for now these are the 
    // last induction plane and collection plane
    int vPlane = geom->Nplanes() - 1;
    geo::View_t vView = geom->Plane(vPlane).View();
    int uPlane = vPlane-1;
    geo::View_t uView = geom->Plane(uPlane).View();
    art::Handle< std::vector< recob::Cluster > > clustHandle;
    evt.getByLabel(fClusterModuleLabel,clustHandle);

    art::FindManyP<recob::Hit> fmh(clustHandle, evt, fClusterModuleLabel);

    art::PtrVector<recob::Cluster> clusters;
    clusters.reserve(clustHandle->size());
    for(unsigned int i = 0; i < clustHandle->size(); ++i) {
      clusters.push_back(art::Ptr<recob::Cluster>(clustHandle,i));
    }
    double indIon(0),colIon(0);
    std::map<int,int> indMap;
    std::map<int,int> colMap;
    std::vector<std::pair<int,int> > rLook;
    int matchNum=1;
    std::vector<std::vector<double> > tGoing;
    std::vector<std::vector<double> > matched;
    std::vector<double> pointTemp(6);
    std::pair<int,int> pairTemp;
    double ionSum(0.0);
    for(size_t cluster = 0; cluster < clusters.size(); ++cluster) {
      ionSum=0.0;
      std::vector< art::Ptr<recob::Hit> > hits = fmh.at(cluster);
      for(unsigned int hit = 0; hit < hits.size(); hit++) {
	ionSum+=hits[hit]->PeakAmplitude();
      }
      if(clusters[cluster]->View() == uView) indIon+=ionSum;
      else if(clusters[cluster]->View() == vView) colIon+=ionSum;
    }
    mf::LogInfo("MuonFilter") << "Ionizations: " << indIon << " " << colIon ;
    art::Handle<std::vector<recob::Cluster > > lines;
    art::PtrVector<recob::Cluster> inductionSegments, collectionSegments;
    evt.getByLabel(fLineModuleLabel,lines);
    art::PtrVector<recob::Cluster> lineVec;
    lineVec.reserve(lines->size());
    for(unsigned int i = 0; i < lines->size(); ++i) {
      lineVec.push_back(art::Ptr<recob::Cluster>(lines,i));
    }

    for(size_t cl = 0;cl < clusters.size(); cl++) {
      std::vector< art::Ptr<recob::Hit> > hits = fmh.at(cl);
      if (hits.size()>0 && clusters[cl]->View()==uView)
	inductionSegments.push_back(clusters[cl]);
      else if(hits.size()>0 && clusters[cl]->View() == vView) collectionSegments.push_back(clusters[cl]);
    } 
    
    art::FindManyP<recob::Hit> fmhi(inductionSegments,  evt, fClusterModuleLabel);
    art::FindManyP<recob::Hit> fmhc(collectionSegments, evt, fClusterModuleLabel);

    if(inductionSegments.size() == 0 || collectionSegments.size() == 0) { 
      mf::LogInfo("MuonFilter") << "At least one plane with no track";
    }
    else {  
      double x1,x2,y1,y2,z1,z2;
      int uPos1,vPos1,uPos2,vPos2;
      std::vector<double> w1Start(3);
      std::vector<double> w1End(3);
      std::vector<double> w2Start(3);
      std::vector<double> w2End(3);

      for(unsigned int i = 0; i < inductionSegments.size(); i++) { 
	if(indMap[i]) continue;
	for(unsigned int j = 0; j < collectionSegments.size(); j++) {
          if(colMap[j]) continue;	

	  art::Ptr<recob::Cluster>  indSeg = inductionSegments[i];
	  art::Ptr<recob::Cluster>  colSeg = collectionSegments[j];

	  std::vector< art::Ptr<recob::Hit> > indHits = fmhi.at(i);
	  
	  std::vector< art::Ptr<recob::Hit> > colHits = fmhc.at(j);
          
	  double trk1Start = indSeg->StartTick()+fDelay;
	  double trk1End = indSeg->EndTick()+fDelay;
	  double trk2Start =colSeg->StartTick();
	  double trk2End =colSeg->EndTick();
	  
	  uPos1 = indSeg->StartWire();
	  uPos2 = indSeg->EndWire(); 
	  vPos1 = colSeg->StartWire();
	  vPos2 = colSeg->EndWire();
	  mf::LogInfo("MuonFilter") << "I J " << i <<" " << j ;
	  mf::LogInfo("MuonFilter") << "Start/end " << indSeg->StartWire() 
				    <<" "<< colSeg->StartWire() 
				    <<" "<< indSeg->EndWire() 
				    <<" "<< colSeg->EndWire() ;
          mf::LogInfo("MuonFilter")<<"U's "<< uPos1 <<" " << uPos2 
				   <<"V's "<< vPos1 <<" " << vPos2 
				   << " times " << trk1End <<" "<< trk2End 
				   <<" "<< trk1Start <<" "<< trk2Start ;
          //need to have the corresponding endpoints matched
          //check if they match in this order else switch
          //really should use the crossing function and then have limits
          //on distance outide tpc, or some other way of dealing with
          //imperfect matches
	  if((TMath::Abs(uPos1-vPos1)>fDeltaWire || TMath::Abs(uPos2-vPos2)>fDeltaWire) &&
	     (TMath::Abs(uPos1-vPos2)<=fDeltaWire && TMath::Abs(uPos2-vPos1)<=fDeltaWire)) {
	    mf::LogInfo("MuonFilter") << "Swapped1" ;
	    std::swap(uPos1,uPos2);
	  }
	  //check for time tolerance 
          if((TMath::Abs(trk1Start-trk2Start) > fTolerance && TMath::Abs(trk1End-trk2End) > fTolerance) &&  
	     (TMath::Abs(trk1Start-trk2End) < fTolerance && TMath::Abs(trk1End-trk2Start) < fTolerance)) {
	    std::swap(trk1Start,trk1End);
	    std::swap(uPos1,uPos2);
            mf::LogInfo("MuonFilter") << "Swapped2" ;
          }
	  mf::LogInfo("MuonFilter") << "Times: " << trk1Start <<" "<< trk2Start <<" "<<trk1End <<" "<<trk2End;
          //again needs to be fixed
	  ///\todo: the delta wire numbers seem a bit magic, 
	  ///\todo: should also change to using Geometry::ChannelsIntersect method
	  ///\todo: should also make this detector agnostic, the following assumes 1 cryostat and 1 tpc
          if((TMath::Abs(trk1Start-trk2Start) < fTolerance && TMath::Abs(trk1End-trk2End) < fTolerance)    && 
	     (TMath::Abs(uPos1-vPos1) <=fDeltaWire+2 && TMath::Abs(uPos2-vPos2) <= fDeltaWire+2))  {
            geom->WireEndPoints(0,0,uPlane,uPos1,&w1Start[0],&w1End[0]);
            geom->WireEndPoints(0,0,vPlane,vPos1,&w2Start[0],&w2End[0]);
            geom->IntersectionPoint(uPos1,vPos1,uPlane,vPlane,0,0,&w1Start[0],&w1End[0],&w2Start[0],&w2End[0],y1,z1);
            geom->WireEndPoints(0,0,uPlane,uPos2,&w1Start[0],&w1End[0]);
            geom->WireEndPoints(0,0,vPlane,vPos2,&w2Start[0],&w2End[0]);
            geom->IntersectionPoint(uPos2,vPos2,uPlane,vPlane,0,0,&w1Start[0],&w1End[0],&w2Start[0],&w2End[0],y2,z2);
	    x1 = (trk1Start+trk2Start)/2.0*drift-fDCenter;
	    x2 = (trk1End+trk2End)/2.0*drift-fDCenter;
	    mf::LogInfo("MuonFilter") <<"Match " << matchNum 
				      <<" " << x1 << " " << y1 << " " << z1 
				      <<" " << x2 << " " << y2 << " " << z2;
	    bool x1edge,x2edge,y1edge, y2edge,z1edge,z2edge;
	    indMap[i]=matchNum;
	    colMap[j]=matchNum;
	    matchNum++;
	    pointTemp[0]=x1;
	    pointTemp[1]=y1;
	    pointTemp[2]=z1;
	    pointTemp[3]=x2;
	    pointTemp[4]=y2;
	    pointTemp[5]=z2;	
	    x1edge =(TMath::Abs(x1) -fCuts[0] > 0);
	    x2edge =(TMath::Abs(x2) -fCuts[0] > 0);
	    y1edge =(TMath::Abs(y1) -fCuts[1] > 0);
	    y2edge =(TMath::Abs(y2) -fCuts[1] > 0);
	    z1edge =(TMath::Abs(z1) -fCuts[2] > 0);
	    z2edge =(TMath::Abs(z2) -fCuts[2] > 0);
	    if((x1edge||y1edge||z1edge) && (x2edge||y2edge||z2edge)) {
	      tGoing.push_back(pointTemp);
	      mf::LogInfo("MuonFilter") << "outside   Removed induction ion: ";          

	      for(size_t h = 0; h < indHits.size(); h++){
		mf::LogInfo("MuonFilter") << indHits[h]->PeakAmplitude() << " ";
		indIon -= indHits[h]->PeakAmplitude();
	      }
	      mf::LogInfo("MuonFilter")  <<"Removed collection ion: ";

	      for(size_t h = 0; h < colHits.size(); h++){ 
		    mf::LogInfo("MuonFilter") << colHits[h]->PeakAmplitude() << " ";
		    colIon -= colHits[h]->PeakAmplitude();
	      }
	      mf::LogInfo("MuonFilter")<<"Ionization outside track I/C: " << indIon << " "<<colIon;	 
	    }
            else if((x1edge || y1edge || z1edge)  && 
		    !(x2edge || y2edge || z2edge) && 
		    (z2-z1)>1.2){
	      tGoing.push_back(pointTemp);
	      mf::LogInfo("MuonFilter") << "stopping   Removed induction ion: ";          
	      for(size_t h = 0; h < indHits.size(); h++){
		mf::LogInfo("MuonFilter") <<indHits[h]->PeakAmplitude() << " ";
		indIon -= indHits[h]->PeakAmplitude();
	      }
	      mf::LogInfo("MuonFilter")  <<"Removed collection ion: ";
	      for(size_t h = 0; h < colHits.size(); h++){ 
		mf::LogInfo("MuonFilter") << colHits[h]->PeakAmplitude()<< " ";
		colIon -= colHits[h]->PeakAmplitude();
	      }
	      mf::LogInfo("MuonFilter") <<"Ionization outside track I/C: " << indIon << " "<<colIon;
	    }
	    else {
	      pairTemp = std::make_pair(i,j);
	      mf::LogInfo("MuonFilter") << "rLook matchnum " <<matchNum << " "<<i << " "<<j ; 
	      rLook.push_back(pairTemp);
	      matched.push_back(pointTemp);  
            }
            break;  //advances i, makes j=0;           	 
	  }
	}
      }
    }
    //after all matches are made, remove deltas
    double distance=0;
    for(unsigned int i = 0; i < tGoing.size(); i++) 
      for(unsigned int j = 0; j < matched.size();j++){
	mf::LogInfo("MuonFilter") << tGoing.size() <<" " << matched.size() << " " << i << " " << j;
	//test if one is contained within the other in the z-direction 
	if((tGoing[i][2] <= matched[j][2]) && 
	   (tGoing[i][5] >= matched[j][5])) { 
	  TVector3 a1(&tGoing[i][0]);
	  TVector3 a2(&tGoing[i][3]);
	  TVector3 b1(&matched[j][0]);
	  distance = TMath::Abs((((a1-a2).Cross((a1-a2).Cross(a1-b1))).Unit()).Dot(a1-b1));
	  mf::LogInfo("MuonFilter") <<"distance "<< distance;
	  if(distance < 6){ 
	    mf::LogInfo("MuonFilter") <<"Removing delta ion "<<rLook.size()<<" "<<rLook[j].first<<" "<<matchNum;
	    std::vector< art::Ptr<recob::Hit> > temp = fmhi.at(rLook[j].first);
	    for(unsigned int h = 0; h < temp.size();h++)	 
	      indIon -= temp[h]->PeakAmplitude();
	    temp = fmhc.at(rLook[j].second);
	    for(unsigned int h = 0; h < temp.size();h++)	   
	      colIon -= temp[h]->PeakAmplitude();   
	  }
	}
      } 
    mf::LogInfo("MuonFilter") <<"indIon "<<indIon*fIonFactor <<" colIon " << colIon ;
    if((indIon*fIonFactor > fMaxIon) && (colIon > fMaxIon)) 
      return true;
    else return false;
  }

  DEFINE_ART_MODULE(MuonFilter)

} //namespace filt

#endif // MUONFILTER_H
