// SSCommandLineInfo.h: interface for the SSCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSCOMMANDLINEINFO_H__9CC28FC8_701B_4263_AEA2_95096C5147F0__INCLUDED_)
#define AFX_SSCOMMANDLINEINFO_H__9CC28FC8_701B_4263_AEA2_95096C5147F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommandLineInfo;

class SSCommandLineInfo : public CCommandLineInfo  
{
public:
	SSCommandLineInfo(SS_Log_WindowApp* pApp);
	virtual ~SSCommandLineInfo();

    virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
    
    SS_Log_WindowApp* m_pApp;
};

#endif // !defined(AFX_SSCOMMANDLINEINFO_H__9CC28FC8_701B_4263_AEA2_95096C5147F0__INCLUDED_)
