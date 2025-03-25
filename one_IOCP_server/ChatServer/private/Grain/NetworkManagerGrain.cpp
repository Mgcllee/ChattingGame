#pragma once

#include "ChatServer/public/Grain/NetworkManagerGrain.h"

NetworkManagerGrain::NetworkManagerGrain()
{
}

NetworkManagerGrain::~NetworkManagerGrain()
{
}

void NetworkManagerGrain::packet_worker(HANDLE h_iocp, 
	SOCKET server_socket, SOCKET accept_client_socket, OverlappedExpansion* accept_overlapped_expansion)
{
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(exoverlapped, GQCS_result)) {
			continue;
		}

		switch (exoverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::CLIENT_ACCEPT: {
			clients[accept_client_socket] = Client(accept_client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_client_socket), h_iocp, accept_client_socket, 0);
			clients[accept_client_socket].recv_packet();

			ZeroMemory(&accept_overlapped_expansion->overlapped, sizeof(accept_overlapped_expansion->overlapped));
			accept_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			BOOL option = TRUE;
			setsockopt(accept_client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

			SOCKADDR_IN client_addr;
			int addr_size = sizeof(client_addr);
			AcceptEx(server_socket, accept_client_socket, accept_overlapped_expansion->packet_buffer, 0, 
				addr_size + 16, addr_size + 16, NULL, 
				&accept_overlapped_expansion->overlapped);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_RECV: {
			construct_receive_packet(reinterpret_cast<HANDLE>(key), exoverlapped, num_bytes);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_SEND: {
			wprintf(L"Complete send work! \t[%d]\n", num_bytes);
			break;
		}
		default: {
			break;
		}
		}
	}
}

bool NetworkManagerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		return false;
	}

	return true;
}

void NetworkManagerGrain::construct_receive_packet(HANDLE client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes)
{
	int remain_data = num_bytes + clients[client_ticket].remain_packet_size;

	wchar_t* p = exoverlapped->packet_buffer;
	while (remain_data > 0) {
		int packet_size = (int)p[0];
		if (packet_size <= remain_data) {
			process_packet(client_ticket, p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}

	clients[client_ticket].remain_packet_size = remain_data;
	if (remain_data > 0) {
		memcpy(exoverlapped->packet_buffer, p, remain_data);
	}
	clients[client_ticket].recv_packet();
}

void NetworkManagerGrain::process_packet(HANDLE ticket, wchar_t* packet)
{
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		S2C_LOGIN_RESULT_PACK result_pack;
		result_pack.size = sizeof(S2C_LOGIN_RESULT_PACK);
		result_pack.type = S2C_PACKET_TYPE::LOGIN_RESULT_PACK;

		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);

		std::wstring chat_format;
		mutex_login_user_list.lock();
		if (login_users.find(login_pack->id) == login_users.end()) {
			login_users.insert(login_pack->id);
			clients[ticket].id = login_pack->id;
			mutex_login_user_list.unlock();
			chat_format = std::format(L"[{}]: {}", clients[ticket].id, L"로그인 성공! 어서오세요!");
		}
		else {
			mutex_login_user_list.unlock();
			chat_format = std::format(L"[{}]: {}", clients[ticket].id, L"중복된 아이디로 로그인 실패");
		}
		wcscpy(result_pack.result, chat_format.c_str());
		int ret = clients[ticket].send_packet(&result_pack);
		wprintf(L"%s [%d]\n", chat_format.c_str(), ret);
		break;
	}
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		C2S_SEND_CHAT_PACK* chat = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);

		auto start = std::chrono::system_clock::now();
		auto start_time_t = std::chrono::system_clock::to_time_t(start);
		wchar_t start_buffer[100];
		struct tm* start_tm = localtime(&start_time_t);
		swprintf(start_buffer, sizeof(start_buffer) / sizeof(wchar_t), L"%02d:%02d:%02d", start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec);
		std::wstring chat_format = std::format(L"[{}][{}]: {}", start_buffer, clients[ticket].id, chat->str);
		wprintf(L"%s\n", chat_format.c_str());
		break;
	}
	case C2S_PACKET_TYPE::LOGOUT_PACK: {
		S2C_LOGOUT_RESULT_PACK result_pack;
		result_pack.type = S2C_PACKET_TYPE::LOGOUT_RESULT_PACK;
		result_pack.size = sizeof(result_pack);

		C2S_LOGOUT_PACK* info = reinterpret_cast<C2S_LOGOUT_PACK*>(packet);

		mutex_login_user_list.lock();
		if (login_users.find(info->id) != login_users.end()) {
			login_users.erase(info->id);
			wcscpy_s(result_pack.result, L"로그아웃 성공! 안녕히가세요!");
		}
		clients[ticket].send_packet(&result_pack);
		clients.erase(ticket);
		mutex_login_user_list.unlock();
		break;
	}
	}
}