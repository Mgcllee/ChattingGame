#pragma once

#include "ChatServer/public/Grain/NetworkWorkerGrain.h"

NetworkWorkerGrain::NetworkWorkerGrain(NetworkManagerGrain& networking)
	: server_socket = std::get<0>(networking.get_socket_variable())
	, accept_client_socket = std::get<1>(networking.get_socket_variable())
	, accept_overlapped_expansion = std::get<2>(networking.get_socket_variable())
{

}

void NetworkWorkerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto [h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp_network, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(exoverlapped, GQCS_result)) {
			continue;
		}

		// TODO: check exist value
		DWORD bytesTransferred = sizeof(overlapped);
		ULONG_PTR completionKey = 0;

		switch (exoverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::CLIENT_ACCEPT: {
			PostQueuedCompletionStatus(h_iocp_clients, bytesTransferred, completionKey, overlapped);

			ZeroMemory(&accept_overlapped_expansion->overlapped, sizeof(accept_overlapped_expansion->overlapped));
			accept_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			BOOL option = TRUE;
			setsockopt(accept_client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server_socket, accept_client_socket,
				accept_overlapped_expansion->packet_buffer, 0, addr_size + 16, addr_size + 16,
				0, &accept_overlapped_expansion->overlapped);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_RECV: {
			PostQueuedCompletionStatus(h_iocp_clients, bytesTransferred, completionKey, overlapped);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_SEND: {
			delete exoverlapped;
			break;
		}
		default: {
			break;
		}
		}
	}
}

bool NetworkWorkerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		// TODO: add error type log
		return false;
	}

	return true;
}
