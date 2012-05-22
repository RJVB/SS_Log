// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__28FC3071_3D7D_49CB_BFF3_41ED2CD2AC76__INCLUDED_)
#define AFX_DSADDIN_H__28FC3071_3D7D_49CB_BFF3_41ED2CD2AC76__INCLUDED_

#include "commands.h"

// {FE935504-88EA-442F-9BED-BB7C625A396B}
DEFINE_GUID(CLSID_DSAddIn,
0xfe935504, 0x88ea, 0x442f, 0x9b, 0xed, 0xbb, 0x7c, 0x62, 0x5a, 0x39, 0x6b);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "SS_Log_AddIn.DSAddIn.1",
		"SS_LOG_ADDIN Developer Studio Add-in", IDS_SS_LOG_ADDIN_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

	CCommandsObj* m_pCommands;

protected:
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__28FC3071_3D7D_49CB_BFF3_41ED2CD2AC76__INCLUDED)
