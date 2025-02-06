#pragma once

#include "JobWorker.h"

std::mutex login_user_mutex;

JobWorker::JobWorker(
	SOCKET& in_server_socket,
	SOCKET& in_accept_client_socket,
	OverlappedExpansion* in_accept_overlapped_expansion,
	std::atomic<int>& in_ticket_number,
	std::unordered_map<int, Client>& in_clients,
	std::wofstream& in_chat_log_file,
	std::vector<std::vector<int>>& in_map,
	std::unordered_map<std::wstring, ChatRoomSession>& in_room_list
)
	: server_socket(in_server_socket)
	, accept_client_socket(in_accept_client_socket)
	, accept_overlapped_expansion(in_accept_overlapped_expansion)
	, ticket_number(in_ticket_number)
	, clients(in_clients)
	, chat_log_file(in_chat_log_file)
	, map(in_map)
	, room_list(in_room_list)
{

}

JobWorker::~JobWorker()
{
}

void JobWorker::job_worker(HANDLE h_iocp) {
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		int client_ticket = static_cast<int>(key);
		if (false == check_exist_job(exoverlapped, GQCS_result, client_ticket)) {
			continue;
		}

		switch (exoverlapped->socket_type) {
		case SOCKET_TYPE::ACCEPT: {
			int ticket = ticket_number.load();
			ticket_number.fetch_add(1);

			clients[ticket] = Client(accept_client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_client_socket), h_iocp, ticket, 0);
			clients[ticket].recv_packet();

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
		case SOCKET_TYPE::RECV: {
			recv_client_packet(static_cast<int>(key), exoverlapped, num_bytes);
			break;
		}
		case SOCKET_TYPE::SEND: {
			printf("Send 작업 완료\n");
			delete exoverlapped;
			break;
		}
		default: {
			printf("[Error]: Packet Type Error!\n");
			break;
		}
		}
	}
}

bool JobWorker::check_exist_job(OverlappedExpansion* exoverlapped, BOOL GQCS_result, int client_ticket) {
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		if (exoverlapped->socket_type == ACCEPT) {
			printf("Accept error\n");
		}
		else {
			std::string error_message = "GQCS Error on client[";
			error_message += std::to_string(client_ticket);
			error_message += "]";
			printf("%s\n", error_message.c_str());
		}
		return false;
	}

	return true;
}

void JobWorker::recv_client_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes) {
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

void JobWorker::process_packet(int player_ticket, short* packet) {
	switch (packet[1]) {
	case C2S_PACKET_TYPE::LOGIN_PACK: {
		C2S_LOGIN_PACK* login_pack = reinterpret_cast<C2S_LOGIN_PACK*>(packet);

		S2C_LOGIN_RESULT_PACK result_packet;
		result_packet.size = sizeof(result_packet);
		result_packet.type = S2C_PACKET_TYPE::LOGIN_RESULT_PACK;

		std::wcout << L"서버에서 " << login_pack->id << L" 로 로그인 시도 수신\n";

		login_user_mutex.lock();
		if (login_user_list.insert(login_pack->id).second) {
			login_user_mutex.unlock();

			clients[player_ticket].id = login_pack->id;
			clients[player_ticket].pw = login_pack->pw;

			std::wstring reason = L"로그인 성공! 어서오세요!";
			reason += login_pack->id;
			wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), reason.c_str(), _TRUNCATE);
			std::wcout << login_pack->id << L"님 어서오세요!\n";
		}
		else {
			login_user_mutex.unlock();
			const wchar_t* reason = L"로그인 실패! 중복된 이름이 사용중입니다...";
			std::wcout << reason << "\n";
			wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), reason, _TRUNCATE);
		}

		clients[player_ticket].send_packet(&result_packet);
		printf("로그인 결과 송신\n");
		break;
	}
	case C2S_PACKET_TYPE::REQUEST_JOIN_ROOM_PACK: {
		C2S_REQUEST_JOIN_ROOM_PACK* rq_room_info = reinterpret_cast<C2S_REQUEST_JOIN_ROOM_PACK*>(packet);
		std::wstring rq_room_name = rq_room_info->room_name;

		if (room_list.find(rq_room_name) == room_list.end()) {
			S2C_RESPONSE_JOIN_ROOM_PACK rs_room_info;
			rs_room_info.size = sizeof(rs_room_info);
			rs_room_info.type = S2C_PACKET_TYPE::RESPONSE_JOIN_ROOM_PACK;
			const wchar_t* reason = L"입력하신 이름의 채팅방이 없습니다.";
			wcsncpy_s(rs_room_info.result, sizeof(rs_room_info.result) / sizeof(wchar_t), reason, _TRUNCATE);
			clients[player_ticket].send_packet(&re_room_info);
		}
		else {
			room_list[rq_room_name].join_client(clients[player_ticket]);
		}
		break;
	}
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		C2S_SEND_CHAT_PACK* chat_packet = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);
		const std::wstring message = std::format(L"[{}]: {}\n", clients[player_ticket].id, chat_packet->str);

		std::wcout << message;
		
		write_to_chat_log(message);
		break;
	}
	case C2S_PACKET_TYPE::REQUEST_CHAT_LOG_PACK: {
		S2C_SEND_CHAT_LOG_PACK log_pack;
		log_pack.size = sizeof(S2C_SEND_CHAT_LOG_PACK);
		log_pack.type = S2C_PACKET_TYPE::SEND_CHAT_LOG_PACK;

		break;
	}
	case C2S_PACKET_TYPE::LOGOUT_PACK: {
		C2S_LOGOUT_PACK* user_info = reinterpret_cast<C2S_LOGOUT_PACK*>(packet);
		const std::wstring message = std::format(L"[{}] {} 님이 서버로 로그아웃 요청\n", user_info->id, user_info->id);

		std::wcout << message;

		S2C_LOGOUT_RESULT_PACK result_packet;
		result_packet.size = sizeof(result_packet);
		result_packet.type = S2C_PACKET_TYPE::LOGOUT_RESULT_PACK;

		login_user_mutex.lock();
		auto user_it = login_user_list.find(user_info->id);
		if (user_it != login_user_list.end()) {
			login_user_list.erase(user_it);
			login_user_mutex.unlock();

			const wchar_t* reason = L"로그아웃 성공! 안녕히 가세요 b^o^d";
			wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), reason, _TRUNCATE);
		}
		else {
			login_user_mutex.unlock();

			const wchar_t* reason = L"로그아웃 실패!";
			wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), reason, _TRUNCATE);
		}
		std::wcout << result_packet.result << "\n";

		clients[player_ticket].send_packet(&result_packet);
		printf("로그아웃 결과 송신\n");

		clients.erase(player_ticket);

		break;
	}
	}
}

void JobWorker::write_to_chat_log(const std::wstring& chat) {
	std::lock_guard<std::mutex> lock(chat_log_mutex);
	chat_log_file << chat;
}