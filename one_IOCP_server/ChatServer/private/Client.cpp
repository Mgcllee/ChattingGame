#pragma once

#include "ChatServer/public/Client.h"

int Client::send_packet(void* packet)
{
    OverlappedExpansion* sendoverlapped = new OverlappedExpansion{ reinterpret_cast<wchar_t*>(packet) };
    DWORD sent_byte;
    int ret = WSASend(client_socket, &sendoverlapped->wsa_buffer, 1, &sent_byte, 0, &sendoverlapped->overlapped, 0);
    if ((ret == SOCKET_ERROR) &&
        (WSA_IO_PENDING != (ret = WSAGetLastError()))) {
        wprintf(L"WSASend failed with error: %d\n", ret);
    } wprintf(L"sent byte: %d\n", sent_byte);
    shutdown(client_socket, SD_SEND);
    return ret;
}
    
int Client::recv_packet()
{
    DWORD recv_flag = 0;
    memset(&overlapped, 0, sizeof(overlapped));
    
    wsa_buffer.len = MAX_PACKET_SIZE - remain_packet_size;
    wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer + remain_packet_size);

    overlapped_type = OVERLAPPED_TYPE::PACKET_RECV;
    return WSARecv(client_socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}

void Client::disconnect_server()
{
    closesocket(client_socket);
    id = L"";
    pw = L"";
}
