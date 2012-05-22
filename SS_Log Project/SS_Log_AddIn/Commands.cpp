// Commands.cpp : implementation file
//

#include "stdafx.h"
#include "SS_Log_AddIn.h"
#include "Commands.h"
#include "DSAddIn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CComBSTR g_bstrFilename;
int g_nLine;
BOOL g_bAppShuttingDown = FALSE;
extern CSS_Log_AddInApp theApp;

void CharToWChar(unsigned short* pTarget, const char* pSource)
{
    while(*pSource)
    {
        *pTarget = *pSource;
        pSource++;
        pTarget++;
    }
    *pTarget = 0;
}


// ----------------------------------------------------------------------- //
//  Function:		PipeThread()
//  Author:			Steve Schaneville
//  Parameters:		pParam [in] - a pointer to the IApplication object.
//  Return Values:	0
//  Notes:			
// ----------------------------------------------------------------------- //
UINT PipeThread( LPVOID pParam )
{
    BOOL bResult = FALSE;

    //CDSAddIn* pAddIn = (CDSAddIn*)pParam;
    IApplication* pApp = (IApplication*)pParam;
    
    // create the pipe with no access restrictions
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

    HANDLE hPipe = CreateNamedPipe( "\\\\.\\pipe\\SS_Log_AddIn",
                                    PIPE_ACCESS_DUPLEX, 
                                    PIPE_TYPE_MESSAGE|PIPE_WAIT|PIPE_READMODE_MESSAGE, 
                                    1, 
                                    256,
                                    0, 5000, &sa );
    
    if( hPipe == INVALID_HANDLE_VALUE )
    {
        MessageBox( NULL, _T("Warning:  The SS_Log_AddIn did not create a pipe for incoming messages.  No messages can be received.\n\n Probable reason is there is another instance of MSDEV already open that created a pipe with the same name."), 
            _T("Warning"), MB_OK|MB_ICONSTOP );
        return FALSE;
    }
    
    // now we wait for a connection to the named pipe.  When a connection is found,
    // call the SS_Log_AddInCommand() function in the add-in, then loop back and
    // wait for another connection.
    while( !g_bAppShuttingDown )
    {
        ConnectNamedPipe( hPipe, NULL );
        
        TCHAR szText[256] = _T("");
        DWORD dwBytesRead;
        ReadFile( hPipe, szText, 256, 
            &dwBytesRead, NULL );
        
        WriteFile( hPipe, NULL, 0, &dwBytesRead, NULL );
        DisconnectNamedPipe( hPipe );
        
        if( _tcscmp(szText, _T("")) && !g_bAppShuttingDown )
        {
            TCHAR* szFile;
            TCHAR* szLine;
            int nLine = 1;
            
            szFile = _tcstok( szText+1, _T(";") );
            *(szFile+_tcslen(szFile)-1) = '\0';
            szLine = _tcstok( NULL, _T(";") );
            nLine = _ttoi( szLine );

            g_bstrFilename = szFile;
            g_nLine = nLine;
            CComBSTR bstrCommand = "SS_Log_AddInCommand";
            HRESULT hr = pApp->ExecuteCommand(bstrCommand);
        }
    } 
    
    int i = CloseHandle(hPipe);
    AfxEndThread(0);
    return 0;
}





/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
	m_pApplication = NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;

	// Create Application event handlers
	XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef();
	m_pApplicationEventsObj->Connect(m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;

	// Create Debugger event handler
	CComPtr<IDispatch> pDebugger;
	if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger)) 
		&& pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef();
		m_pDebuggerEventsObj->Connect(pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}
}

void CCommands::UnadviseFromEvents()
{
	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect(m_pApplication);
	m_pApplicationEventsObj->Release();
	m_pApplicationEventsObj = NULL;

	if (m_pDebuggerEventsObj != NULL)
	{
		// Since we were able to connect to the Debugger events, we
		//  should be able to access the Debugger object again to
		//  unadvise from its events (thus the VERIFY_OK below--see stdafx.h).
		CComPtr<IDispatch> pDebugger;
		VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect(pDebugger);
		m_pDebuggerEventsObj->Release();
        m_pDebuggerEventsObj = NULL;
    }
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// TODO: Fill out the implementation for those events you wish handle
//  Use m_pCommands->GetApplicationObject() to access the Developer
//  Studio Application object

// Application events

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // let the PipeThread() function know that we are shutting down.  
    // To do that, we set the global variable g_bAppShuttingDown, then we
    // release it from waiting for a connection to the named-pipe by 
    // connecting to the pipe.
    g_bAppShuttingDown = TRUE;
    DWORD dwBytesRead = 0;
    
    BOOL bResult = CallNamedPipe( "\\\\.\\pipe\\SS_Log_AddIn",
                                  (LPVOID)NULL,
                                  0, 
                                  (LPVOID)NULL, 
                                  0, 
                                  &dwBytesRead, 
                                  5000 );
    
    
    return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCommands methods


// ----------------------------------------------------------------------- //
//  Function:		CCommands::SS_Log_AddInCommandMethod()
//  Author:			Steve Schaneville
//  Parameters:		none
//  Return Values:	S_OK, the standard HRESULT com return value
//  Notes:			The global variables 'g_bstrFilename' and 'g_nLine' should
//                  be set immediately before calling this function.  The
//                  function will then attempt to open the specified file
//                  and move to the line # indicated.  Finally it selects the
//                  line just as a visual queue.
// ----------------------------------------------------------------------- //
STDMETHODIMP CCommands::SS_Log_AddInCommandMethod() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    // TODO: Replace this with the actual code to execute this command
    //  Use m_pApplication to access the Developer Studio Application object,
    //  and VERIFY_OK to see error strings in DEBUG builds of your add-in
    //  (see stdafx.h)

    IDocuments* pDocs = NULL;
    m_pApplication->get_Documents((IDispatch**)&pDocs);
    
    // open the file
    ITextDocument* pDoc = NULL; 
    //CComBSTR bstrFileName = szFilename;
    CComVariant vtDocType = "Text";
    CComVariant vtReadOnly = FALSE;
    HRESULT hr = pDocs->Open(g_bstrFilename, vtDocType, vtReadOnly, (IDispatch**)&pDoc);

    if( FAILED(hr) )
    {
        MessageBox(NULL, "Could not find file specified by SS_Log_Window", "File not found...", MB_OK);
        return S_OK;
    }
    
    ITextSelection* pSel;
    pDoc->get_Selection((IDispatch**)&pSel);

    CComVariant var = dsMove;
	CComVariant num = 5;
    pSel->GoToLine(g_nLine, var);
	pSel->LineDown(var, num);
    pSel->GoToLine(g_nLine, var);
    pSel->LineUp(var, num);
    pSel->GoToLine(g_nLine, var);
    pSel->SelectLine();

    pSel->Release();
    pDoc->Release();
    pDocs->Release();

    return S_OK;
}

