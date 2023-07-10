///////////////////////////////////////////////////////////////////////////////////////
/// \file spitfireoutput.h
/// \brief Output module for the most commonly needed output files
///
/// \author Joe Siltberg
/// $Date: 2016-12-16 16:21:33 +0100 (Fri, 16 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_BENCHMARK_OUTPUT_H
#define LPJ_GUESS_BENCHMARK_OUTPUT_H

#include "outputmodule.h"
#include "outputchannel.h"
#include "gutil.h"

namespace GuessOutput {

/// Output module for the most commonly needed output files
class BenchmarkOutput : public OutputModule {
public:

	BenchmarkOutput();

	~BenchmarkOutput();

	// implemented functions inherited from OutputModule
	// (see documentation in OutputModule)

	void init();

	void outannual(Gridcell& gridcell);

	void outdaily(Gridcell& gridcell);

private:

	/// Defines all output tables
	void define_output_tables();

	xtring file_vegcover;
	xtring file_canopyheight;
	xtring file_mfpar;

	Table out_vegcover;
	Table out_canopyheight;
	Table out_mfpar;


};

}

#endif // LPJ_GUESS_BENCHMARK_OUTPUT_H
