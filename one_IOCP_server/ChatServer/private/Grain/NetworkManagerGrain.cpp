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
	DWORD num_bytes = 0;
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
			tcp_keepalive tcp_opt = { 0 };
			tcp_opt.onoff = TRUE;	
			tcp_opt.keepalivetime = 5'000;
			tcp_opt.keepaliveinterval = 1'000;

			DWORD dwBytes;
			if (0 != WSAIoctl(accept_client_socket, SIO_KEEPALIVE_VALS, &tcp_opt, sizeof(tcp_keepalive),
				0, 0, &dwBytes, NULL, NULL)) {
				wprintf(L"Error setting SO_KEEPALIVE_VALS [error_code: %d]\n", GetLastError());
				WSACleanup();
				break;
			}

			uint64_t ticket = static_cast<uint64_t>(accept_client_socket);
			mutex_login_user_list.lock();
			clients[ticket] = Client(accept_client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_client_socket), h_iocp, ticket, 0);
			clients[ticket].recv_packet();
			mutex_login_user_list.unlock();

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
			uint64_t ticket = static_cast<uint64_t>(key);
			construct_receive_packet(h_iocp, ticket, exoverlapped, num_bytes);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_SEND: {
			break;
		}
		case OVERLAPPED_TYPE::CHECK_EXIST_CLIENTS: {
			if (false == LogViewers.empty()) {
				S2C_SEND_CHAT_LOG_PACK log_pack;
				log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
				log_pack.type = S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS;

				auto now = std::chrono::system_clock::now();
				std::time_t end_time = std::chrono::system_clock::to_time_t(now);
				tm tm_2;
				gmtime_s(&tm_2, &end_time);
				wchar_t cStrfTime[64];
				wcsftime(cStrfTime, 64, L"%Y-%m-%d_%H:%M:%S\n", &tm_2);
				std::wstring chat_format = std::format(L"[{}] 접속 유저 수: {}", cStrfTime, login_users.size());
				wcscpy_s(log_pack.str, chat_format.c_str());

				for (auto& [_, client] : LogViewers) {
					client.send_packet(&log_pack);
				}
			}
			break;
		}
		}
	}
}

bool NetworkManagerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr)return false;
	if (GQCS_result == FALSE)	return false;
	else						return true;
}

void NetworkManagerGrain::construct_receive_packet(HANDLE h_iocp, uint64_t client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes)
{
	if (num_bytes <= 0) {
		C2S_LOGOUT_PACK logout_packet;
		logout_packet.size = sizeof(logout_packet);
		logout_packet.type = C2S_PACKET_TYPE::LOGOUT_PACK;
		wcscpy_s(logout_packet.id, clients[client_ticket].id.c_str());

		wchar_t packet_buffer[MAX_PACKET_SIZE];
		memcpy(packet_buffer, &logout_packet, sizeof(logout_packet));
		post_exoverlapped(h_iocp, packet_buffer, L"", OVERLAPPED_TYPE::PACKET_RECV);
		return;
	}

	int remain_data = num_bytes + clients[client_ticket].remain_packet_size;

	wchar_t* p = exoverlapped->packet_buffer;
	while (remain_data > 0) {
		int packet_size = (int)p[0];
		if (packet_size <= remain_data) {
			process_packet(h_iocp, client_ticket, p);
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

void NetworkManagerGrain::process_packet(HANDLE h_iocp, uint64_t ticket, wchar_t* packet)
{
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		S2C_LOGIN_RESULT_PACK result_pack;
		result_pack.size = sizeof(S2C_LOGIN_RESULT_PACK);
		result_pack.type = S2C_PACKET_TYPE::LOGIN_RESULT_PACK;

		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);

		if (0 == wcscmp(login_pack->id, L"ChatServerLogViewer")) {
			mutex_login_user_list.lock();
			clients.erase(ticket);
			mutex_login_user_list.unlock();

			LogViewers[ticket] = Client(static_cast<SOCKET>(ticket));
		}
		else {
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
			DWORD send_bytes = clients[ticket].send_packet(&result_pack);

			if (static_cast<DWORD>(result_pack.size) > send_bytes) {
				C2S_LOGOUT_PACK logout_packet;
				logout_packet.size = sizeof(logout_packet);
				logout_packet.type = C2S_PACKET_TYPE::LOGOUT_PACK;
				wcscpy_s(logout_packet.id, clients[ticket].id.c_str());

				wchar_t packet_buffer[MAX_PACKET_SIZE];
				memcpy(packet_buffer, &logout_packet, sizeof(logout_packet));
				post_exoverlapped(h_iocp, packet_buffer, L"", OVERLAPPED_TYPE::PACKET_RECV);
			}
			wprintf(L"%s\n", result_pack.result);
		}
		break;
	}
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		if (false == LogViewers.empty()) {
			S2C_SEND_CHAT_LOG_PACK log_pack;
			log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
			log_pack.type = S2C_PACKET_TYPE::RESPONSE_CHAT_LOG_PACK;

			C2S_SEND_CHAT_PACK* chat = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);
			auto start = std::chrono::system_clock::now();
			auto start_time_t = std::chrono::system_clock::to_time_t(start);
			struct tm* start_tm = localtime(&start_time_t);
			std::wstring time_stamp = std::format(L"{:02}:{:02}:{:02}", start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec);
			std::wstring chat_format = std::format(L"[{}][{}]: {}", time_stamp, clients[ticket].id, chat->str);
			wcscpy_s(log_pack.str, chat_format.c_str());

			for (auto& [_, client] : LogViewers) {
				client.send_packet(&log_pack);
			}
		}
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
			clients[ticket].send_packet(&result_pack);
			clients[ticket].disconnect_server();
			clients.erase(ticket);
		}
		else {
			wcscpy_s(result_pack.result, L"로그아웃 실패 재시도 해주세요.");
			clients[ticket].send_packet(&result_pack);
		}
		mutex_login_user_list.unlock();
		break;
	}
	}
}