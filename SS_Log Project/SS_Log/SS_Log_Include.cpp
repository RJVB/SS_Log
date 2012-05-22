// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Log_Include.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	24 Oct 2001, 14:49
//
//  PURPOSE:	
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
#ifdef _SS_LOG_ACTIVE

TCHAR g_szFile[MAX_PATH]; 
int g_nLine;

VOID WriteLog(SS_Log* pLog, DWORD dwFilter, TCHAR* pMsg, va_list* args)
{
    pLog->WriteLog(g_szFile, g_nLine, dwFilter, pMsg, args);
}

VOID WriteLog(SS_Log* pLog, DWORD dwFilter, TCHAR* pMsg, ...)
{
    va_list args;
    va_start(args, pMsg);
    
    WriteLog( pLog, dwFilter, pMsg, &args );
    
    va_end(args);
}

VOID WriteLog(SS_Log* pLog, TCHAR* pMsg, ...)
{
    va_list args;
    va_start(args, pMsg);
    
    WriteLog( pLog, pLog->Filter(), pMsg, &args );
    
    va_end(args);
}

VOID WriteLog(DWORD dwFilter, TCHAR* pMsg, ...)
{
    va_list args;
    va_start(args, pMsg);
    
    WriteLog( &g_Log, dwFilter, pMsg, &args );
    
    va_end(args);
}

VOID WriteLog(TCHAR* pMsg, ...)
{
    va_list args;
    va_start(args, pMsg);
    
    WriteLog( &g_Log, g_Log.Filter(), pMsg, &args );
    
    va_end(args);
}

VOID LogStoreFileLine(TCHAR* szFile, int nLine)
{
	_tcscpy(g_szFile, szFile);
	g_nLine = nLine;
}


#else // _SS_LOG_ACTIVE

#define Log

#endif // _SS_LOG_ACTIVE



// ------------------[    Class Function Definitions   ]------------------ //
