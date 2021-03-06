// FreePcb.cpp : Defines the class behaviors for the application. 
//

#include "stdafx.h"
#include "freepcb.h"
#include "resource.h"
#include "DlgShortcuts.h"
#include "afxwin.h"
#include "freepcb.h"
//#include "QAFDebug.h"

// STL
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;  
#endif

/////////////////////////////////////////////////////////////////////////////
// CFreePcbApp

BEGIN_MESSAGE_MAP(CFreePcbApp, CWinApp)
	//{{AFX_MSG_MAP(CFreePcbApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_VIEW_FOOTPRINT, OnViewFootprint)
	ON_COMMAND(ID_VIEW_PCB_EDITOR, OnViewPcbEditor)
	ON_COMMAND(ID_FILE_OPENFOOTPRINTEDITOR, OnViewFootprint)
	ON_COMMAND(ID_HELP_GOTOWEBSITE, OnHelpGotoWebsite)
	ON_COMMAND(ID_FILE_MRU_FILE1, OnFileMruFile1)
	ON_COMMAND(ID_FILE_MRU_FILE2, OnFileMruFile2)
	ON_COMMAND(ID_FILE_MRU_FILE3, OnFileMruFile3)
	ON_COMMAND(ID_FILE_MRU_FILE4, OnFileMruFile4)
	ON_COMMAND(ID_HELP_KEYBOARDSHORTCUTS, OnHelpKeyboardShortcuts)
	ON_COMMAND(ID_TOOLS_OPENONLINEAUTOROUTER, OnToolsOpenOnlineAutorouter)
	ON_COMMAND(ID_HELP_FREEROUTINGWEBSITE, OnHelpFreeRoutingWebsite)
	ON_COMMAND(ID_HELP_USERGUIDE_PDF, OnHelpUserGuidePdf)
	ON_COMMAND(ID_HELP_FPCROUTE, OnHelpFpcRoute)
	ON_COMMAND(ID_HELP_USERGUIDESUPPLEMENT_PDF, OnHelpUserGuideSupplementPdf)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreePcbApp construction

CFreePcbApp::CFreePcbApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFreePcbApp object

CFreePcbApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFreePcbApp initialization

BOOL CFreePcbApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
//** deprecated **	
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("eebit"));

	CWinApp::LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	if( CWinApp::m_pRecentFileList == NULL)
	{
		AfxMessageBox( "NOTE: The recent file list is disabled on your system.\nUse the system policy editor to re-enable." );
	}

	EnableShellOpen();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	m_pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFreePcbDoc),
		RUNTIME_CLASS(CMainFrame),    
		RUNTIME_CLASS(CFreePcbView));
	AddDocTemplate(m_pDocTemplate);

	// load menus
	VERIFY( m_main.LoadMenu( IDR_MAINFRAME ) );
	VERIFY( m_main_drag.LoadMenu( IDR_MAINFRAME_DRAG ) );
	VERIFY( m_foot.LoadMenu( IDR_FOOTPRINT ) );
	// VERIFY( m_foot_drag.LoadMenu( IDR_FOOTPRINT_DRAG ) );		// CPT2 obsolete?

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The window has been initialized, so show and update it.
	UINT iShowCmd = GetProfileInt(_T("Settings"),_T("ShowCmd"),SW_SHOW);
	m_pMainWnd->ShowWindow(iShowCmd);
	m_pMainWnd->UpdateWindow();

	// set pointers to document and view
	CMainFrame * pMainWnd = (CMainFrame*)AfxGetMainWnd();
	m_doc = (CFreePcbDoc*)pMainWnd->GetActiveDocument();
	m_view = (CFreePcbView*)pMainWnd->GetActiveView();
	m_view->InitInstance();

	// set initial view mode
	m_view_mode = PCB;
	m_doc->InitializeNewProject();
	m_pMainWnd->SetWindowTextA(m_doc->m_window_title);			// CPT --- want a consistent window title from the get-go

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen )
	{
		CString fn = cmdInfo.m_strFileName;
		m_doc->OnFileAutoOpen( fn );
	}

	// CPT2 TODO for testing only:
	extern void TestGpc();
	TestGpc();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit_build;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, m_edit_build);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
#ifdef _DEBUG
		m_edit_build.SetWindowText( "$WCREV$ Debug: ($WCDATE$)" );
#else
		m_edit_build.SetWindowText( "$WCREV$ Release: ($WCDATE$)" );
#endif
	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFreePcbApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFreePcbApp message handlers


void CFreePcbApp::OnViewFootprint()
{
	SwitchToView( RUNTIME_CLASS( CFootprintView ) );
	m_view_mode = FOOTPRINT;
	return;
}

void CFreePcbApp::OnViewPcbEditor()
{	
	SwitchToView( RUNTIME_CLASS( CFreePcbView ) );
	m_view_mode = PCB;
	return;
}

// switch to a new CView
//
BOOL CFreePcbApp::SwitchToView( CRuntimeClass * pNewViewClass )
{
	// I have no idea what most of this code actually does, but it seems to work
	CFrameWnd * pMainWnd = (CFrameWnd*)AfxGetMainWnd();
	CView * pOldActiveView = pMainWnd->GetActiveView();
	if( pOldActiveView->IsKindOf( pNewViewClass ) )
		return TRUE;

	bool bSwitchToPcb = pNewViewClass == RUNTIME_CLASS( CFreePcbView );
	CView * pNewView;
	if( bSwitchToPcb )
		pNewView = m_view;
	else
	{
		// Switch to footprint:
		CCreateContext context;
		context.m_pNewViewClass = pNewViewClass;
		context.m_pCurrentDoc = m_doc;
		pNewView = STATIC_DOWNCAST(CView, pMainWnd->CreateView(&context));
		m_view_fp = (CFootprintView*)pNewView;
	}
	if (!pNewView) return false;

#if 0
	CMenu m_NewMenu;
	if( pNewViewClass == RUNTIME_CLASS( CFreePcbView ) )
		m_NewMenu.LoadMenu(IDR_MAINFRAME);
	else
		m_NewMenu.LoadMenu(IDR_FOOTPRINT);
	ASSERT(m_NewMenu);
	// Add the new menu
	pMainWnd->SetMenu(&m_NewMenu);
	m_NewMenu.Detach();
#endif
	if( bSwitchToPcb )
		pMainWnd->SetMenu(&m_main);
	else
		pMainWnd->SetMenu(&m_foot);

	// Exchange view window IDs so RecalcLayout() works.
	UINT temp = ::GetWindowLong(pOldActiveView->m_hWnd, GWL_ID);
	::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
	pNewView->ShowWindow( SW_SHOW );
	pOldActiveView->ShowWindow( SW_HIDE );
	pNewView->OnInitialUpdate();
	pMainWnd->SetActiveView( pNewView );
	pMainWnd->RecalcLayout();

	if( bSwitchToPcb )
	{
		// first, see if we were editing the footprint of the selected part
		CShape * temp_footprint = NULL;
		bool bModifiedSelPartFootprint = m_view->m_cursor_mode == CUR_PART_SELECTED 
											&& m_doc->m_edit_footprint
											&& (m_doc->m_footprint_modified || m_doc->m_footprint_name_changed);
		if (bModifiedSelPartFootprint)
			// yes, make a copy of the footprint from the editor
			temp_footprint = new CShape (m_view_fp->m_fp);
		// destroy old footprint view
		pOldActiveView->DestroyWindow();
		if( !m_doc->m_project_open )
		{
			m_doc->m_project_modified = FALSE;
			m_doc->m_project_modified_since_autosave = FALSE;
			m_doc->OnFileClose();	
		}
		// CPT2.  Ensure that all drawing henceforth goes to the main pcb's display-list:
		m_doc->m_dlist = m_doc->m_dlist_pcb;
		m_doc->m_edit_footprint = NULL;		// clear this flag for next time
		// restore toolbar stuff
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		frm->m_wndMyToolBar.SetLists( &m_doc->m_visible_grid, &m_doc->m_part_grid, &m_doc->m_routing_grid,
			m_doc->m_visual_grid_spacing, m_doc->m_part_grid_spacing, m_doc->m_routing_grid_spacing, 
			m_doc->m_snap_angle, m_doc->m_view->m_units );
		m_view->m_dlist->SetVisibleGrid( 1, m_doc->m_visual_grid_spacing );
		frm->SetWindowText( m_doc->m_window_title ); 
		m_view->ShowSelectStatus();
		m_view->ShowActiveLayer();
		if(	bModifiedSelPartFootprint )
			// now offer to replace the footprint of the selected part 
			m_view->OnExternalChangeFootprint( temp_footprint );
	}

	else
	{
		// switching to footprint view, create it
		int units = MIL;
		m_view_fp = (CFootprintView*)pNewView;
		m_doc->m_dlist = m_doc->m_dlist_fp;												// CPT2.  Ensures that we draw to the fp display-list
		if( m_view->m_cursor_mode == CUR_PART_SELECTED && m_doc->m_edit_footprint )
		{
			m_view_fp->InitInstance( m_doc->m_edit_footprint );
			units = m_doc->m_edit_footprint->m_units;
		}
		else
			m_view_fp->InitInstance( NULL );
		// restore toolbar stuff
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		frm->m_wndMyToolBar.SetLists( &m_doc->m_fp_visible_grid, &m_doc->m_fp_part_grid, NULL,
			m_doc->m_fp_visual_grid_spacing, m_doc->m_fp_part_grid_spacing, 0, m_doc->m_fp_snap_angle, units );
		m_view_fp->m_dlist->SetVisibleGrid( 1, m_doc->m_fp_visual_grid_spacing );
	}

	// resize window in case it changed
	CRect client_rect;
	pMainWnd->GetClientRect( client_rect );
	// TODO: replace these constants
	client_rect.top += 24;		// leave room for toolbar
	client_rect.bottom -= 18;	// leave room for status bar
	pNewView->MoveWindow( client_rect, 1 );
	return TRUE;
}

int CFreePcbApp::ExitInstance()
{
	return( CWinApp::ExitInstance() );
}

void CFreePcbApp::OnHelpGotoWebsite()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = "http://www.freepcb.com";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreePcbApp::OnFileMruFile1() 
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[0];
	m_doc->OnFileAutoOpen( str );
	return;
}

void CFreePcbApp::OnFileMruFile2()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[1];
	m_doc->OnFileAutoOpen( str );
	return;
}

void CFreePcbApp::OnFileMruFile3()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[2];
	m_doc->OnFileAutoOpen( str );
	return;
}

void CFreePcbApp::OnFileMruFile4()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[3];
	m_doc->OnFileAutoOpen( str );
	return;
}

BOOL CFreePcbApp::OnOpenRecentFile(UINT nID)
{
	return( CWinApp::OnOpenRecentFile( nID ) );
}

CString CFreePcbApp::GetMRUFile()
{
	CRecentFileList * pRecentFileList = CWinApp::m_pRecentFileList;
	if( pRecentFileList == NULL )
		return "";
	if( pRecentFileList->GetSize() == 0 )
		return "";
	CString str = (*CWinApp::m_pRecentFileList)[0];
	return str;
}

void CFreePcbApp::AddMRUFile( CString * str )
{
	CRecentFileList * pRecentFileList = CWinApp::m_pRecentFileList;
	if( m_pRecentFileList == NULL )
		return;
	(*CWinApp::m_pRecentFileList).Add( *str );
}

void CFreePcbApp::OnHelpKeyboardShortcuts()
{
	CDlgShortcuts dlg = new CDlgShortcuts;
	dlg.DoModal();
}

void CFreePcbApp::OnToolsOpenOnlineAutorouter()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_doc->m_app_dir + "\\freeroute.jnlp";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreePcbApp::OnHelpFreeRoutingWebsite()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = "http://www.freerouting.net";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreePcbApp::OnHelpUserGuidePdf()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_doc->m_app_dir + "\\..\\doc\\freepcb_user_guide.pdf";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreePcbApp::OnHelpUserGuideSupplementPdf()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_doc->m_app_dir + "\\..\\doc\\freepcb_user_guide_supplement.pdf";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreePcbApp::OnHelpFpcRoute()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_doc->m_app_dir + "\\..\\doc\\fpcroute_user_guide.pdf";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

int CFreePcbApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	// show cursor
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	::ShowCursor( TRUE );
	int ret = CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
	::ShowCursor( FALSE );
	return ret;
}

