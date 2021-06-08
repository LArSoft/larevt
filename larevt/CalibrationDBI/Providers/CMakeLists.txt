cet_find_library(LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH)
cet_find_library(SQLITE NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH)

include_directories($ENV{LIBWDA_FQ_DIR}/include)

art_make(LIB_LIBRARIES
           larevt_CalibrationDBI_IOVData
           ${LIBWDA}
           ${SQLITE}
           ${MF_MESSAGELOGGER}
           ${FHICLCPP}
           canvas
           cetlib
           cetlib_except
           larcorealg_Geometry
           ${ART_FRAMEWORK_SERVICES_REGISTRY}
           ROOT::Core
           ${ART_UTILITIES}
        )

install_headers()
install_source()
