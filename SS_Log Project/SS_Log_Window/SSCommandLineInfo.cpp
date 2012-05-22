// SSCommandLineInfo.cpp: implementation of the SSCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SS_Log_Window.h"
#include "SSCommandLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SSCommandLineInfo::SSCommandLineInfo(SS_Log_WindowApp* pApp) :
    m_pApp(pApp)
{
}

SSCommandLineInfo::~SSCommandLineInfo()
{
}

void SSCommandLineInfo::ParseParam( LPCTSTR lpszParam, BOOL bFlag, 
                                            BOOL bLast )
{
    //CCommandLineInfo::ParseParam( lpszParam, bFlag, bLast );
    if( bLast )
        _tcscpy( m_pApp->m_szWindowName, lpszParam );    
}

