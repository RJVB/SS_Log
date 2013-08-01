// SS_Log_WindowView.cpp : implementation of the SS_Log_WindowView class
//

#include "stdafx.h"
#include <winspool.h>
#include "SS_Log_Window.h"

#include "SS_Log_Window.h"
#include "SS_Log_WindowDoc.h"
#include "SS_Log_WindowView.h"
#include "MainFrm.h"
#include "SS_RegistryKey.h"

#include "SS_Log_Include.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SS_Log_WindowApp theApp;


// ----------------------------------------------------------------------- //
//  Function:		EnumWindowsProc()
//  Author:			Steve Schaneville
//  Parameters:		hwnd [in] - Handle to the found window (see MSDN).
//                  lParam [out] - pointer to an HWND object so we can
//                  return the found-window's window handle.
//  Return Values:	(see MSDN)
//  Notes:			Looking for an instance of "Microsoft Visual C++".
//                  If we find it, return the associated window handle
//                  via the lParam parameter.
// ----------------------------------------------------------------------- //
BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    HWND* phWnd = (HWND*)lParam;
    
    TCHAR szText[256];
    TCHAR* szTextBuff = szText;
    GetWindowText(hwnd, szText, 256);
    
    while( TRUE )
    {
        int nResult = strcspn( szTextBuff, "Microsoft Visual C++" );
        if( _tcslen(szTextBuff) == (UINT)nResult )
            break;
        
        szTextBuff += nResult;
        if( !_tcsncmp(szTextBuff, "Microsoft Visual C++", _tcslen("Microsoft Visual C++")) )
        {                
            *phWnd = hwnd;
            return FALSE;
        }

        szTextBuff++;
    }
    
    return TRUE;
}

// ----------------------------------------------------------------------- //
//  Function:		PipeThread()
//  Author:			Steve Schaneville
//  Parameters:		pParam [in] - a pointer to the SS_Log_WindowView window.
//  Return Values:	(see MSDN)
//  Notes:			This function created a named pipe.  The name is based
//                  on the name of the app's window, which was set by
//                  the SS_Log class (so it is possible to have multiple
//                  pipes, and hence, multiple separate logs).  The function
//                  then waits for a connection to the pipe from a client.
//                  On connection, we check the message to see if it is an
//                  erase command (szText == "~~WMSS_LOG_ERASE_LOG~~") and
//                  if so, erase the log window.  Assume that if it is not
//                  an erase message, then it is a log message, so send the
//                  message to the SS_Log_WindowView::WriteLog() function.
//                  Finally, loop back and wait for another message.
// ----------------------------------------------------------------------- //
UINT PipeThread( LPVOID pParam )
{
    BOOL bResult = FALSE;
    SS_Log_WindowView* pView = (SS_Log_WindowView*)pParam;
    
    // let's create our named pipe with no restrictions on access.
    SECURITY_DESCRIPTOR sd;
    bResult = InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );
    sd.Dacl = NULL;
    
    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = &sd;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    
    BOOL bDaclPresent = TRUE;
    BOOL bDaclDefaulted = FALSE; 
    bResult = SetSecurityDescriptorDacl( &sd, bDaclPresent, 
										 (PACL)NULL, 
										 bDaclDefaulted );

    TCHAR szPipeName[MAX_PATH];
    sprintf( szPipeName, _T("\\\\.\\pipe\\%s"), theApp.m_szWindowName );
    HANDLE hPipe = CreateNamedPipe( szPipeName,
                                    PIPE_ACCESS_DUPLEX, 
                                    PIPE_TYPE_MESSAGE|PIPE_WAIT|PIPE_READMODE_MESSAGE, 
                                    PIPE_UNLIMITED_INSTANCES, 
                                    SSLOG_MAX_MESSAGE_LENGTH,
                                    0, 5000, &sa );

	pView->Pipe(hPipe);
  
    if( pView->Pipe() == INVALID_HANDLE_VALUE )
    {
        pView->MessageBox( _T("Could not create the pipe for incoming messages.  No messages can be received.   "), 
            _T("Fatal Error"), MB_OK|MB_ICONSTOP );
        return FALSE;
    }
    
    // now we loop forever, waiting for messages.  As they come in, send them
    // to the SS_Log_WindowView::EraseLog() and SS_Log_WindowView::WriteLog()
    // functions as appropriate.
    while( TRUE )
    {
        ConnectNamedPipe( pView->Pipe(), NULL );

        TCHAR szText[SSLOG_MAX_MESSAGE_LENGTH] = _T("");
        DWORD dwBytesRead;
		ReadFile( pView->Pipe(), szText, SSLOG_MAX_MESSAGE_LENGTH, 
				  &dwBytesRead, NULL );

		WriteFile( pView->Pipe(), NULL, 0, &dwBytesRead, NULL );
		DisconnectNamedPipe( pView->Pipe() );

        if( _tcscmp(szText, _T("")) )
        {
            if( !_tcscmp(szText, _T("~~WMSS_LOG_ERASE_LOG~~")) )
                pView->EraseLog();
            else
                pView->WriteLog(szText);
        }
    } 
    
    return TRUE;
}

// helps us pass some information to the ListCtrlSortItems() function
typedef struct tagSortItemsInfo
{
    CListCtrl*  pListCtrl;
    int         nColumn;
} SORT_ITEMS_INFO, *LPSORT_ITEMS_INFO;

// this helps us sort any column reverse or forward when the user clicks a
// column heading multiple times.  Out of laziness, the variable is used for 
// all columns instead of having a separate variable for each column.
BOOL g_bReverseSort = TRUE;

// ----------------------------------------------------------------------- //
//  Function:		ListCtrlSortItems()
//  Author:			Steve Schaneville
//  Parameters:		(see MSDN)
//  Return Values:	(see MSDN)
//  Notes:			We are sorting the columns here when the user clicks
//                  on a column heading.  There are different comparisons
//                  depending on which column is clicked.
// ----------------------------------------------------------------------- //
int CALLBACK ListCtrlSortItems(LPARAM lParam1, LPARAM lParam2, 
                         LPARAM lParamSort)
{
    LPSORT_ITEMS_INFO pSII = (LPSORT_ITEMS_INFO)lParamSort;
    CListCtrl* pList = pSII->pListCtrl;
    
    LVFINDINFO fi;
    fi.flags = LVFI_PARAM;
    fi.lParam = lParam1;
    int nItem1 = pList->FindItem(&fi);

    fi.flags = LVFI_PARAM;
    fi.lParam = lParam2;
    int nItem2 = pList->FindItem(&fi);

    if( -1 == nItem1 || -1 == nItem2 )
        return 0;

    CString cs1 = pSII->pListCtrl->GetItemText(nItem1, pSII->nColumn);
    CString cs2 = pSII->pListCtrl->GetItemText(nItem2, pSII->nColumn);

    int nResult = 0;
    switch( pSII->nColumn )
    {
    case SSLW_COLUMN_TIMEDATE:
        {
            SYSTEMTIME st1;
            SYSTEMTIME st2;
            FILETIME ft1;
            FILETIME ft2;
            
            st1.wHour   = _ttoi(cs1.Mid( 0,2).GetBuffer(10));
            st1.wMinute = _ttoi(cs1.Mid( 3,2).GetBuffer(10));
            st1.wSecond = _ttoi(cs1.Mid( 6,2).GetBuffer(10));
            st1.wDay    = _ttoi(cs1.Mid(10,2).GetBuffer(10));
            st1.wMonth  = _ttoi(cs1.Mid(13,2).GetBuffer(10));
            st1.wYear   = _ttoi(cs1.Mid(16,4).GetBuffer(10));
            st1.wMilliseconds = 0;
            
            st2.wHour   = _ttoi(cs2.Mid( 0,2).GetBuffer(10));
            st2.wMinute = _ttoi(cs2.Mid( 3,2).GetBuffer(10));
            st2.wSecond = _ttoi(cs2.Mid( 6,2).GetBuffer(10));
            st2.wDay    = _ttoi(cs2.Mid(10,2).GetBuffer(10));
            st2.wMonth  = _ttoi(cs2.Mid(13,2).GetBuffer(10));
            st2.wYear   = _ttoi(cs2.Mid(16,4).GetBuffer(10));
            st2.wMilliseconds = 0;
            
            SystemTimeToFileTime( &st1, &ft1 );
            SystemTimeToFileTime( &st2, &ft2 );

            nResult = CompareFileTime( &ft1, &ft2 );
        }
    	break;
    case SSLW_COLUMN_LINE:
    case SSLW_COLUMN_ENTRY:
    case SSLW_COLUMN_THREAD:
        {
            int nLine1 = _ttoi(cs1.GetBuffer(10));
            int nLine2 = _ttoi(cs2.GetBuffer(10));
            nResult = nLine1 < nLine2 ? -1 : 1;
            if( nLine1 == nLine2 )
                nResult = 0;
        }
        break;
    case SSLW_COLUMN_MESSAGE:
    case SSLW_COLUMN_FILE:
    case SSLW_COLUMN_LEVEL:
        {
            nResult = cs1.Compare(cs2);
        }
        break;
    default:
        break;
    }
    
    // reverse the order if it is time
	if( g_bReverseSort ){
		if( nResult ){
            nResult = nResult == -1 ? 1 : -1;
		}
	}
   
        return nResult;
}


/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView

IMPLEMENT_DYNCREATE(SS_Log_WindowView, CListView)

BEGIN_MESSAGE_MAP(SS_Log_WindowView, CListView)
//{{AFX_MSG_MAP(SS_Log_WindowView)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnInsertItem)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_COMMAND(ID_VIEW_CLEAR, EraseLog)
	ON_COMMAND(ID_VIEW_GROWUP, OnGrowUpToggle)
	ON_COMMAND(ID_TOGGLE_HELD, ToggleHeld)
	ON_COMMAND(ID_EDIT_COPY, ToClipBoard)
	//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView construction/destruction

SS_Log_WindowView::SS_Log_WindowView() : 
m_hPipe(NULL), m_bInitialized(FALSE), m_pThread(NULL),
m_bGrowUp(FALSE), m_bHeld(FALSE)
{
    // TODO: add construction code here
}

SS_Log_WindowView::~SS_Log_WindowView()
{
}

BOOL SS_Log_WindowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs
    cs.style |= WS_CHILD|WS_VISIBLE|LVS_REPORT;
    cs.dwExStyle |= LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_TRACKSELECT;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView drawing

void SS_Log_WindowView::OnDraw(CDC* pDC)
{
	SS_Log_WindowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void SS_Log_WindowView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

    // create our columns and the pipe thread
    if( !m_bInitialized )
	{
        GetListCtrl().InsertColumn(SSLW_COLUMN_ENTRY, _T("ID"), LVCFMT_LEFT, 45, -1);
		GetListCtrl().InsertColumn(SSLW_COLUMN_TIMEDATE, _T("Time, Date"), LVCFMT_LEFT, 200, -1);
		GetListCtrl().InsertColumn(SSLW_COLUMN_MESSAGE, _T("Message"), LVCFMT_LEFT, 520, -1);
		GetListCtrl().InsertColumn(SSLW_COLUMN_FILE, _T("File"), LVCFMT_LEFT, 130, -1);
		GetListCtrl().InsertColumn(SSLW_COLUMN_LINE, _T("Line #"), LVCFMT_LEFT, 50, -1);
        GetListCtrl().InsertColumn(SSLW_COLUMN_THREAD, _T("Thread"), LVCFMT_LEFT, 50, -1);
        GetListCtrl().InsertColumn(SSLW_COLUMN_LEVEL, _T("Level"), LVCFMT_LEFT, 50, -1);

        GetListCtrl().SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|
                                       LVS_EX_INFOTIP|LVS_EX_ONECLICKACTIVATE);

		CreatePipeThread();

		m_bInitialized = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView printing

BOOL SS_Log_WindowView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void SS_Log_WindowView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void SS_Log_WindowView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView diagnostics

#ifdef _DEBUG
void SS_Log_WindowView::AssertValid() const
{
	CListView::AssertValid();
}

void SS_Log_WindowView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}

SS_Log_WindowDoc* SS_Log_WindowView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SS_Log_WindowDoc)));
    return (SS_Log_WindowDoc*)m_pDocument;
}
#endif //_DEBUG

//  Function:		SS_Log_WindowView::OnGrowUpToggle()
//  Author:			RJVB
//  Notes:			Toggle the m_bGrowUp member. Should mark the current state in the menuitem!
// ----------------------------------------------------------------------- //
void SS_Log_WindowView::OnGrowUpToggle()
{ SORT_ITEMS_INFO sii;

	m_bGrowUp = !m_bGrowUp;
    sii.nColumn = SSLW_COLUMN_ENTRY;
    sii.pListCtrl = &GetListCtrl();
    g_bReverseSort = m_bGrowUp;
    GetListCtrl().SortItems(ListCtrlSortItems, (LPARAM)&sii);
	if( m_bGrowUp ){
		GetListCtrl().EnsureVisible( 0, FALSE );
	}
	else{
		GetListCtrl().EnsureVisible( GetListCtrl().GetItemCount(), FALSE );
	}
}

//  Function:		SS_Log_WindowView::ToggleHeld()
//  Author:			RJVB
//  Notes:			Toggle the m_bHeld member. Should mark the current state in the menuitem!
// ----------------------------------------------------------------------- //
void SS_Log_WindowView::ToggleHeld()
{
	m_bHeld = !m_bHeld;
}

// from http://us.generation-nt.com/answer/copying-contents-listview-report-mode-help-7183412.html
void SS_Log_WindowView::ToClipBoard()
{ CListCtrl &log = GetListCtrl();
  CString sSelection = "", sRow, sID, sTimeDate, sMsg, sFile, sLineNr, sThread;
  int row;
  POSITION line = log.GetFirstSelectedItemPosition();

	if( line ){
		while( line ){
			sRow = "";
			row = log.GetNextSelectedItem(line);
			sID = log.GetItemText(row, SSLW_COLUMN_ENTRY);
			sTimeDate = log.GetItemText(row, SSLW_COLUMN_TIMEDATE);
			sMsg = log.GetItemText(row, SSLW_COLUMN_MESSAGE);
			sFile = log.GetItemText(row, SSLW_COLUMN_FILE);
			sLineNr = log.GetItemText(row, SSLW_COLUMN_LINE);
			sThread = log.GetItemText(row, SSLW_COLUMN_THREAD);
			sRow.Format( "#%s %s[%s] (%s::%s):\t%s\n", sID, sTimeDate, sThread, sFile, sLineNr, sMsg );
			sSelection += sRow;
		}
		if( OpenClipboard() ){
		 HGLOBAL clipbuffer;
		 char *buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc( GMEM_DDESHARE, sSelection.GetLength() + 1 );
			buffer = (char*) GlobalLock(clipbuffer);
			strcpy( buffer, LPCSTR(sSelection) );
			GlobalUnlock(clipbuffer);
			SetClipboardData( CF_TEXT, clipbuffer );
			CloseClipboard();
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
// SS_Log_WindowView message handlers

// ----------------------------------------------------------------------- //
//  Function:		SS_Log_WindowView::WriteLog()
//  Author:			Steve Schaneville
//  Parameters:		szLog [in] - a tab-delimited log entry string.
//  Return Values:	none
//  Notes:			The input string, presumably created by the SS_Log class,
//                  should be tab-delimited with these entries:
//                  Time/date -> Filename -> Line # -> Level -> Message Text.
//		RJVB 20110312: modified to treat \n characters in the log message text.
// ----------------------------------------------------------------------- //
VOID SS_Log_WindowView::WriteLog(TCHAR *szLog)
{
	TCHAR *szTok, *msgText = NULL, *c, *d;
    TCHAR chrTmp = '\0';
    TCHAR szEntry[32];
	
    LVITEM newline, entry, timedate, prog, file, line, thread, level, message;
	int lines = 0, loop = 0;
    BOOL hasThreadField = FALSE;

	do{
		newline.mask = LVIF_TEXT|LVIF_PARAM;
		newline.lParam = GetListCtrl().GetItemCount();
		newline.pszText = &chrTmp;
		// RJVB 20110312: changed iItem from 0 to get 'normal' sorting!
		if( m_bGrowUp ){
			newline.iItem = 0;
		}
		else{
			newline.iItem = GetListCtrl().GetItemCount();
		}
		newline.iSubItem = 0;
		GetListCtrl().InsertItem(&newline);

		entry = newline;
		_itot(entry.lParam, szEntry, 10);
		entry.mask = LVIF_TEXT;
		entry.pszText = szEntry;
		entry.iSubItem = SSLW_COLUMN_ENTRY;
		GetListCtrl().SetItem(&entry);

        if( szLog[0] == 'T' ){
            // 20130320: flag indicating that the thread field is present.
            hasThreadField = TRUE;
            szLog++;
        }

		timedate = entry;
//		szTok = _tcstok( szLog, _T("\t") );
		szTok = szLog;
		if( (c = strchr(szTok, '\t')) ){
			*c = '\0';
		}
		// treat non-conforming strings (i.e. without tab separators) as a simple, untagged/dated log message
		if( c ){
			timedate.pszText = szTok;
			timedate.iSubItem = SSLW_COLUMN_TIMEDATE;
			GetListCtrl().SetItem(&timedate);
			if( c ){
				*c = '\t';
				szTok = &c[1];
			}

			file = timedate;
	//		szTok = _tcstok( NULL, _T("\t") );
			if( (c = strchr(szTok, '\t')) ){
				*c = '\0';
			}
			file.pszText = szTok;
			file.iSubItem = SSLW_COLUMN_FILE;
			GetListCtrl().SetItem(&file);
			if( c ){
				*c = '\t';
				szTok = &c[1];
			}

			line = file;
	//		szTok = _tcstok( NULL, _T("\t") );
			if( (c = strchr(szTok, '\t')) ){
				*c = '\0';
			}
			line.pszText = szTok;
			line.iSubItem = SSLW_COLUMN_LINE;
			GetListCtrl().SetItem(&line);
			if( c ){
				*c = '\t';
				szTok = &c[1];
			}

            thread = line;
            if( hasThreadField ){
        //		szTok = _tcstok( NULL, _T("\t") );
                if( (c = strchr(szTok, '\t')) ){
                    *c = '\0';
                }
                thread.pszText = szTok;
                thread.iSubItem = SSLW_COLUMN_THREAD;
                GetListCtrl().SetItem(&thread);
                if( c ){
                    *c = '\t';
                    szTok = &c[1];
                }
            }

			level = thread;
	//		szTok = _tcstok( NULL, _T("\t") );
			if( (c = strchr(szTok, '\t')) ){
				*c = '\0';
			}
			level.pszText = szTok;
			level.iSubItem = SSLW_COLUMN_LEVEL;
			GetListCtrl().SetItem(&level);
			if( c ){
				*c = '\t';
				szTok = &c[1];
			}

			message = level;
	//		szTok = _tcstok( NULL, _T("\t") );
			if( (c = strchr(szTok, '\t')) ){
				*c = '\0';
			}
		}
		else{
			message = entry;
		}
		if( !msgText ){
			// first time around, we pick up the start of the log message, which
			// is in szTok.
			msgText = szTok;
		}
		// we check for a newline character, and cut the string if there's one.
		if( (d = strchr(msgText, '\n')) ){
			*d = '\0';
		}
		message.pszText = msgText;
		message.iSubItem = SSLW_COLUMN_MESSAGE;
		GetListCtrl().SetItem(&message);
		if( d ){
			// a non-null 'd' means we'd found a newline: restore it
			*d = '\n';
			// spool the msgText to the new line:
			msgText = &d[1];
			// if there's more to print, we loop to do all of this once more
			if( *msgText ){
				loop = 1;
			}
			else{
				loop = 0;
			}
		}
		else{
			loop = 0;
		}
		if( c ){
			*c = '\t';
			szTok = &c[1];
		}

		lines += 1;
	}
	while( loop );
	if( !m_bHeld ){
		GetListCtrl().EnsureVisible( newline.iItem, FALSE );
	}
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log_WindowView::ReadLog()
//  Author:			Steve Schaneville
//  Parameters:		szText [out] - A textbuffer that will receive the log 
//                                 entry information (tab delimited).
//                  nItem [in]   - The item in the ListCtrl that is to be read.
//                  nSize [in]   - The size of the szText buffer.  
//  Return Values:	none
//  Notes:			This function produces a tab-delimited text string 
//                  of the sub-entries of a given ListCtrl item.  The order
//                  of the sub-entries are NOT in the same order as they are
//                  in the ListCtrl, but rather, in the order that they are
//                  produced by the SS_Log class.
// ----------------------------------------------------------------------- //
VOID SS_Log_WindowView::ReadLog(TCHAR *szText, int nItem, int nSize)
{
    TCHAR szTextIn[SSLOG_MAX_MESSAGE_LENGTH];

    LVITEM item;
    item.mask = LVIF_TEXT;
    item.pszText = szTextIn;
    item.cchTextMax = SSLOG_MAX_MESSAGE_LENGTH;
    item.iItem = nItem;
    item.iSubItem = SSLW_COLUMN_TIMEDATE;
    GetListCtrl().GetItem(&item);
    _tcsncpy( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\t"), nSize );
    nSize -= _tcslen(_T("\t"));
    
    item.iSubItem = SSLW_COLUMN_FILE;
    GetListCtrl().GetItem(&item);
    _tcsncat( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\t"), nSize );
    nSize -= _tcslen(_T("\t"));
    
    item.iSubItem = SSLW_COLUMN_LINE;
    GetListCtrl().GetItem(&item);
    _tcsncat( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\t"), nSize );
    nSize -= _tcslen(_T("\t"));
    
    item.iSubItem = SSLW_COLUMN_THREAD;
    GetListCtrl().GetItem(&item);
    _tcsncat( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\t"), nSize );
    nSize -= _tcslen(_T("\t"));
    
    item.iSubItem = SSLW_COLUMN_LEVEL;
    GetListCtrl().GetItem(&item);
    _tcsncat( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\t"), nSize );
    nSize -= _tcslen(_T("\t"));
    
    item.iSubItem = SSLW_COLUMN_MESSAGE;
    GetListCtrl().GetItem(&item);
    _tcsncat( szText, szTextIn, nSize );
    nSize -= _tcslen(szTextIn);
    _tcsncat( szText, _T("\n"), nSize );
    nSize -= _tcslen(_T("\t"));
}

void SS_Log_WindowView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
    // TODO: Add your specialized code here and/or call the base class
    MessageBox("This feature is not currently implimented.  Don't expect the print-out to look good...   :-(");

    CFont font;
    CFont* pFont;
    
    font.CreateFont(80, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
        "Arial");
    pFont = pDC->SelectObject(&font);

    pDC->SetMapMode(MM_TEXT);
    CSize size = pDC->GetViewportExt();
    pDC->HIMETRICtoDP(&size);

    int nLvl=30, nMsg=1000, nTime=3000, nFile=4000, nLine=5000, nEnd=6000;
    int nSpace=10, nHeight=100;

    for( int i=GetListCtrl().GetItemCount()-1, j=0; i>-1; i--, j++ )
    {
        CString csMsg, csLvl, csLine, csFile, csTime;
        TCHAR szTextFinal[SSLOG_MAX_MESSAGE_LENGTH];            
        ReadLog(szTextFinal, i, SSLOG_MAX_MESSAGE_LENGTH);

        csTime = _tcstok( szTextFinal, "\t" );
        csFile = _tcstok( NULL, "\t" );
        csLine = _tcstok( NULL, "\t" );
        csLvl = _tcstok( NULL, "\t" );
        csMsg = _tcstok( NULL, "\n" );

        pDC->TextOut(nLvl, nHeight*j, csLvl);
        pDC->TextOut(nMsg, nHeight*j, csMsg);
        pDC->TextOut(nTime, nHeight*j, csTime);
        pDC->TextOut(nFile, nHeight*j, csFile);
        pDC->TextOut(nLine, nHeight*j, csLine);
    }

    CListView::OnPrint(pDC, pInfo);
}

VOID SS_Log_WindowView::EraseLog()
{
    GetListCtrl().DeleteAllItems();
}

VOID SS_Log_WindowView::CreatePipeThread()
{
    // create the pipe thread
    AFX_THREADPROC pfnThreadProc = PipeThread;
    m_pThread = AfxBeginThread(pfnThreadProc, this, THREAD_PRIORITY_NORMAL, 0/*, CREATE_SUSPENDED*/);
}

void SS_Log_WindowView::Serialize(CArchive& ar) 
{
    if (ar.IsStoring())
    {	// storing code
        if( m_bGrowUp ){
            for( int i=GetListCtrl().GetItemCount()-1; i>-1; i-- )
            {
                TCHAR szTextFinal[SSLOG_MAX_MESSAGE_LENGTH];            
                ReadLog(szTextFinal, i, SSLOG_MAX_MESSAGE_LENGTH);
                ar.WriteString(szTextFinal);
            }
        }
        else{
            for( int i = 0, N = GetListCtrl().GetItemCount() ; i < N ; i++ ){
              TCHAR szTextFinal[SSLOG_MAX_MESSAGE_LENGTH];            
                ReadLog( szTextFinal, i, SSLOG_MAX_MESSAGE_LENGTH );
                ar.WriteString(szTextFinal);
            }
        }
    }
    else
    {	// loading code
        GetListCtrl().DeleteAllItems();

        CString csText;
        ar.ReadString(csText);
        while( !csText.IsEmpty() )
        {
            WriteLog(csText.GetBuffer(10));
            ar.ReadString(csText);
        }
    }
}


void SS_Log_WindowView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    // TODO: Add your specialized code here and/or call the base class
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log_WindowView::OnInsertItem()
//  Author:			Steve Schaneville
//  Parameters:		(see MSDN)
//  Return Values:	(see MSDN)
//  Notes:			here we just increment the "item count" displayed
//                  in the status bar.
// ----------------------------------------------------------------------- //
void SS_Log_WindowView::OnInsertItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    TCHAR szStatus[256];
    sprintf( szStatus, _T("Log Entry Count: %d"), GetListCtrl().GetItemCount() );
    int i = ((CMainFrame*)GetTopLevelFrame())->GetStatusBar()->SetPaneText(ID_SEPARATOR, szStatus);
    
    *pResult = 0;
}


void SS_Log_WindowView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
    if(pDC->IsPrinting())
    {
        TCHAR szPrinter[MAX_PATH];
        DWORD dwSize = MAX_PATH;
        //GetDefaultPrinter(szPrinter, &dwSize);
        GetProfileString( "windows", "device", "", szPrinter, MAX_PATH );
        _tcstok(szPrinter, ",");
        
        lpDevMode = ChangeDevMode(m_hWnd,szPrinter);
        pDC->ResetDC(lpDevMode);
    }	
    CListView::OnPrepareDC(pDC, pInfo);
}

LPVOID SS_Log_WindowView::MyAllocMem(DWORD cb)
{
    return VirtualAlloc(NULL, cb, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

LPDEVMODE SS_Log_WindowView::ChangeDevMode(HWND hWnd, char *pDevice)
{
   HANDLE      hPrinter;
   LPDEVMODE   pDevMode;
   DWORD       dwNeeded, dwRet;

   /* Start by opening the printer */
   if (!OpenPrinter(pDevice, &hPrinter, NULL))
       return NULL;

   /*
    * Step 1:
    * Allocate a buffer of the correct size.
    */
   dwNeeded = DocumentProperties(hWnd,
       hPrinter,       /* handle to our printer */
       pDevice,        /* Name of the printer */
       NULL,           /* Asking for size so */
       NULL,           /* these are not used. */
       0);             /* Zero returns buffer size. */
   pDevMode = (LPDEVMODE)malloc(dwNeeded);

   /*
    * Step 2:
    * Get the default DevMode for the printer and
    * modify it for our needs.
    */
   dwRet = DocumentProperties(hWnd,
       hPrinter,
       pDevice,
       pDevMode,       /* The address of the buffer to fill. */
       NULL,           /* Not using the input buffer. */
       DM_OUT_BUFFER); /* Have the output buffer filled. */
   if (dwRet != IDOK)
   {
       /* if failure, cleanup and return failure */
       free(pDevMode);
       ClosePrinter(hPrinter);
       return NULL;
   }
   
   /*
   * Make changes to the DevMode which are supported.
   */
   if (pDevMode->dmFields & DM_ORIENTATION)       
   {
       /* if the printer supports paper orientation, set it*/
       pDevMode->dmOrientation = DMORIENT_LANDSCAPE;       
   }
   /*
   * Step 3:
   * Merge the new settings with the old.
   * This gives the driver a chance to update any private
   * portions of the DevMode structure.
   */
   dwRet = DocumentProperties(hWnd,
       hPrinter,
       pDevice,
       pDevMode,       /* Reuse our buffer for output. */
       pDevMode,       /* Pass the driver our changes. */
       DM_IN_BUFFER |  /* Commands to Merge our changes and */
       DM_OUT_BUFFER); /* write the result. */
   
   /* Done with the printer */
   ClosePrinter(hPrinter);
   
   if (dwRet != IDOK)
   {
       /* if failure, cleanup and return failure */
       free(pDevMode);
       return NULL;
   }
   
   /* return the modified DevMode structure */
   return pDevMode;

}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log_WindowView::OnColumnClick()
//  Author:			Steve Schaneville
//  Parameters:		(see MSDN)
//  Return Values:	(see MSDN)
//  Notes:			This is for sorting.  See the global ListCtrlSortItems()
//                  function at the top of this file.
// ----------------------------------------------------------------------- //
void SS_Log_WindowView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    SORT_ITEMS_INFO sii;
    sii.nColumn = pNMListView->iSubItem;
    sii.pListCtrl = &GetListCtrl();

    g_bReverseSort = !g_bReverseSort;
    GetListCtrl().SortItems(ListCtrlSortItems, (LPARAM)&sii);

    *pResult = 0;
}

// ----------------------------------------------------------------------- //
//  Function:		SS_Log_WindowView::OnDoubleClick()
//  Author:			Steve Schaneville
//  Parameters:		(see MSDN)
//  Return Values:	(see MSDN)
//  Notes:			here we are going to send a message to an open instance
//                  of MSDEV to open a file and go to the correct line number.
//                  If there is no open instance of MSDEV, open one.  If
//                  the SS_Log_AddIn is not installed, let the user know
//                  and offer to install it.  Do the install.  
// ----------------------------------------------------------------------- //
void SS_Log_WindowView::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	{ // RJVB 20110310:
	  static char shown = 0;
		if( !shown ){
			MessageBox("This function is no longer operative.", "Legacy dysfunction", MB_OK|MB_ICONEXCLAMATION);
			shown = 1;
		}
        *pResult = 0;
        return;
	}

    // prepare the message for the MSDEV window
    TCHAR szCommandLine[MAX_PATH];
    sprintf( szCommandLine, _T("\"%s\";%s"), 
        GetListCtrl().GetItemText(pNMListView->iItem, SSLW_COLUMN_FILE).GetBuffer(10),
        GetListCtrl().GetItemText(pNMListView->iItem, SSLW_COLUMN_LINE).GetBuffer(10));

	// Check to see if the SS_Log_AddIn is properly installed.  Prompt the 
    // user for installation if necessary. 
    BOOL bShowMessage = FALSE;
    SS_RegistryKey key(HKEY_CURRENT_USER);
    if( key.OpenKey("Software\\Microsoft\\DevStudio\\6.0\\AddIns\\SS_Log_AddIn.DSAddIn.1") )
    {
        TCHAR szMessage[1024];
        LoadString(AfxGetInstanceHandle(), IDS_INSTALL_SS_LOG_ADDIN_PROMPT, szMessage, 1024);
        int nMBResult = MessageBox(szMessage, "Install SS_Log_AddIn", MB_YESNO);
        if( IDNO == nMBResult ) // user responded "yes"?
        {
            *pResult = 0;
            return;
        }
        else
        {
            bShowMessage = TRUE;

            TCHAR szDesc[1024];
            LoadString(AfxGetInstanceHandle(), IDS_ADDIN_DESCRIPTION, szDesc, 1024);

            // open the regkey that contains the system root for either NT or Win98
            key.OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
            if(!key.IsOpen())
                key.OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion");


            TCHAR* szSystemRoot = NULL;
            if(key.IsOpen())
            {
                DWORD dwSize = NULL;
                key.QueryValue("SystemRoot", (unsigned char**)&szSystemRoot, &dwSize);

                // did we find the SystemRoot?
                if( dwSize )
                {	
                    TCHAR szFile[MAX_PATH];
                    _tcscpy(szFile, szSystemRoot);
                    _tcscat(szFile, _T("\\SS_Log_AddIn.dll"));

                    // is the add-in dll present in the SystemRoot directory already?
                    // if not, abort... the user should place the file there.
                    WIN32_FIND_DATA fd;
                    HANDLE hFile = FindFirstFile(szFile, &fd);
                    if( INVALID_HANDLE_VALUE != hFile )
                    {
                        // create the registry entries for MSDEV to find the add-in
                        key.CreateKey(HKEY_CURRENT_USER, "Software\\Microsoft\\DevStudio\\6.0\\AddIns\\SS_Log_AddIn.DSAddIn.1");
                        key.SetValue("", "1");
                        key.SetValue("Description", szDesc);
                        key.SetValue("DisplayName", "SS_Log_AddIn");
                        key.SetValue("Filename", szFile);

                        key.CreateKey(HKEY_CURRENT_USER, "Software\\Microsoft\\DevStudio\\6.0\\AddIns\\SS_Log_AddIn.DSAddIn.1\\Toolbar");
                        key.SetValue("SS_Log_AddInCommand", 9);

                        // register the add-in dll with regsvr32 (it is a com object).
                        TCHAR szCommand[MAX_PATH*2];
                        _tcscpy(szCommand, "regsvr32 ");
                        _tcscat(szCommand, szFile);
                        WinExec(szCommand, SW_SHOWNORMAL);
                        
                        bShowMessage = FALSE;
                    }
                    else
                    {
                        MessageBox("Could not find the SS_Log_AddIn.dll file in your %SYSTEMROOT% directory.  Please copy SS_Log_AddIn.dll and SS_Log_Window.exe to your %SYSTEMROOT% (ex. \"C:\\Winnt\") and try this again.");
                        bShowMessage = FALSE;
                    }
                }
            }
		}
	}

    if( bShowMessage )
    {
        MessageBox("Installation failed.  See the notes in the SS_Log_AddIn.h file for instructions on how to install the plug-in manually.", "Information", MB_OK|MB_ICONINFORMATION);
        *pResult = 0;
        return;
    }

    // look to see if there is an open MSDEV window
    HWND hWnd = NULL;
    WNDENUMPROC lpEnumFunc = EnumWindowsProc;
    EnumWindows( lpEnumFunc, (LPARAM)&hWnd );
    
    // open a new instance of MSDEV if there is not already one running
    if( !hWnd )	
    {
        WinExec("MSDEV", SW_SHOWNORMAL);
        while( !hWnd )
            EnumWindows( lpEnumFunc, (LPARAM)&hWnd );
    }
    
    // bring the MSDEV window to the top and send it the message
	BOOL bResult = FALSE;
    if( hWnd )
	{
        ::BringWindowToTop(hWnd);

		DWORD dwBytesRead = 0;
		bResult = WaitNamedPipe( "\\\\.\\pipe\\SS_Log_AddIn", 20000 );
		bResult = CallNamedPipe( "\\\\.\\pipe\\SS_Log_AddIn", 
								 (LPVOID)szCommandLine, 
								 _tcslen(szCommandLine)+1, 
								 (LPVOID)NULL, 
								 0, &dwBytesRead, 
								 5000 );
	}

    *pResult = 0;
}
