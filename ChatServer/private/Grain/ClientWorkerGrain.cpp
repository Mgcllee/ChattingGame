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
			construct_receive_packet(ticket, exoverlapped, num_bytes);
			break;
		}
		case OVERLAPPED_TYPE::PACKET_SEND: {
			delete exoverlapped;
			break;
		}
		}

		/*TIMEROverlapped* timeroverlapped = reinterpret_cast<TIMEROverlapped*>(overlapped);
		switch (timeroverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::CHECK_EXIST_CLIENTS: {
			if (login_users.empty()) {
				wprintf(L"login user is empty\n");
				break;
			}

			int connection_status;
			std::unordered_map<int, std::wstring> disconnect_clients;
			
			BASIC_PACK check_exist_user_packet;
			check_exist_user_packet.size = sizeof(check_exist_user_packet);
			check_exist_user_packet.type = S2C_PACKET_TYPE::CHECK_EXIST_USER;

			for (auto& [ticket, client] : clients) {
				if (ticket == 0) continue;
				connection_status = clients[ticket].send_packet(&check_exist_user_packet);
				if (0 > connection_status) {
					disconnect_clients[ticket] = client.id;
					client.disconnect_server();
				}
			}

			for (const auto& [ticket, id] : disconnect_clients) {
				login_users.erase(id);
				clients.erase(ticket);
			}

			std::wstring chat_log = std::format(L"\n\n현재 접속중인 멤버: {}", login_users.size());
			DBOverlapped* dboverlapped = new DBOverlapped(chat_log, OVERLAPPED_TYPE::PRINT_CHAT_LOG);
			DWORD num_bytes = sizeof(chat_log);
			PostQueuedCompletionStatus(h_iocp_database, num_bytes, 0, dboverlapped);
			break;
		}
		}*/
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

	unsigned short* p = exoverlapped->packet_buffer;
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

void ClientWorkerGrain::process_packet(int player_ticket, unsigned short* packet)
{
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		S2C_LOGIN_RESULT_PACK result_pack;
		result_pack.type = S2C_PACKET_TYPE::LOGIN_RESULT_PACK;
		result_pack.size = sizeof(result_pack);
		
		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);
		if (login_users.find(login_pack->id) == login_users.end()) {
			login_users.insert(login_pack->id);
			clients[player_ticket].id = login_pack->id;
			
			memcpy(result_pack.result, L"로그인 성공! 어서오세요!", sizeof(L"로그인 성공! 어서오세요!"));
			
			std::wstring chat_log = std::format(L"[{}]: {}", clients[player_ticket].id, result_pack.result);
			OverlappedExpansion* dboverlapped = new OverlappedExpansion();
			dboverlapped->overlapped_type = OVERLAPPED_TYPE::PRINT_CHAT_LOG;
			memcpy(dboverlapped->packet_buffer, chat_log.c_str(), chat_log.length() * 2);
			DWORD num_bytes = sizeof(dboverlapped);
			PostQueuedCompletionStatus(h_iocp_database, num_bytes, 0, &dboverlapped->overlapped);
		}
		clients[player_ticket].send_packet(&result_pack);
		break;
	}
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		C2S_SEND_CHAT_PACK* chat = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);
		
		std::wstring chat_log = std::format(L"[{}]: {}", clients[player_ticket].id, chat->str);
		OverlappedExpansion* dboverlapped = new OverlappedExpansion();
		dboverlapped->overlapped_type = OVERLAPPED_TYPE::PRINT_CHAT_LOG;
		memcpy(dboverlapped->packet_buffer, chat_log.c_str(), chat_log.length() * 2);
		DWORD num_bytes = sizeof(dboverlapped);
		PostQueuedCompletionStatus(h_iocp_database, num_bytes, 0, &dboverlapped->overlapped);
		break;
	}
	case C2S_PACKET_TYPE::LOGOUT_PACK: {
		S2C_LOGOUT_RESULT_PACK result_pack;
		result_pack.type = S2C_PACKET_TYPE::LOGOUT_RESULT_PACK;
		result_pack.size = sizeof(result_pack);

		C2S_LOGOUT_PACK* info = reinterpret_cast<C2S_LOGOUT_PACK*>(packet);
		if (login_users.find(info->id) != login_users.end()) {
			login_users.erase(info->id);
			wcsncpy_s(result_pack.result, sizeof(result_pack.result) / sizeof(wchar_t),
				L"로그아웃 성공! 안녕히가세요!", _TRUNCATE);

			std::wstring chat_log = std::format(L"[{}]: {}", clients[player_ticket].id, result_pack.result);
			OverlappedExpansion* dboverlapped = new OverlappedExpansion();
			dboverlapped->overlapped_type = OVERLAPPED_TYPE::PRINT_CHAT_LOG;
			memcpy(dboverlapped->packet_buffer, chat_log.c_str(), chat_log.length() * 2);
			DWORD num_bytes = sizeof(dboverlapped);
			PostQueuedCompletionStatus(h_iocp_database, num_bytes, 0, &dboverlapped->overlapped);
		}
		clients[player_ticket].send_packet(&result_pack);
		break;
	}
	}
}