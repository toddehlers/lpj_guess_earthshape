//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file spoutput.cpp
/// \brief Subpixel Implementation of the common output module
///        (based on CommonOuput)
/// \author Christian Werner
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "spoutput.h"
#include "parameters.h"
#include "guess.h"
#include <string>

namespace GuessOutput {

REGISTER_OUTPUT_MODULE("sp", SPOutput)

    
SPOutput::SPOutput() {
	declare_parameter("file_sp_cmass", &file_cmass, 300, "C biomass output file");
	declare_parameter("file_sp_anpp", &file_anpp, 300, "Annual NPP output file");
	declare_parameter("file_sp_agpp", &file_agpp, 300, "Annual GPP output file");
	declare_parameter("file_sp_fpc", &file_fpc, 300, "FPC output file");
	declare_parameter("file_sp_aaet", &file_aaet, 300, "Annual AET output file");
	declare_parameter("file_sp_lai", &file_lai, 300, "LAI output file");
	declare_parameter("file_sp_cflux", &file_cflux, 300, "C fluxes output file");
	declare_parameter("file_sp_doc", &file_doc, 300, "DOC output file");
	
	declare_parameter("file_sp_dens", &file_dens, 300, "Tree density output file");
	declare_parameter("file_sp_cpool", &file_cpool, 300, "Soil C output file");
	declare_parameter("file_sp_clitter", &file_clitter, 300, "Litter C output file");
	declare_parameter("file_sp_runoff", &file_runoff, 300, "Runoff output file");
	declare_parameter("file_sp_firert", &file_firert, 300, "Fire retrun time output file");
	
	declare_parameter("file_sp_nmass", &file_nmass, 300, "N biomass output file");
	declare_parameter("file_sp_cton_leaf", &file_cton_leaf, 300, "Mean leaf C:N output file");
	declare_parameter("file_sp_nsources", &file_nsources, 300, "Annual nitrogen sources output file");
	declare_parameter("file_sp_npool", &file_npool, 300, "Soil nitrogen output file");
	declare_parameter("file_sp_nlitter", &file_nlitter, 300, "Litter nitrogen output file");
	declare_parameter("file_sp_nuptake", &file_nuptake, 300, "Annual nitrogen uptake output file");
	declare_parameter("file_sp_vmaxnlim", &file_vmaxnlim, 300, "Annual nitrogen limitation on vm output file");
	declare_parameter("file_sp_nflux", &file_nflux, 300, "Annual nitrogen fluxes output file");
	declare_parameter("file_sp_ngases", &file_ngases, 300, "Annual nitrogen gases output file");
	declare_parameter("file_sp_speciesheights", &file_speciesheights, 300, "Mean species heights");

	// bvoc
	declare_parameter("file_sp_aiso", &file_aiso, 300, "annual isoprene flux output file");
	declare_parameter("file_sp_miso", &file_miso, 300, "monthly isoprene flux output file");
	declare_parameter("file_sp_amon", &file_amon, 300, "annual monoterpene flux output file");
	declare_parameter("file_sp_mmon", &file_mmon, 300, "monthly monoterpene flux output file");
	
	// Monthly output variables
	declare_parameter("file_sp_mnpp", &file_mnpp, 300, "Monthly NPP output file");
	declare_parameter("file_sp_mlai", &file_mlai, 300, "Monthly LAI output file");
	declare_parameter("file_sp_mgpp", &file_mgpp, 300, "Monthly GPP-LeafResp output file");
	declare_parameter("file_sp_mra", &file_mra, 300, "Monthly autotrophic respiration output file");
	declare_parameter("file_sp_maet", &file_maet, 300, "Monthly AET output file");
	declare_parameter("file_sp_mpet", &file_mpet, 300, "Monthly PET output file");
	declare_parameter("file_sp_mevap", &file_mevap, 300, "Monthly Evap output file");
	declare_parameter("file_sp_mrunoff", &file_mrunoff, 300, "Monthly runoff output file");
	declare_parameter("file_sp_mintercep", &file_mintercep, 300, "Monthly intercep output file");
	declare_parameter("file_sp_mrh", &file_mrh, 300, "Monthly heterotrophic respiration output file");
	declare_parameter("file_sp_mnee", &file_mnee, 300, "Monthly NEE output file");
	declare_parameter("file_sp_mwcont_upper", &file_mwcont_upper, 300, "Monthly wcont_upper output file");
	declare_parameter("file_sp_mwcont_lower", &file_mwcont_lower, 300, "Monthly wcont_lower output file");
    declare_parameter("file_sp_mwcont_deep", &file_mwcont_deep, 300, "Monthly wcont_deep output file");
	// Extra output variables (in addition, sp variants of commonoutput files will be produced)
	declare_parameter("file_sp_mtemp", &file_mtemp, 300, "Monthly mean temperature");
	declare_parameter("file_sp_mrad", &file_mrad, 300, "Monthly mean radiation");
	declare_parameter("file_sp_mprec", &file_mprec, 300, "Monthly precipitation");
	declare_parameter("file_sp_mgdd5", &file_mgdd5, 300, "Monthly growing degree days");
	declare_parameter("file_sp_mndepo", &file_mndepo, 300, "Monthly nitrogen deposition");
	declare_parameter("file_sp_mfpc", &file_mfpc, 300, "monthly fractional cover output file");
}


SPOutput::~SPOutput() {
}

/// Define all output tables and their formats
void SPOutput::init() {

	define_output_tables();
}


    
/** This function specifies all columns in all output tables, their names,
 *  column widths and precision.
 *
 *  For each table a TableDescriptor object is created which is then sent to
 *  the output channel to create the table.
 */
void SPOutput::define_output_tables() {

	//Extra number of decimals when output for benchmarks
#ifdef RUN_BENCHMARKS
	const int bm_extra_prec = 2;
#else
	const int bm_extra_prec =0;
#endif

	// create a vector with the pft names
	std::vector<std::string> pfts;

	// create a vector with the crop pft names
	std::vector<std::string> crop_pfts;

	pftlist.firstobj();
	while (pftlist.isobj) {
		 Pft& pft=pftlist.getobj();

		 pfts.push_back((char*)pft.name);

		 if(pft.landcover==CROPLAND)
			 crop_pfts.push_back((char*)pft.name);

		 pftlist.nextobj();
	}

    // cw REMOVE LATER
	/*
    // create a vector with the landcover column titles
	std::vector<std::string> landcovers;

	if (run_landcover) {
		 const char* landcover_string[]={"Urban_sum", "Crop_sum", "Pasture_sum",
			 "Forest_sum", "Natural_sum", "Peatland_sum", "Barren_sum"};
		 for (int i=0; i<NLANDCOVERTYPES; i++) {
			  if(run[i]) {
					landcovers.push_back(landcover_string[i]);
			  }
		 }
	}
     
    */

	// Create the month columns
	ColumnDescriptors month_columns;
	ColumnDescriptors month_columns_wide;
	xtring months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	for (int i = 0; i < 12; i++) {
		month_columns      += ColumnDescriptor(months[i], 8,  3);
		month_columns_wide += ColumnDescriptor(months[i], 10, 3);
	}

	// Create the columns for each output file

	// CMASS
	ColumnDescriptors cmass_columns;
	cmass_columns += ColumnDescriptors(pfts,              12, 3);
	cmass_columns += ColumnDescriptor("Total",             8, 3);
	//cmass_columns += ColumnDescriptors(landcovers,        13, 3);

	// ANPP
	ColumnDescriptors anpp_columns = cmass_columns;

	// AGPP
	ColumnDescriptors agpp_columns = cmass_columns;

	// FPC
	ColumnDescriptors fpc_columns = cmass_columns;

	// AET
	ColumnDescriptors aaet_columns;
	aaet_columns += ColumnDescriptors(pfts,               12, 2);
	aaet_columns += ColumnDescriptor("Total",              8, 2);
	//aaet_columns += ColumnDescriptors(landcovers,         13, 2);

	// DENS
	ColumnDescriptors dens_columns;
	dens_columns += ColumnDescriptors(pfts,               12, 4);
	dens_columns += ColumnDescriptor("Total",              8, 4);
	//dens_columns += ColumnDescriptors(landcovers,         13, 4);

	// LAI
	ColumnDescriptors lai_columns = dens_columns;

	// CFLUX
	ColumnDescriptors cflux_columns;
	cflux_columns += ColumnDescriptor("Veg",               8, 3);
	cflux_columns += ColumnDescriptor("Repr",              8, 3);
	cflux_columns += ColumnDescriptor("Soil",              8, 3);
	cflux_columns += ColumnDescriptor("Fire",              8, 3);
	cflux_columns += ColumnDescriptor("Est",               8, 3);
	if (run_landcover) {
		 cflux_columns += ColumnDescriptor("Seed",         8, 3);
		 cflux_columns += ColumnDescriptor("Harvest",      9, 3);
		 cflux_columns += ColumnDescriptor("LU_ch",        9, 3);
		 cflux_columns += ColumnDescriptor("Slow_h",       9, 3);
	}
	cflux_columns += ColumnDescriptor("NEE",              10 + bm_extra_prec, 5 + bm_extra_prec);

	ColumnDescriptors doc_columns;
	doc_columns += ColumnDescriptor("Total",              10, 3);
	//doc_columns += ColumnDescriptors(landcovers,          13, 3);

	// CPOOL
	ColumnDescriptors cpool_columns;
	cpool_columns += ColumnDescriptor("VegC",              8, 3);

	if (!ifcentury) {
		cpool_columns += ColumnDescriptor("LittC",         8, 3);
		cpool_columns += ColumnDescriptor("SoilfC",        8, 3);
		cpool_columns += ColumnDescriptor("SoilsC",        8, 3);
	}
	else {
		cpool_columns += ColumnDescriptor("LitterC",       8, 3);
		cpool_columns += ColumnDescriptor("SoilC",         8, 3);
	}
	if (run_landcover && ifslowharvestpool) {
		 cpool_columns += ColumnDescriptor("HarvSlowC",   10, 3);
	}
	cpool_columns += ColumnDescriptor("Total",            10 + bm_extra_prec, 3 + bm_extra_prec);

	// CLITTER
	ColumnDescriptors clitter_columns = cmass_columns;

	// FIRERT
	ColumnDescriptors firert_columns;
	firert_columns += ColumnDescriptor("FireRT",           8, 1);

	// RUNOFF
	ColumnDescriptors runoff_columns;
	runoff_columns += ColumnDescriptor("Surf",             8, 1);
	runoff_columns += ColumnDescriptor("Drain",            8, 1);
	runoff_columns += ColumnDescriptor("Base",             8, 1);
    runoff_columns += ColumnDescriptor("Deep",             8, 1);
	runoff_columns += ColumnDescriptor("All",            8, 1);

	// SPECIESHEIGHTS
	ColumnDescriptors speciesheights_columns;
	speciesheights_columns += ColumnDescriptors(pfts,     12, 2);

	// AISO
	ColumnDescriptors aiso_columns;
	aiso_columns += ColumnDescriptors(pfts,               12, 3);
	aiso_columns += ColumnDescriptor("Total",             10, 3);
	//aiso_columns += ColumnDescriptors(landcovers,         13, 3);

	// AMON
	ColumnDescriptors amon_columns = aiso_columns;

	// CTON
	ColumnDescriptors cton_columns;
	cton_columns += ColumnDescriptors(pfts,               12, 1);
	cton_columns += ColumnDescriptor("Total",              8, 1);
	//cton_columns += ColumnDescriptors(landcovers,         12, 1);

	// NSOURCES
	ColumnDescriptors nsources_columns;
	nsources_columns += ColumnDescriptor("dep",            8, 2);
	nsources_columns += ColumnDescriptor("fix",            8, 2);
	nsources_columns += ColumnDescriptor("fert",           8, 2);
	nsources_columns += ColumnDescriptor("input",          8, 2);
	nsources_columns += ColumnDescriptor("min",            7, 2);
	nsources_columns += ColumnDescriptor("imm",            7, 2);
	nsources_columns += ColumnDescriptor("netmin",         7, 2);
	nsources_columns += ColumnDescriptor("Total",          7, 2);

	// NPOOL
	ColumnDescriptors npool_columns;
	npool_columns += ColumnDescriptor("VegN",              9, 4);
	npool_columns += ColumnDescriptor("LitterN",           9, 4);
	npool_columns += ColumnDescriptor("SoilN",             9, 4);

	if (run_landcover && ifslowharvestpool) {
		npool_columns += ColumnDescriptor("HarvSlowN",    10, 4);
	}

	npool_columns += ColumnDescriptor("Total",            10 + bm_extra_prec, 4 + bm_extra_prec);

	// NMASS
	ColumnDescriptors nmass_columns;
	nmass_columns += ColumnDescriptors(pfts,              12, 2);
	nmass_columns += ColumnDescriptor("Total",             8, 2);
	//nmass_columns += ColumnDescriptors(landcovers,        11, 2);

	// NUPTAKE
	ColumnDescriptors nuptake_columns = nmass_columns;

	// NLITTER
	ColumnDescriptors nlitter_columns = nmass_columns;

	// VMAXNLIM
	ColumnDescriptors vmaxnlim_columns;
	vmaxnlim_columns += ColumnDescriptors(pfts,           12, 2);
	vmaxnlim_columns += ColumnDescriptor("Total",          8, 2);
	//vmaxnlim_columns += ColumnDescriptors(landcovers,     13, 2);

	// NFLUX
	ColumnDescriptors nflux_columns;
	nflux_columns += ColumnDescriptor("dep",               8, 2);
	nflux_columns += ColumnDescriptor("fix",               8, 2);
	nflux_columns += ColumnDescriptor("fert",              8, 2);
	nflux_columns += ColumnDescriptor("flux",              8, 2);
	nflux_columns += ColumnDescriptor("leach",             8, 2);
	if (run_landcover) {
		nflux_columns += ColumnDescriptor("seed",		   8, 2);
		nflux_columns += ColumnDescriptor("harvest",       8, 2);
		nflux_columns += ColumnDescriptor("LU_ch",         8, 3);
		nflux_columns += ColumnDescriptor("Slow_h",        8, 3);
	}
	nflux_columns += ColumnDescriptor("NEE",               8 + bm_extra_prec, 2 + bm_extra_prec);

	// NGASES
	ColumnDescriptors ngases_columns;
	ngases_columns += ColumnDescriptor("NH3",              9, 3);
	ngases_columns += ColumnDescriptor("NOx",              9, 3);
	ngases_columns += ColumnDescriptor("N2O",              9, 3);
	ngases_columns += ColumnDescriptor("N2",               9, 3);
	ngases_columns += ColumnDescriptor("NSoil",            9, 3);
	ngases_columns += ColumnDescriptor("Total",            9, 3);

	// *** ANNUAL OUTPUT VARIABLES ***
	create_output_table(out_cmass,          file_cmass,          cmass_columns);
	create_output_table(out_anpp,           file_anpp,           anpp_columns);
	create_output_table(out_agpp,           file_agpp,           agpp_columns);
	create_output_table(out_fpc,            file_fpc,            fpc_columns);
	create_output_table(out_aaet,           file_aaet,           aaet_columns);
	create_output_table(out_dens,           file_dens,           dens_columns);
	create_output_table(out_lai,            file_lai,            lai_columns);
	create_output_table(out_cflux,          file_cflux,          cflux_columns);
	create_output_table(out_doc,	        file_doc,			 doc_columns);
	create_output_table(out_cpool,          file_cpool,          cpool_columns);
	create_output_table(out_clitter,        file_clitter,        clitter_columns);

	create_output_table(out_firert,         file_firert,         firert_columns);
	create_output_table(out_runoff,         file_runoff,         runoff_columns);
	create_output_table(out_speciesheights, file_speciesheights, speciesheights_columns);
	create_output_table(out_aiso,           file_aiso,           aiso_columns);
	create_output_table(out_amon,           file_amon,           amon_columns);

	create_output_table(out_nmass,          file_nmass,          nmass_columns);
	create_output_table(out_cton_leaf,      file_cton_leaf,      cton_columns);
	create_output_table(out_nsources,       file_nsources,       nsources_columns);
	create_output_table(out_npool,          file_npool,          npool_columns);
	create_output_table(out_nlitter,        file_nlitter,        nlitter_columns);
	create_output_table(out_nuptake,        file_nuptake,        nuptake_columns);
	create_output_table(out_vmaxnlim,       file_vmaxnlim,       vmaxnlim_columns);
	create_output_table(out_nflux,          file_nflux,          nflux_columns);
	create_output_table(out_ngases,         file_ngases,         ngases_columns);

	// *** MONTHLY OUTPUT VARIABLES ***

	create_output_table(out_mnpp,           file_mnpp,           month_columns);
	create_output_table(out_mlai,           file_mlai,           month_columns);
	create_output_table(out_mgpp,           file_mgpp,           month_columns);
	create_output_table(out_mra,            file_mra,            month_columns);
	create_output_table(out_maet,           file_maet,           month_columns);
	create_output_table(out_mpet,           file_mpet,           month_columns);
	create_output_table(out_mevap,          file_mevap,          month_columns);
	create_output_table(out_mrunoff,        file_mrunoff,        month_columns_wide);
	create_output_table(out_mintercep,      file_mintercep,      month_columns);
	create_output_table(out_mrh,            file_mrh,            month_columns);
	create_output_table(out_mnee,           file_mnee,           month_columns);
	create_output_table(out_mwcont_upper,   file_mwcont_upper,   month_columns);
	create_output_table(out_mwcont_lower,   file_mwcont_lower,   month_columns);
    create_output_table(out_mwcont_deep,   file_mwcont_deep,   month_columns);
	create_output_table(out_miso,           file_miso,           month_columns_wide);
	create_output_table(out_mmon,           file_mmon,           month_columns_wide);
	// f_js_20170223 climate output
	create_output_table(out_mtemp,          file_mtemp,          month_columns);
	create_output_table(out_mprec,          file_mprec,          month_columns_wide);
	create_output_table(out_mrad,           file_mrad,           month_columns);
	create_output_table(out_mgdd5,          file_mgdd5,          month_columns_wide);
	create_output_table(out_mndepo,         file_mndepo,         month_columns_wide);

    // cw SubPixel extra output for LandLab
    create_output_table(out_mfpc,           file_mfpc,           month_columns_wide);

}


/// Local analogue of OutputRows::add_value for restricting output
/** Use to restrict output to specified range of years
  * (or other user-specified limitation)
  *
  * If only yearly output between, say 1961 and 1990 is requred, use:
  *  if (date.get_calendar_year() >= 1961 && date.get_calendar_year() <= 1990)
  *  (assuming the input module has set the first calendar year in the date object)
  */


void outlimit_sp(OutputRows& out, const Table& table, double d) {
    // use spoutput_startyear and spoutput_step to restrict output
	if ( spoutput_startyear == -1 ) {
		// do not restrict output
		out.add_value(table, d);
	} else {
		// restrict output
		if ( (date.year >= spoutput_startyear + nyear_spinup) && (date.get_calendar_year() % spoutput_step) == 0 )// nyear_spinup)
			out.add_value(table, d);
	}
}
		

/// Output of simulation results at the end of each year
/** Output of simulation results at the end of each year, or for specific years in
  * the simulation of each stand or grid cell.
  * This function does not have to provide any information to the framework.
  *
  * Restrict output to specific years in the local helper function outlimit_sp().
  *
  * Changes in the structure of this function should be mirrored in outannual()
  * of the other output modules, e.g. MiscOutput::outannual().
  */
void SPOutput::outannual(Gridcell& gridcell) {

	int m;
	double flux_veg, flux_repr, flux_soil, flux_fire, flux_est, flux_seed, flux_charvest;
	double c_fast, c_slow, c_harv_slow;

	double surfsoillitterc,surfsoillittern,cwdc,cwdn,centuryc,centuryn,n_harv_slow,availn;
	double flux_nh3, flux_nox, flux_n2o, flux_n2, flux_nsoil, flux_ntot, flux_nharvest, flux_nseed;

	// Nitrogen output is in kgN/ha instead of kgC/m2 as for carbon
	double m2toha = 10000.0;

	// hold the monthly average across patches
	double mnpp[12];
	double mgpp[12];
	double mlai[12];
	double maet[12];
	double mpet[12];
	double mevap[12];
	double mintercep[12];
	double mrunoff[12];
	double mrh[12];
	double mra[12];
	double mnee[12];
	double mwcont_upper[12];
	double mwcont_lower[12];
    double mwcont_deep[12];

    // bvoc
	double miso[12];
	double mmon[12];
    // cw SubPixel extra output for LandLab
	double mfpc[12];
    
	double aaet, apet, aevap, arunoff, aintercep;

	double lon = gridcell.get_lon();
	double lat = gridcell.get_lat();

    for (m=0;m<12;m++){
		mnpp[m]=mlai[m]=mgpp[m]=mra[m]=maet[m]=mpet[m]=mevap[m]=mintercep[m]=mrunoff[m]=mrh[m]=mnee[m]=mwcont_upper[m]=mwcont_lower[m]=miso[m]=mmon[m]=0.0;
        // cw SubPixel extra output for LandLab
		mfpc[m]=0.0;
        mwcont_deep[m]=0.0;
    }
	aaet = apet = aevap = arunoff = aintercep = 0.0;

	double mean_standpft_cmass=0.0;
	double mean_standpft_nmass=0.0;
	double mean_standpft_cmass_leaf=0.0;
	double mean_standpft_nmass_leaf=0.0;
	double mean_standpft_cmass_veg=0.0;
	double mean_standpft_nmass_veg=0.0;
	double mean_standpft_clitter=0.0;
	double mean_standpft_nlitter=0.0;
	double mean_standpft_anpp=0.0;
	double mean_standpft_agpp=0.0;
	double mean_standpft_fpc=0.0;
	double mean_standpft_aaet=0.0;
	double mean_standpft_lai=0.0;
	double mean_standpft_densindiv_total=0.0;
	double mean_standpft_aiso=0.0;
	double mean_standpft_amon=0.0;
	double mean_standpft_nuptake=0.0;
	double mean_standpft_vmaxnlim=0.0;

	double cmass_gridcell=0.0;
	double nmass_gridcell= 0.0;
	double cmass_leaf_gridcell=0.0;
	double nmass_leaf_gridcell=0.0;
	double cmass_veg_gridcell=0.0;
	double nmass_veg_gridcell=0.0;
	double clitter_gridcell=0.0;
	double nlitter_gridcell= 0.0;
	double anpp_gridcell=0.0;
	double agpp_gridcell=0.0;
	double fpc_gridcell=0.0;
	double aaet_gridcell=0.0;
	double lai_gridcell=0.0;
	double surfrunoff_gridcell=0.0;
	double drainrunoff_gridcell=0.0;
	double baserunoff_gridcell=0.0;
	double runoff_gridcell=0.0;
	double dens_gridcell=0.0;
	double firert_gridcell=0.0;
	double aiso_gridcell=0.0;
	double amon_gridcell=0.0;
	double nuptake_gridcell=0.0;
	double vmaxnlim_gridcell=0.0;

	double andep_gridcell=0.0;
	double anfert_gridcell=0.0;
	double anmin_gridcell=0.0;
	double animm_gridcell=0.0;
	double anfix_gridcell=0.0;
	double n_min_leach_gridcell=0.0;
	double n_org_leach_gridcell=0.0;
	double c_org_leach_gridcell=0.0;

	double standpft_cmass=0.0;
	double standpft_nmass=0.0;
	double standpft_cmass_leaf=0.0;
	double standpft_nmass_leaf=0.0;
	double standpft_cmass_veg=0.0;
	double standpft_nmass_veg=0.0;
	double standpft_clitter=0.0;
	double standpft_nlitter=0.0;
	double standpft_anpp=0.0;
	double standpft_agpp=0.0;
	double standpft_fpc=0.0;
	double standpft_aaet=0.0;
	double standpft_lai=0.0;
	double standpft_densindiv_total=0.0;
	double standpft_aiso=0.0;
	double standpft_amon=0.0;
	double standpft_nuptake=0.0;
	double standpft_vmaxnlim=0.0;
    
    // Determine area fraction of stands where this pft is active:
    double active_fraction = 0.0;
    
	// Loop through Stands
    Gridcell::iterator gc_itr = gridcell.begin();
    
	while (gc_itr != gridcell.end()) {
		Stand& stand = *gc_itr;

        stand.firstobj();
		
		int stand_id(stand.id);
		if (run_landform) {
			stand_id = stand.landform.id;
			
		}
		// output table (stand only, no patch)
		OutputRows out_st(output_channel, lon, lat, date.get_calendar_year(), -1, stand_id);

        // Loop through Patches
        while (stand.isobj) {
            Patch& patch = stand.getobj();
            Vegetation& vegetation = patch.vegetation;

            
            active_fraction = 0.0;
            double all_pfts(0.0);
            
            pftlist.firstobj();
            while (pftlist.isobj) {
                Pft& pft = pftlist.getobj();
                
                // Determine area fraction of stands where this pft is active:
                if (stand.pft[pft.id].active) {
                    active_fraction += 1;
                }
                all_pfts += 1.0;
                pftlist.nextobj();
            }

            active_fraction /= all_pfts;

            // reset runoff vars (now per patch)
            surfrunoff_gridcell=0.0;
            drainrunoff_gridcell=0.0;
            baserunoff_gridcell=0.0;
            runoff_gridcell=0.0;
            
            // The OutputRows object manages the next row of output for each
            // output table
			OutputRows out(output_channel, lon, lat, date.get_calendar_year(), -1, stand_id, patch.id);

			
            // Sum C biomass, NPP, LAI and BVOC fluxes across patches and PFTs
            mean_standpft_cmass=0.0;
            mean_standpft_nmass=0.0;
            mean_standpft_cmass_leaf=0.0;
            mean_standpft_nmass_leaf=0.0;
            mean_standpft_cmass_veg=0.0;
            mean_standpft_nmass_veg=0.0;
            mean_standpft_clitter=0.0;
            mean_standpft_nlitter=0.0;
            mean_standpft_anpp=0.0;
            mean_standpft_agpp=0.0;
            mean_standpft_fpc=0.0;
            mean_standpft_aaet=0.0;
            mean_standpft_lai=0.0;
            mean_standpft_densindiv_total=0.0;
            mean_standpft_aiso=0.0;
            mean_standpft_amon=0.0;
            mean_standpft_nuptake=0.0;
            mean_standpft_vmaxnlim=0.0;
            
            for (m=0;m<12;m++){
                mnpp[m]=mlai[m]=mgpp[m]=mra[m]=maet[m]=mpet[m]=mevap[m]=mintercep[m]=mrunoff[m]=mrh[m]=mnee[m]=mwcont_upper[m]=mwcont_lower[m]=miso[m]=mmon[m]=0.0;
                
                // cw SubPixel data for LandLab
                mfpc[m] = 0.0;
                mwcont_deep[m] = 0.0;
            }
            flux_veg = flux_repr = flux_soil = flux_fire = flux_est = flux_seed = flux_charvest = 0.0;
            
            // guess2008 - carbon pools
            c_fast = c_slow = c_harv_slow = 0.0;
            
            surfsoillitterc = surfsoillittern = cwdc = cwdn = centuryc = centuryn = n_harv_slow = availn = 0.0;
            andep_gridcell = anfert_gridcell = anmin_gridcell = animm_gridcell = anfix_gridcell = 0.0;
            n_org_leach_gridcell = n_min_leach_gridcell = c_org_leach_gridcell = 0.0;
            flux_nh3 = flux_nox = flux_n2o = flux_n2 = flux_nsoil = flux_ntot = flux_nharvest = flux_nseed = 0.0;
            
            double c_org_leach_lc[NLANDCOVERTYPES];
            
            for (int i=0; i<NLANDCOVERTYPES; i++) {
                c_org_leach_lc[i]=0.0;
            }
           
            
            // *** Loop through PFTs ***
            pftlist.firstobj();
            while (pftlist.isobj) {
                
                Pft& pft=pftlist.getobj();
                
                Patchpft& patchpft = patch.pft[pft.id];

                // Sum C biomass, NPP, LAI and BVOC fluxes in a patch // RENAME !!!
                standpft_cmass=0.0;
                standpft_nmass=0.0;
                standpft_cmass_leaf=0.0;
                standpft_nmass_leaf=0.0;
                standpft_cmass_veg=0.0;
                standpft_nmass_veg=0.0;
                standpft_clitter=0.0;
                standpft_nlitter=0.0;
                standpft_anpp=0.0;
                standpft_agpp=0.0;
                standpft_fpc=0.0;
                standpft_aaet=0.0;
                standpft_lai=0.0;
                standpft_densindiv_total = 0.0;
                standpft_aiso=0.0;
                standpft_amon=0.0;
                standpft_nuptake=0.0;
                standpft_vmaxnlim=0.0;
                
                standpft_anpp = patch.fluxes.get_annual_flux(Fluxes::NPP, pft.id);
                standpft_agpp = patch.fluxes.get_annual_flux(Fluxes::GPP, pft.id);
                standpft_aiso = patch.fluxes.get_annual_flux(Fluxes::ISO, pft.id);
                standpft_amon = patch.fluxes.get_annual_flux(Fluxes::MON, pft.id);

                standpft_clitter = patchpft.litter_leaf + patchpft.litter_root + patchpft.litter_sap + patchpft.litter_heart + patchpft.litter_repr;
                standpft_nlitter = patchpft.nmass_litter_leaf + patchpft.nmass_litter_root + patchpft.nmass_litter_sap + patchpft.nmass_litter_heart;

            
                double heightindiv_total = 0.0;

                vegetation.firstobj();
                while (vegetation.isobj) {
                    Individual& indiv=vegetation.getobj();

                    if (indiv.id!=-1 && indiv.alive) {
                        
                        if (indiv.pft.id==pft.id) {
                            standpft_cmass_leaf += indiv.cmass_leaf;
                            standpft_cmass += indiv.ccont();
                            standpft_nmass += indiv.ncont();
                            standpft_nmass_leaf += indiv.cmass_leaf / indiv.cton_leaf_aavr;
                            standpft_nmass_veg += indiv.nmass_veg;
                            standpft_fpc += indiv.fpc;
                            standpft_aaet += indiv.aaet;
                            standpft_lai += indiv.lai;
                            if (pft.lifeform==TREE) {
                                standpft_densindiv_total += indiv.densindiv;
                                heightindiv_total += indiv.height * indiv.densindiv;
                            }
                            standpft_vmaxnlim += indiv.avmaxnlim * indiv.cmass_leaf;
                            standpft_nuptake += indiv.anuptake;

                            if(pft.landcover == CROPLAND) {
                                standpft_cmass_veg += indiv.cmass_leaf + indiv.cmass_root;
                                if(indiv.cropindiv) {
                                    standpft_cmass_veg += indiv.cropindiv->cmass_ho + indiv.cropindiv->cmass_agpool + indiv.cropindiv->cmass_stem;
                                    standpft_nmass_leaf += indiv.cropindiv->ynmass_leaf + indiv.cropindiv->ynmass_dead_leaf;
                                    standpft_nmass_veg += indiv.cropindiv->ycmass_leaf + indiv.cropindiv->ynmass_dead_leaf + indiv.cropindiv->ynmass_root + indiv.cropindiv->ynmass_ho + indiv.cropindiv->ynmass_agpool;
                                }
                            }
                            else {
                                standpft_cmass_veg += indiv.cmass_veg;
                            }
                        }

                    } // alive?
                    vegetation.nextobj();
                }

                // cw output data
                // Print PFT sums to files
				
				// cw SubPixel - quick fix for missing cpool/ npool vegc and vegn
				cmass_gridcell += standpft_cmass;
				nmass_gridcell += standpft_nmass;
                
                double standpft_mean_cton_leaf = limited_cton(standpft_cmass_leaf, standpft_nmass_leaf);

                outlimit_sp(out,out_cmass,     standpft_cmass);
                outlimit_sp(out,out_anpp,      standpft_anpp);
                outlimit_sp(out,out_agpp,      standpft_agpp);
                outlimit_sp(out,out_fpc,       standpft_fpc);
                outlimit_sp(out,out_aaet,      standpft_aaet);
                outlimit_sp(out,out_clitter,   standpft_clitter);
                outlimit_sp(out,out_dens,      standpft_densindiv_total);
                outlimit_sp(out,out_lai,       standpft_lai);
                outlimit_sp(out,out_aiso,      standpft_aiso);
                outlimit_sp(out,out_amon,      standpft_amon);
                outlimit_sp(out,out_nmass,     (standpft_nmass + standpft_nlitter) * m2toha);
                outlimit_sp(out,out_cton_leaf, standpft_mean_cton_leaf);
                outlimit_sp(out,out_vmaxnlim,  standpft_vmaxnlim);
                outlimit_sp(out,out_nuptake,   standpft_nuptake * m2toha);
                outlimit_sp(out,out_nlitter,   standpft_nlitter * m2toha);


                // print species heights
                double height = 0.0;
                if (standpft_densindiv_total > 0.0)
                    height = heightindiv_total / standpft_densindiv_total;

                outlimit_sp(out,out_speciesheights, height);

                // add vals to pft total column
                mean_standpft_cmass += standpft_cmass / active_fraction;
                mean_standpft_nmass += standpft_nmass / active_fraction;
                mean_standpft_cmass_leaf += standpft_cmass_leaf / active_fraction;
                mean_standpft_nmass_leaf += standpft_nmass_leaf / active_fraction;
                mean_standpft_cmass_veg += standpft_cmass_veg / active_fraction;
                mean_standpft_nmass_veg += standpft_nmass_veg / active_fraction;
                mean_standpft_clitter += standpft_clitter / active_fraction;
                mean_standpft_nlitter += standpft_nlitter / active_fraction;
                mean_standpft_anpp += standpft_anpp / active_fraction;
                mean_standpft_agpp += standpft_agpp / active_fraction;
                mean_standpft_fpc += standpft_fpc / active_fraction;
                mean_standpft_aaet += standpft_aaet / active_fraction;
                mean_standpft_lai += standpft_lai / active_fraction;
                mean_standpft_densindiv_total += standpft_densindiv_total / active_fraction;
                mean_standpft_aiso += standpft_aiso / active_fraction;
                mean_standpft_amon += standpft_amon / active_fraction;
                mean_standpft_nuptake += standpft_nuptake / active_fraction;
                mean_standpft_vmaxnlim += standpft_vmaxnlim / active_fraction;

                pftlist.nextobj();

            } // *** End of PFT loop ***
            
            

            double standpft_mean_cton_leaf = limited_cton(mean_standpft_cmass_leaf, mean_standpft_nmass_leaf);
            
            outlimit_sp(out,out_cmass,     mean_standpft_cmass);
            outlimit_sp(out,out_anpp,      mean_standpft_anpp);
            outlimit_sp(out,out_agpp,      mean_standpft_agpp);
            outlimit_sp(out,out_fpc,       mean_standpft_fpc);
            outlimit_sp(out,out_aaet,      mean_standpft_aaet);
            outlimit_sp(out,out_clitter,   mean_standpft_clitter);
            outlimit_sp(out,out_dens,      mean_standpft_densindiv_total);
            outlimit_sp(out,out_lai,       mean_standpft_lai);
            outlimit_sp(out,out_aiso,      mean_standpft_aiso);
            outlimit_sp(out,out_amon,      mean_standpft_amon);
            outlimit_sp(out,out_nmass,     (mean_standpft_nmass + mean_standpft_nlitter) * m2toha);
            outlimit_sp(out,out_cton_leaf, standpft_mean_cton_leaf);
            outlimit_sp(out,out_vmaxnlim,  mean_standpft_vmaxnlim);
            outlimit_sp(out,out_nuptake,   mean_standpft_nuptake * m2toha);
            outlimit_sp(out,out_nlitter,   mean_standpft_nlitter * m2toha);

            
            
            // non veg data
            flux_veg+=-patch.fluxes.get_annual_flux(Fluxes::NPP);
            flux_repr+=-patch.fluxes.get_annual_flux(Fluxes::REPRC);
            flux_soil+=patch.fluxes.get_annual_flux(Fluxes::SOILC);
            flux_fire+=patch.fluxes.get_annual_flux(Fluxes::FIREC);
            flux_est+=patch.fluxes.get_annual_flux(Fluxes::ESTC);
            flux_seed+=patch.fluxes.get_annual_flux(Fluxes::SEEDC);
            flux_charvest+=patch.fluxes.get_annual_flux(Fluxes::HARVESTC);
            
            flux_nseed+=patch.fluxes.get_annual_flux(Fluxes::SEEDN);
            flux_nharvest+=patch.fluxes.get_annual_flux(Fluxes::HARVESTN);
            flux_nh3+=patch.fluxes.get_annual_flux(Fluxes::NH3_FIRE);
            flux_nox+=patch.fluxes.get_annual_flux(Fluxes::NOx_FIRE);
            flux_n2o+=patch.fluxes.get_annual_flux(Fluxes::N2O_FIRE);
            flux_n2+=patch.fluxes.get_annual_flux(Fluxes::N2_FIRE);
            flux_nsoil+=patch.fluxes.get_annual_flux(Fluxes::N_SOIL);
            flux_ntot+=(patch.fluxes.get_annual_flux(Fluxes::NH3_FIRE) +
                        patch.fluxes.get_annual_flux(Fluxes::NOx_FIRE) +
                        patch.fluxes.get_annual_flux(Fluxes::N2O_FIRE) +
                        patch.fluxes.get_annual_flux(Fluxes::N2_FIRE) +
                        patch.fluxes.get_annual_flux(Fluxes::N_SOIL));
            
            c_fast+=patch.soil.cpool_fast;
            c_slow+=patch.soil.cpool_slow;
            
            surfrunoff_gridcell+=patch.asurfrunoff * (stand.landform.fraction * 0.01);
            drainrunoff_gridcell+=patch.adrainrunoff * (stand.landform.fraction * 0.01);
            baserunoff_gridcell+=patch.abaserunoff * (stand.landform.fraction * 0.01);
            runoff_gridcell+=patch.arunoff * (stand.landform.fraction * 0.01);
            
            // Fire return time
            if (!patch.has_fires() || patch.fireprob < 0.001)
                firert_gridcell+=1000.0; // Set a limit of 1000 years
            else
                firert_gridcell+=(1.0/patch.fireprob);
            
            
            andep_gridcell += stand.get_climate().andep;
            anfert_gridcell += patch.anfert;
            anmin_gridcell += patch.soil.anmin;
            animm_gridcell += patch.soil.animmob;
            anfix_gridcell += patch.soil.anfix;
            n_min_leach_gridcell += patch.soil.aminleach;
            n_org_leach_gridcell += patch.soil.aorgNleach;
            c_org_leach_gridcell += patch.soil.aorgCleach;
            availn += (patch.soil.nmass_avail + patch.soil.snowpack_nmass);
            
            c_org_leach_lc[stand.landcover] += patch.soil.aorgCleach;
            
            for (int r = 0; r < NSOMPOOL-1; r++) {
                
                if(r == SURFMETA || r == SURFSTRUCT || r == SOILMETA || r == SOILSTRUCT){
                    surfsoillitterc += patch.soil.sompool[r].cmass;
                    surfsoillittern += patch.soil.sompool[r].nmass;
                }
                else if (r == SURFFWD || r == SURFCWD) {
                    cwdc += patch.soil.sompool[r].cmass;
                    cwdn += patch.soil.sompool[r].nmass;
                }
                else {
                    centuryc += patch.soil.sompool[r].cmass;
                    centuryn += patch.soil.sompool[r].nmass;
                }
            }
            
            // Monthly output variables

            for (m=0;m<12;m++) {
                maet[m] += patch.maet[m];
                mpet[m] += patch.mpet[m];
                mevap[m] += patch.mevap[m];
                mintercep[m] += patch.mintercep[m];
                mrunoff[m] += patch.mrunoff[m];
                mrh[m] += patch.fluxes.get_monthly_flux(Fluxes::SOILC, m);
                mwcont_upper[m] += patch.soil.mwcont[m][0];
                mwcont_lower[m] += patch.soil.mwcont[m][1];
                mwcont_deep[m] += patch.soil.mwcont_deep[m];

                mgpp[m] += patch.fluxes.get_monthly_flux(Fluxes::GPP, m);
                mra[m] += patch.fluxes.get_monthly_flux(Fluxes::RA, m);
                
                miso[m]+=patch.fluxes.get_monthly_flux(Fluxes::ISO, m);
                mmon[m]+=patch.fluxes.get_monthly_flux(Fluxes::MON, m);
            }
            
            
            // Calculate monthly NPP and LAI
            vegetation.firstobj();
            while (vegetation.isobj) {
                Individual& indiv = vegetation.getobj();
                
                // guess2008 - alive check added
                if (indiv.id != -1 && indiv.alive) {
                    
                    for (m=0;m<12;m++) {
                        mlai[m] += indiv.mlai[m];
                        mfpc[m] += indiv.mfpc[m];
                    }
                    
                } // alive?
                
                vegetation.nextobj();
                
            } // while/vegetation loop
            
            // In contrast to annual NEE, monthly NEE does not include fire
            // or establishment fluxes
            for (m=0;m<12;m++) {
                mnpp[m] = mgpp[m] - mra[m];
                mnee[m] = mrh[m] - mnpp[m];
                
                // cw SubPixel
                mfpc[m] = min(mfpc[m], 1.0);
            }
            

            // Write fluxes to file
            Landcover& lc = gridcell.landcover;
            
            outlimit_sp(out,out_cflux, flux_veg);
            outlimit_sp(out,out_cflux, -flux_repr);
            outlimit_sp(out,out_cflux, flux_soil + c_org_leach_gridcell);
            outlimit_sp(out,out_cflux, flux_fire);
            outlimit_sp(out,out_cflux, flux_est);
            if (run_landcover) {
                outlimit_sp(out,out_cflux, flux_seed);
                outlimit_sp(out,out_cflux, flux_charvest);
                outlimit_sp(out,out_cflux, lc.acflux_landuse_change);
                outlimit_sp(out,out_cflux, lc.acflux_harvest_slow);
            }
            outlimit_sp(out,out_cflux, flux_veg - flux_repr + flux_soil + flux_fire + flux_est + c_org_leach_gridcell +
                        flux_seed + flux_charvest + lc.acflux_landuse_change + lc.acflux_harvest_slow);
            
            outlimit_sp(out,out_doc, (c_org_leach_gridcell) * m2toha);

            outlimit_sp(out,out_nflux, -andep_gridcell * m2toha);
            outlimit_sp(out,out_nflux, -anfix_gridcell * m2toha);
            outlimit_sp(out,out_nflux, -anfert_gridcell * m2toha);
            outlimit_sp(out,out_nflux, flux_ntot * m2toha);
            outlimit_sp(out,out_nflux, (n_min_leach_gridcell + n_org_leach_gridcell) * m2toha);
            if (run_landcover) {
                outlimit_sp(out,out_nflux, flux_nseed * m2toha);
                outlimit_sp(out,out_nflux, flux_nharvest * m2toha);
                outlimit_sp(out,out_nflux, lc.anflux_landuse_change * m2toha);
                outlimit_sp(out,out_nflux, lc.anflux_harvest_slow * m2toha);
            }
            outlimit_sp(out,out_nflux, (flux_nharvest + lc.anflux_landuse_change +
                                        lc.anflux_harvest_slow + flux_nseed + flux_ntot +
                                        n_min_leach_gridcell + n_org_leach_gridcell -
                                        (andep_gridcell + anfix_gridcell + anfert_gridcell)) * m2toha);
            
            outlimit_sp(out,out_ngases, flux_nh3   * m2toha);
            outlimit_sp(out,out_ngases, flux_nox   * m2toha);
            outlimit_sp(out,out_ngases, flux_n2o   * m2toha);
            outlimit_sp(out,out_ngases, flux_n2    * m2toha);
            outlimit_sp(out,out_ngases, flux_nsoil * m2toha);
            outlimit_sp(out,out_ngases, flux_ntot  * m2toha);
            
            // Write cpool to file
            outlimit_sp(out,out_cpool, cmass_gridcell);
            if (!ifcentury) {
                outlimit_sp(out,out_cpool, clitter_gridcell);
                outlimit_sp(out,out_cpool, c_fast);
                outlimit_sp(out,out_cpool, c_slow);
            }
            else {
                outlimit_sp(out,out_cpool, clitter_gridcell + surfsoillitterc + cwdc);
                outlimit_sp(out,out_cpool, centuryc);
            }
            
            if (run_landcover && ifslowharvestpool) {
                outlimit_sp(out,out_cpool, c_harv_slow);
            }
            
            // Calculate total cpool, starting with cmass and litter...
            double cpool_total = cmass_gridcell + clitter_gridcell;
            
            // Add SOM pools
            if (!ifcentury) {
                cpool_total += c_fast + c_slow;
            }
            else {
                cpool_total += centuryc + surfsoillitterc + cwdc;
            }
            
            // Add slow harvest pool if needed
            if (run_landcover && ifslowharvestpool) {
                cpool_total += c_harv_slow;
            }
            
            outlimit_sp(out,out_cpool, cpool_total);
            
            // NPOOL Write npool to file
            
            if (ifcentury) {
                outlimit_sp(out,out_npool, nmass_gridcell + nlitter_gridcell);
                outlimit_sp(out,out_npool, surfsoillittern + cwdn);
                outlimit_sp(out,out_npool, centuryn + availn);
                
                if(run_landcover && ifslowharvestpool) {
                    outlimit_sp(out,out_npool, n_harv_slow);
                    outlimit_sp(out,out_npool, (nmass_gridcell + nlitter_gridcell + surfsoillittern + cwdn + centuryn + availn + n_harv_slow));
                }
                else {
                    outlimit_sp(out,out_npool, (nmass_gridcell + nlitter_gridcell + surfsoillittern + cwdn + centuryn + availn));
                }
            }
            
            // Print gridcell totals to files
            
            // Determine total leaf C:N ratio
            double cton_leaf_gridcell = limited_cton(cmass_leaf_gridcell, nmass_leaf_gridcell);
            
            // Determine total vmax nitrogen limitation
            if (cmass_leaf_gridcell > 0.0) {
                vmaxnlim_gridcell /= cmass_leaf_gridcell;
            }
            
            /*
            outlimit_sp(out,out_cmass,  cmass_gridcell);
            outlimit_sp(out,out_anpp,   anpp_gridcell);
            outlimit_sp(out,out_agpp,   agpp_gridcell);
            outlimit_sp(out,out_fpc,    fpc_gridcell);
            outlimit_sp(out,out_aaet,   aaet_gridcell);
            outlimit_sp(out,out_dens,   dens_gridcell);
            outlimit_sp(out,out_lai,    lai_gridcell);
            outlimit_sp(out,out_clitter,clitter_gridcell);
            outlimit_sp(out,out_firert, firert_gridcell);
            */

             
             /*
            outlimit_sp(out,out_aiso,   aiso_gridcell);
            outlimit_sp(out,out_amon,   amon_gridcell);
            
            outlimit_sp(out,out_nmass,    (nmass_gridcell + nlitter_gridcell) * m2toha);
            outlimit_sp(out,out_cton_leaf, cton_leaf_gridcell);
            outlimit_sp(out,out_vmaxnlim,  vmaxnlim_gridcell);
            outlimit_sp(out,out_nuptake,   nuptake_gridcell * m2toha);
            outlimit_sp(out,out_nlitter,   nlitter_gridcell * m2toha);
             */
        
            outlimit_sp(out,out_nsources, andep_gridcell * m2toha);
            outlimit_sp(out,out_nsources, anfix_gridcell * m2toha);
            outlimit_sp(out,out_nsources, anfert_gridcell * m2toha);
            outlimit_sp(out,out_nsources, (andep_gridcell + anfix_gridcell + anfert_gridcell) * m2toha);
            outlimit_sp(out,out_nsources, anmin_gridcell * m2toha);
            outlimit_sp(out,out_nsources, animm_gridcell * m2toha);
            outlimit_sp(out,out_nsources, (anmin_gridcell - animm_gridcell) * m2toha);
            outlimit_sp(out,out_nsources, (anmin_gridcell - animm_gridcell + andep_gridcell +
                                           anfix_gridcell + anfert_gridcell) * m2toha);

            
            outlimit_sp(out,out_runoff, patch.asurfrunoff);
            outlimit_sp(out,out_runoff, patch.adrainrunoff);
            outlimit_sp(out,out_runoff, patch.abaserunoff);
            outlimit_sp(out,out_runoff, patch.adeeprunoff);
            outlimit_sp(out,out_runoff, patch.arunoff);
            
			// Fire return time
			double firert_patch;
			if (!patch.has_fires() || patch.fireprob < 0.001)
				firert_patch = 1000.0; // Set a limit of 1000 years
			else
				firert_patch = (1.0/patch.fireprob);
			outlimit_sp(out,out_firert, firert_patch);
            
            // Print monthly output variables
            for (m=0;m<12;m++) {
                outlimit_sp(out,out_mnpp,         mnpp[m]);
                outlimit_sp(out,out_mlai,         mlai[m]);
                outlimit_sp(out,out_mgpp,         mgpp[m]);
                outlimit_sp(out,out_mra,          mra[m]);
                outlimit_sp(out,out_maet,         maet[m]);
                outlimit_sp(out,out_mpet,         mpet[m]);
                outlimit_sp(out,out_mevap,        mevap[m]);
                outlimit_sp(out,out_mrunoff,      mrunoff[m]);
                outlimit_sp(out,out_mintercep,    mintercep[m]);
                outlimit_sp(out,out_mrh,          mrh[m]);
                outlimit_sp(out,out_mnee,         mnee[m]);
                outlimit_sp(out,out_mwcont_upper, mwcont_upper[m]);
                outlimit_sp(out,out_mwcont_lower, mwcont_lower[m]);
                outlimit_sp(out,out_mwcont_deep, mwcont_deep[m]);
                outlimit_sp(out,out_miso,         miso[m]);
                outlimit_sp(out,out_mmon,         mmon[m]);

                // cw SubPixel extra output for LandLab
                outlimit_sp(out,out_mfpc,         mfpc[m]);
                
                aaet += maet[m];
                apet += mpet[m];
                aevap += mevap[m];
                arunoff += mrunoff[m];
                aintercep += mintercep[m];
            }
            
            stand.nextobj();
        } // end of patch loop
        
        // output per stand vars
	
		
        for (m=0;m<12;m++) {
                // f_js_20170223 climate driver
                outlimit_sp(out_st,out_mprec,     stand.local_climate.m_prec[m]);
                outlimit_sp(out_st,out_mtemp,     stand.local_climate.m_temp[m]);
            outlimit_sp(out_st,out_mrad,      stand.local_climate.m_rad[m]);  //    / 86400.0 ); // J/m^2/day -> W/m^2
                outlimit_sp(out_st,out_mgdd5,     stand.local_climate.m_gdd5[m]);
				outlimit_sp(out_st,out_mndepo,    stand.local_climate.m_ndepo[m] * m2toha);
        }
        ++gc_itr;
    }//End of loop through stands
    
}

/// Output of simulation results at the end of each day
/** This function does not have to provide any information to the framework.
  */
void SPOutput::outdaily(Gridcell& gridcell) {
}

} // namespace
