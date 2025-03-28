#pragma once

#include "ChatServer/public/Silo/GameServerSilo.h"
#include "ChatServer/public/Grain/NetworkManagerGrain.h"

GameServerSilo::GameServerSilo() {
	h_iocp_network = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

GameServerSilo::~GameServerSilo() {
	CloseHandle(h_iocp_network);
}

void GameServerSilo::run_game_logic_grains() {
	WSADATA WSAData;
	int error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	int option;
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	error_code = setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);
	
	error_code = bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	error_code = listen(server_socket, SOMAXCONN);

	accept_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	setsockopt(accept_client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	accept_overlapped_expansion = new OverlappedExpansion();
	accept_overlapped_expansion->overlapped_type = OVERLAPPED_TYPE::CLIENT_ACCEPT;

	h_iocp_network = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp_network, (ULONG_PTR)accept_overlapped_expansion, 0);

	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);
	AcceptEx(server_socket, accept_client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);

	std::vector<std::thread> workers;
	for (int i = 0; i < 1; ++i) {
		workers.emplace_back(&NetworkManagerGrain::packet_worker, new NetworkManagerGrain(), h_iocp_network,
			server_socket, accept_client_socket, accept_overlapped_expansion);
	}
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(10));

		OverlappedExpansion timer_overlapped;
		timer_overlapped.overlapped_type = OVERLAPPED_TYPE::CHECK_EXIST_CLIENTS;
		DWORD sent_byte = 0;
		ULONG_PTR key = 0;
		PostQueuedCompletionStatus(h_iocp_network, sent_byte, key, &timer_overlapped.overlapped);
	}
	for (auto& ths : workers) {
		ths.join();
	}
}
