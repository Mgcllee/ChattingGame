#pragma once

#include "GameServerSilo.h"

#include "NetworkManagerGrain.h"
#include "NetworkWorkerGrain.h"
#include "ClientWorkerGrain.h"
#include "ChatRoomWorkerGrain.h"
#include "DataBaseWorkerGrain.h"

GameServerSilo::GameServerSilo() {
	h_iocp_network = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_clients = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_chatroom = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_database = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

GameServerSilo::~GameServerSilo() {
	// free windows handle
}

void GameServerSilo::run_gameserver_silo() {
	NetworkManagerGrain networksetting("127.0.0.1", h_iocp_network);

	std::vector<std::thread> grain_threads;
	const short count_network_grain = 4;
	const short count_clients_grain = 8;
	const short count_chatroom_grain = 8;
	const short count_database_grain = 4;
	
	for (int i = 0; i < count_network_grain; ++i) {
		grain_threads.emplace_back(
			&NetworkWorkerGrain::network_packet_worker, new NetworkWorkerGrain(networksetting), 
			h_iocp_network, h_iocp_clients
		);
	}

	for (int i = 0; i < count_clients_grain; ++i) {
		grain_threads.emplace_back(
			&ClientWorkerGrain::job_worker, new ClientWorkerGrain(), h_iocp_clients
		);
	}

	for (int i = 0; i < count_chatroom_grain; ++i) {
		grain_threads.emplace_back(
			&ChatRoomWorkerGrain::job_worker, new ChatRoomWorkerGrain(), h_iocp_chatroom
		);
	}

	for (int i = 0; i < count_database_grain; ++i) {
		grain_threads.emplace_back(
			&DataBaseWorkerGrain::job_worker, new DataBaseWorkerGrain(), h_iocp_database
		);
	}

	for (auto& th : grain_threads) {
		th.join();
	}
}
