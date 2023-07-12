//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file outputmodule.cpp
/// \brief Implementation of the common output module
///
/// \author Joe Siltberg
/// $Date: 2016-12-16 16:21:33 +0100 (Fri, 16 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "spbenchmarkoutput.h"
#include "parameters.h"
#include "guess.h"

namespace GuessOutput {

REGISTER_OUTPUT_MODULE("sp_benchmark", BenchmarkOutput)

BenchmarkOutput::BenchmarkOutput() {

	// MF Fire season length and litter for re-fitting GlobFIRM
	declare_parameter("file_vegcover", &file_vegcover, 300, "Fraction of land covered by different vegetation types");
	declare_parameter("file_canopyheight", &file_canopyheight, 300, "Mean height of top of canopy");
	declare_parameter("file_mfpar", &file_mfpar, 300, "Monthly fraction of photosynthetic radiation absorbed");

}


BenchmarkOutput::~BenchmarkOutput() {
}

/// Define all output tables and their formats
void BenchmarkOutput::init() {

	define_output_tables();
}

/** This function specifies all columns in all output tables, their names,
 *  column widths and precision.
 *
 *  For each table a TableDescriptor object is created which is then sent to
 *  the output channel to create the table.
 */
void BenchmarkOutput::define_output_tables() {

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

	// Create the month columns
	ColumnDescriptors month_columns;
	ColumnDescriptors month_columns_wide;
	xtring months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	for (int i = 0; i < 12; i++) {
		month_columns      += ColumnDescriptor(months[i], 8,  3);
		month_columns_wide += ColumnDescriptor(months[i], 10, 3);
	}

	// VEGCOVER
	ColumnDescriptors vegcover_columns;
	vegcover_columns += ColumnDescriptor("Tree", 8, 3);
	vegcover_columns += ColumnDescriptor("Grass", 8, 3);
	vegcover_columns += ColumnDescriptor("Bare", 8, 3);

	// CANOPY HEIGHT
	ColumnDescriptors canopyheight_columns;
	canopyheight_columns += ColumnDescriptor("CanHght", 8, 1);


	// *** ANNUAL OUTPUT VARIABLES ***

	create_output_table(out_vegcover, file_vegcover, vegcover_columns);
	create_output_table(out_canopyheight, file_canopyheight, canopyheight_columns);
	create_output_table(out_mfpar, file_mfpar, month_columns);


}


/// Local analogue of OutputRows::add_value for restricting output
/** Use to restrict output to specified range of years
  * (or other user-specified limitation)
  *
  * If only yearly output between, say 1961 and 1990 is requred, use:
  *  if (date.get_calendar_year() >= 1961 && date.get_calendar_year() <= 1990)
  *  (assuming the input module has set the first calendar year in the date object)	
  */
void outlimit_benchmark(OutputRows& out, const Table& table, double d) {

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
  * Restrict output to specific years in the local helper function outlimit().
  *
  * Changes in the structure of this function should be mirrored in outannual()
  * of the other output modules, e.g. MiscOutput::outannual().
  */
void BenchmarkOutput::outannual(Gridcell& gridcell) {

	int c, m;

	// Annual
	double treecover_gridcell = 0.0;
	double barecover_gridcell = 0.0;
	double grasscover_gridcell = 0.0;
	double canopyheight_gridcell = 0.0;

	// Monthly
	double mfpar[12];


	double lon = gridcell.get_lon();
	double lat = gridcell.get_lat();

	// The OutputRows object manages the next row of output for each
	// output table
    OutputRows out(output_channel, lon, lat, date.get_calendar_year());


	// guess2008 - reset monthly and annual sums across patches each year
	for (m=0;m<12;m++) {
		mfpar[m]=0.0;
	}


	// *** Loop through PFTs ***

	pftlist.firstobj();
	while (pftlist.isobj) {

		Pft& pft=pftlist.getobj();
		Gridcellpft& gridcellpft=gridcell.pft[pft.id];

		// Sum per-PFT output such as monthly C fluxes, litter, instances of leaf shedding across patches

		// MF: Phenology checks
		//mean_standpft_aleafshed = 0.0;


		double heightindiv_total = 0.0;

		// Determine area fraction of stands where this pft is active:
		double active_fraction = 0.0;

		Gridcell::iterator gc_itr = gridcell.begin();

		while (gc_itr != gridcell.end()) {
			Stand& stand = *gc_itr;

			if(stand.pft[pft.id].active) {
				active_fraction += stand.get_gridcell_fraction();
			}

			++gc_itr;
		}

		// Loop through Stands
		gc_itr = gridcell.begin();

		while (gc_itr != gridcell.end()) {
			Stand& stand = *gc_itr;

			Standpft& standpft=stand.pft[pft.id];
			if(standpft.active) {

			stand.firstobj();

			// Loop through Patches
			while (stand.isobj) {
				Patch& patch = stand.getobj();
				Patchpft& patchpft = patch.pft[pft.id];
				Vegetation& vegetation = patch.vegetation;

				double to_gridcell_average = stand.get_gridcell_fraction() / (double)stand.npatch();

					// Monthly *and* perPFT output (initially for FireMIP)
					for(m = 0; m <12; m++){
						//mean_standpft_monthly_gpp[m] += 0; //patchpft.mgpp[m] * to_gridcell_average;
					}

					stand.nextobj();
				} // end of patch loop


			}//if(active)

			++gc_itr;

		}//End of loop through stands

		pftlist.nextobj();

	} // *** End of PFT loop ***




	// Loop through stands and patches

	Gridcell::iterator gc_itr = gridcell.begin();

	// Loop through Stands
	while (gc_itr != gridcell.end()) {
		Stand& stand = *gc_itr;
		stand.firstobj();

        int stand_id(stand.id);
        if (run_landform) {
            stand_id = stand.landform.id;
        }
        
        
		//Loop through Patches
		while (stand.isobj) {
			Patch& patch = stand.getobj();

			double to_gridcell_average = stand.get_gridcell_fraction() / (double)stand.npatch();


			// Calculate vegetation cover
			double treecover_patch = 0;
			double grasscover_patch = 0;
			double barecover_patch = 0;

            // The OutputRows object manages the next row of output for each
            // output table
            OutputRows out(output_channel, lon, lat, date.get_calendar_year(), -1, stand_id, patch.id);
            
			Vegetation& vegetation = patch.vegetation;

			vegetation.firstobj();
			while (vegetation.isobj) {
				Individual& indiv = vegetation.getobj();
                if (indiv.id != -1 && indiv.alive) {
                    if (indiv.pft.lifeform == TREE && indiv.height > 5) {
                        treecover_patch += indiv.fpc;
                    } else if (indiv.pft.lifeform == GRASS || indiv.age > 0 ) {
                        grasscover_patch += indiv.fpc;
                    }
                }
				vegetation.nextobj();
			}

			treecover_patch = min(treecover_patch, 1.0);
			grasscover_patch = min(grasscover_patch, 1.0);
			grasscover_patch = min(grasscover_patch, 1 - treecover_patch);
			barecover_patch = max(0.0, 1 - treecover_patch - grasscover_patch);
			treecover_gridcell += treecover_patch * to_gridcell_average;
			grasscover_gridcell += grasscover_patch * to_gridcell_average;
			barecover_gridcell += barecover_patch * to_gridcell_average;


			// Calculate average top of canopy height

			// STEP 1: Copy all heights and fpc of all vegetation which is not a grass into vectors
			std::vector<double> fpc_vector;
			std::vector<double> height_vector;

			vegetation.firstobj();
			while (vegetation.isobj) {
				Individual& indiv = vegetation.getobj();
				if (indiv.pft.lifeform != GRASS) {
					fpc_vector.push_back(indiv.fpc);
					height_vector.push_back(indiv.height);
				}
				vegetation.nextobj();
			}

			// STEP 2:
			size_t vector_length;
			double cumulative_fpc = 0.0;
			double max_height = 0.0;
			double average_height = 0.0;
			int tallest = -1;
			//dprintf("START\n");
			bool breakout = false;
			while(cumulative_fpc <= 1.0 && height_vector.size() > 0 && !breakout) {


				// get the index of the tallest
				max_height = 0.0;
				vector_length = height_vector.size();
				tallest = -1;
				for(int index = 0; index < vector_length; index++) {

					//dprintf("height = %f, fpc = %f\n", height_vector[index], fpc_vector[index]);

					if(height_vector[index] > max_height) {
						tallest = index;
						max_height = height_vector[index];
						//dprintf("in loop taking height = %f, fpc = %f\n", height_vector[index], fpc_vector[index]);
					}

				}

				// if found the tallest, update the sums and delete the tallest
				if(tallest > -1) {

					//dprintf("taking height = %f, fpc = %f\n", height_vector[tallest], fpc_vector[tallest]);

					//  add the sums
					cumulative_fpc += fpc_vector[tallest];
					average_height += fpc_vector[tallest] * height_vector[tallest];

					// remove tallest (from both vectors!)
					fpc_vector.erase(fpc_vector.begin() + tallest);
					height_vector.erase(height_vector.begin() + tallest);

					//dprintf("n left = %i, cumulative_fpc = %f\n", height_vector.size(), cumulative_fpc);
				}

				// else no tallest then breakout
				else{
					breakout = true;
				}

			}

			if(cumulative_fpc > 0.0) average_height /= cumulative_fpc;
			canopyheight_gridcell += average_height * to_gridcell_average;
			//dprintf("canopy height = %f\n", average_height);


			// Calculate monthly FAPAR

			/*

			vegetation.firstobj();
			while (vegetation.isobj) {
				Individual& indiv = vegetation.getobj();

				// guess2008 - alive check added
				if (indiv.id != -1 && indiv.alive) {

					for (m=0;m<12;m++) {
						mfpar[m] += indiv.mlai[m] * to_gridcell_average;
					}

				} // alive?

				vegetation.nextobj();
			} // while/vegetation loop

			*/
            
            // Print out the annual values
            outlimit_benchmark(out, out_vegcover, treecover_patch);
            outlimit_benchmark(out, out_vegcover, grasscover_patch);
            outlimit_benchmark(out, out_vegcover, barecover_patch);
            outlimit_benchmark(out, out_canopyheight, average_height);


			stand.nextobj();
		} // patch loop

		++gc_itr;
	} // stand loop


	// Print monthly output variables
	/*
	for (m=0;m<12;m++) {
		outlimit_benchmark(out, out_mfpar,  mfpar[m]);
	}
	*/



}


/// Output of simulation results at the end of each day
/** This function does not have to provide any information to the framework.
  */
void BenchmarkOutput::outdaily(Gridcell& gridcell) {
}

} // namespace
