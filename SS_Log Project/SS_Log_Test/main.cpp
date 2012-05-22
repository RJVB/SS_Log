// ----------------------------------------------------------------------- //
//
//  FILENAME:	main.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	13 Nov 2001, 09:04
//
//  DESCRIPTION:
//
//	    Before trying to figure this stuff out, you should probably read the
//      Help file located in the Project Workspace directory, "SS_Log.htm".
//
//      This file really shows the possibilities of the SS_Log project.  We're
//      using the global log two local logs.  
//
//      A debug build should, when run, produce two log windows, one with 30 
//      entries, the other with 11.  Also we should get a file named 
//      "\LocalLog1.log" with 20 entries.
//
//      The release build will produce 1 log windows with 20 entries, and 2 
//      log files ("\LocalLog1.log" and "SS_Log.log") each with 20 entries.
//
//      This is, of course, if you leave all the filters turned on in the 
//      registry.  Try turning off some of the values for different results.
//      (HKEY_CURRENT_USER\Software\SS_Log) (0=off, 1=on)
//
//
//  Copyright (c) 2001
//
// ----------------------------------------------------------------------- //

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include "SS_Log_Include.h"

// ------------------[      Macros/Constants/Types     ]------------------ //
// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //


void main()
{
	// global log
	//
	// Using only defaults, goes to window in debug build, goes to "\ss_log.log" 
	// in release build, and has "Critical" level.  NOTE!!!:  You will never see
	// this message (just below) appear in the log in debug builds.  Why?  
	// Because the localLog1 below writes to the same Log Window as the global
	// log (because localLog1 never call WindowName()), and the localLog1 calls
	// EraseLog() after the message below is sent from the global log.  
	LogEraseLog();
	Log("This is the first %s from the global log.", "message");


	// local log 1
	//
	// We give this one a new filename, but not a new windowname, so it should
	// log to a file by itself, but be combined in a window with the global log.
	// We also specify LOGTYPE_WARNING, so the LOGTYPE_CRITICAL default level
	// will be removed.  We also specify LOGTYPE_LOGTOWINDOW and LOGTYPE_LOGTOFILE,
	// so we will log to both a file and a window all the time.  NOTE!!!: notice
	// that we are calling EraseLog() before we set the filter type.  Because
	// by default, the filter includes LOGTYPE_LOGTOWINDOW in debug builds but not
	// LOGTYPE_LOGTOFILE, the erase only erases the Log Window, but does not
	// erase the file (in debug builds.  it is just the oposite in release builds).
	// In order to erase both the file and window, call the Filter() function 
	// before calling the EraseLog() function.
	SS_Log localLog1;
	localLog1.Filename("\\LocalLog1.log");
	localLog1.EraseLog();
	localLog1.Filter(LOGTYPE_LOGTOWINDOW|LOGTYPE_LOGTOFILE|LOGTYPE_WARNING);


	// local log 2
	//
	// We give this one a new windowname, but not a new filename.  So it will
	// spawn it's own log window, and will be combined with the global log
	// in the file "\SS_Log.log".  Also, we specify LOGTYPE_DEBUG, so this
	// log will only be output in debug builds.
	SS_Log localLog2;
	localLog2.WindowName("The Local Log #2");
	localLog2.EraseLog();
	localLog2.Filter(LOGTYPE_DEBUG|LOGTYPE_TRACE);

	// a little loop
	for( int i=0; i<10; i++ )
	{
		// global log output
		Log("The %s Log - Critical.  i = %d.", "Global", i);

		// localLog1 output
		Log(&localLog1, "The %s Log - Warning, LogFilter=%d, i=%d.", "localLog1", localLog1.Filter(), i);

		// localLog2 output
		Log(&localLog2, "The %s Log - Trace, LogFilter=%d, i=%d.", "localLog2", localLog2.Filter(), i);

		// Global log output, but we override some of the defaults for this message.
		// We make it a "Normal" level, and only output in release builds.
		Log(LOGTYPE_RELEASE|LOGTYPE_NORMAL, "The %s Log - Normal, i=%d.", "global", i);
	
		// the localLog1 output, with some overrides
		Log(&localLog1, LOGTYPE_NORMAL, "The %s Log - Normal. LogFilter=%d, i=%d.", "localLog1", localLog1.Filter(), i);
	}

    // we add the Normal level filter to the localLog2 here.  Note that since we are
    // "adding" this filter (calling AddFilters() instead of Filter() ), the Trace 
    // level filter that we added above (just below the declaration of localLog2) 
    // does not get removed.
	localLog2.AddFilters(LOGTYPE_NORMAL);
	Log(&localLog2, "localLog2, Normal AND Trace, to Window only");
}
