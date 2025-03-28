#pragma once

#include "ChatServer/public/Client.h"

DWORD Client::send_packet(void* packet)
{
    OverlappedExpansion* sendoverlapped = new OverlappedExpansion{ reinterpret_cast<wchar_t*>(packet) };
    DWORD sent_byte;
    int ret = WSASend(client_socket, &sendoverlapped->wsa_buffer, 1, &sent_byte, 0, &sendoverlapped->overlapped, 0);
    if ((ret == SOCKET_ERROR) &&
        (WSA_IO_PENDING != (ret = WSAGetLastError()))) {
        wprintf(L"WSASend failed with error: %d\n", ret);
    }
    return sent_byte;
}
    
DWORD Client::recv_packet()
{
    DWORD recv_flag = 0;
    DWORD recv_byte = 0;
    memset(&overlapped, 0, sizeof(overlapped));
    
    wsa_buffer.len = MAX_PACKET_SIZE - remain_packet_size;
    wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer + remain_packet_size);

    overlapped_type = OVERLAPPED_TYPE::PACKET_RECV;
    WSARecv(client_socket, &wsa_buffer, 1, &recv_byte, &recv_flag, &overlapped, 0);
    return recv_byte;
}

void Client::disconnect_server()
{
    closesocket(client_socket);
    id = L"";
    pw = L"";
}
