#pragma once

#include "GameSession.h"

GameSession::GameSession()
{
	WSADATA WSAData;
	int error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	int option;
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	error_code = setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	error_code = bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	error_code = listen(server_socket, SOMAXCONN);

	accept_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	setsockopt(accept_client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));


	accept_overlapped_expansion = new OverlappedExpansion();
	accept_overlapped_expansion->socket_type = SOCKET_TYPE::ACCEPT;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, (ULONG_PTR)accept_overlapped_expansion, 0);


	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);

	AcceptEx(server_socket, accept_client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);


	chat_log_file.open("chat_log.txt", std::ios::out | std::ios::app);
}

GameSession::~GameSession()
{
}

void GameSession::run_game_session() {
	ticket_number.store(0);

	std::vector<std::thread> worker_threads;
	int num_threads = 8;
	
	for (int i = 0; i < num_threads; ++i) {
		worker_threads.emplace_back(
			&JobWorker::job_worker, 
			new JobWorker(server_socket, accept_client_socket, accept_overlapped_expansion, 
				ticket_number, clients,
				chat_log_file),
			h_iocp);
	}

	for (auto& th : worker_threads) {
		th.join();
	}
}
