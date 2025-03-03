// CSocket.cpp : implementation file
//

#include "pch.h"
#include "ChatServerLogViewer.h"
#include "CClientSocket.h"

#include "Common/Packet.h"
#include "ChatServerLogViewerDlg.h"

CClientSocket::CClientSocket(class CChatServerLogViewerDlg* pDlg)
    : m_pDlg(pDlg)
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnReceive(int nErrorCode)
{
    if (nErrorCode == 0)
    {
        char buffer[MAX_BUF_SIZE * 2];
        int nReceived = Receive(buffer, MAX_BUF_SIZE * 2);
        if (nReceived > 0)
        {
            BASIC_PACK* basic_packet = reinterpret_cast<BASIC_PACK*>(buffer);
            process_packet(basic_packet);
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}

void CClientSocket::process_packet(BASIC_PACK* recv_packet)
{
    switch (recv_packet->type) {
    case S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS: {
        S2C_SEND_CHAT_LOG_PACK* chat_log = reinterpret_cast<S2C_SEND_CHAT_LOG_PACK*>(recv_packet);
        CString strData(chat_log->str);
        m_pDlg->AddLoginUserList(strData);
        break;
    }
    case S2C_PACKET_TYPE::RESPONSE_CHAT_LOG_PACK: {
        S2C_SEND_CHAT_LOG_PACK* chat_log = reinterpret_cast<S2C_SEND_CHAT_LOG_PACK*>(recv_packet);
        CString strData(chat_log->str);
        m_pDlg->AddReceivedChatLog(strData);
        break;
    }
    }
}