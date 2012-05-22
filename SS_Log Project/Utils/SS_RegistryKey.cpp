// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_RegistryKey.cpp
//  AUTHOR:		Steve Schaneville
//  CREATED:	25 Oct 2001, 08:17
//
//  PURPOSE:	
//
//  Copyright (c) 2001
//
// ----------------------------------------------------------------------- //

// ------------------[       Pre-Include Defines       ]------------------ //
// ------------------[          Include Files          ]------------------ //
#include "SS_RegistryKey.h"
#include "tchar.h"
#include "assert.h"

// ------------------[      Macros/Constants/Types     ]------------------ //
// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //
LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 DWORD& dwBuffer)
{
    LONG lResult = NULL;
    DWORD dwSize = 1;
    
    SS_RegistryKey key(hHive);
    lResult = key.OpenKey( szKey );
    if( ERROR_SUCCESS != lResult )
    {
        dwBuffer = 0;
        return lResult;
    } 
    
    lResult = key.QueryValue( szValue, &dwBuffer );
    if( ERROR_SUCCESS != lResult )
    {
        dwBuffer = 0;
        return lResult;
    } 
    
    return lResult;    
}

LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPTSTR szBuffer, LPDWORD pdwSize)
{
    LONG lResult = NULL;
    
    SS_RegistryKey key(hHive);
    lResult = key.OpenKey( szKey );
    if( ERROR_SUCCESS != lResult )
    {
        *szBuffer = '\0';
        return lResult;
    } 
    
    LPTSTR szLocalBuffer = NULL;
    lResult = key.QueryValue( szValue, &szLocalBuffer );
    if( ERROR_SUCCESS != lResult )
    {
        *szBuffer = '\0';
        return lResult;
    } 

    if( _tcslen(szLocalBuffer) >= *pdwSize )
    {
        *szBuffer = '\0';
        _tcsncpy(szBuffer, szLocalBuffer, *pdwSize-1);
        *(szBuffer+*pdwSize-1) = '\0';
        *pdwSize = _tcslen(szLocalBuffer);
        return ERROR_MORE_DATA;
    }

    *pdwSize = _tcslen(szLocalBuffer);
    _tcscpy(szBuffer, szLocalBuffer);
    return lResult;    
}

LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPBYTE pBuffer, LPDWORD pdwSize)
{
    LONG lResult = NULL;

    SS_RegistryKey key(hHive);
    lResult = key.OpenKey( szKey );
    if( ERROR_SUCCESS != lResult )
    {
        *pBuffer = '\0';
        return lResult;
    } 

    LPBYTE pLocalBuffer = NULL;
    DWORD dwSize = 0;
    lResult = key.QueryValue( szValue, &pLocalBuffer, &dwSize );
    if( ERROR_SUCCESS != lResult )
    {
        *pBuffer = '\0';
        return lResult;
    } 
    
    if( dwSize > *pdwSize )
    {
        *pBuffer = '\0';
        memcpy(pBuffer, pLocalBuffer, *pdwSize);
        *pdwSize = dwSize;
        return ERROR_MORE_DATA;
    }
    
    *pdwSize = dwSize;
    memcpy(pBuffer, pLocalBuffer, *pdwSize);
    return lResult;    
}

LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPCTSTR szBuffer)
{
    LONG lResult = 0;

    SS_RegistryKey key(hHive);
    lResult = key.OpenKey(szKey);
    if( ERROR_SUCCESS != lResult )
    {
        lResult = key.CreateKey(hHive, szKey);
        if( ERROR_SUCCESS != lResult )
            return lResult;
    }
    
    return key.SetValue( szValue, szBuffer );
}

LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 const LPBYTE pBuffer, DWORD dwSize)
{
    LONG lResult = 0;

    SS_RegistryKey key(hHive);
    lResult = key.OpenKey(szKey);
    if( ERROR_SUCCESS != lResult )
    {
        lResult = key.CreateKey(hHive, szKey);
        if( ERROR_SUCCESS != lResult )
            return lResult;
    }
    
    return key.SetValue( szValue, pBuffer, dwSize );    
}

LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 DWORD dwBuffer)
{
    LONG lResult = 0;
    
    SS_RegistryKey key(hHive);
    lResult = key.OpenKey(szKey);
    if( ERROR_SUCCESS != lResult )
    {
        lResult = key.CreateKey(hHive, szKey);
        if( ERROR_SUCCESS != lResult )
            return lResult;
    }
    
    return key.SetValue( szValue, dwBuffer );    
}

// ------------------[    Class Function Definitions   ]------------------ //
/*
// Standard Constructor
SS_RegistryKey::SS_RegistryKey()
{
    InitObject();
}
*/
// Constructor
SS_RegistryKey::SS_RegistryKey(HKEY hKey)
{
    ASSERT( hKey );
    InitObject();
    HKey( hKey );
}

// Standard Destructor
SS_RegistryKey::~SS_RegistryKey()
{
    CloseKey();
    delete Buffer();
    Buffer(NULL);
}

// Copy Constructor
SS_RegistryKey::SS_RegistryKey( SS_RegistryKey& rhs )
{
    ASSERT( rhs.HKey() );
    *this = rhs;
}

// Assignment Operator
SS_RegistryKey& SS_RegistryKey::operator = ( SS_RegistryKey& rhs )
{
    InitObject();
    HKey( rhs.HKey() );
    Buffer(rhs.Buffer());

    return *this;
}

// Object Initialization
VOID SS_RegistryKey::InitObject()
{
    HKey(NULL);
    Buffer(NULL);
}

LONG SS_RegistryKey::CreateKey(LPCTSTR szKey, REGSAM nSecurity, 
                                       LPDWORD pdwDisposition)
{
    CloseKey();
    ASSERT(HKey());
    ASSERT(szKey);
    DWORD dwLocalDisp;
    LONG lResult;
    
    lResult = RegCreateKeyEx( HKey(), szKey, NULL, NULL, 
                              REG_OPTION_NON_VOLATILE, nSecurity, 
                              NULL, &m_hKey, &dwLocalDisp );

    if( pdwDisposition )
        *pdwDisposition = dwLocalDisp;

    return lResult;
}

LONG SS_RegistryKey::CreateKey(HKEY hKey, LPCTSTR szKey, 
                                       REGSAM nSecurity, 
                                       LPDWORD pdwDisposition)
{
    CloseKey();
    ASSERT(hKey);
    ASSERT(szKey);
    DWORD dwLocalDisp;
    LONG lResult;
    
    lResult = RegCreateKeyEx( hKey, szKey, NULL, NULL, 
        REG_OPTION_NON_VOLATILE, nSecurity, 
        NULL, &m_hKey, &dwLocalDisp );
    
    if( pdwDisposition )
        *pdwDisposition = dwLocalDisp;
    
    return lResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::OpenKey
//  Author:			Steve Schaneville
//  Parameters:     szKey - the path to the key in interest
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::OpenKey(LPCTSTR szKey, REGSAM nSecurity)
{
    HKEY hKey = HKey();
    CloseKey();
    HKey(hKey);
    ASSERT(HKey());
    ASSERT(szKey);
    return RegOpenKeyEx( HKey(), szKey, NULL, nSecurity, &m_hKey );
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::OpenKey
//  Author:			Steve Schaneville
//  Parameters:		hKey - HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, 
//                         HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE,
//                         HKEY_USERS, HKEY_PERFORMANCE_DATA (Windows NT/2000),
//                         HKEY_DYN_DATA (Windows 95/98/Me)
//                  szKey - the path to the key in interest
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::OpenKey(HKEY hKey, LPCTSTR szKey, REGSAM nSecurity)
{
    CloseKey();
    HKey(hKey);
    ASSERT(HKey());
    ASSERT(szKey);
    return RegOpenKeyEx( HKey(), szKey, NULL, nSecurity, &m_hKey );
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::CloseKey
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::CloseKey()
{
    int nResult = NULL;
    try
    {
        nResult = RegCloseKey(HKey());
    }
    catch(...) 
    {
        // intentionally left blank... occasionally we might try to close
        // keys that were never actually opened.
    }

    HKey(NULL);
	return nResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::QueryValue
//  Author:			Steve Schaneville
//  Parameters:		szName (in)     - the name of the Key (including path)
//                  ppBuffer (out)  - double-pointer to a buffer that will 
//                                    hold the returned string
//                                    NOTE: *ppBuffer MUST BE NULL!!  This 
//                                    just helps verify that we are not 
//                                    creating memory leaks due to those who
//                                    mistakingly declare memory in ppBuffer
//                                    before passing it into this function.
//                                    Create the variable like this:  
//                                    'char* pBuffer = NULL;'
//                                    Pass it in like this:
//                                    'key.QueryValue("some\\key", &pBuffer);'
//                  bExpandEnvVars (in) - if FALSE, values such as "%PATH%" that
//                                    are imbedded in the string will NOT
//                                    be expanded automatically before 
//                                    returning. 
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			This function allocates space with the "new" directive,
//                  so successive calls to QueryKey() will delete the 
//                  previous buffer.  If you need to make multiple calls
//                  you must store the first returned value in a local 
//                  variable before making the second call to QueryKey().
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::QueryValue(LPCTSTR szName, LPTSTR* ppBuffer)
{
    ASSERT( *ppBuffer == NULL );
    ASSERT( HKey() );

    DWORD dwSize = 1;
    DWORD dwType = NULL;
    LONG lResult = 0;

    delete Buffer();
    Buffer(NULL);

    lResult = RegQueryValueEx( HKey(), szName, NULL, &dwType, NULL, &dwSize);
    if( ERROR_SUCCESS != lResult )
    {
        *ppBuffer = NULL;
        return lResult;
    }

    Buffer( new TCHAR[dwSize] );
    lResult = RegQueryValueEx( HKey(), szName, NULL, &dwType, 
                               (LPBYTE)Buffer(), &dwSize );
    if( ERROR_SUCCESS != lResult )
    {
        *ppBuffer = NULL;
        return lResult;
    }

    *ppBuffer = (LPTSTR)Buffer();
    return lResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::QueryValue
//  Author:			Steve Schaneville
//  Parameters:		szName (in)     - the name of the Key (including path)
//                  pdwBuffer (out) - pointer to the DWORD to receive value
//                  bExpandEnvVars (in) - if FALSE, values such as "%PATH%" 
//                                    that are imbedded in the string will NOT
//                                    be expanded automatically before 
//                                    returning. 
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::QueryValue(LPCTSTR szName, LPDWORD pdwBuffer)
{
    ASSERT( pdwBuffer != NULL );
    ASSERT( HKey() );
    
    DWORD dwSize = sizeof(DWORD);
    DWORD dwType = 0;
    LONG lResult = 0;
    
    lResult = RegQueryValueEx( HKey(), szName, NULL, &dwType, 
                               (LPBYTE)pdwBuffer, &dwSize );
    if( ERROR_SUCCESS != lResult )
    {
        *pdwBuffer = 0;
        return lResult;
    }
    
    return lResult;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_RegistryKey::QueryValue
//  Author:			Steve Schaneville
//  Parameters:		szName (in)     - the name of the Key (including path)
//                  ppBuffer (out)  - double-pointer to a buffer that will 
//                                    hold the returned string
//                                    NOTE: *ppBuffer MUST BE NULL!!  This 
//                                    just helps verify that we are not 
//                                    creating memory leaks due to those who
//                                    mistakingly declare memory in ppBuffer
//                                    before passing it into this function.
//                                    Create the variable like this:  
//                                    'LPBYTE pBuffer = NULL;'
//                                    Pass it in like this:
//                                    'key.QueryValue("some\\key", &pBuffer);'
//                  bExpandEnvVars (in) - if FALSE, values such as "%PATH%" that
//                                    are imbedded in the string will NOT
//                                    be expanded automatically before 
//                                    returning. 
//  Return Values:	ERROR_SUCCESS if successful, or a nonzero error code 
//                  defined in "Winerror.h" if function failed.
//  Notes:			This function allocates space with the "new" directive,
//                  so successive calls to QueryKey() will delete the 
//                  previous buffer.  If you need to make multiple calls
//                  you must store the first returned value in a local 
//                  variable before making the second call to QueryKey().
// ----------------------------------------------------------------------- //
LONG SS_RegistryKey::QueryValue(LPCTSTR szName, LPBYTE* ppBuffer, 
                                LPDWORD pdwSize)
{
    ASSERT( *ppBuffer == NULL );
    ASSERT( HKey() );
    
    DWORD dwType = NULL;
    LONG lResult = 0;
    *pdwSize = 1;
    
    delete Buffer();
    Buffer(NULL);
    
    lResult = RegQueryValueEx( HKey(), szName, NULL, &dwType, NULL, pdwSize);
    if( ERROR_SUCCESS != lResult )
    {
        *ppBuffer = NULL;
        return lResult;
    }
    
    Buffer( new BYTE[*pdwSize] );
    lResult = RegQueryValueEx( HKey(), szName, NULL, &dwType, 
        (LPBYTE)Buffer(), pdwSize );
    if( ERROR_SUCCESS != lResult )
    {
        *ppBuffer = NULL;
        return lResult;
    }
    
    *ppBuffer = (LPBYTE)Buffer();
    return lResult;
}

LONG SS_RegistryKey::SetValue(LPCTSTR szName, LPCTSTR szBuffer)
{
    ASSERT(HKey());
    ASSERT(szBuffer);

    DWORD dwSize = _tcslen(szBuffer)*sizeof(TCHAR);
    DWORD lResult = RegSetValueEx( HKey(), szName, NULL, REG_SZ, 
                                   (LPBYTE)szBuffer, dwSize );

    return lResult;
}

LONG SS_RegistryKey::SetValue(LPCTSTR szName, DWORD dwBuffer)
{
    ASSERT(HKey());

    DWORD dwSize = sizeof(DWORD);
    DWORD lResult = RegSetValueEx( HKey(), szName, NULL, REG_DWORD, 
        (LPBYTE)&dwBuffer, dwSize );

    return lResult;
}

LONG SS_RegistryKey::SetValue(LPCTSTR szName, const LPBYTE pBuffer,
                              DWORD dwSize)
{
    ASSERT(HKey());
    ASSERT(pBuffer);

    DWORD lResult = RegSetValueEx( HKey(), szName, NULL, REG_BINARY, 
        pBuffer, dwSize );
    
    return lResult;
}
