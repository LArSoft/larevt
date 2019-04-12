////////////////////////////////////////////////////////////////////////
//
// ADCFilter class:
// Algorithm to ignore events with no ADC values 
// above user-defined threshold.
//
// msoderbe@syr.edu
//
////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <stdint.h>

//Framework Includes
#include "fhiclcpp/ParameterSet.h" 
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h" 
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Principal/View.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 

//Larsoft Includes
#include "lardataobj/RawData/raw.h"
#include "lardataobj/RawData/RawDigit.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"


namespace filter {

   class ADCFilter : public art::EDFilter  {
    
   public:
    
      explicit ADCFilter(fhicl::ParameterSet const& ); 
    
   private:
      bool filter(art::Event& evt);
      void reconfigure(fhicl::ParameterSet const& p);
 
      std::string fDigitModuleLabel;
      double      fMinADC;  
   }; // class ADCFilter

   //-------------------------------------------------
   ADCFilter::ADCFilter(fhicl::ParameterSet const & pset)  
     : EDFilter{pset}
   {   
      this->reconfigure(pset);
   }

   //-------------------------------------------------
   void ADCFilter::reconfigure(fhicl::ParameterSet const& p)
   {
      fDigitModuleLabel = p.get< std::string > ("DigitModuleLabel"); 
      fMinADC           = p.get< double      > ("MinADC");         
   } 

   //-------------------------------------------------
   bool ADCFilter::filter(art::Event &evt)
   { 
      //Read in raw data
      art::View<raw::RawDigit> rawdigitView;
      evt.getView(fDigitModuleLabel, rawdigitView);
      
      if(!rawdigitView.size()) return false;
      
      lariov::ChannelStatusProvider const& channelFilter
        = art::ServiceHandle<lariov::ChannelStatusService const>()->GetProvider();

      // look through the good channels
//      for(const raw::RawDigit* digit: filter::SelectGoodChannels(rawdigitView))
      for(const raw::RawDigit* digit: rawdigitView)
      {
         if (!channelFilter.IsGood(digit->Channel())) continue;
         //get ADC values after decompressing
         std::vector<short> rawadc(digit->Samples());
         raw::Uncompress(digit->ADCs(),rawadc,digit->Compression());
         short max = *std::max_element(rawadc.begin(),rawadc.end()) - digit->GetPedestal();
         if(max>=fMinADC) return true;//found one ADC value above threshold, pass filter  
      }

      return false;//didn't find ADC above threshold

   }

   DEFINE_ART_MODULE(ADCFilter)

} //namespace filter
