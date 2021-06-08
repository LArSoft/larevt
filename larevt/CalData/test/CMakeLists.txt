cet_enable_asserts()

simple_plugin(FFTTest "module"
			lardata_Utilities_LArFFT_service
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_ROOT_IO_TFILE_SUPPORT} ROOT::Core
			${ART_ROOT_IO_TFILESERVICE_SERVICE}
			art_Persistency_Common
			art_Persistency_Provenance
	 		art_Utilities
                        canvas
			messagefacility::MF_MessageLogger

	                fhiclcpp::fhiclcpp fhiclcpp::types
			cetlib cetlib_except
			ROOT::ROOTDataFrame
			ROOT::Physics
			ROOT::Geom
			ROOT::FFTW
			ROOT::Tree
			ROOT::Core
)

install_headers()
install_fhicl()
install_source()

