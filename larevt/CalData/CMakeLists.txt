simple_plugin(CalWireAna "module"
              lardataobj_RecoBase
              lardata_Utilities_LArFFT_service
              lardata_Utilities
              larcorealg_Geometry
              art::Framework_Services_Registry
              art_root_io::TFileService_service
              art_root_io::tfile_support
	      messagefacility::MF_MessageLogger
              ROOT::Core
              ROOT::Hist
              ROOT::MathCore)

simple_plugin(CalWire "module"
              larcorealg_Geometry
              lardataobj_RawData
              lardata_ArtDataHelper
              art::Framework_Services_Registry
              messagefacility::MF_MessageLogger
              ROOT::Core
              ROOT::Hist
              ROOT::MathCore
              ROOT::RIO)

simple_plugin(CalWireT962 "module"
              larcorealg_Geometry
              lardataobj_RawData
              lardata_ArtDataHelper
              art::Framework_Services_Registry
              messagefacility::MF_MessageLogger
              ROOT::Core
              ROOT::Hist
              ROOT::MathCore
              ROOT::RIO)

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)
