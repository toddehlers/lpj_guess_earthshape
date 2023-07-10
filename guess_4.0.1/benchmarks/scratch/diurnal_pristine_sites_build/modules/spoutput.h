///////////////////////////////////////////////////////////////////////////////////////
/// \file spoutput.h
/// \brief SubPixel output module for the most commonly needed output files
///        (based on CommonOuput)
/// \author Christian Werner
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_SP_OUTPUT_H
#define LPJ_GUESS_SP_OUTPUT_H

#include "outputmodule.h"
#include "outputchannel.h"
#include "gutil.h"

namespace GuessOutput {

/// Output module for the most commonly needed output files
class SPOutput : public OutputModule {
public:

	SPOutput();

	~SPOutput();

	// implemented functions inherited from OutputModule
	// (see documentation in OutputModule)

	void init();

	void outannual(Gridcell& gridcell);

	void outdaily(Gridcell& gridcell);
    

private:

	/// Defines all output tables
	void define_output_tables();

	// Output file names ...
	xtring file_cmass,file_anpp,file_agpp,file_fpc,file_aaet,file_dens,file_lai,file_cflux,file_doc,file_cpool,file_clitter,file_runoff;
	xtring file_mnpp,file_mlai,file_mgpp,file_mra,file_maet,file_mpet,file_mevap,file_mrunoff,file_mintercep,file_mrh;
	xtring file_mnee,file_mwcont_upper,file_mwcont_lower,file_mwcont_deep;
	xtring file_firert,file_speciesheights;
	// f_js_20170223 climate driver
	xtring file_mtemp, file_mprec, file_mrad, file_mgdd5, file_mndepo;

	// bvoc
	xtring file_aiso,file_miso,file_amon,file_mmon;

	// nitrogen
	xtring file_nmass, file_cton_leaf, file_nsources, file_npool, file_nlitter, file_nuptake, file_vmaxnlim, file_nflux, file_ngases;

    // cw SubPixel - extra files for LandLab
    xtring file_mfpc;

	// extra summary files
	xtring file_site;
	
	// Output tables
	Table out_cmass, out_anpp, out_agpp, out_fpc, out_aaet, out_dens, out_lai, out_cflux, out_doc, out_cpool, out_clitter, out_firert, out_runoff, out_speciesheights;

	Table out_mnpp, out_mlai, out_mgpp, out_mra, out_maet, out_mpet, out_mevap, out_mrunoff, out_mintercep;
	Table out_mrh, out_mnee, out_mwcont_upper, out_mwcont_lower, out_mwcont_deep;
	// f_js_20170223 climate driver
	Table out_mtemp, out_mprec, out_mrad, out_mgdd5, out_mndepo;

	// bvoc
	Table out_aiso, out_miso, out_amon, out_mmon;

	Table out_nmass, out_cton_leaf, out_nsources, out_npool, out_nlitter, out_nuptake, out_vmaxnlim, out_nflux, out_ngases;
    
    // cw SubPixel - extra output table for LandLab
    Table out_mfpc;
	
	// extra summary tables
	Table out_site;
};

}

#endif // LPJ_GUESS_SP_OUTPUT_H
