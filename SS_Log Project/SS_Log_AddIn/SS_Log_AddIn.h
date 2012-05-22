// SS_Log_AddIn.h : main header file for the SS_LOG_ADDIN DLL
//

#if !defined(AFX_SS_LOG_ADDIN_H__B12E6046_CC33_42AD_8678_DE880FE39735__INCLUDED_)
#define AFX_SS_LOG_ADDIN_H__B12E6046_CC33_42AD_8678_DE880FE39735__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#include <ObjModel\textguid.h>
#include <ObjModel\dbgguid.h>


/////////////////////////////////////////////////////////////////////////////
// CSS_Log_AddInApp

class CSS_Log_AddInApp : public CWinApp
{
public:
    CSS_Log_AddInApp();
    
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSS_Log_AddInApp)
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL
    
    //{{AFX_MSG(CSS_Log_AddInApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SS_LOG_ADDIN_H__B12E6046_CC33_42AD_8678_DE880FE39735__INCLUDED)
