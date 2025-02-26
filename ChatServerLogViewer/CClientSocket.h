#pragma once

struct BASIC_PACK;
class CChatServerLogViewerDlg;

class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket(class CChatServerLogViewerDlg* pDlg);
	virtual ~CClientSocket();

	void OnReceive(int nErrorCode);

private:
	CChatServerLogViewerDlg* m_pDlg;

	void process_packet(BASIC_PACK* recv_packet);
};


