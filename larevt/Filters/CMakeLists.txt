include(lar::CalibrationDBIServiceBuilders)

cet_make_library(
  SOURCE
  SimpleChannelStatus.cpp
  LIBRARIES
  PUBLIC
  larevt::ChannelStatusProvider
  PRIVATE
  larevt::ChannelStatusService
  art::Framework_Services_Registry
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::container_algorithms
  cetlib_except::cetlib_except
)

cet_build_plugin(ADCFilter art::EDFilter
  LIBRARIES PRIVATE
  larevt::ChannelStatusService
  larevt::ChannelStatusProvider
  lardataobj::RawData
)
cet_build_plugin(EventFilter art::EDFilter)
cet_build_plugin(EventTimeFilter art::EDFilter
  LIBRARIES PRIVATE
  ROOT::Core
  ROOT::MathCore
)
cet_build_plugin(FinalStateParticleFilter art::EDFilter
  LIBRARIES PRIVATE
  nusimdata::SimulationBase
  art_root_io::TFileService_service
  art::Framework_Services_Registry
  canvas::canvas
  ROOT::Hist
)
cet_build_plugin(LArG4ParticleFilter art::EDFilter
  LIBRARIES PRIVATE
  larcore::Geometry_Geometry_service
  larcoreobj::headers
  nusimdata::SimulationBase
  art::Framework_Services_Registry
  ROOT::Physics
)
cet_build_plugin(MuonFilter art::EDFilter
  LIBRARIES PRIVATE
  lardata::DetectorClocksService
  lardata::DetectorPropertiesService
  larcore::Geometry_Geometry_service
  larcorealg::Geometry
  lardataobj::RecoBase
  messagefacility::MF_MessageLogger
  canvas::canvas
  ROOT::MathCore
)
cet_build_plugin(SmallClusterFilter art::EDFilter
  LIBRARIES PRIVATE
  larcore::Geometry_Geometry_service
  lardata::Utilities
  lardataobj::RecoBase
)
cet_build_plugin(SimpleChannelStatusService lar::ChannelStatusService
  LIBRARIES
  PUBLIC
  larevt::Filters
  PRIVATE
  art::Framework_Principal
  larcore::Geometry_Geometry_service
  messagefacility::MF_MessageLogger
)

install_headers()
install_fhicl()
install_source()
