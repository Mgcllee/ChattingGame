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

SOCKET server_socket;
SOCKET client_socket;
OverlappedExpansion* accept_overlapped_expansion;

unordered_map<int, Client> clients;

int main() {
	WSADATA WSAData;
	int error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	int option = TRUE;
	setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

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

	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	option = TRUE;
	setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);

	AcceptEx(server_socket, client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);

	ticket_number.store(0);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(process_packet, h_iocp);
	for (auto& th : worker_threads)
		th.join();

	return 0;
}