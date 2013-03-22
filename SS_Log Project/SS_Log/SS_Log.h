// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_Log.h
//  AUTHOR:		Steve Schaneville
//  CREATED:	23 Oct 2001, 16:01
//
//  NOTES:	    1. Do not include this file directly into your project.
//                 Instead, include the SS_Log_Include.h file.
//              2. The class below 
//              3. See the SS_Log.htm file for a discussion on how the
//                 filters work.
//                  
// ----------------------------------------------------------------------- //
#ifndef __SS_Log_h__
#define __SS_Log_h__

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include <windows.h>
#include <assert.h>
#include <tchar.h>

// ------------------[      Macros/Constants/Types     ]------------------ //
#ifndef __SS_Log_Include_h__
	#error Do not include this file directly.  Instead, include 'SS_Log_Include.h'
#endif // __SS_Log_Include_h__

#ifndef ASSERT
#define ASSERT assert
#endif


// Steps for Defining a new SS_LogFilterType:
//
// 1. Enter an new value in the SS_LogFilterType enum. (ex. LOGTYPE_JUNK)
//    Give this entry an appropriate value (ex. (1<<8))
// 2. Add that new entry to the LOGTYPE_BUILDS, LOGTYPE_OUTPUTS, or LOGTYPE_LEVELS
//    #defines just below the SS_LogFilterType enum.
// 3. If it is a LOGTYPE_LEVELS type entry, create a #define LOTYPE_??_TEXT.
// 4. Create a #define SS_LOG_REG_FILTER_??. 
// 5. In the SS_Log::GetFilterFromReg(...) function, copy one of the sections
//    of code to create a new section for for your new entry (you'll figure it
//    out by looking at the function).
// 6. If creating a LOGTYPE_LEVELS type entry, look in the 
//    SS_Log::WriteLog(...) function for the following code:
//
//    if( dwFilterLog & LOGTYPE_NORMAL )
//    {
//        if( _tcscmp(szLevel,_T("")) )
//        _tcscat(szLevel, _T("/"));
//        _tcscat(szLevel, LOGTYPE_NORMAL_TEXT);
//    }
//
//    Copy this code and create another entry like it, making appropriate 
//    adjustments.


// define the filter types
typedef enum SS_LogFilterType
{
    LOGTYPE_DEBUG                   = (1<<0), //    1 - for debug builds
    LOGTYPE_RELEASE                 = (1<<1), //    2 - for release builds
    
    LOGTYPE_LOGTOWINDOW             = (1<<2), //    4 - write log to screen window
    LOGTYPE_LOGTOFILE               = (1<<3), //    8 - write log to file

    LOGTYPE_CRITICAL                = (1<<4), //   16 - for critical errors
    LOGTYPE_WARNING                 = (1<<5), //   32 - for warning errors
    LOGTYPE_NORMAL                  = (1<<6), //   64 - non-error info (everything's ok)
    LOGTYPE_TRACE                   = (1<<7)  //  128 - for low priority info

} SS_LOG_FILTER_TYPE;

#define LOGTYPE_BUILDS                  (LOGTYPE_DEBUG|LOGTYPE_RELEASE)
#define LOGTYPE_OUTPUTS                 (LOGTYPE_LOGTOWINDOW|LOGTYPE_LOGTOFILE)
#define LOGTYPE_LEVELS                  (LOGTYPE_CRITICAL|LOGTYPE_WARNING|LOGTYPE_NORMAL|LOGTYPE_TRACE)

#define LOGTYPE_CRITICAL_TEXT           (_T("CRITICAL"))
#define LOGTYPE_WARNING_TEXT            (_T("WARNING"))
#define LOGTYPE_NORMAL_TEXT             (_T("NORMAL"))
#define LOGTYPE_TRACE_TEXT              (_T("TRACE"))

// define the registry entries for turning filters on and off
#define SS_LOG_REG_HIVE                 HKEY_CURRENT_USER
#define SS_LOG_REG_KEY                  (_T("Software\\SS_Log"))
#define SS_LOG_REG_LOGFILE              (_T("Default Log File"))

#define SS_LOG_REG_FILTER_DEBUG         (_T("Filter - Build Debug"))
#define SS_LOG_REG_FILTER_RELEASE       (_T("Filter - Build Release"))
#define SS_LOG_REG_FILTER_CRITICAL      (_T("Filter - Status Critical"))
#define SS_LOG_REG_FILTER_WARNING       (_T("Filter - Status Warning"))
#define SS_LOG_REG_FILTER_NORMAL        (_T("Filter - Status Normal"))
#define SS_LOG_REG_FILTER_TRACE         (_T("Filter - Status Trace"))
#define SS_LOG_REG_FILTER_LOGTOWINDOW   (_T("Filter - Log to Window"))
#define SS_LOG_REG_FILTER_LOGTOFILE     (_T("Filter - Log to File"))
#define SS_LOG_DEFAULT_LOGFILE          (_T("\\SS_Log.log"))

// the maximum message length
#ifndef SSLOG_MAX_MESSAGE_LENGTH
#define SSLOG_MAX_MESSAGE_LENGTH 2048
#endif

#if (SSLOG_MAX_MESSAGE_LENGTH < 10) // necessary?? let's just make sure that it's not 0 or less
#pragma message(" ")
#pragma message("  ERROR: You must define SSLOG_MAX_MESSAGE_LENGTH as an integer greater than 9.")
#pragma message("         i.e. \"#define SSLOG_MAX_MESSAGE_LENGTH 2048\"")
#pragma message(" ")
#error
#endif

// ------------------[       Forward Declarations      ]------------------ //
// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //
// ------------------[             Classes             ]------------------ //

// ----------------------------------------------------------------------- //
//  Class:			SS_Log
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //
class SS_Log
{
public:

    // construction, destruction, assignment, copy, initialization
    SS_Log                                  ();
    SS_Log                                  (LPCTSTR szFilename, LPCTSTR szWindowName);
    ~SS_Log                                 ();

    SS_Log							        (SS_Log& rhs);
    SS_Log&			    operator =		    (SS_Log& rhs);

protected:

    VOID			    InitObject		    ();

public:
    
    // accessor functions
    const DWORD         Filter              () const;
    const DWORD         Filter              (DWORD nType);
    const DWORD         AddFilters          (const DWORD nType);
    const DWORD         RemoveFilters       (const DWORD nType);
    LPCTSTR             Filename            () const;
    VOID                Filename            (LPCTSTR szWindowName);
    LPCTSTR             WindowName          () const;
    VOID                WindowName          (LPCTSTR szWindowName);
    LPCTSTR             WindowPipeName      () const;
    LPCTSTR             ProgName            () const;
    VOID                ProgName            (LPCTSTR szProgName);
    
    // utilities
    VOID                StoreFileLine       (TCHAR *szFile, int nLine);
    VOID                WriteLog            (TCHAR* szFile, int nLine, DWORD dwFilter, 
											 TCHAR* pMsg, va_list* args);
    VOID                WriteLog            (DWORD dwFilter, TCHAR* pMsg, va_list* args);
    VOID                EraseLog            ();
    inline BOOL         LocationSet         (){ return m_bLocationSet; }
    
protected:
    
    LONG                GetLogFileFromReg   (LPTSTR szFilename, DWORD& dwSize);
    BOOL                OpenLogWindow       ();
    VOID                SetFilter           (DWORD& dwFilter);
    LONG                GetFilterFromReg    (DWORD& dwFilter);
    const HANDLE        FileHandle          () const;
    VOID                FileHandle          (const HANDLE hFile);
    TCHAR               m_szFile[MAX_PATH]; 
    int                 m_nLine;
    BOOL                m_bLocationSet;

private:
    
    DWORD               m_dwFilter;         // the filter used when not otherwise specified
    LPTSTR              m_szLogFile;        // the filename to write to
    LPTSTR              m_szLogWindow;      // the name of the window to write to
    HANDLE              m_hLogProcess;      // handle to the log viewer process
    LPTSTR              m_szPipeName;       // the named pipe connected to the log window process
	LPTSTR				m_szProgName;		// the name of the programme that called us.
    HANDLE              m_hLogFile;         // handle to the file to write to
    HANDLE              m_hNamedPipe;
	// RJVB 20110313: did we already warn about a missing SS_Log_Window.exe for this log file?
	BOOL				m_warnedLogWindowMissing;
	unsigned short		m_OpenLogWindowAttempts;
};


// ----------------------------------------------------------------------- //
//  SS_Log Inline Functions
// ----------------------------------------------------------------------- //

// return the filter
inline const DWORD SS_Log::Filter() const
{ return m_dwFilter; }
 
// set the filter and return the previous filter
inline const DWORD SS_Log::Filter(DWORD nType)
{ DWORD dwOld = Filter(); SetFilter(nType); m_dwFilter = nType; return dwOld; }

inline const DWORD SS_Log::AddFilters (const DWORD nType)
{ DWORD dwOld = Filter(); Filter(Filter() | nType); return dwOld; }

inline const DWORD SS_Log::RemoveFilters (const DWORD nType)
{ DWORD dwOld = Filter(); Filter(Filter() & ~nType); return dwOld; }

// return the filename
inline LPCTSTR SS_Log::Filename() const
{ return m_szLogFile; }

// set the filename
inline VOID SS_Log::Filename(LPCTSTR szFilename)
{ ASSERT(_tcslen(szFilename) < MAX_PATH); _tcscpy(m_szLogFile, szFilename); }

// return the log window's name
inline LPCTSTR SS_Log::WindowName() const
{ return m_szLogWindow; }

// return the log window's associated named pipe
inline LPCTSTR SS_Log::WindowPipeName() const
{ return m_szPipeName; }

// return the programme name
inline LPCTSTR SS_Log::ProgName() const
{ return m_szProgName; }

inline VOID SS_Log::ProgName(LPCTSTR szProgName)
{ ASSERT(szProgName && _tcslen(szProgName)); _tcscpy(m_szProgName, szProgName); }

// return the handle to the log file
inline const HANDLE SS_Log::FileHandle() const
{ return m_hLogFile; }

// set the handle to the log file
inline VOID SS_Log::FileHandle(const HANDLE hFile)
{ m_hLogFile = hFile; }


#endif // __SS_Log_h__
