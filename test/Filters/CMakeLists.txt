cet_enable_asserts()

simple_plugin ( SimpleChannelStatusTest "module"
                    larevt_Filters
                    messagefacility::MF_MessageLogger
                    
                    fhiclcpp::fhiclcpp fhiclcpp::types
                    cetlib cetlib_except
                    ${ROOT_BASIC_LIB_LIST}
              )

cet_test(ChannelStatusServiceTest HANDBUILT
  DATAFILES test_channel_status.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_channel_status.fcl
)

cet_test(SimpleChannelStatus_test
  SOURCES SimpleChannelStatus_test.cxx
  LIBRARIES larevt_Filters
            larevt_CalibrationDBI_IOVData
            fhiclcpp::fhiclcpp fhiclcpp::types
  USE_BOOST_UNIT
)

# install_headers()
# install_fhicl()
# install_source()
