
// ChatServerLogViewerDlg.h : header file
//

#pragma once


// CChatServerLogViewerDlg dialog
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
	CSocket* m_pSocket;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam); // 데이터 수신 핸들러
	DECLARE_MESSAGE_MAP()
public:
	CListBox LoginUser;
	CListBox ChatLog;
};
