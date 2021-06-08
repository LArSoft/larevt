art_make(NO_PLUGINS
         LIB_LIBRARIES
           canvas
           ${FHICLCPP}
           ROOT::Core
           ROOT::Hist
           ROOT::RIO
           cetlib
           cetlib_except
         )

install_headers()
install_fhicl()
install_source()
