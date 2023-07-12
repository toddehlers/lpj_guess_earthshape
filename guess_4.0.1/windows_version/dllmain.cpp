//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


///////////////////////////////////////////////////////////////////////////////////////
/// \file dllmain.cpp
/// \brief Main module for interface to Windows shell
///
/// The shell should call function dll_main, passing a GuessParam object containing run
/// time argument(s) for GUESS and pointers to the executable's own callback functions.
///
/// \author Ben Smith
/// $Date: 2016-12-08 18:24:04 +0100 (Thu, 08 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "dllmain.h"
#include "framework.h"
#include "commandlinearguments.h"
#include "parallel.h"
#include "inputmodule.h"

#include <process.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////
// LOG FILE
// The name of the log file to which output from all dprintf and fail calls is sent is
// set here

xtring file_log="guess.log";



///////////////////////////////////////////////////////////////////////////////////////
// FILE SCOPE GLOBAL VARIABLES

bool waiting;
bool ifabort;
xtring* poutput;
PlotArgs* pplotargs;
MessagePrintString* message_print_string;
MessagePlot* message_plot;
MessageFinished* message_finished;
MessageResetwindow* message_resetwindow;
MessageClearGraphs* message_clear_graphs;
MessageOpen3d* message_open3d;
MessagePlot3d* message_plot3d;


class WindowsShell : public Shell {
public:
	WindowsShell(const char* logfile_path) {
		// Open log file if possible
		logfile=fopen(logfile_path,"wt");
	}

	~WindowsShell() {
		if (logfile) {
			fclose(logfile);
		}
	}

	/// Sends a message to the user somehow and terminates the program
	void fail(const char* message) {
		log_message(xtring(message)+"\n");

		message_finished();

		_endthread();
	}

	/// Sends a message to the user somehow
	void log_message(const char* message) {
		xtring* pbuf=new xtring;
		*pbuf=message;
		message_print_string(pbuf);

		if (logfile) {
			fprintf(logfile,"%s",(char*)message);
			fflush(logfile);
		}
	}

	/// Adds data point (x,y) to series 'series_name' of line graph 'window_name'.
	void plot(const char* window_name, 
	          const char* series_name, 
	          double x, 
			  double y) {
		PlotArgs* pplotargs=new PlotArgs;

		pplotargs->window_name=window_name;
		pplotargs->series_name=series_name;
		pplotargs->x=x;
		pplotargs->y=y;
		pplotargs->rescale=true;

		pplotargs->preferredrgb[0] = 0;
		pplotargs->preferredrgb[1] = 0;
		pplotargs->preferredrgb[2] = 0;
	
		message_plot(pplotargs);
	}

	/// 'Forgets' series and data for line graph 'window_name'.
	void resetwindow(const char* window_name) {
		xtring* pxtring=new xtring;
		*pxtring=window_name;
		message_resetwindow(pxtring);
	}

	/// 'Forgets' series and data for all currently-defined line graphs.
	void clear_all_graphs() {
		waiting=true;
		message_clear_graphs();
	}

	/// Initiates a 3D view of stand vegetation in the Windows shell
	void open3d() {
		message_open3d();
	}

	/// Sends data on current stand structure to 3D vegetation plot in the Windows shell
	void plot3d(const char* filename) {
		xtring* pxtring = new xtring;
		*pxtring = filename;
		message_plot3d(pxtring);
	}

	/// May be called by framework to respond to abort request from the user.
	bool abort_request_received() {
		return ifabort;
	}

private:
	FILE* logfile;
};



__declspec(dllexport) void cleanup_print_string(xtring* pxtring) {

	// To be called by shell to deallocate memory after GUESS sends a
	// message_print_string message

	delete pxtring;
}


__declspec(dllexport) void cleanup_plot(PlotArgs* pplotargs) {

	// To be called by shell to deallocate memory after GUESS sends a
	// message_plot message

	delete pplotargs;
}


///////////////////////////////////////////////////////////////////////////////////////
// DLL_MAIN
// This is the function called by the Windows shell to run the model

__declspec(dllexport) int dll_main(GuessParam arg) {

	// Store parameters sent from shell as file scope global variables

	poutput=arg.poutput;
	pplotargs=arg.pplotargs;
	message_print_string=arg.message_print_string;
	message_plot=arg.message_plot;
	message_plot3d = arg.message_plot3d;
	message_open3d = arg.message_open3d;	message_finished = arg.message_finished;
	message_resetwindow=arg.message_resetwindow;
	message_clear_graphs=arg.message_clear_graphs;

	ifabort=false;

	// Parse arguments from shell 
	CommandLineArguments args(arg.argc, arg.argv);

	// Set our shell for the model to communicate with the world
	set_shell(new WindowsShell(file_log));

	// Initialize parallel communication if available
	// Note that the graphical user interface doesn't support
	// parallel runs yet.
	GuessParallel::init(arg.argc, arg.argv);

	// If GetClim input module specified, use global paramlist to store
	// name of driver file for retrieval by input module
	xtring input_module = args.get_input_module();
	if (input_module.lower() == "getclim")
		param.addparam("getclim_driver_file", args.get_driver_file());

	// Call the framework
	framework(args);

	// Say goodbye
	message_finished();

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////
// ABORT_RUN
// Called by windows shell to request abort of current run

__declspec(dllexport) void abort_run() {


	dprintf("\nAbort request received\n");
	ifabort=true;
}


///////////////////////////////////////////////////////////////////////////////////////
// DLL_GET_INPUT_MODULE_LIST
// Called by windows shell to obtain list of available input modules

__declspec(dllexport) void dll_get_input_module_list(xtring& ximlist) {

	std::string imlist;
	InputModuleRegistry::get_instance().get_input_module_list(imlist);
	ximlist = imlist.c_str();
}