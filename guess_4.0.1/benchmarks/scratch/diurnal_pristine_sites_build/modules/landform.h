///////////////////////////////////////////////////////////////////////////////////////
/// \file landform.h
/// \brief Functions handling landform aspects
///
/// Landform functionality.
///
/// \author Christian Werner,
/// $Date$
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_LANDFORM_H
#define LPJ_GUESS_LANDFORM_H

#ifdef HAVE_NETCDF

#include "guess.h"
#include "guessnc.h"
#include "inputmodule.h"

/// Class that deals with landform input
class LandformInput {

public:
	LandformInput();
	~LandformInput();
    
	/// Opens landform data files
	void init();
	/// Loads landform info (elevation, slope, aspect, soil depth from input files)
	bool loadlandform(double lat, double lon, int landid);
	/// get landform(s) for gridcell
	std::vector<Landform> get_landforms() const;
    
private:
	xtring file_landform;
    
	// The Variables
	GuessNC::CF::GridcellOrderedVariableNoTime* nc_fraction;
	GuessNC::CF::GridcellOrderedVariableNoTime* nc_elevation;
	GuessNC::CF::GridcellOrderedVariableNoTime* nc_slope;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_aspect;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_asp_slope;
	GuessNC::CF::GridcellOrderedVariableNoTime* nc_soildepth;
    
	/// \returns all (used) variables
	std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> all_variables() const;
	std::vector<Landform> landforms;
};


/// Class that deals with site input (gridcell specific elevation, soil properties etc.)
class SiteInput {

public:
    SiteInput();
    ~SiteInput();

	/// Opens site data file
	void init();
	/// Loads site info (soil properties, soil depth and elevation)
	bool loadsite(double lon, double lat, int landid);
	/// Get site info for a stand
	Site get_site() const;
	
private:
    xtring file_site;
	
    // The Variables
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_elevation;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_sand;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_silt;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_clay;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_soc;
    GuessNC::CF::GridcellOrderedVariableNoTime* nc_soildepth;
    
    /// \returns all (used) variables
    std::vector<GuessNC::CF::GridcellOrderedVariableNoTime*> all_variables() const;
	Site site;
};


#endif // HAVE_NETCDF

#endif // LPJ_GUESS_LANDFORM_H
