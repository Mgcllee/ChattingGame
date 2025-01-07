#pragma once

#include "Client.h"

void Client::send_packet(void* packet)
{
    if (packet == nullptr || client_socket == NULL) 
        return;

    OverlappedExpansion* sendoverlapped = new OverlappedExpansion(reinterpret_cast<short*>(packet));
    WSASend(client_socket, &sendoverlapped->wsa_buffer, 1, 0, 0, &sendoverlapped->overlapped, 0);
}

void Client::recv_packet()
{
    DWORD recv_flag = 0;
    memset(&overlapped, 0, sizeof(overlapped));
    wsa_buffer.len = BUF_SIZE;
    wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);
    socket_type = RECV;
    WSARecv(client_socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}
