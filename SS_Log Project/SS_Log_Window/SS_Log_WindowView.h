// SS_Log_WindowView.h : interface of the SS_Log_WindowView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SS_LOG_WINDOWVIEW_H__DB20F73D_B580_46B7_B9F0_4C5CE4A608B2__INCLUDED_)
#define AFX_SS_LOG_WINDOWVIEW_H__DB20F73D_B580_46B7_B9F0_4C5CE4A608B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// our columns in the ListCtrl
typedef enum SSLWColumn
{
    SSLW_COLUMN_ENTRY,
    SSLW_COLUMN_TIMEDATE,
    SSLW_COLUMN_MESSAGE,
    SSLW_COLUMN_FILE,
    SSLW_COLUMN_LINE,
    SSLW_COLUMN_THREAD,
    SSLW_COLUMN_LEVEL,
    SSLW_COLUMN_COUNT // this MUST be the last item in the list
};


class SS_Log_WindowApp;

class SS_Log_WindowView : public CListView
{
protected: // create from serialization only
	SS_Log_WindowView();
	DECLARE_DYNCREATE(SS_Log_WindowView)

// Attributes
public:
	SS_Log_WindowDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SS_Log_WindowView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void Serialize(CArchive& ar);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SS_Log_WindowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(SS_Log_WindowView)
	afx_msg void OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGrowUpToggle();
	afx_msg void ToggleHeld(), ToClipBoard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    LPVOID MyAllocMem( DWORD cb);
    LPDEVMODE ChangeDevMode(HWND hWnd, char * pDevice);
    VOID ReadLog(TCHAR* szText, int nItem, int nSize);
	VOID CreatePipeThread();
	virtual VOID WriteLog(TCHAR* szLog);

    const HANDLE Pipe() const { return m_hPipe; }
    VOID Pipe(const HANDLE hPipe) { m_hPipe = hPipe; }
    
    VOID EraseLog();

private:

    HANDLE m_hPipe;
	BOOL m_bInitialized;
    CWinThread* m_pThread;
	BOOL m_bGrowUp, m_bHeld;

protected:
    
    LPDEVMODE lpDevMode;
    
};

#ifndef _DEBUG  // debug version in SS_Log_WindowView.cpp
inline SS_Log_WindowDoc* SS_Log_WindowView::GetDocument()
   { return (SS_Log_WindowDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SS_LOG_WINDOWVIEW_H__DB20F73D_B580_46B7_B9F0_4C5CE4A608B2__INCLUDED_)
