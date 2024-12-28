#pragma once

#include "JobWorker.h"

JobWorker::JobWorker(
	SOCKET in_server_socket,
	SOCKET in_accept_client_socket, 
	OverlappedExpansion* in_accept_overlapped_expansion,
	std::atomic<int>& in_ticket_number,
	std::unordered_map<int, Client>& in_clients,
	std::ofstream& in_chat_log_file
)
	: server_socket(in_server_socket)
	, accept_client_socket(in_accept_client_socket)
	, accept_overlapped_expansion(in_accept_overlapped_expansion)
	, ticket_number(in_ticket_number)
	, clients(in_clients)
	, chat_log_file(in_chat_log_file)
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

	char* p = exoverlapped->packet_buffer;
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

void JobWorker::process_packet(int player_ticket, char* packet) {
	switch (packet[1]) {
	case C2S_PACKET_TYPE::SEND_CHAT_PACK: {
		C2S_SEND_CHAT_PACK* chat_packet = reinterpret_cast<C2S_SEND_CHAT_PACK*>(packet);
		const std::string message = std::format("[{}]: {}\n", player_ticket, chat_packet->content);
		printf("%s\n", message.c_str());
		write_to_chat_log(message);
		break;
	}
	case C2S_PACKET_TYPE::REQUEST_CHAT_LOG_PACK: {
		S2C_SEND_CHAT_LOG log_pack;
		log_pack.size = sizeof(S2C_SEND_CHAT_LOG);
		log_pack.type = S2C_PACKET_TYPE::SEND_CHAT_LOG_PACK;

		break;
	}
	}
}

void JobWorker::write_to_chat_log(const std::string& chat) {
	std::lock_guard<std::mutex> lock(chat_log_mutex);
	chat_log_file << chat << std::endl;
}