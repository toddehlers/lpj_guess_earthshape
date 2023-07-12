//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file framework.cpp
/// \brief Implementation of the framework() function
///
/// \author Ben Smith
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "framework.h"
#include "commandlinearguments.h"
#include "guessserializer.h"
#include "parallel.h"

#include "inputmodule.h"
#include "driver.h"
#include "canexch.h"
#include "soilwater.h"
#include "somdynam.h"
#include "growth.h"
#include "vegdynam.h"
#include "landcover.h"
#include "bvoc.h"
#include "commonoutput.h"

#include <memory>

/// Prints the date and time together with the name of this simulation
void print_logfile_heading() {
	xtring datetime;
	unixtime(datetime);

	xtring header = xtring("[LPJ-GUESS  ") + datetime + "]\n\n";
	dprintf((char*)header);

	// Print the title of this run
	std::string dashed_line(50, '-');
	dprintf("\n\n%s\n%s\n%s\n",
	        dashed_line.c_str(), (char*)title, dashed_line.c_str());
}

/// Simulate one day for a given Gridcell
/**
 * The climate object in the gridcell needs to be set up with
 * the day's forcing data before calling this function.
 *
 * \param gridcell            The gridcell to simulate
 * \param input_module        Used to get land cover fractions
 */
void simulate_day(Gridcell& gridcell, InputModule* input_module) {

	// Update daily climate drivers etc
    // cw SubPixel call dailaccounting on gridcell.climate
	dailyaccounting_gridcell(gridcell, gridcell.climate);

	// Calculate daylength, insolation and potential evapotranspiration
	daylengthinsoleet(gridcell.climate);

	if (run_landcover) {
		if (run[CROPLAND]) {
			// Update crop sowing date calculation framework
			crop_sowing_gridcell(gridcell);
		}
		if (date.day == 0) {
			// Dynamic landcover and crop fraction data during historical
			// period and create/kill stands.
			landcover_dynamics(gridcell, input_module);

			// Update dynamic management options
			input_module->getmanagement(gridcell);
		}
	}


	Gridcell::iterator gc_itr = gridcell.begin();
	while (gc_itr != gridcell.end()) {

		// START OF LOOP THROUGH STANDS
		Stand& stand = *gc_itr;

		dailyaccounting_stand(stand);

    // cw SubPixel call dailyaccounting on stand.local_climate
    dailyaccounting_gridcell(gridcell, stand.local_climate);
    daylengthinsoleet(stand.local_climate);

		stand.firstobj();
		while (stand.isobj) {
			// START OF LOOP THROUGH PATCHES

			// Get reference to this patch
			Patch& patch = stand.getobj();
			// Update daily soil drivers including soil temperature
			dailyaccounting_patch(patch);

			// Determine nitrogen fertilisation amount
			if(run_landcover)
				nfert(patch);

			if (stand.landcover == CROPLAND) {
				// Calculate crop sowing dates
				crop_sowing_patch(patch);
				// Crop phenology
				crop_phenology(patch);
				// necessary updates after changing growingperiod status
				update_patch_fpc(patch);
			}

      // cw SubPixel - we now pass stand.local_climate instead of gridcell.climate
			// Leaf phenology for PFTs and individuals
      leaf_phenology(patch, stand.local_climate);	// gridcell.climate);
			// Interception
      interception(patch, stand.local_climate);   // gridcell.climate);
      initial_infiltration(patch, stand.local_climate); // gridcell.climate);
			// Photosynthesis, respiration, evapotranspiration
      canopy_exchange(patch, stand.local_climate); // gridcell.climate);
			// Sum total required irrigation
			irrigation(patch);
			// Soil water accounting, snow pack accounting
      soilwater(patch, stand.local_climate); // gridcell.climate);
			// Daily C allocation (cropland)
			growth_daily(patch);
			// Soil organic matter and litter dynamics
			som_dynamics(patch);

			if (date.islastday && date.islastmonth) {

				// LAST DAY OF YEAR
				// Tissue turnover, allocation to new biomass and reproduction,
				// updated allometry
				growth(stand, patch);
			}
			stand.nextobj();
		}// End of loop through patches

		// Update crop rotation status
		crop_rotation(stand);

		if (date.islastday && date.islastmonth) {
			// LAST DAY OF YEAR
			stand.firstobj();
			while (stand.isobj) {

				// For each patch ...
				Patch& patch = stand.getobj();
				// Establishment, mortality and disturbance by fire
				vegetation_dynamics(stand, patch);
				stand.nextobj();
			}
		}

		++gc_itr;
	}	// End of loop through stands
}


int framework(const CommandLineArguments& args) {

	// The 'mission control' of the model, responsible for maintaining the
	// primary model data structures and containing all explicit loops through
	// space (grid cells/stands) and time (days and years).

	using std::auto_ptr;

	const char* input_module_name = args.get_input_module();

	auto_ptr<InputModule> input_module(InputModuleRegistry::get_instance().create_input_module(input_module_name));

  // WK: add output for LandLab here:
	GuessOutput::OutputModuleContainer output_modules;
	GuessOutput::OutputModuleRegistry::get_instance().create_all_modules(output_modules);

	// Read the instruction file to obtain PFT static parameters and
	// simulation settings
	read_instruction_file(args.get_instruction_file());

	// Initialise input/output

	input_module->init();
	output_modules.init();

	print_logfile_heading();

	// Nitrogen limitation
	if (ifnlim && !ifcentury) {
		fail("\n\nIf nitrogen limitation is switched on then century soil module also needs to be switched on!");
	}

	// bvoc
	if (ifbvoc) {
		initbvoc();
	}

	// Create objects for (de)serializing grid cells
	auto_ptr<GuessSerializer> serializer;
	auto_ptr<GuessDeserializer> deserializer;

	if (save_state) {
    	// cw SubPixel make sure we can restart without MPI
    	if(args.get_parallel()){
      		dprintf("Save state initiated, parallel mode\n");
      		serializer = auto_ptr<GuessSerializer>(new GuessSerializer(state_path, GuessParallel::get_rank(), GuessParallel::get_num_processes()));
    	} else {
      		dprintf("Save state initiated, non-parallel mode\n");
      		serializer = auto_ptr<GuessSerializer>(new GuessSerializer(state_path, 0, 1));
    	}
  }

  // cw SubPixel run_landform serializer
  auto_ptr<GuessSerializer> landform_serializer;

  if (run_landform) {
      // cw SubPixel make sure we can restart without MPI
      if(args.get_parallel()){
          dprintf("Auto-save state initiated, parallel mode\n");
          landform_serializer = auto_ptr<GuessSerializer>(new GuessSerializer(landform_state_path, GuessParallel::get_rank(), GuessParallel::get_num_processes()));
      } else {
          dprintf("Auto-save state initiated, non-parallel mode\n");
          landform_serializer = auto_ptr<GuessSerializer>(new GuessSerializer(landform_state_path, 0, 1));
      }
  }



	if (restart) {
		deserializer = auto_ptr<GuessDeserializer>(new GuessDeserializer(state_path));
	}

	while (true) {

		// START OF LOOP THROUGH GRID CELLS

		// Initialise global variable date
		// (argument nyear not used in this implementation)
		date.init(1);

		// Create and initialise a new Gridcell object for each locality
		Gridcell gridcell;

		// Call input module to obtain latitude and driver data for this grid cell.
		if (!input_module->getgridcell(gridcell)) {
			break;
		}

		// Initialise certain climate and soil drivers
		gridcell.climate.initdrivers(gridcell.get_lat());

		if (run_landcover && !restart) {
			// Read landcover and cft fraction data from
			// data files for the spinup period and create stands
			landcover_init(gridcell, input_module.get());
		}

		if (restart) {
			// Get the whole grid cell from file...
			deserializer->deserialize_gridcell(gridcell);
			// ...and jump to the restart year
			date.year = state_year;

      // cw SubPixel extra debugging
      dprintf(" >>>>> read: %d (%d)\n", date.year, date.get_calendar_year());
		}

		// Call input/output to obtain climate, insolation and CO2 for this
		// day of the simulation. Function getclimate returns false if last year
		// has already been simulated for this grid cell

		while (input_module->getclimate(gridcell)) {

			// START OF LOOP THROUGH SIMULATION DAYS
			simulate_day(gridcell, input_module.get());

			output_modules.outdaily(gridcell);

			if (date.islastday && date.islastmonth) {
				// LAST DAY OF YEAR
				// Call output module to output results for end of year
				// or end of simulation for this grid cell
				output_modules.outannual(gridcell);

				gridcell.balance.check_year(gridcell);

				// cw SubPixel extra logging
				if (date.year >= nyear_spinup + spoutput_startyear-1)
          dprintf("End of year: %d (%d) co2:%.2f ndep:%.2f\n", date.year, date.get_calendar_year(), gridcell.climate.co2, gridcell.climate.andep * 10000 );

				// Time to save state?
				if (date.year == state_year-1 && save_state) {
					dprintf(" <<<<< dump: %d (%d)\n", date.year, date.get_calendar_year());

					serializer->serialize_gridcell(gridcell);
				}

				// Check whether to abort
				if (abort_request_received()) {
					return 99;
				}
			}

			// Advance timer to next simulation day
			date.next();

			// End of loop through simulation days
		}	//while (getclimate())

    // cw SubPixel - end of sim autodump
    //
    // - triggered at the end of the sim (we always dump the state in run_landform)
    // - dumping into a dedicated directory
    if (run_landform) {
        // reduce by one since ww already incremented the year
        dprintf(" < autodump: %d (%d)\n", date.year-1, date.get_calendar_year()-1);
        landform_serializer->serialize_gridcell(gridcell);
    }

		gridcell.balance.check_period(gridcell);

	}		// End of loop through grid cells



	// END OF SIMULATION

	return 0;
}
