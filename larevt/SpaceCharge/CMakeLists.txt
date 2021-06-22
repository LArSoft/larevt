art_make(NO_PLUGINS
         LIB_LIBRARIES
           canvas
           fhiclcpp::fhiclcpp fhiclcpp::types
           ROOT::Core
           ROOT::Hist
           ROOT::RIO
           cetlib
           cetlib_except
         )

install_headers()
install_fhicl()
install_source()
