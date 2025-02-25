// CSocket.cpp : implementation file
//

#include "pch.h"
#include "ChatServerLogViewer.h"
#include "CClientSocket.h"

#include "Common/Packet.h"

// CSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode == 0)
    {
        S2C_SEND_CHAT_LOG_PACK packet;
        int nReceived = Receive(&packet, BUF_SIZE);
        if (nReceived > 0)
        {
            CString strData(packet.str);
            AfxGetApp()->GetMainWnd()->PostMessage(WM_USER + 1, (WPARAM)new CString(strData));
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}