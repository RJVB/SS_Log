// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Log_Include.h
//  AUTHOR:		Steve Schaneville
//  CREATED:	23 Oct 2001, 16:23
//
//  PURPOSE:	
//
//  Copyright (c) 2001
//
// ----------------------------------------------------------------------- //
#ifndef __SS_Log_Include_h__
#define __SS_Log_Include_h__

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include "SS_Log.h"

// ------------------[          Macros/Defines         ]------------------ //
#ifdef _SS_LOG_ACTIVE

    #define LogFilter           g_Log.Filter
    #define LogRemoveFilters    g_Log.RemoveFilters
    #define LogAddFilters       g_Log.AddFilters
    #define LogFilename         g_Log.Filename
    #define LogWindowName       g_Log.WindowName
	#define LogProgName			g_Log.ProgName
    #define LogEraseLog         g_Log.EraseLog

#else

    #define LogFilter
    #define LogRemoveFilters
    #define LogAddFilters
    #define LogFilename
    #define LogWindowName
    #define LogProgName
    #define LogEraseLog()

#endif

// ------------------[      Constants/Enumerations     ]------------------ //
// ------------------[       Forward Declarations      ]------------------ //
// ------------------[         Global Variables        ]------------------ //
#ifdef _SS_LOG_ACTIVE
extern SS_Log g_Log;
#endif

// ------------------[         Global Functions        ]------------------ //
#ifdef _SS_LOG_ACTIVE

#define Log LogStoreFileLine(__FILE__, __LINE__), WriteLog

extern VOID WriteLog(SS_Log* pLog, DWORD dwFilter, TCHAR* pMsg, va_list* args );
extern VOID WriteLog(SS_Log* pLog, DWORD dwFilter, TCHAR* pMsg, ...);
extern VOID WriteLog(SS_Log* pLog, TCHAR* pMsg, ...);
extern VOID WriteLog(DWORD dwFilter, TCHAR* pMsg, ...);
extern VOID WriteLog(TCHAR* pMsg, ...);
extern VOID LogStoreFileLine(TCHAR* szFile, int nLine);

#else // _SS_LOG_ACTIVE

#define Log

#endif // _SS_LOG_ACTIVE

// ------------------[             Classes             ]------------------ //


#endif // __SS_Log_Include_h__
