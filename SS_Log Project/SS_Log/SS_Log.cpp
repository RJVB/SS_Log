// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Log.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	23 Oct 2001, 16:07
//
//  PURPOSE:	
//
//  Copyright (c) 2001
//
// ----------------------------------------------------------------------- //

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "SS_RegistryKey.h"

// we #define __SS_Log_Include_h__ so the SS_Log.h file does not tell is to 
// include SS_Log_Include.h instead
#define __SS_Log_Include_h__ 
#include "SS_Log.h"
#undef __SS_Log_Include_h__

// ------------------[      Macros/Constants/Types     ]------------------ //
typedef struct tagFindWindow
{
    TCHAR   szWindowName[MAX_PATH];
    HWND    hWnd;
    HANDLE  process;
    DWORD   queries;
} SS_FIND_WINDOW;

// ------------------[         Global Variables        ]------------------ //
#ifdef _SS_LOG_ACTIVE

SS_Log g_Log;
static HANDLE g_hPipe; 

#endif

// ------------------[         Global Functions        ]------------------ //
BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    SS_FIND_WINDOW* pFindWindow = (SS_FIND_WINDOW*)lParam;

    TCHAR szText[256];
    int n;
    DWORD pID = 0;
    GetWindowThreadProcessId(hwnd, &pID);
    // GetWindowText can block if we query one of our own windows that is currently unresponsive (e.g. because we're querying...)
    if( pID != GetCurrentProcessId() ){
        pFindWindow->queries += 1;
	    n = GetWindowText(hwnd, szText, 256);
	    if( n && pFindWindow && !_tcsncmp( szText, pFindWindow->szWindowName, n )  )
	    {
		   pFindWindow->hWnd = hwnd;
           pFindWindow->process = OpenProcess( SYNCHRONIZE|PROCESS_QUERY_INFORMATION, FALSE, pID );
		   return FALSE;
	    }
    }

    return TRUE;
}

// ------------------[    Class Function Definitions   ]------------------ //

//std constructor
SS_Log::SS_Log()
{
    InitObject();
}

SS_Log::SS_Log(LPCTSTR szFilename, LPCTSTR szWindowName)
{
    InitObject();

    // set log window name
    WindowName(szWindowName);

    // set the log filename, adding ".log" if not there
    TCHAR szFile[MAX_PATH];
    _tcscpy(szFile, szFilename);
    TCHAR* szTmp = _tcstok( szFile, _T(".") );
    if( !_tcscmp(szFile,szFilename) )
    {
        _tcscat(szFile, _T(".log") );
        Filename(szFile);
    }
	else{
        Filename(szFilename);
	}
}

// std destructor
SS_Log::~SS_Log()
{
    delete m_szLogFile;
    m_szLogFile = NULL;
    if( m_hNamedPipe ){
        CloseHandle(m_hNamedPipe);
    }
    if( m_hLogProcess ){
        CloseHandle(m_hLogProcess);
    }
}

// copy constructor
SS_Log::SS_Log( SS_Log& rhs )
{
    *this = rhs;
}

// assignment operator
SS_Log&	SS_Log::operator = (SS_Log& rhs)
{
    Filter( rhs.Filter() );
    Filename( rhs.Filename() );
    WindowName( rhs.WindowName() );
    FileHandle( rhs.FileHandle() );
    
    return *this;
}

// ----------------------------------------------------------------------- //
//  Function:		InitObject
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	none
//  Notes:			All constructors should call this function.
//                  The defaults are different for release and debug builds.
// ----------------------------------------------------------------------- //
VOID SS_Log::InitObject()
{
    DWORD dwSize = MAX_PATH;
    m_szLogFile = new TCHAR[MAX_PATH];
    GetLogFileFromReg(m_szLogFile, dwSize);
    m_szLogWindow = new TCHAR[MAX_PATH];
    m_szPipeName = new TCHAR[MAX_PATH];
    m_hNamedPipe = NULL;
    WindowName( _T("Default Log Window") );
    m_szProgName= new TCHAR[MAX_PATH];
    _tcscpy( m_szProgName, _T("") );
    FileHandle(NULL);
    m_hLogProcess = NULL;
    ASSERT(m_szLogFile);

#ifdef _DEBUG

    Filter( LOGTYPE_DEBUG | LOGTYPE_CRITICAL | LOGTYPE_LOGTOWINDOW );
    
#else // _DEBUG

    Filter( LOGTYPE_RELEASE | LOGTYPE_CRITICAL | LOGTYPE_LOGTOFILE );
    
#endif // _DEBUG

#ifdef _SS_LOG_ACTIVE
    g_hPipe = INVALID_HANDLE_VALUE;
	m_warnedLogWindowMissing = FALSE;
	m_OpenLogWindowAttempts = 0;
#endif // _SS_LOG_ACTIVE
}
  
// ----------------------------------------------------------------------- //
//  Function:		SS_Log::GetLogFile
//  Author:			Steve Schaneville
//  Parameters:		szFilenname (out) - a buffer for the filename
//                  dwSize (in, out) - in, size of the filename buffer.
//                                     out, size written to the filename buffer.
//  Return Values:	ERROR_SUCCESS if successful, MS error code if not.
//  Notes:			Get the default filename for the log file, stored in
//                  the registry.
// ----------------------------------------------------------------------- //
LONG SS_Log::GetLogFileFromReg(LPTSTR szFilename, DWORD& dwSize)
{
    LONG lResult = ERROR_SUCCESS;

#ifdef _SS_LOG_ACTIVE
    
    lResult = GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
                           SS_LOG_REG_LOGFILE, szFilename, &dwSize);
    if( ERROR_SUCCESS != lResult )
    {
        //SS_RegistryKey key;
        lResult = SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
                               SS_LOG_REG_LOGFILE, 
                               SS_LOG_DEFAULT_LOGFILE );
        _tcsncpy(szFilename, SS_LOG_DEFAULT_LOGFILE, dwSize);
    }

#endif // _SS_LOG_ACTIVE

    return lResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log::GetFilterFromReg
//  Author:			Steve Schaneville
//  Parameters:		dwFilter (out) - the filter in the registry
//  Return Values:	0 if successful, non-zero if not.
//  Notes:			We simply look in the registry for each entry
//                  specified by the SS_LOG_REG_FILTER_* values.  If we
//                  don't find them, we create them and give them default
//                  values.  We place all filters into the provided dwFilter
//                  for returning to the calling function.
// ----------------------------------------------------------------------- //
LONG SS_Log::GetFilterFromReg(DWORD& dwFilter)
{
    LONG lResult = 0;
    DWORD dwFilterIsOn = 0;
    dwFilter = 0;
    
#ifdef _SS_LOG_ACTIVE
    
    // Debug
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_DEBUG, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_DEBUG, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_DEBUG;
    
    
    // Release
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_RELEASE, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_RELEASE, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_RELEASE;
    
    
    // Critical
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_CRITICAL, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_CRITICAL, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_CRITICAL;


    // Warning
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_WARNING, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_WARNING, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_WARNING;
    
    
    // Normal
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_NORMAL, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_NORMAL, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_NORMAL;
    
    
    // Trace
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_TRACE, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_TRACE, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_TRACE;
    
    
    // Log to Window
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_LOGTOWINDOW, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_LOGTOWINDOW, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_LOGTOWINDOW;
    
    
    // Log to File
    lResult += GetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
        SS_LOG_REG_FILTER_LOGTOFILE, dwFilterIsOn);
    if( ERROR_SUCCESS != lResult )
    {
        dwFilterIsOn = 1;
        lResult += SetRegValue( SS_LOG_REG_HIVE, SS_LOG_REG_KEY, 
            SS_LOG_REG_FILTER_LOGTOFILE, dwFilterIsOn );
    }
    if( dwFilterIsOn )
        dwFilter |= LOGTYPE_LOGTOFILE;
    
#endif // _SS_LOG_ACTIVE
        
    return lResult;
}

// set the log window's name
VOID SS_Log::WindowName(LPCTSTR szLogWindow)
{
    ASSERT(szLogWindow && _tcslen(szLogWindow));
    _tcscpy(m_szLogWindow, szLogWindow);
    sprintf( m_szPipeName, _T("\\\\.\\pipe\\%s"), m_szLogWindow );
    if( m_hNamedPipe ){
        CloseHandle(m_hNamedPipe);
        m_hNamedPipe = NULL;
    }
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log::WriteLog()
//  Author:			Steve Schaneville
//  Parameters:		szFile      - The name of the file in which the log
//                                originated.  Use __FILE__ in the MS 
//                                compiler.
//                  nLine       - The line number of the file in which the log
//                                originated.  Use __LINE__ in the MS 
//                                compiler.
//                  dwFilterLog - The filter to use for this logged message.
//                                Valid values are those defined in the 
//                                SS_LogFilterType enum in "SS_Log.h"
//                  pMsg        - A sprintf-style format string.
//                  args        - The parameter list for the sprintf arguments.
//  Return Values:	none
//  Notes:			This function takes the following steps:
//                  1. Prep the Filter - this involves determining if any
//                     LOGTYPE_OUTPUTS filters were specified, and if not,
//                     adding the default ones.  We also check if any
//                     LOGTYPE_BUILDS filters were specified, and if not,
//                     add the default ones.  Finally we check if any
//                     LOGTYPE_LEVELS filters were specified, and if not,
//                     add the default ones.  
//                  2. Perform the sprintf (_vstprintf) on the pMsg and args.
//                  3. Get the current local time.
//                  4. Create the level string with a bunch of ifs.  This
//                     really needs to be made differently, because currently,
//                     if we add any new levels, this code will have to be 
//                     changed, but we'd like to be able to add levels without
//                     making changes to this code.
//                  4. Put all info (time, file, line #, level, message) into
//                     a tab-delimited buffer.
//                  5. Check to see if we need to process this message by
//                     comparing the provided filter (dwFilterLog) with the 
//                     filter in the registry.  See comments below for details.
//                  6. If we are processing this message, check to see if
//                     we need to send it to a log file.  If so, sent it to
//                     the file in SS_Log::Filename().
//                  7. If we are processing this message, check to see if
//                     we need to send it to a log window.  If so, first
//                     make sure that the log window specified in 
//                     SS_Log::WindowName() is open, and if not, create it.  
//                     Then send the message to it.
// ----------------------------------------------------------------------- //
VOID SS_Log::WriteLog( TCHAR* szFile, int nLine, DWORD dwFilterLog, 
					   TCHAR* pMsg, va_list* args )
{
#ifdef _SS_LOG_ACTIVE
    
    ASSERT(pMsg);
    ASSERT(args);
    
    TCHAR       szBuffer[SSLOG_MAX_MESSAGE_LENGTH];
    TCHAR       szFinalBuffer[SSLOG_MAX_MESSAGE_LENGTH];
    int         nchars;
    TCHAR       szTime[256];
    TCHAR       szLevel[256] = _T("");
	TCHAR       szTimeFormat[256] = _T("%02d.%02d.%02d %02d:%02d:%02d");
    SYSTEMTIME  ust;
    SYSTEMTIME  lst;
    DWORD       dwFilterReg;
    // 20130320: prepended a T to indicate the presence of the thread field
    TCHAR       szFinalFormat1[256] = _T("T%s\t%s\t%d\t%lu\t%s\t%s");
    TCHAR       szFinalFormat2[256] = _T("T%s [%s]\t%s\t%d\t%lu\t%s\t%s");
    DWORD       dwFilterLevelReg;
    DWORD       dwFilterLevelLog;
#ifdef _DEBUG
    DWORD       dwFilterBuild = LOGTYPE_DEBUG;
#else // _DEBUG
    DWORD       dwFilterBuild = LOGTYPE_RELEASE;
#endif // _DEBUG            

    m_bLocationSet = 0;

    // prep the filter... if we are using the default filter, just
    // use it as is.  If not, we need to add the default LOGTYPE_OUTPUTS
    // and default LOGTYPE_BUILDS type filters
    SetFilter(dwFilterLog);
    
    // prep sprintf message
    nchars = _vstprintf(szBuffer, pMsg, *args);
    if( 0 == nchars )
        return;

    // prep message time
    GetSystemTime(&ust);
    SystemTimeToTzSpecificLocalTime( NULL, &ust, &lst );
    sprintf(szTime, szTimeFormat, lst.wYear, lst.wMonth, lst.wDay,
			lst.wHour, lst.wMinute, lst.wSecond );

    // prep the levels text (surely there is a better way to do this?!)
    if( dwFilterLog & LOGTYPE_CRITICAL )
    {
        _tcscat(szLevel, LOGTYPE_CRITICAL_TEXT);
    }
    if( dwFilterLog & LOGTYPE_WARNING )
    {
        if( _tcscmp(szLevel,_T("")) )
            _tcscat(szLevel, _T("/"));
        _tcscat(szLevel, LOGTYPE_WARNING_TEXT);
    }
    if( dwFilterLog & LOGTYPE_NORMAL )
    {
        if( _tcscmp(szLevel,_T("")) )
            _tcscat(szLevel, _T("/"));
        _tcscat(szLevel, LOGTYPE_NORMAL_TEXT);
    }
    if( dwFilterLog & LOGTYPE_TRACE )
    {
        if( _tcscmp(szLevel,_T("")) )
            _tcscat(szLevel, _T("/"));
        _tcscat(szLevel, LOGTYPE_TRACE_TEXT);
    }
    if( !_tcscmp(szLevel,_T("")) )
        _tcscat(szLevel, _T(" "));
    
    
    // prep the final output buffer
	if( *ProgName() ){
		sprintf(szFinalBuffer, szFinalFormat2, szTime, ProgName(), szFile, nLine, GetCurrentThreadId(), szLevel, szBuffer);
	}
	else{
		sprintf(szFinalBuffer, szFinalFormat1, szTime, szFile, nLine, GetCurrentThreadId(), szLevel, szBuffer);
	}
    
    // Here is the filter meat.  Basically, the 'if' statement checks the
    // current message's filter type against the registry filter's 
    // LOGTYPE_LEVELS type filters.  If all types of the current message's 
    // filter are present in the registry filter, we record the message 
    // (and ignore it otherwise).
    GetFilterFromReg(dwFilterReg);
    dwFilterLevelReg = dwFilterReg & LOGTYPE_LEVELS;
    dwFilterLevelLog = dwFilterLog & LOGTYPE_LEVELS;
    if( (!(dwFilterLevelLog & ~dwFilterLevelReg)) && // all LOGTYPE_LEVELS that are in dwFilterLog must also be in the registry
        (dwFilterLog & dwFilterBuild & dwFilterReg) // the current build type must be in dwFilterLog AND in the registry
      )
    {        
        // yes, we log this message... now we check to see where the message
        // should go... to a file?  to a log window?

        // do we log to file?
        if( dwFilterLog & dwFilterReg & LOGTYPE_LOGTOFILE )
        {
            // log to file
            HANDLE hLog = ::CreateFile( Filename(), GENERIC_WRITE, 0, NULL,
                                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
                                        NULL );
            
            if( INVALID_HANDLE_VALUE != hLog ) 
            {
                ::SetFilePointer(hLog, 0, NULL, FILE_END);
                
                DWORD cbWritten;
                ::WriteFile( hLog, (LPCVOID)szFinalBuffer, 
                             _tcslen(szFinalBuffer)*sizeof(TCHAR),
                             &cbWritten, NULL );
                ::WriteFile( hLog, _T("\n"), 
                             _tcslen(_T("\n"))*sizeof(TCHAR),
                             &cbWritten, NULL );
                ::FlushFileBuffers(hLog);
                ::CloseHandle(hLog);
            }
        }

        // do we log to window?
        if( dwFilterLog & dwFilterReg & LOGTYPE_LOGTOWINDOW)
        {
            // log to window
            if( OpenLogWindow() ){
                DWORD dwBytesRead = 0;
				BOOL bResult = WaitNamedPipe(WindowPipeName(), 20000);
// 20130320: creating a file HANDLE corresponding to our named pipe would preempt access to the log viewer
// to the 1st client creating the HANDLE. While this could be OK in certain situations it also requires
// changes to the viewer's pipe reading loop, to prevent it from blocking after the 1st message.
//                if( bResult && !m_hNamedPipe ){
//                    m_hNamedPipe = ::CreateFile( WindowPipeName(), GENERIC_READ|GENERIC_WRITE,
//                                                FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
//                                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 
//                                               NULL );
//                    if( m_hNamedPipe == INVALID_HANDLE_VALUE ){
//                        m_hNamedPipe = NULL;
//                    }
//                }
                bResult = CallNamedPipe(WindowPipeName(), (LPVOID)szFinalBuffer, 
                                 _tcslen(szFinalBuffer)+1, (LPVOID)NULL, 
                                 0, &dwBytesRead, 
                                 5000);
		  }
        }
    }

#endif // _SS_LOG_ACTIVE
    
}

VOID SS_Log::WriteLog( DWORD dwFilterLog, TCHAR* pMsg, va_list* args )
{
#ifdef _SS_LOG_ACTIVE
    WriteLog( m_szFile, m_nLine, dwFilterLog, pMsg, args );
#endif // _SS_LOG_ACTIVE
}

VOID SS_Log::StoreFileLine(TCHAR *szFile, int nLine)
{
	_tcscpy(m_szFile, szFile);
	m_nLine = nLine;
    m_bLocationSet = 1;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log::CreateLogWindowPipe
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	true if successful, false if not
//  Notes:			We use the EnumWindows() API function to compare the
//                  names of all open windows with the name specified in
//                  SS_Log::WindowName().  If we don't find a match, we
//                  create a new instance of SS_Log_Window.exe and pass
//                  it the SS_Log::WindowName() name as a commandline param.
//                  IMPORTANT:  SS_Log_Window.exe must be somewhere in the 
//                  path for this function to succeed.
// ----------------------------------------------------------------------- //
BOOL SS_Log::OpenLogWindow()
{

#ifdef _SS_LOG_ACTIVE
    
    // we must create the log window if is it not already running
#ifdef _LCS
    WNDENUMPROC lpEnumFunc = (int(_stdcall*)())EnumWindowsProc;
#else
    WNDENUMPROC lpEnumFunc = EnumWindowsProc;
#endif
    DWORD logStatus = 0;

    if( !m_hLogProcess || !GetExitCodeProcess(m_hLogProcess, &logStatus) || logStatus != STILL_ACTIVE ){
      SS_FIND_WINDOW findWindow;
        _tcscpy( findWindow.szWindowName, WindowName() );
        findWindow.hWnd = NULL;
        findWindow.queries = 0;
        findWindow.process = NULL;
        EnumWindows( lpEnumFunc, (LPARAM)&findWindow );
        if( !findWindow.hWnd )
        {
            if( m_OpenLogWindowAttempts < 5 || ((m_OpenLogWindowAttempts % 100) == 0) ){
                PROCESS_INFORMATION pi;
                STARTUPINFO si;
                si.lpDesktop = NULL;
                si.lpTitle = NULL;
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = TRUE;
                si.cb = sizeof(si);
                si.cbReserved2 = 0;
                si.lpReserved = NULL;
                si.lpReserved2 = NULL;
                
                TCHAR szCommandLine[MAX_PATH];
                sprintf( szCommandLine, "SS_Log_Window \"%s\"", WindowName() );
                INT nResult = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 
                    NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);            
                
//                Sleep(500);
                if( !nResult && !m_warnedLogWindowMissing )
                {
                    ::MessageBox(NULL, _T("Could not start the SS_Log_Window.exe application... does it reside in the system path?"), _T("Error"), MB_OK|MB_ICONSTOP);
                    m_warnedLogWindowMissing = TRUE;
                    return FALSE;
                }
                m_OpenLogWindowAttempts += 1;
                WaitForInputIdle( pi.hProcess, 500 );
                findWindow.process = pi.hProcess;
                // apparently this is necessary - pi being a non-optional argument to CreateProcess?!
                CloseHandle(pi.hThread);
            }
        }
        if( m_hLogProcess ){
            CloseHandle(m_hLogProcess);
        }
        // m_hLogProcess will not be initialised immediately after CreateProcess("SS_Log_Window"):
        m_hLogProcess = findWindow.process;
    }

#endif // _SS_LOG_ACTIVE
    
    return TRUE;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log::EraseLog()
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	none
//  Notes:			Erases both the log file and window of current log.
//                  Note that if multiple logs are writing to the same
//                  file or window simultaniously, this function will
//                  erase info from all logs involved, not just the entries
//                  that were created by the log calling this function.
// ----------------------------------------------------------------------- //
VOID SS_Log::EraseLog()
{

#ifdef _SS_LOG_ACTIVE
    
#ifdef _LCS
    WNDENUMPROC lpEnumFunc = (int(_stdcall*)())EnumWindowsProc;
#else
    WNDENUMPROC lpEnumFunc = EnumWindowsProc;
#endif
    
    SS_FIND_WINDOW findWindow;
    _tcscpy( findWindow.szWindowName, WindowName() );
    findWindow.hWnd = NULL;
    EnumWindows( lpEnumFunc, (LPARAM)&findWindow );

    if( (Filter() & LOGTYPE_LOGTOWINDOW) && findWindow.hWnd )
    {
        DWORD dwBytesRead = 0;
        TCHAR szBuffer[] = _T("~~WMSS_LOG_ERASE_LOG~~");

        BOOL bResult = CallNamedPipe(WindowPipeName(), szBuffer, 
            _tcslen(szBuffer)+1, (LPVOID)NULL, 
            0, &dwBytesRead, 
            NMPWAIT_USE_DEFAULT_WAIT);
    }

    if( (Filter() & LOGTYPE_LOGTOFILE) )
        ::DeleteFile(Filename());

#endif // _SS_LOG_ACTIVE
    
}

VOID SS_Log::SetFilter(DWORD& dwFilter)
{

#ifdef _SS_LOG_ACTIVE
    
    if( !(dwFilter & LOGTYPE_OUTPUTS) )
    {
        // no LOGTYPE_OUTPUTS were specified, so we add the default
        dwFilter |= Filter() & LOGTYPE_OUTPUTS;
    }
    
    if( !(dwFilter & LOGTYPE_BUILDS) )
    {
        // no LOGTYPE_BUILDS were specified, so we add the default
        dwFilter |= Filter() & LOGTYPE_BUILDS;
    }

    if( !(dwFilter & LOGTYPE_LEVELS) )
    {
        // no LOGTYPE_LEVELS were specified, so we add the default
        dwFilter |= Filter() & LOGTYPE_LEVELS;
    }

#endif // _SS_LOG_ACTIVE
    
}
