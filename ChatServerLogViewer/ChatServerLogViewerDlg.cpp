
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

CChatServerLogViewerDlg::CChatServerLogViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSERVERLOGVIEWER_DIALOG, pParent)
	, m_Socket(this)
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
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CChatServerLogViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (false == m_Socket.Create()) {
		AfxMessageBox(_T("소켓 생성 실패"));
		return FALSE;
	}
	if (!m_Socket.Connect(_T("127.0.0.1"), 9785)) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			AfxMessageBox(_T("서버 연결 실패"));
			return FALSE;
		}
	}
	m_Socket.AsyncSelect(FD_READ);

	C2S_LOGIN_PACK* login_pack = new C2S_LOGIN_PACK();
	login_pack->size = sizeof(C2S_LOGIN_PACK);
	login_pack->type = C2S_PACKET_TYPE::LOGIN_PACK;
	wcscpy_s(login_pack->id, L"ChatServerLogViewer");
	int ret_value = m_Socket.Send(login_pack, sizeof(C2S_LOGIN_PACK));
	
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

void CChatServerLogViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

HCURSOR CChatServerLogViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChatServerLogViewerDlg::AddLoginUserList(const CString str)
{
	LoginUser.AddString(str);
	LoginUser.SetCurSel(LoginUser.GetCount() - 1);
}

void CChatServerLogViewerDlg::AddReceivedChatLog(const CString str)
{
	ChatLog.AddString(str);
	ChatLog.SetCurSel(ChatLog.GetCount() - 1);
}