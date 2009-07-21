#pragma once

const DWORD COPYDATA_CMDLINE = 0x0001;

class CMainWnd;

class CPandionModule : public CAtlExeModuleT< CPandionModule >
{
private:
	CMainWnd *m_pMainWnd;

	IHTTP *m_pHTTP;
	IXMPP *m_pXMPP;
	ISASL *m_pSASL;

	CComQIPtr< ScrRun::IDictionary > m_spGlobals;
	CComQIPtr< ScrRun::IDictionary > m_spWindows;
public :
	DECLARE_LIBID(LIBID_PandionLib)
	~CPandionModule();

	void GetMainWnd( CMainWnd **ppMainWnd );
	void GetHTTP( IHTTP **ppHTTP );
	void GetXMPP( IXMPP **ppXMPP );
	void GetSASL( ISASL **ppSASL );
	void GetGlobals( ScrRun::IDictionary **ppGlobals );
	void GetWindows( ScrRun::IDictionary **ppWindows );

	BOOL IsRunning();
	BOOL IsIEVersionOK();

	HRESULT PreMessageLoop( int nShowCmd );
	void    RunMessageLoop();
	HRESULT PostMessageLoop();
	BOOL    PreTranslateAccelerator( MSG* pMsg );
};