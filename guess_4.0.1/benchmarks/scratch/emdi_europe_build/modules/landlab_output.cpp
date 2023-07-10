///////////////////////////////////////////////////////////////////////////////////////
/// \file outputmodule.cpp
/// \brief Implementation of the common output module
///
/// \author Joe Siltberg
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "landlab_output.h"
#include "parameters.h"
#include "guess.h"

// Years between output time points in plot function output (Windows shell only)
const int PLOT_INTERVAL = 1;

// Years between updates of soil water and stand structure plot (Windows shell only)
const int PLOT_UPDATE_INTERVAL = 20;

// Years between updates of 3D vegetation view (Windows shell only)
const int VEG3D_UPDATE_INTERVAL = 5;

// Name of temporary file for output of 3D vegetation structure (Windows shell only)
const char VEG3DFILENAME[] = "xxxtemp0.bin";

namespace GuessOutput {

REGISTER_OUTPUT_MODULE("common", LandLabOutput)

LandLabOutput::LandLabOutput() {
}


LandLabOutput::~LandLabOutput() {
}

/// Define all output tables and their formats
void LandLabOutput::init() {
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
void LandLabOutput::outannual(Gridcell& gridcell) {
}

/// Output of simulation results at the end of each day
/** This function does not have to provide any information to the framework.
  */
void LandLabOutput::outdaily(Gridcell& gridcell) {
}

} // namespace
