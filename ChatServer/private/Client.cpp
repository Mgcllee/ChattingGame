#pragma once

#include "ChatServer/public/Client.h"

void Client::send_packet(void* packet)
{
    OverlappedExpansion* sendoverlapped = new OverlappedExpansion(reinterpret_cast<short*>(packet));

    int err;
    LPDWORD sent_byte = 0;
    int ret = WSASend(client_socket, &sendoverlapped->wsa_buffer, 1, sent_byte, 0, &sendoverlapped->overlapped, 0);
    if ((ret == SOCKET_ERROR) &&
        (WSA_IO_PENDING != (err = WSAGetLastError()))) {
        printf("WSASend failed with error: %d\n", err);
    }
}

void Client::recv_packet()
{
    DWORD recv_flag = 0;
    memset(&overlapped, 0, sizeof(overlapped));
    
    wsa_buffer.len = BUF_SIZE - remain_packet_size;
    wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer + remain_packet_size);

    overlapped_type = OVERLAPPED_TYPE::PACKET_RECV;
    WSARecv(client_socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}
