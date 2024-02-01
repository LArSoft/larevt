cet_enable_asserts()

cet_transitive_paths(FHICL_DIR BINARY IN_TREE)
cet_test_env_prepend(FHICL_FILE_PATH ${TRANSITIVE_PATHS_WITH_FHICL_DIR})

cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

cet_transitive_paths(GDML_DIR BINARY IN_TREE)
cet_test_env_prepend(FW_SEARCH_PATH ${TRANSITIVE_PATHS_WITH_GDML_DIR})

cet_build_plugin(SimpleChannelStatusTest art::EDAnalyzer NO_INSTALL
  LIBRARIES PRIVATE
  larevt::ChannelStatusProvider
  larevt::ChannelStatusService
  larevt::Filters
)

cet_test(ChannelStatusServiceTest HANDBUILT
  DATAFILES test_channel_status.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_channel_status.fcl
)

cet_test(SimpleChannelStatus_test USE_BOOST_UNIT
  SOURCE SimpleChannelStatus_test.cxx
  LIBRARIES PRIVATE
  larevt::Filters
  larevt::CalibrationDBI_IOVData
  fhiclcpp::fhiclcpp
)
