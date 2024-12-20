#pragma once

#include "stdafx.h"
#include "OverlappedExpansion.h"
#include "Client.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <atomic>
#include <thread>

atomic<int> ticket_number;

#pragma pack (1)
struct PACKET {
	char size;
	char content[64];
};
#pragma pack (pop)

SOCKET server_socket;
SOCKET client_socket;
OverlappedExpansion* accept_overlapped_expansion;

unordered_map<int, Client> clients;

void process_packet(HANDLE h_iocp) {
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	printf("Running Server...\n");

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &overlapped, INFINITY);
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (exoverlapped == nullptr) continue;
		if (FALSE == GQCS_result) {
			if (exoverlapped->socket_type == ACCEPT) {
				printf("Accept error\n");
			}
			else {
				int player_ticket = static_cast<int>(key);
				string error_message = "GQCS Error on client[";
				error_message += to_string(player_ticket);
				error_message += "]";
				printf("%s\n", error_message.c_str());
			}
			continue;
		}

		switch (exoverlapped->socket_type) {
		case SOCKET_TYPE::ACCEPT: {
			
			// printf("new clinet accept\n");
			
			int ticket = ticket_number.load();
			ticket_number.fetch_add(1);
			clients[ticket] = Client(client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, ticket, 0);
			clients[ticket].recv_packet();

			ZeroMemory(&accept_overlapped_expansion->overlapped,
				sizeof(accept_overlapped_expansion->overlapped));
			client_socket =
				WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server_socket, client_socket,
				accept_overlapped_expansion->packet_buffer, 0, addr_size + 16, addr_size + 16,
				0, &accept_overlapped_expansion->overlapped);
			break;
		}
		case SOCKET_TYPE::RECV: {
			int ticket = static_cast<int>(key);
			int remain_data = num_bytes + clients[ticket].remain_packet_size;

			char* p = exoverlapped->packet_buffer;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {

					if (ticket % 374 == 0) {
						PACKET* chat_packet = reinterpret_cast<PACKET*>(p);
						printf("[%d]: %s\n", ticket, chat_packet->content);
					}

					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else {
					break;
				}
			}

			clients[ticket].remain_packet_size = remain_data;
			if (remain_data > 0) {
				memcpy(exoverlapped->packet_buffer, p, remain_data);
			}
			clients[ticket].recv_packet();
			break;
		}
		case SOCKET_TYPE::SEND: {

			break;
		}
		default: {
			break;
		}
		}
	}
}

int main() {
	ios_base::sync_with_stdio(false);


	WSADATA WSAData;
	int error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


	int option = TRUE;
	setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option,
		sizeof(option));

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server_socket, SOMAXCONN);



	accept_overlapped_expansion = new OverlappedExpansion();
	accept_overlapped_expansion->socket_type = SOCKET_TYPE::ACCEPT;

	HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, (ULONG_PTR)accept_overlapped_expansion, 0);

	if (h_iocp == NULL) {
		cout << GetLastError();
		return 0;
	}

	ticket_number.store(0);

	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	option = TRUE;
	setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));


	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);

	AcceptEx(server_socket, client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(process_packet, h_iocp);
	
	for (auto& th : worker_threads)
		th.join();

	return 0;
}