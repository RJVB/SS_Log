// SS_Log_WindowDoc.h : interface of the SS_Log_WindowDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SS_LOG_WINDOWDOC_H__C6834C2A_D036_42BA_AE0E_679A719E60E3__INCLUDED_)
#define AFX_SS_LOG_WINDOWDOC_H__C6834C2A_D036_42BA_AE0E_679A719E60E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class SS_Log_WindowDoc : public CDocument
{
protected: // create from serialization only
	SS_Log_WindowDoc();
    DECLARE_DYNCREATE(SS_Log_WindowDoc)
        
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SS_Log_WindowDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SS_Log_WindowDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(SS_Log_WindowDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SS_LOG_WINDOWDOC_H__C6834C2A_D036_42BA_AE0E_679A719E60E3__INCLUDED_)
