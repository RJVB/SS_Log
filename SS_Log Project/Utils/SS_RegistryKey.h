// ----------------------------------------------------------------------- //
//
//  FILENAME:	SS_RegistryKey.h
//  AUTHOR:		Steve Schaneville
//  CREATED:	25 Oct 2001, 08:14
//
//  PURPOSE:	
//
//  Copyright (c) 2001
//
// ----------------------------------------------------------------------- //
#ifndef __SS_RegistryKey_h__
#define __SS_RegistryKey_h__

// ------------------[       Pre-Include Defines       ]------------------ //
#include "windows.h"

// ------------------[          Include Files          ]------------------ //
// ------------------[          Macros/Defines         ]------------------ //
#ifndef ASSERT
#define ASSERT assert
#endif
// ------------------[      Constants/Enumerations     ]------------------ //
// ------------------[       Forward Declarations      ]------------------ //
// ------------------[         Global Variables        ]------------------ //
// ------------------[         Global Functions        ]------------------ //
LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 DWORD& dwBuffer);
LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPTSTR szBuffer, LPDWORD pdwSize);
LONG GetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPBYTE pBuffer, LPDWORD pdwSize);
LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 DWORD dwBuffer);
LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 LPCTSTR szBuffer);
LONG SetRegValue(HKEY hHive, LPCTSTR szKey, LPCTSTR szValue, 
                 const LPBYTE pBuffer, DWORD dwSize);

// ------------------[             Classes             ]------------------ //

// ----------------------------------------------------------------------- //
//  Class:			SS_RegistryKey
//  Author:			Steve Schaneville
//  Notes:			
// ----------------------------------------------------------------------- //
class SS_RegistryKey
{
public:

    // construction, destruction, assignment, copy, initialization
	//SS_RegistryKey();
    SS_RegistryKey(HKEY hKey);
    virtual ~SS_RegistryKey();

	SS_RegistryKey					    		(SS_RegistryKey& rhs);
	SS_RegistryKey&		operator =		        (SS_RegistryKey& rhs);

protected:

    virtual VOID	    InitObject		        ();

public:

	// accessor functions

    // Utilities
    virtual LONG        CreateKey               (LPCTSTR szKey, 
                                                 REGSAM nSecurity=KEY_ALL_ACCESS,
                                                 LPDWORD pdwDisposition=NULL);
    virtual LONG        CreateKey               (HKEY hKey, LPCTSTR szKey, 
                                                 REGSAM nSecurity=KEY_ALL_ACCESS,
                                                 LPDWORD pdwDisposition=NULL);
    virtual LONG        OpenKey                 (LPCTSTR szKey, 
                                                 REGSAM nSecurity=KEY_ALL_ACCESS);
    virtual LONG        OpenKey                 (HKEY hKey, LPCTSTR szKey, 
                                                 REGSAM nSecurity=KEY_ALL_ACCESS);
    virtual LONG        CloseKey                ();
    virtual LONG        QueryValue              (LPCTSTR szName, 
                                                 LPTSTR* ppBuffer);
    virtual LONG        QueryValue              (LPCTSTR szName, 
                                                 LPDWORD pdwBuffer);
    virtual LONG        QueryValue              (LPCTSTR szName, 
                                                 LPBYTE* ppBuffer,
                                                 LPDWORD pdwSize);
    virtual LONG        SetValue                (LPCTSTR szName, 
                                                 LPCTSTR szBuffer);
    virtual LONG        SetValue                (LPCTSTR szName, 
                                                 DWORD dwBuffer);
    virtual LONG        SetValue                (LPCTSTR szName, 
                                                 const LPBYTE pBuffer,
                                                 DWORD dwSize);
    virtual BOOL        IsOpen                  ();
    
protected:

    // accessor functions
    const HKEY          HKey                    () const;
    const HKEY          HKey                    (const HKEY hKey);
    LPVOID              Buffer                  () const;
    VOID                Buffer                  (LPVOID pBuffer);

private:

    HKEY                m_hKey;
    LPVOID              m_pBuffer;

};


// ----------------------------------------------------------------------- //
//  SS_RegistryKey Inline Functions
// ----------------------------------------------------------------------- //

// return the m_hKey
inline const HKEY SS_RegistryKey::HKey() const
{ return m_hKey; }

// set the m_hKey and return the previous value
inline const HKEY SS_RegistryKey::HKey(const HKEY hKey)
{ HKEY hOld = HKey(); m_hKey = hKey; return hOld; }

inline LPVOID SS_RegistryKey::Buffer() const
{ return m_pBuffer; }

inline VOID SS_RegistryKey::Buffer(LPVOID pBuffer)
{ m_pBuffer = pBuffer; } 

inline BOOL SS_RegistryKey::IsOpen()
{ BOOL bResult = HKey()? TRUE : FALSE; return bResult; } 


#endif // __SS_RegistryKey_h__
