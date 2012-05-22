// SS_Log_WindowDoc.cpp : implementation of the SS_Log_WindowDoc class
//

#include "stdafx.h"
#include "SS_Log_Window.h"

#include "SS_Log_WindowDoc.h"
#include "SS_Log_WindowView.h"
#include "SS_Log_Include.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowDoc

IMPLEMENT_DYNCREATE(SS_Log_WindowDoc, CDocument)

BEGIN_MESSAGE_MAP(SS_Log_WindowDoc, CDocument)
	//{{AFX_MSG_MAP(SS_Log_WindowDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowDoc construction/destruction

SS_Log_WindowDoc::SS_Log_WindowDoc()
{
	// TODO: add one-time construction code here

}
 
SS_Log_WindowDoc::~SS_Log_WindowDoc()
{
}

BOOL SS_Log_WindowDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowDoc serialization

void SS_Log_WindowDoc::Serialize(CArchive& ar)
{
	POSITION pos = GetFirstViewPosition();
	SS_Log_WindowView* pView = (SS_Log_WindowView*)GetNextView(pos);
    pView->Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowDoc diagnostics

#ifdef _DEBUG
void SS_Log_WindowDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void SS_Log_WindowDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowDoc commands

void SS_Log_WindowDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//CDocument::SetTitle(lpszTitle);
}
