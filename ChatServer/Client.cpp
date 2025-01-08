#pragma once

#include "Client.h"

void Client::send_packet(void* packet)
{
    wsa_buffer;

    OverlappedExpansion* sendoverlapped = new OverlappedExpansion(reinterpret_cast<short*>(packet));
    WSASend(client_socket, &sendoverlapped->wsa_buffer, 1, 0, 0, &sendoverlapped->overlapped, 0);
}

void Client::recv_packet()
{
    DWORD recv_flag = 0;
    memset(&overlapped, 0, sizeof(overlapped));
    
    // wsa_buffer.len = BUF_SIZE;
    // wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);

    wsa_buffer.len = BUF_SIZE - remain_packet_size;
    wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer + remain_packet_size);

    socket_type = RECV;
    WSARecv(client_socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}
