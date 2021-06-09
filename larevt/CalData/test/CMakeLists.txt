cet_enable_asserts()

simple_plugin(FFTTest "module"
			lardata_Utilities_LArFFT_service
			art::Framework_Services_Registry
			art_root_io::tfile_support ROOT::Core
			art_root_io::TFileService_service
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

