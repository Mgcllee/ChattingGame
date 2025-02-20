#pragma once

#include "ChatServer/public/Grain/ClientWorkerGrain.h"

ClientWorkerGrain::ClientWorkerGrain()
{
}

ClientWorkerGrain::~ClientWorkerGrain()
{
}

void ClientWorkerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto [h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp_clients, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(exoverlapped, GQCS_result)) {
			continue;
		}
		
		int ticket = static_cast<int>(key);

		switch (exoverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::CLIENT_ACCEPT: {
			SOCKET accept_client_socket = static_cast<SOCKET>(key);
			clients[ticket] = Client(accept_client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_client_socket), h_iocp_clients, ticket, 0);
			clients[ticket].recv_packet();
			break;
		}
		case OVERLAPPED_TYPE::PACKET_RECV: {
			construct_receive_packet(key, exoverlapped, num_bytes);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_SEND: {
			delete exoverlapped;
			break;
		}
		}
	}
}

bool ClientWorkerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		// TODO: add error type log
		return false;
	}

	return true;
}

void ClientWorkerGrain::construct_receive_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes)
{
	int remain_data = num_bytes + clients[client_ticket].remain_packet_size;

	short* p = exoverlapped->packet_buffer;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {

			process_packet(client_ticket, p);

			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else {
			break;
		}
	}

	clients[client_ticket].remain_packet_size = remain_data;
	if (remain_data > 0) {
		memcpy(exoverlapped->packet_buffer, p, remain_data);
	}
	clients[client_ticket].recv_packet();
}

void ClientWorkerGrain::process_packet(int player_ticket, short* packet)
{
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);
		std::wcout << login_pack->id << L'\n';
		break;
	}
	}
}

