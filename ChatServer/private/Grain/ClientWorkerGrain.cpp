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
	std::tie(h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database) = h_iocps;

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
			if (ticket == 0) break;
			construct_receive_packet(ticket, exoverlapped, num_bytes);
			break;
		}
		case OVERLAPPED_TYPE::SEND_CHAT_LOG: {
			if (false == LogViewers.empty()) {
				S2C_SEND_CHAT_LOG_PACK log_pack;
				log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
				log_pack.type = S2C_PACKET_TYPE::RESPONSE_CHAT_LOG_PACK;
				wcscpy_s(log_pack.str, exoverlapped->packet_buffer);
				
				for (auto& [_, client] : LogViewers) {
					client.send_packet(&log_pack);
				}
			}
			break;
		}
		case OVERLAPPED_TYPE::SEND_EXIST_CLIENTS: {
			if (false == LogViewers.empty()) {
				S2C_SEND_CHAT_LOG_PACK log_pack;
				log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
				log_pack.type = S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS;
				wcscpy_s(log_pack.str, exoverlapped->packet_buffer);
				
				// TODO: using Multicast(UDP)
			}
			break;
		}
		case OVERLAPPED_TYPE::CHECK_EXIST_CLIENTS: {
			auto now = std::chrono::system_clock::now();
			std::time_t end_time = std::chrono::system_clock::to_time_t(now);
			tm tm_2;
			gmtime_s(&tm_2, &end_time);
			wchar_t cStrfTime[64];
			wcsftime(cStrfTime, 64, L"%Y-%m-%d_%H:%M:%S\n", &tm_2);
			
			std::wstring chat_format = std::format(L"{} 접속중 유저: {}", cStrfTime, login_users.size());

			wchar_t chat_log[MAX_BUF_SIZE * 2];
			wcscpy_s(chat_log, chat_format.c_str());
			post_exoverlapped(h_iocp_database, chat_log, clients[ticket].id, CHECK_EXIST_CLIENTS);
			break;
		}
		case OVERLAPPED_TYPE::MULTICAST_CHAT_LOG: {
			S2C_SEND_CHAT_LOG_PACK log_pack;
			log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
			log_pack.type = S2C_PACKET_TYPE::RESPONSE_CHAT_LOG_PACK;
			wcscpy_s(log_pack.str, exoverlapped->packet_buffer);

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

	wchar_t* p = exoverlapped->packet_buffer;
	while (remain_data > 0) {
		int packet_size = p[0];
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

void ClientWorkerGrain::process_packet(int ticket, wchar_t* packet)
{
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		S2C_LOGIN_RESULT_PACK result_pack;
		result_pack.type = S2C_PACKET_TYPE::LOGIN_RESULT_PACK;
		result_pack.size = sizeof(result_pack);
		
		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);

		if (0 == wcscmp(login_pack->id, L"ChatServerLogViewer")) {
			mutex_login_user_list.lock();
			clients.erase(ticket);
			mutex_login_user_list.unlock();

			LogViewers[ticket] = Client(static_cast<SOCKET>(ticket));
		}
		else {
			mutex_login_user_list.lock();
			if (login_users.find(login_pack->id) == login_users.end()) {
				login_users.insert(login_pack->id);
				clients[ticket].id = login_pack->id;
				mutex_login_user_list.unlock();
			

				memcpy(result_pack.result, L"로그인 성공! 어서오세요!", sizeof(L"로그인 성공! 어서오세요!"));
			
				std::wstring chat_format = std::format(L"[{}]: {}", clients[ticket].id, result_pack.result);
			
				wchar_t chat_log[MAX_BUF_SIZE];
				wcscpy_s(chat_log, chat_format.c_str());
				post_exoverlapped(h_iocp_database, chat_log, clients[ticket].id, PRINT_CHAT_LOG);
			}
			else mutex_login_user_list.unlock();
			clients[ticket].send_packet(&result_pack);
		}
		break;
	}
	case C2S_PACKET_TYPE::REQUEST_JOIN_ROOM_PACK: {
		C2S_REQUEST_JOIN_ROOM_PACK* room = reinterpret_cast<C2S_REQUEST_JOIN_ROOM_PACK*>(packet);
		post_exoverlapped(h_iocp_database, room->room_name, clients[ticket].id, REQUEST_JOIN_ROOM);
		break;
	}
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		C2S_SEND_CHAT_PACK* chat = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);
		std::wstring chat_format = std::format(L"[{}]: {}", clients[ticket].id, chat->str);
		
		wchar_t chat_log[MAX_BUF_SIZE];
		wcscpy_s(chat_log, chat_format.c_str());

		post_exoverlapped(h_iocp_database, chat_log, clients[ticket].id, PRINT_CHAT_LOG);
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