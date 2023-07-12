//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file spinput.cpp
/// \brief Input module for SubPixel conforming NetCDF files (based on cfinput.h)
///
/// \author Christian Werner
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "spinput.h"

#ifdef HAVE_NETCDF

#include "guess.h"
#include "driver.h"
#include "guessstring.h"
#include <fstream>
#include <sstream>
#include <algorithm>

REGISTER_INPUT_MODULE("sp", SPInput)

using namespace GuessNC::CF;

namespace {

// cw little helper: check that all elements are equal
template <typename T, typename U>
bool allequal(const T &t, const U &u) {
	return t == u;
}

template <typename T, typename U, typename... Others>
bool allequal(const T &t, const U &u, Others const &... args) {
	return (t == u) && allequal(u, args...);
}

const int SECONDS_PER_DAY = 24*60*60;

// Converts a CF standard name to one of our insolation types
// Calls fail() if the standard name is invalid
insoltype cf_standard_name_to_insoltype(const std::string& standard_name) {
	if (standard_name == "surface_downwelling_shortwave_flux_in_air" ||
	    standard_name == "surface_downwelling_shortwave_flux") {
		return SWRAD_TS;
	}
	else if (standard_name == "surface_net_downward_shortwave_flux") {
		return NETSWRAD_TS;
	}
	else if (standard_name == "cloud_area_fraction") {
		return SUNSHINE;
	}
	else {
		fail("Unknown insolation type: %s", standard_name.c_str());
		return SUNSHINE; // To avoid compiler warning
	}
}

// Gives the maximum allowed value for insolation, given an insolation type
// Used as an upper limit when interpolating from monthly to daily values
double max_insolation(insoltype instype) {
	if (instype == SUNSHINE) {
		return 100;
	}
	else {
		return std::numeric_limits<double>::max();
	}
}

// Checks if a DateTime is at the first day of the year
bool first_day_of_year(GuessNC::CF::DateTime dt) {
	return dt.get_month() == 1 && dt.get_day() == 1;
}

// Checks if a DateTime is in January
bool first_month_of_year(GuessNC::CF::DateTime dt) {
	return dt.get_month() == 1;
}

// Compares a Date with a GuessNC::CF::DateTime to see if the Date is on an earlier day
bool earlier_day(const Date& date, int calendar_year,
                 const GuessNC::CF::DateTime& date_time,
				 int paleo_offset) {
	std::vector<int> d1(3),d2(3);

	// cw SubPixel we need to subtract the paleo_offset again to make this work
	d1[0] = calendar_year - paleo_offset;
	d2[0] = date_time.get_year();

	d1[1] = date.month+1;
	d2[1] = date_time.get_month();

	d1[2] = date.dayofmonth+1;
	d2[2] = date_time.get_day();

	return d1 < d2;
}

// Compares a Date with a GuessNC::CF::DateTime to see if the Date is on a later day
// The date object must know about its calendar years (i.e. set_first_calendar_year must
// have been called)
bool later_day(const Date& date,
               const GuessNC::CF::DateTime& date_time,
			   int paleo_offset) {
	std::vector<int> d1(3),d2(3);

	// cw SubPixel we need to subtract the paleo_offset again to make this work 
	d1[0] = date.get_calendar_year() - paleo_offset;
	d2[0] = date_time.get_year();

	d1[1] = date.month+1;
	d2[1] = date_time.get_month();

	d1[2] = date.dayofmonth+1;
	d2[2] = date_time.get_day();

	return d1 > d2;
}

// Checks if the variable contains daily data
bool is_daily(const GuessNC::CF::GridcellOrderedVariable* cf_var) {

	// Check if first and second timestep is one day apart

	DateTime dt1 = cf_var->get_date_time(0);
	DateTime dt2 = cf_var->get_date_time(1);

	dt1.add_time(1, GuessNC::CF::DAYS, cf_var->get_calendar_type());

	return dt1 == dt2;
}

// Returns a DateTime in the last day for which the variable has data.
// For daily data, this is simply the day of the last timestep, for monthly data
// we need to find the last day of the last timestep's month.
GuessNC::CF::DateTime last_day_to_simulate(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	GuessNC::CF::DateTime last = cf_var->get_date_time(cf_var->get_timesteps()-1);
	if (is_daily(cf_var)) {
		return last;
	}
	else {
		// Not daily, assume monthly.
		GuessNC::CF::DateTime prev = last;
		GuessNC::CF::DateTime next = last;

		do {
			prev = next;
			next.add_time(1, GuessNC::CF::DAYS, cf_var->get_calendar_type());
		} while (next.get_month() == last.get_month());

		return prev;
	}
}

// Verifies that a CF variable with air temperature data contains what we expect
void check_temp_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "air_temperature") {
		fail("Temperature variable doesn't seem to contain air temperature data");
	}
	if (cf_var->get_units() != "K") {
		fail("Temperature variable doesn't seem to be in Kelvin");
	}
}

// Verifies that a CF variable with precipitation data contains what we expect
void check_prec_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() == "precipitation_flux") {
		if (cf_var->get_units() != "kg m-2 s-1") {
			fail("Precipitation is given as flux but does not have the correct unit (kg m-2 s-1)");
		}
	}
	else if (cf_var->get_standard_name() == "precipitation_amount") {
		if (cf_var->get_units() != "kg m-2") {
			fail("Precipitation is given as amount but does not have the correct unit (kg m-2)");
		}
	}
	else {
		fail("Unrecognized precipitation type");
	}
}

// Verifies that a CF variable with insolation data contains what we expect
void check_insol_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "surface_downwelling_shortwave_flux_in_air" &&
	    cf_var->get_standard_name() != "surface_downwelling_shortwave_flux" &&
	    cf_var->get_standard_name() != "surface_net_downward_shortwave_flux" &&
	    cf_var->get_standard_name() != "cloud_area_fraction") {
		fail("Insolation variable doesn't seem to contain insolation data");
	}

	if (cf_var->get_standard_name() == "cloud_area_fraction") {
		if (cf_var->get_units() != "1") {
			fail("Unrecognized unit for cloud cover");
		}
	}
	else {
		if (cf_var->get_units() != "W m-2") {
			fail("Insolation variable given as radiation but unit doesn't seem to be in W m-2");
		}
	}
}

// Verifies that a CF variable with wetdays data contains what we expect
void check_wetdays_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	const char* wetdays_standard_name =
		"number_of_days_with_lwe_thickness_of_precipitation_amount_above_threshold";

	if (cf_var && cf_var->get_standard_name() != wetdays_standard_name) {
		fail("Wetdays variable should have standard name %s", wetdays_standard_name);
	}
}

// cw SubPixel ndepo
void check_ndep_nhxdry(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "NHxDry" &&
		cf_var->get_standard_name() != "nhxdry" &&
		cf_var->get_standard_name() != "NHXDRY") {
		fail("N-Deposition variable NHxDry doesn't seem to contain nhxdry data");
	}
	if (cf_var->get_units() != "g N ha-1 d-1") {
		fail("NHxDry variable given but unit doesn't seem to be g ha-1 d-1");
	}
}

void check_ndep_nhxwet(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "NHxWet" &&
		cf_var->get_standard_name() != "nhxwet" &&
		cf_var->get_standard_name() != "NHXWET") {
		fail("N-Deposition variable NHxWet doesn't seem to contain nhxwet data");
	}
	if (cf_var->get_units() != "g N ha-1 d-1") {
		fail("NHxWet variable given but unit doesn't seem to be g ha-1 d-1");
	}
}

void check_ndep_noydry(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "NOyDry" &&
		cf_var->get_standard_name() != "noydry" &&
		cf_var->get_standard_name() != "NOYDRY") {
		fail("N-Deposition variable NOyDry doesn't seem to contain noydry data");
	}
	if (cf_var->get_units() != "g N ha-1 d-1") {
		fail("NOyDry variable given but unit doesn't seem to be g ha-1 d-1");
	}
}

void check_ndep_noywet(const GuessNC::CF::GridcellOrderedVariable* cf_var) {
	if (cf_var->get_standard_name() != "NOyWet" &&
		cf_var->get_standard_name() != "noywet" &&
		cf_var->get_standard_name() != "NOYWET") {
		fail("N-Deposition variable NOyWet doesn't seem to contain noywet data");
	}
	if (cf_var->get_units() != "g N ha-1 d-1") {
		fail("NOyWet variable given but unit doesn't seem to be g ha-1 d-1");
	}
}

// Checks if two variables contain data for the same time period
//
// Compares start and end of time series, the day numbers are only compared if
// both variables are daily.
void check_compatible_timeseries(const GuessNC::CF::GridcellOrderedVariable* var1,
                                 const GuessNC::CF::GridcellOrderedVariable* var2) {
	GuessNC::CF::DateTime start1, start2, end1, end2;

	const std::string error_message = format_string("%s and %s have incompatible timeseries",
		var1->get_variable_name().c_str(), var2->get_variable_name().c_str());

	start1 = var1->get_date_time(0);
	start2 = var2->get_date_time(0);

	end1 = var1->get_date_time(var1->get_timesteps() - 1);
	end2 = var2->get_date_time(var2->get_timesteps() - 1);

	if (start1.get_year() != start2.get_year() ||
		start1.get_month() != start2.get_month()) {
		fail(error_message.c_str());
	}

	if (end1.get_year() != end2.get_year() ||
		end1.get_month() != end2.get_month()) {
		fail(error_message.c_str());
	}

	if (is_daily(var1) && is_daily(var2)) {
		if (start1.get_day() != start2.get_day() ||
			end1.get_day() != end2.get_day()) {
			fail(error_message.c_str());
		}
	}
}

// Makes sure all variables have compatible time series
void check_compatible_timeseries(const std::vector<GuessNC::CF::GridcellOrderedVariable*> variables) {

	for (size_t i = 0; i < variables.size(); ++i) {
		for (size_t j = i + 1; j < variables.size(); ++j) {
			check_compatible_timeseries(variables[i], variables[j]);
		}
	}
}

void check_same_spatial_domains(const std::vector<GuessNC::CF::GridcellOrderedVariable*> variables) {

	for (size_t i = 1; i < variables.size(); ++i) {
		if (!variables[0]->same_spatial_domain(*variables[i])) {
			fail("%s and %s don't have the same spatial domain",
				variables[0]->get_variable_name().c_str(),
				variables[i]->get_variable_name().c_str());
		}
	}
}

}

SPInput::SPInput()
	: cf_temp(0),
	  cf_prec(0),
	  cf_insol(0),
	  cf_wetdays(0),
	  cf_min_temp(0),
	  cf_max_temp(0),
	  cf_ndep_nhxdry(0),
	  cf_ndep_nhxwet(0),
	  cf_ndep_noydry(0),
	  cf_ndep_noywet(0),
	  ndep_timeseries("historic") {

	declare_parameter("ndep_timeseries", &ndep_timeseries, 10, "Nitrogen deposition time series to use (historic, rcp26, rcp45, rcp60 or rcp85");

}

SPInput::~SPInput() {
	delete cf_temp;
	delete cf_prec;
	delete cf_insol;
	delete cf_wetdays;
	delete cf_min_temp;
	delete cf_max_temp;
	delete cf_ndep_nhxdry;
	delete cf_ndep_nhxwet;
	delete cf_ndep_noydry;
	delete cf_ndep_noywet;

	cf_temp = 0;
	cf_prec = 0;
	cf_insol = 0;
	cf_wetdays = 0;
	cf_min_temp = 0;
	cf_max_temp = 0;
	cf_ndep_nhxdry = 0;
	cf_ndep_nhxwet = 0;
	cf_ndep_noydry = 0;
	cf_ndep_noywet = 0;
}

void SPInput::init() {

	// Read CO2 data from file
	co2.load_file(param["file_co2"].str);

	// Try to open the NetCDF files
	try {
		cf_temp = new GridcellOrderedVariable(param["file_temp"].str, param["variable_temp"].str);
		cf_prec = new GridcellOrderedVariable(param["file_prec"].str, param["variable_prec"].str);
		cf_insol = new GridcellOrderedVariable(param["file_insol"].str, param["variable_insol"].str);

		if (param["file_wetdays"].str != "") {
			cf_wetdays = new GridcellOrderedVariable(param["file_wetdays"].str, param["variable_wetdays"].str);
		}

		if (param["file_min_temp"].str != "") {
			cf_min_temp = new GridcellOrderedVariable(param["file_min_temp"].str, param["variable_min_temp"].str);
		}

		if (param["file_max_temp"].str != "") {
			cf_max_temp = new GridcellOrderedVariable(param["file_max_temp"].str, param["variable_max_temp"].str);
		}

		cf_ndep_nhxdry = new GridcellOrderedVariable(param["file_ndep"].str, param["variable_nhxdry"].str);
		cf_ndep_nhxwet = new GridcellOrderedVariable(param["file_ndep"].str, param["variable_nhxwet"].str);
		cf_ndep_noydry = new GridcellOrderedVariable(param["file_ndep"].str, param["variable_noydry"].str);
		cf_ndep_noywet = new GridcellOrderedVariable(param["file_ndep"].str, param["variable_noywet"].str);
	}
	catch (const std::runtime_error& e) {
		fail(e.what());
	}

	// Make sure they contain what we expect

	check_temp_variable(cf_temp);

	check_prec_variable(cf_prec);

	check_insol_variable(cf_insol);

	check_wetdays_variable(cf_wetdays);

	if (cf_min_temp) {
		check_temp_variable(cf_min_temp);
	}

	if (cf_max_temp) {
		check_temp_variable(cf_max_temp);
	}

	check_compatible_timeseries(all_variables());

	check_same_spatial_domains(all_variables());

	extensive_precipitation = cf_prec->get_standard_name() == "precipitation_amount";

	check_ndep_nhxdry(cf_ndep_nhxdry);
	check_ndep_nhxwet(cf_ndep_nhxwet);
	check_ndep_noydry(cf_ndep_noydry);
	check_ndep_noywet(cf_ndep_noywet);

	check_compatible_timeseries(ndep_variables());
	check_same_spatial_domains(ndep_variables());

	// Read list of localities and store in gridlist member variable

	// Retrieve name of grid list file as read from ins file
	xtring file_gridlist=param["file_gridlist_cf"].str;

	std::ifstream ifs(file_gridlist, std::ifstream::in);

	if (!ifs.good()) fail("CFInput::init: could not open %s for input",(char*)file_gridlist);

	std::string line;
	while (getline(ifs, line)) {

		// Read next record in file
		int rlat, rlon;
		int landid;
		std::string descrip;
		Coord c;

		std::istringstream iss(line);

		if (cf_temp->is_reduced()) {
			if (iss >> landid) {
				getline(iss, descrip);

				c.landid = landid;
			}
		}
		else {
			if (iss >> rlon >> rlat) {
				getline(iss, descrip);

				c.rlat = rlat;
				c.rlon = rlon;

			}
		}
		c.descrip = trim(descrip);
		gridlist.push_back(c);
	}

	current_gridcell = gridlist.begin();

	// Open landcover files
	landcover_input.init();
	// Open management files
	management_input.init();
    
    // cw SubPixel
    // Open site files
    site_input.init();
    // Open landform files
    landform_input.init();

	// cw SubPixel we need to make sure that we add the offset from the calendar PALEO
	//             entry before get_year() is called; -1 netcdf units are expected to start with year 01
	date.set_first_calendar_year(cf_temp->get_date_time(0).get_year() - nyear_spinup +
								 cf_temp->get_paleo_offset());

	// Set timers
	tprogress.init();
	tmute.init();

	tprogress.settimer();
	tmute.settimer(MUTESEC);
}

bool SPInput::getgridcell(Gridcell& gridcell) {

	double lon, lat;
	int soilcode;

	// Load data for next gridcell, or if that fails, skip ahead until
	// we find one that works.
	while (current_gridcell != gridlist.end() &&
		   !load_data_from_files()){
		++current_gridcell;
	}

	if (current_gridcell == gridlist.end()) {
		// simulation finished
		return false;
	}

	// Get lon/lat for the gridcell
	if (cf_temp->is_reduced()) {
		cf_temp->get_coords_for(current_gridcell->landid, lon, lat);
	}
	else {
		cf_temp->get_coords_for(current_gridcell->rlon, current_gridcell->rlat, lon, lat);
	}
	gridcell.set_coordinates(lon, lat);
    
    // cw SubPixel
    if(run_landform) {
		bool LFworked = false;
		LFworked = site_input.loadsite(lon, lat, current_gridcell->landid);
		if (LFworked)
			gridcell.set_site(site_input.get_site());
			LFworked = landform_input.loadlandform(lon, lat, current_gridcell->landid);
		if (!LFworked) {
			dprintf("\nError: could not find landform info at (%g,%g) in site/landform data file(s)\n", lon, lat);
			return false;
		}
	}

	// Load spinup data for all variables

	load_spinup_data(cf_temp, spinup_temp);
	load_spinup_data(cf_prec, spinup_prec);
	load_spinup_data(cf_insol, spinup_insol);

	if (cf_wetdays) {
		load_spinup_data(cf_wetdays, spinup_wetdays);
	}

	if (cf_min_temp) {
		load_spinup_data(cf_min_temp, spinup_min_temp);
	}

	if (cf_max_temp) {
		load_spinup_data(cf_max_temp, spinup_max_temp);
	}

	load_spinup_data(cf_ndep_nhxdry, spinup_ndep_nhxdry);
	load_spinup_data(cf_ndep_nhxwet, spinup_ndep_nhxwet);
	load_spinup_data(cf_ndep_noydry, spinup_ndep_noydry);
	load_spinup_data(cf_ndep_noywet, spinup_ndep_noywet);

	spinup_temp.detrend_data();

	gridcell.climate.instype = cf_standard_name_to_insoltype(cf_insol->get_standard_name());

    // cw SubPixel
	// Setup the landforms/ stands and soil type

    // create stands for valid landforms
    std::vector<Landform> landforms = landform_input.get_landforms();
    for (size_t i = 0; i < landforms.size(); ++i) {
        Stand& stand = gridcell.create_stand(NATURAL, landforms[i]);
	}

	soilparameters(gridcell.soiltype, soilcode, site_input.get_site());

	historic_timestep_temp = -1;
	historic_timestep_prec = -1;
	historic_timestep_insol = -1;
	historic_timestep_wetdays = -1;
	historic_timestep_min_temp = -1;
	historic_timestep_max_temp = -1;
	historic_timestep_ndep_nhxdry = -1;
	historic_timestep_ndep_nhxwet = -1;
	historic_timestep_ndep_noydry = -1;
	historic_timestep_ndep_noywet = -1;

	dprintf("\nCommencing simulation for gridcell at (%g,%g)\n", lon, lat);
	if (current_gridcell->descrip != "") {
		dprintf("Description: %s\n", current_gridcell->descrip.c_str());
	}
	//dprintf("Using soil code and Nitrogen deposition for (%3.1f,%3.1f)\n", cru_lon, cru_lat);

	return true;
}

bool SPInput::load_data_from_files(){

	int rlon = current_gridcell->rlon;
	int rlat = current_gridcell->rlat;
	int landid = current_gridcell->landid;

	// Try to load the data from the NetCDF files

	if (cf_temp->is_reduced()) {
		if (!cf_temp->load_data_for(landid) ||
		    !cf_prec->load_data_for(landid) ||
		    !cf_insol->load_data_for(landid) ||
		    (cf_wetdays && !cf_wetdays->load_data_for(landid)) ||
		    (cf_min_temp && !cf_min_temp->load_data_for(landid)) ||
		    (cf_max_temp && !cf_max_temp->load_data_for(landid))) {
			dprintf("Failed to load data for (%d) from NetCDF files, skipping.\n", landid);
			return false;
		}
	}
	else {
		if (!cf_temp->load_data_for(rlon, rlat) ||
		    !cf_prec->load_data_for(rlon, rlat) ||
		    !cf_insol->load_data_for(rlon, rlat) ||
		    (cf_wetdays && !cf_wetdays->load_data_for(rlon, rlat)) ||
		    (cf_min_temp && !cf_min_temp->load_data_for(rlon, rlat)) ||
		    (cf_max_temp && !cf_max_temp->load_data_for(rlon, rlat))) {
			dprintf("Failed to load data for (%d, %d) from NetCDF files, skipping.\n", rlon, rlat);
			return false;
		}
	}

	if (cf_ndep_nhxdry->is_reduced()) {
		if (!cf_ndep_nhxdry->load_data_for(landid) ||
			!cf_ndep_nhxwet->load_data_for(landid) ||
			!cf_ndep_noydry->load_data_for(landid) ||
			!cf_ndep_noywet->load_data_for(landid)) {
			dprintf("Failed to load data for (%d) from NetCDF files, skipping.\n", landid);
			return false;
		}
	}
	else {
		if (!cf_ndep_nhxdry->load_data_for(rlon, rlat) ||
			!cf_ndep_nhxwet->load_data_for(rlon, rlat) ||
			!cf_ndep_noydry->load_data_for(rlon, rlat) ||
			!cf_ndep_noywet->load_data_for(rlon, rlat)) {
			dprintf("Failed to load data for (%d, %d) from NDep NetCDF files, skipping.\n", rlon, rlat);
		}
	}
	return true;
}

void SPInput::get_yearly_data(std::vector<double>& data,
                              const GenericSpinupData& spinup,
                              GridcellOrderedVariable* cf_historic,
                              int& historic_timestep) {
	// Extract all values for this year, for one variable,
	// either from spinup dataset or historical dataset

	int calendar_year = date.get_calendar_year();

	if (is_daily(cf_historic)) {

		data.resize(date.year_length());

		// This function is called at the first day of the year, so current_day
		// starts at Jan 1, then we step through the whole year, getting data
		// either from spinup or historical period.
		Date current_day = date;

		while (current_day.year == date.year) {

			// In the spinup?
			// WARNING THIS WAS NOT TESTED YET, WE ONLY HAVE MONTHLY PALEO DATA
			if (earlier_day(current_day, calendar_year, cf_historic->get_date_time(0), cf_temp->get_paleo_offset())) {

				int spinup_day = current_day.day;
				// spinup object always has 365 days, deal with leap years
				if (current_day.ndaymonth[1] == 29 && current_day.month > 1) {
					--spinup_day;
				}
				data[current_day.day]  = spinup[spinup_day];
			}
			else {
				// Historical period

				if (historic_timestep + 1 < cf_historic->get_timesteps()) {

					++historic_timestep;
					GuessNC::CF::DateTime dt = cf_historic->get_date_time(historic_timestep);

					// Deal with calendar mismatch

					// Leap day in NetCDF variable but not in LPJ-GUESS?
					if (dt.get_month() == 2 && dt.get_day() == 29 &&
						current_day.ndaymonth[1] == 28) {
						++historic_timestep;
					}
					// Leap day in LPJ-GUESS but not in NetCDF variable?
					else if (current_day.month == 1 && current_day.dayofmonth == 28 &&
						cf_historic->get_calendar_type() == NO_LEAP) {
						--historic_timestep;
					}
				}

				if (historic_timestep < cf_historic->get_timesteps()) {
					data[current_day.day]  = cf_historic->get_value(max(0, historic_timestep));
				}
				else {
					// Past the end of the historical period, these days wont be simulated.
					data[current_day.day] = data[max(0, current_day.day-1)];
				}
			}

			current_day.next();
		}
	}
	else {

		// for now, assume that data set must be monthly since it isn't daily

		data.resize(12);

		for (int m = 0; m < 12; ++m) {

			int first_year(0);
			GuessNC::CF::DateTime first_date = cf_historic->get_date_time(0);

			// cw If we have a paelo calendar we need to add it to start year
			if(cf_historic->get_calendar_type() == PALEO){
				first_year = first_date.get_year() + cf_historic->get_paleo_offset();
			} else {
				first_year = first_date.get_year();
			}
			
			// In the spinup?
			if (calendar_year < first_year ||
			    (calendar_year == first_year &&
			     m+1 < first_date.get_month())) {
				data[m] = spinup[m];
			}
			else {
				// Historical period
				if (historic_timestep + 1 < cf_historic->get_timesteps()) {
					++historic_timestep;
				}

				if (historic_timestep < cf_historic->get_timesteps()) {
					data[m] = cf_historic->get_value(historic_timestep);
				}
				else {
					// Past the end of the historical period, these months wont be simulated.
					data[m] = data[max(0, m-1)];
				}
			}
		}
	}
}

void SPInput::populate_daily_array(double* daily,
                                   const GenericSpinupData& spinup,
                                   GridcellOrderedVariable* cf_historic,
                                   int& historic_timestep,
                                   double minimum,
                                   double maximum) {

	// Get the data from spinup and/or historic
	std::vector<double> data;
	get_yearly_data(data, spinup, cf_historic, historic_timestep);

	if (is_daily(cf_historic)) {
		// Simply copy from data to daily

		std::copy(data.begin(), data.end(), daily);
	}
	else {
		// for now, assume that data set must be monthly since it isn't daily

		// Interpolate from monthly to daily values

		interp_monthly_means_conserve(&data.front(), daily, minimum, maximum);
	}
}

void SPInput::populate_daily_prec_array(long& seed) {

	// Get the data from spinup and/or historic
	std::vector<double> prec_data;
	get_yearly_data(prec_data, spinup_prec, cf_prec, historic_timestep_prec);

	std::vector<double> wetdays_data;
	if (cf_wetdays) {
		get_yearly_data(wetdays_data, spinup_wetdays, cf_wetdays, historic_timestep_wetdays);
	}

	if (is_daily(cf_prec)) {
		// Simply copy from data to daily, and if needed convert from
		// precipitation rate to precipitation amount

		for (size_t i = 0; i < prec_data.size(); ++i) {
			dprec[i] = prec_data[i];

			if (!extensive_precipitation) {
				dprec[i] *= SECONDS_PER_DAY;
			}
		}
	}
	else {
		// for now, assume that data set must be monthly since it isn't daily

		// If needed convert from precipitation rate to precipitation amount
		if (!extensive_precipitation) {
			for (int m = 0; m < 12; ++m) {
				// TODO: use the dataset's calendar type to figure out number of days in month?
				prec_data[m] *= SECONDS_PER_DAY * date.ndaymonth[m];
			}
		}

		if (cf_wetdays) {
			prdaily(&prec_data.front(), dprec, &wetdays_data.front(), seed);
		}
		else {
			interp_monthly_totals_conserve(&prec_data.front(), dprec, 0);
		}
	}
}

void SPInput::populate_daily_arrays(long& seed) {
	// Extract daily values for all days in this year, either from
	// spinup dataset or historical dataset

	populate_daily_array(dtemp, spinup_temp, cf_temp, historic_timestep_temp, 0);
	populate_daily_prec_array(seed);
	populate_daily_array(dinsol, spinup_insol, cf_insol, historic_timestep_insol, 0,
	                     max_insolation(cf_standard_name_to_insoltype(cf_insol->get_standard_name())));

	if (cf_min_temp) {
		populate_daily_array(dmin_temp, spinup_min_temp, cf_min_temp, historic_timestep_min_temp, 0);
	}

	if (cf_max_temp) {
		populate_daily_array(dmax_temp, spinup_max_temp, cf_max_temp, historic_timestep_max_temp, 0);
	}

	// Convert to units the model expects
	bool cloud_fraction_to_sunshine = (cf_standard_name_to_insoltype(cf_insol->get_standard_name()) == SUNSHINE);
	for (int i = 0; i < date.year_length(); ++i) {
		dtemp[i] -= K2degC;

		if (cf_min_temp) {
			dmin_temp[i] -= K2degC;
		}

		if (cf_max_temp) {
			dmax_temp[i] -= K2degC;
		}

		if (cloud_fraction_to_sunshine) {
			// Invert from cloudiness to sunshine,
			// and convert fraction (0-1) to percent (0-100)
			dinsol[i] = (1-dinsol[i]) * 100.0;
		}
	}

	// Move to next year in spinup dataset

	spinup_temp.nextyear();
	spinup_prec.nextyear();
	spinup_insol.nextyear();

	if (cf_wetdays) {
		spinup_wetdays.nextyear();
	}

	if (cf_min_temp) {
		spinup_min_temp.nextyear();
	}

	if (cf_max_temp) {
		spinup_max_temp.nextyear();
	}

	// Get monthly ndep values and convert to daily

	double mndrydep[12];
	double mnwetdep[12];

	// values copied into carrays at the end
	std::vector<double>nhxdry;
	std::vector<double>nhxwet;
	std::vector<double>noydry;
	std::vector<double>noywet;
	std::vector<double>dry;
	std::vector<double>wet;

	get_yearly_data(nhxdry, spinup_ndep_nhxdry, cf_ndep_nhxdry, historic_timestep_ndep_nhxdry);
	get_yearly_data(nhxwet, spinup_ndep_nhxwet, cf_ndep_nhxwet, historic_timestep_ndep_nhxwet);
	get_yearly_data(noydry, spinup_ndep_noydry, cf_ndep_noydry, historic_timestep_ndep_noydry);
	get_yearly_data(noywet, spinup_ndep_noywet, cf_ndep_noywet, historic_timestep_ndep_noywet);

	// make sure all vectors have the same length
	assert(allequal(nhxdry.size(), nhxwet.size(), noydry.size(), noywet.size()));

	dry.resize(nhxdry.size(), 0.0);
	wet.resize(nhxwet.size(), 0.0);

	// add them
	std::transform(nhxdry.begin(), nhxdry.end(), noydry.begin(), dry.begin(), std::plus<double>());
	std::transform(nhxwet.begin(), nhxwet.end(), noywet.begin(), wet.begin(), std::plus<double>());

	// apply unit conversion:
	const double convert = 1e-7;				// converting from gN ha-1 to kgN m-2

	// copy summed vectors to arrays
	std::transform(dry.begin(), dry.end(), dry.begin(), std::bind1st(std::multiplies<double>(),convert));
	std::transform(wet.begin(), wet.end(), wet.begin(), std::bind1st(std::multiplies<double>(),convert));

	std::copy(dry.begin(), dry.end(), mndrydep);
	std::copy(wet.begin(), wet.end(), mnwetdep);

	// Distribute N deposition
	distribute_ndep(mndrydep, mnwetdep, dprec, dndep);

	spinup_ndep_nhxdry.nextyear();
	spinup_ndep_nhxwet.nextyear();
	spinup_ndep_noydry.nextyear();
	spinup_ndep_noywet.nextyear();
}

void SPInput::getlandcover(Gridcell& gridcell) {

	landcover_input.getlandcover(gridcell);
	landcover_input.get_land_transitions(gridcell);
}

bool SPInput::getclimate(Gridcell& gridcell) {

	Climate& climate = gridcell.climate;

	GuessNC::CF::DateTime last_date = last_day_to_simulate(cf_temp);

	if (later_day(date, last_date, cf_temp->get_paleo_offset())) {
		++current_gridcell;
		return false;
	}

	climate.co2 = co2[date.get_calendar_year()];

	if (date.day == 0) {
		populate_daily_arrays(gridcell.seed);
	}

	climate.temp = dtemp[date.day];
	climate.prec = dprec[date.day];
	climate.insol = dinsol[date.day];

	// Nitrogen deposition
	climate.dndep = dndep[date.day];

	// bvoc
	if(ifbvoc){
		if (cf_min_temp && cf_max_temp) {
			climate.dtr = dmax_temp[date.day] - dmin_temp[date.day];
		}
		else {
			fail("When BVOC is switched on, valid paths for minimum and maximum temperature must be given.");
		}
	}

	// First day of year only ...

	if (date.day == 0) {

		// Progress report to user and update timer

		if (tmute.getprogress()>=1.0) {

			int first_historic_year = cf_temp->get_date_time(0).get_year();
			int last_historic_year = cf_temp->get_date_time(cf_temp->get_timesteps()-1).get_year();
			int historic_years = last_historic_year - first_historic_year + 1;

			int years_to_simulate = nyear_spinup + historic_years;

			int cells_done = distance(gridlist.begin(), current_gridcell);

			double progress=(double)(cells_done*years_to_simulate+date.year)/
				(double)(gridlist.size()*years_to_simulate);
			tprogress.setprogress(progress);
			dprintf("%3d%% complete, %s elapsed, %s remaining\n",(int)(progress*100.0),
				tprogress.elapsed.str,tprogress.remaining.str);
			tmute.settimer(MUTESEC);
		}
	}

    // cw SubPixel create local_climate conditions (dummy change for now)
    // loop over stand, copy data to local climate, apply local correction

    Gridcell::iterator gc_itr = gridcell.begin();
    while (gc_itr != gridcell.end())
    {
        Stand& stand = *gc_itr;
    
        // climate variation with respect to gridcell default
        // delta_elevation: diff between avg. landform elevation and gridcell elevation
        //                  > 0: landform is lower: delta_temp = positiv
        //                  < 0: landform is higher: delta_temp = negativ
        //
        // example:
        // 500m cell avg - 750m landform (peak) = -250m * 0.65 degrees/100m = -1.65 deg
        //

        // f_js_20171120 (could also be queried by the elevation value, which is set to -9999 in landform.cpp)
        if (stand.landform.id == 0)
          stand.landform.elevation = gridcell.get_site().elevation;
        
        double LAPSE_RATE = 0.65;    // 0.65 temp change per 100m elevation diff
        double delta_elevation = gridcell.get_site().elevation - stand.landform.elevation;
        double delta_temp = delta_elevation * (LAPSE_RATE / 100.0);

        double asp = stand.landform.avg_aspect;

        // f_js_20170223+
        // modify the radiation only for north and south facing slopes
        // and if there is any radiation at all
        if ((climate.insol > 0.0) && (stand.landform.avg_aspect != -1)) { //} && (stand.landform.aspect == NORTH || stand.landform.aspect == SOUTH)) {
            // f_js_20170222 solar declination as defined in driver.cpp (as 'delta' in function 'daylengthinsoleet')
            double d = -23.4 * DEGTORAD * cos(2.0 * PI * ((double)date.day + 10.5) / date.year_length());
            // solar angle at noon (0.0 means perpendicular, vertically, normal)
            double sola = climate.lat * DEGTORAD - d;
          
            // factor to project radiation on a surface normal to incoming radiation
            double norm = 1.0 / cos(sola);

            // facing towards the sun
            if ((((asp >= 90) && (asp < 270)) && (sola >= 0.0)) ||
                (((asp >= 270) || (asp < 90)) && (sola <= 0.0))) {
                //if ((stand.landform.aspect == SOUTH && sola >= 0.0) ||
                //    (stand.landform.aspect == NORTH && sola <= 0.0)) {
                stand.local_climate.insol = climate.insol * norm * cos(fabs(sola) - stand.landform.asp_slope * DEGTORAD);
			  
            // facing away from the sun
            } else if ((((asp >= 90) && (asp < 270)) && (sola < 0.0)) ||
                       (((asp >= 270) || (asp < 90)) && (sola > 0.0))) {
                // ((stand.landform.aspect == SOUTH && sola < 0.0) ||
                //      (stand.landform.aspect == NORTH && sola > 0.0)) {

                // catch if solar angle and slope are larger than 90deg (PI/2)
                if (fabs(sola) + stand.landform.slope * DEGTORAD > PI/2.0) {
                    stand.local_climate.insol = 0.0;
                } else {
                    stand.local_climate.insol = climate.insol * norm * cos(fabs(sola) + stand.landform.asp_slope * DEGTORAD);
                }
            }
        } else {
            stand.local_climate.insol = climate.insol;
        }

        if (stand.local_climate.insol < 0.0)
          fail("spinput: negative radiation in getclimate.\n");
        // f_js_20170223-
        
        stand.local_climate.temp  = climate.temp + delta_temp;
        stand.local_climate.prec  = climate.prec;
        stand.local_climate.co2   = climate.co2;
        //stand.local_climate.insol = climate.insol;
        
        // cw SubPixel this is currently not altered by changed precip (!)
        stand.local_climate.dndep = climate.dndep;
        
        if(ifbvoc){
            stand.local_climate.dtr = climate.dtr;
        }
        
        ++gc_itr;
    }
	
	return true;

}

void SPInput::load_spinup_data(const GuessNC::CF::GridcellOrderedVariable* cf_var,
                               GenericSpinupData& spinup_data) {

	const std::string error_message =
		format_string("Not enough data to build spinup, at least %d years needed",
		              NYEAR_SPINUP_DATA);

	GenericSpinupData::RawData source;

	int timestep = 0;

	bool daily = is_daily(cf_var);
	// for now, assume that each data set is either daily or monthly
	bool monthly = !daily;

	// Skip the first year if data doesn't start at the beginning of the year
	while ((daily && !first_day_of_year(cf_var->get_date_time(timestep))) ||
	       (monthly && !first_month_of_year(cf_var->get_date_time(timestep)))) {
		++timestep;

		if (timestep >= cf_var->get_timesteps()) {
			fail(error_message.c_str());
		}
	}

	// Get all the values for the first NYEAR_SPINUP_DATA years,
	// and put them into source
	for (int i = 0; i < NYEAR_SPINUP_DATA; ++i) {
		std::vector<double> year(daily ? GenericSpinupData::DAYS_PER_YEAR : 12);

		for (size_t i = 0; i < year.size(); ++i) {
			if (timestep < cf_var->get_timesteps()) {
				GuessNC::CF::DateTime dt = cf_var->get_date_time(timestep);

				if (daily && dt.get_month() == 2 && dt.get_day() == 29) {
					++timestep;
				}
			}

			if (timestep >= cf_var->get_timesteps()) {
				fail(error_message.c_str());
			}

			year[i] = cf_var->get_value(timestep);
			++timestep;
		}

		source.push_back(year);
	}

	spinup_data.get_data_from(source);
}

namespace {

// Help function for call to remove_if below - checks if a pointer is null
bool is_null(const GuessNC::CF::GridcellOrderedVariable* ptr) {
	return ptr == 0;
}

}

std::vector<GuessNC::CF::GridcellOrderedVariable*> SPInput::all_variables() const {
	std::vector<GuessNC::CF::GridcellOrderedVariable*> result;
	result.push_back(cf_temp);
	result.push_back(cf_prec);
	result.push_back(cf_insol);
	result.push_back(cf_wetdays);
	result.push_back(cf_min_temp);
	result.push_back(cf_max_temp);

	// Get rid of null pointers
	result.erase(std::remove_if(result.begin(), result.end(), is_null),
	             result.end());

	return result;
}

std::vector<GuessNC::CF::GridcellOrderedVariable*> SPInput::ndep_variables() const {
	std::vector<GuessNC::CF::GridcellOrderedVariable*> result;
	result.push_back(cf_ndep_nhxdry);
	result.push_back(cf_ndep_nhxwet);
	result.push_back(cf_ndep_noydry);
	result.push_back(cf_ndep_noywet);

	// Get rid of null pointers
	result.erase(std::remove_if(result.begin(), result.end(), is_null),
				 result.end());

	return result;
}

#endif // HAVE_NETCDF
