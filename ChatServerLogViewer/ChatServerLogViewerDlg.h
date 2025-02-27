#pragma once

#include "CClientSocket.h"

class CChatServerLogViewerDlg : public CDialogEx
{
// Construction
public:
	CChatServerLogViewerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSERVERLOGVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void AddLoginUserList(const CString& str);
	void AddReceivedChatLog(const CString& data);

	CClientSocket m_Socket;
	CListBox LoginUser;
	CListBox ChatLog;
};
