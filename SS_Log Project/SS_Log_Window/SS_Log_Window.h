// SS_Log_Window.h : main header file for the SS_LOG_WINDOW application
//

#if !defined(AFX_SS_LOG_WINDOW_H__64FC0947_3265_44B2_8B43_6FF5A16BF80F__INCLUDED_)
#define AFX_SS_LOG_WINDOW_H__64FC0947_3265_44B2_8B43_6FF5A16BF80F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowApp:
// See SS_Log_Window.cpp for the implementation of this class
//

class SS_Log_WindowApp : public CWinApp
{
public:
	SS_Log_WindowApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SS_Log_WindowApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(SS_Log_WindowApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

    TCHAR m_szWindowName[MAX_PATH];
    
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SS_LOG_WINDOW_H__64FC0947_3265_44B2_8B43_6FF5A16BF80F__INCLUDED_)
