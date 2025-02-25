
// ChatServerLogViewerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ChatServerLogViewer.h"
#include "ChatServerLogViewerDlg.h"
#include "afxdialogex.h"

#include "Common/Packet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatServerLogViewerDlg dialog



CChatServerLogViewerDlg::CChatServerLogViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSERVERLOGVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatServerLogViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, LoginUser);
	DDX_Control(pDX, IDC_LIST2, ChatLog);
}

BEGIN_MESSAGE_MAP(CChatServerLogViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_USER + 1, &CChatServerLogViewerDlg::OnReceiveData)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CChatServerLogViewerDlg message handlers

BOOL CChatServerLogViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pSocket = new CSocket();
	m_pSocket->Create();
	bool ret = m_pSocket->Connect(L"127.0.0.1", SERVER_PORT); // 외부 프로그램의 IP와 포트
	
	if(ret) ChatLog.AddString(L"Connect Success!");
	else ChatLog.AddString(L"Connect Fail...");

	C2S_LOGIN_PACK* login_pack = new C2S_LOGIN_PACK();
	login_pack->size = sizeof(C2S_LOGIN_PACK);
	login_pack->type = C2S_PACKET_TYPE::LOGIN_PACK;
	wcscpy_s(login_pack->id, L"ChatServerLogViewer");
	int ret_value = m_pSocket->Send(login_pack, sizeof(C2S_LOGIN_PACK));
	
	if (ret_value != SOCKET_ERROR) ChatLog.AddString(L"Send Success!");
	else ChatLog.AddString(L"Send Fail...");
	
	ChatLog.AddString(L"Send LoginPacket");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatServerLogViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatServerLogViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatServerLogViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CChatServerLogViewerDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	CString* pData = (CString*)wParam;
	ChatLog.AddString(*pData);
	LoginUser;
	delete pData; // 메모리 해제
	return 0;
}