//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file landform.cpp
/// \brief Functions handling landform aspects
///
/// Landform functionality.
///
/// \author Christian Werner,
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "landform.h"

#ifdef HAVE_NETCDF

#include "guess.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void check_same_spatial_domains(const std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> variables) {
	
	for (size_t i = 1; i < variables.size(); ++i) {
		if (!variables[0]->same_spatial_domain(*variables[i])) {
			fail("%s and %s don't have the same spatial domain",
				 variables[0]->get_variable_name().c_str(),
				 variables[i]->get_variable_name().c_str());
		}
	}
}

using namespace GuessNC::CF;

namespace {

	bool is_null_NoTime(const GuessNC::CF::GridcellOrderedVariableNoTime* ptr) {
		return ptr == 0;
	}

	// Verifies that a NC variable with sand data contains what we expect
	void check_sand_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "sand") {
			fail("Sand variable doesn't seem to contain sand fraction data");
		}
		if (nc_var->get_units() != "percent" && nc_var->get_units() != "%") {
			fail("Sand variable doesn't seem to be in percent");
		}
	}

	// Verifies that a NC variable with silt data contains what we expect
	void check_silt_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "silt") {
			fail("Silt variable doesn't seem to contain silt fraction data");
		}
		if (nc_var->get_units() != "percent" && nc_var->get_units() != "%") {
			fail("Silt variable doesn't seem to be in percent");
		}
	}
	
	// Verifies that a NC variable with clay data contains what we expect
	void check_clay_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "clay") {
			fail("Clay variable doesn't seem to contain clay fraction data");
		}
		if (nc_var->get_units() != "percent" && nc_var->get_units() != "%") {
			fail("Clay variable doesn't seem to be in percent");
		}
	}
	
	// Verifies that a NC variable with soc data contains what we expect
	void check_soc_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "soc") {
			fail("SOC variable doesn't seem to contain SOC fraction data");
		}
		if (nc_var->get_units() != "percent" && nc_var->get_units() != "%") {
			fail("SOC variable doesn't seem to be in percent");
		}
	}
	
	// Verifies that a NC variable with elevation data contains what we expect
	void check_elevation_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "elevation" && nc_var->get_standard_name() != "altitude") {
			fail("Elevation variable doesn't seem to contain elevation data");
		}
		if (nc_var->get_units() != "m" && nc_var->get_units() != "meters") {
			fail("Elevation variable doesn't seem to be in meters");
		}
	}
	
	// Verifies that a NC variable with fraction data contains what we expect
	void check_fraction_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "fraction" && nc_var->get_standard_name() != "frac") {
			fail("Fraction variable doesn't seem to contain fraction data");
		}
		if (nc_var->get_units() != "1/1") {
			fail("Fraction variable doesn't seem to be dimensionless (1/1)");
		}
	}
	
	// Verifies that a NC variable with slope data contains what we expect
	void check_slope_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "slope") {
			fail("Slope variable doesn't seem to contain slope data");
		}
		if (nc_var->get_units() != "deg") {
			fail("Slope variable doesn't seem to be in deg(rees) (0-90)");
		}
	}

    // Verifies that a NC variable with asp_slope data contains what we expect
    void check_asp_slope_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
        if (nc_var->get_standard_name() != "asp_slope") {
            fail("Slope variable doesn't seem to contain slope data");
        }
        if (nc_var->get_units() != "deg") {
            fail("Slope variable doesn't seem to be in deg(rees) (0-90)");
        }
    }

    // Verifies that a NC variable with aspect data contains what we expect
    void check_aspect_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
        if (nc_var->get_standard_name() != "aspect") {
            fail("Aspect variable doesn't seem to contain aspect data");
        }
        if (nc_var->get_units() != "deg") {
            fail("Aspect variable doesn't seem to be in deg(rees) (0-360)");
        }
    }
    
    
    
	// this will be enabled later
	/*
	// Verifies that a NC variable with soildepth data contains what we expect
	void check_soildepth_variable(const GuessNC::CF::GridcellOrderedVariableNoTime* nc_var) {
		if (nc_var->get_standard_name() != "soildepth") {
			fail("Soildepth variable doesn't seem to contain soildepth data");
		}
		if (nc_var->get_units() != "m" && nc_var->get_units() != "meters") {
			fail("Soildepth variable doesn't seem to be in meters");
		}
	}
	
	
	void check_same_spatial_domains(const std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> variables) {
		
		for (size_t i = 1; i < variables.size(); ++i) {
			if (!variables[0]->same_spatial_domain(*variables[i])) {
				fail("%s and %s don't have the same spatial domain",
					 variables[0]->get_variable_name().c_str(),
					 variables[i]->get_variable_name().c_str());
			}
		}
	}
	 */
	
}
// cw SubPixel - Site Input Class

SiteInput::SiteInput() {}

SiteInput::~SiteInput() {
	delete nc_elevation;
	delete nc_sand;
	delete nc_silt;
	delete nc_clay;
	delete nc_soc;
	delete nc_soildepth;
	
	nc_elevation = 0;
	nc_sand = 0;
	nc_silt = 0;
	nc_clay = 0;
	nc_soc = 0;
	nc_elevation = 0;
}


void SiteInput::init() {
	// Try to open the NetCDF files
	try {
		nc_elevation = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_elevation"].str);
		nc_sand = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_sand"].str);
		nc_silt = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_silt"].str);
		nc_clay = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_clay"].str);
		nc_soc = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_soc"].str);
		// nc_soildepth = new GridcellOrderedVariableNoTime(param["file_site"].str, param["variable_soildepth"].str);
	}
	catch (const std::runtime_error& e) {
		fail(e.what());
	}
	
	// Make sure they contain what we expect
	
	check_elevation_variable(nc_elevation);
	
	check_sand_variable(nc_sand);
	
	check_silt_variable(nc_silt);
	
	check_clay_variable(nc_clay);
	
	check_soc_variable(nc_soc);
	
	//check_soildepth_variable(nc_soildepth);

	check_same_spatial_domains(all_variables());

}

bool SiteInput::loadsite(double lon, double lat, int landid){
	dprintf("\nAttempting to load Site NC data\n");
	// Try to load the data from the NetCDF files
	if (nc_sand->is_reduced()) {
		if (!nc_sand->load_data_for(landid) ||
			!nc_elevation->load_data_for(landid) ||
			!nc_silt->load_data_for(landid) ||
			!nc_clay->load_data_for(landid) ||
			!nc_soc->load_data_for(landid)
			//!nc_soildepth->load_data_for(landid)
			) {
			dprintf("Failed to load data for (%d) from NetCDF files, skipping.\n", landid);
			return false;
		}
	}
	else {
		if (!nc_elevation->load_data_for(lat, lon) ||
			!nc_sand->load_data_for(lat, lon) ||
			!nc_silt->load_data_for(lat, lon) ||
			!nc_clay->load_data_for(lat, lon) ||
			!nc_soc->load_data_for(lat, lon)
			//!nc_soildepth->load_data_for(lat, lon)
			) {
			dprintf("Failed to load data for (%d, %d) from NetCDF files, skipping.\n", lon, lat);
			return false;
		}
	}
	
	site.elevation = nc_elevation->get_value();
	site.sand = nc_sand->get_value();
	site.silt = nc_silt->get_value();
	site.clay = nc_clay->get_value();
	site.soc = nc_soc->get_value();
	// this will be enabled later
	// site.soildepth = nc_soildepth->get_value();
	
	dprintf("\n SITE elevation: %g\n", site.elevation);
	dprintf("           sand: %g\n", site.sand);
	dprintf("           silt: %g\n", site.silt);
	dprintf("           clay: %g\n", site.clay);
	dprintf("            soc: %g\n", site.soc);
	dprintf("     soil depth: %g\n", site.soildepth);
	
	return true;
};


Site SiteInput::get_site() const{
	return site;
};


std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> SiteInput::all_variables() const {
	std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> result;
	result.push_back(nc_elevation);
	result.push_back(nc_sand);
	result.push_back(nc_silt);
	result.push_back(nc_clay);
	result.push_back(nc_soc);
	// this will be enabled later
	//result.push_back(nc_soildepth);
	
	// Get rid of null pointers
	result.erase(std::remove_if(result.begin(), result.end(), is_null_NoTime),
				 result.end());
	
	return result;
}


// cw SubPixel - Landform Input Class


LandformInput::LandformInput()
	: nc_fraction(0),
	nc_elevation(0),
	nc_slope(0),
    nc_aspect(0),
    nc_asp_slope(0),
	nc_soildepth(0) {}
	
LandformInput::~LandformInput() {
	delete nc_fraction;
	delete nc_elevation;
	delete nc_slope;
    delete nc_aspect;
    delete nc_asp_slope;
	delete nc_soildepth;
	
	nc_fraction = 0;
	nc_elevation = 0;
	nc_slope = 0;
    nc_aspect = 0;
    nc_asp_slope = 0;
	nc_soildepth = 0;
}
	
void LandformInput::init() {
	
	if(!run_landform)
		return;
	
	
	// Try to open the NetCDF files
	try {
		nc_fraction = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_fraction"].str);
		nc_elevation = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_elevation"].str);
		nc_slope = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_slope"].str);
        nc_aspect = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_aspect"].str);
        nc_asp_slope = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_asp_slope"].str);
        //nc_soildepth = new GridcellOrderedVariableNoTime(param["file_landform"].str, param["variable_soildepth"].str);
	}
	catch (const std::runtime_error& e) {
		fail(e.what());
	}
	
	// Make sure they contain what we expect
	
	check_fraction_variable(nc_fraction);
	
	check_elevation_variable(nc_elevation);
	
	check_slope_variable(nc_slope);

    check_aspect_variable(nc_aspect);
    
    check_asp_slope_variable(nc_asp_slope);
    
	//check_soildepth_variable(nc_soildepth);
	
	check_same_spatial_domains(all_variables());
}

	
bool LandformInput::loadlandform(double lon, double lat, int landid) {
	
	// Try to load the data from the NetCDF files
	dprintf("\nAttempting to load Landform NC data\n");

	if (nc_fraction->is_reduced()) {
		if (!nc_fraction->load_data_for(landid) ||
			!nc_elevation->load_data_for(landid) ||
			!nc_slope->load_data_for(landid) ||
            !nc_aspect->load_data_for(landid) ||
            !nc_asp_slope->load_data_for(landid)
			//!nc_soildepth->load_data_for(landid)
			) {
			dprintf("Failed to load data for (%d) from NetCDF files, skipping.\n", landid);
			return false;
		}
	}
	else {
		if (!nc_fraction->load_data_for(lon, lat) ||
			!nc_elevation->load_data_for(lon, lat) ||
            !nc_slope->load_data_for(lon, lat) ||
            !nc_aspect->load_data_for(lon, lat) ||
            !nc_asp_slope->load_data_for(lon, lat)
			//!nc_soildepth->load_data_for(lon, lat)
			) {
			dprintf("Failed to load data for (%d, %d) from NetCDF files, skipping.\n", lon, lat);
			return false;
		}
	}
	
	std::vector<int> lf_ids;
	std::vector<aspect_type> aspect;
  std::vector<slopepos_type> slopepos;

	std::vector<double> fraction;
  std::vector<double> slope;
  std::vector<double> avg_aspect;
  std::vector<double> asp_slope;
  std::vector<double> elevation;
	std::vector<bool> lf_valid;
	
	nc_elevation->get_values(elevation, lf_valid);
	nc_fraction->get_values(fraction);
	nc_slope->get_values(slope);
  nc_asp_slope->get_values(asp_slope);
  nc_aspect->get_values(avg_aspect);

  // derive lf_ids, classification and stwp_width from netcdf file
  nc_elevation->get_landforms(lf_ids);
    
  // not required atm
  // std::string lgt_classification;
  // int lgt_stepwidth;
  // nc_elevation->get_lgt_classification(lgt_classification);
  // nc_elevation->get_lgt_elevation_step(lgt_stepwidth);

	aspect_type aspect_lookup[] = {NOASPECT, NORTH, EAST, SOUTH, WEST};
    
	// loop over possible landforms
	landforms.clear();
	int _cnt(1);
	for (size_t i = 0; i < lf_ids.size(); i++){
		if (lf_valid[i]){
			Landform lf;
			lf.id = lf_ids[i];
            
      int aspect_class = lf.id % 10;       // aspect class = last digit
      int slope_class = (lf.id / 10) % 10; // slope class = second last digit
      // int elevation_class = lf.id / 100;   // elevation class = front digits

			lf.aspect = aspect_lookup[aspect_class];
			lf.elevation = elevation[i];
			lf.fraction = fraction[i];
            lf.avg_aspect = avg_aspect[i];
			lf.slope = slope[i];
            lf.asp_slope = asp_slope[i];

            // mod lower soil layer depth according to landform (slope component)
            //
            // 1 (hilltop) * 0.25 -> 0.75
            // 2 (upslope) * 0.33 ? -> 1m
            // 3 (mid-slope) * 0.5 -> 1m
            // 4 (mesa/ flat) * 1 -> 1.5m (orig)
            // 5 (low-slope) * 1.25 -> 1.75m
            // 6 (valley) * 1.5  -> 2m
            switch (slope_class) {
                case RIDGE:
                    lf.soildepth_lower_mod = 0.25;
                    lf.avg_aspect = -1;
                    break;
                case UPSLOPE:
                    lf.soildepth_lower_mod = 0.33; break;
                case MIDSLOPE:
                    lf.soildepth_lower_mod = 0.5; break;
                case FLAT:
                    lf.soildepth_lower_mod = 1.0;
                    lf.avg_aspect = -1;
                    // deepwater
                    if(ifdeepwater) {
                        lf.has_deepwater = true;
                    }
                    break;
                case DOWNSLOPE:
                    lf.soildepth_lower_mod = 1.25;
                    // deepwater
                    if(ifdeepwater) {
                        lf.has_deepwater = true;
                    }
                    break;
                case VALLEY:
                    lf.soildepth_lower_mod = 1.5;
                    lf.avg_aspect = -1;
                    
                    // deepwater
                    if(ifdeepwater) {
                        lf.has_deepwater = true;
                    }
                    break;
                default:
                    lf.soildepth_lower_mod = 1.0;
                    lf.avg_aspect = -1;
                    break;
            }
            
            if (sp_modifysoildepth == false) lf.soildepth_lower_mod = 1.0;
			landforms.push_back(lf);

			dprintf("\n LANDFORM id: %d (%d)\n", _cnt, lf.id);
			dprintf("      aspect: %d\n", lf.aspect);
			dprintf("   elevation: %g\n", lf.elevation);
			dprintf("    fraction: %g\n", lf.fraction);
            dprintf("      aspect: %g\n", lf.avg_aspect);
			dprintf("       slope: %g\n", lf.slope);
            dprintf(" (asp) slope: %g\n", lf.asp_slope);
            dprintf("   soildepth: %g\n", (SOILDEPTH_UPPER + SOILDEPTH_LOWER * lf.soildepth_lower_mod));

			_cnt += 1;
		}
	}

  // f_js_20171110 standard/unmodified landform at landform position 0
  Landform lf;
  lf.id = 0;
  lf.aspect     = aspect_lookup[0];
  lf.elevation  = -9999.0;
  lf.fraction   = 0.0;
  lf.avg_aspect = 0.0;
  lf.slope      = 0.0;
  lf.asp_slope  = 0.0;
  lf.soildepth_lower_mod = 1.0;
  lf.avg_aspect = -1;
  landforms.insert(landforms.begin(), lf);
  
	return true;
}


std::vector<Landform> LandformInput::get_landforms() const {
	return landforms;
}

std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> LandformInput::all_variables() const {
	std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> result;
	result.push_back(nc_fraction);
	result.push_back(nc_elevation);
	result.push_back(nc_slope);
    result.push_back(nc_aspect);
    result.push_back(nc_asp_slope);
	result.push_back(nc_soildepth);
	
	// Get rid of null pointers
	result.erase(std::remove_if(result.begin(), result.end(), is_null_NoTime),
				 result.end());
	
	return result;
}




#endif // HAVE_NETCDF
