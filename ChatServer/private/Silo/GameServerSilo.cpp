#pragma once

#include "ChatServer/public/Silo/GameServerSilo.h"

#include "ChatServer/public/Grain/NetworkManagerGrain.h"
#include "ChatServer/public/Grain/NetworkWorkerGrain.h"
#include "ChatServer/public/Grain/ClientWorkerGrain.h"
#include "ChatServer/public/Grain/ChatRoomWorkerGrain.h"
#include "ChatServer/public/Grain/DataBaseWorkerGrain.h"
#include "ChatServer/public/Grain/TimerGrain.h"

GameServerSilo::GameServerSilo() {
	h_iocp_network = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_clients = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_chatroom = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	h_iocp_database = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

GameServerSilo::~GameServerSilo() {
	CloseHandle(h_iocp_network);
	CloseHandle(h_iocp_clients);
	CloseHandle(h_iocp_chatroom);
	CloseHandle(h_iocp_database);
}

void GameServerSilo::run_game_logic_grains() {
	NetworkManagerGrain networksetting("127.0.0.1", h_iocp_network);

	std::vector<std::thread> grain_threads;
	const short count_network_grain = 2;
	const short count_clients_grain = 2;
	const short count_chatroom_grain = 2;
	const short count_database_grain = 1;
	
	std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps
		{ h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database };

	grain_threads.emplace_back(&TimerGrain::packet_worker, new TimerGrain(), h_iocps);

	for (int i = 0; i < count_network_grain; ++i) {
		grain_threads.emplace_back(
			&NetworkWorkerGrain::packet_worker, new NetworkWorkerGrain(networksetting), h_iocps
		);
	}

	for (int i = 0; i < count_clients_grain; ++i) {
		grain_threads.emplace_back(
			&ClientWorkerGrain::packet_worker, new ClientWorkerGrain(), h_iocps
		);
	}

	for (int i = 0; i < count_chatroom_grain; ++i) {
		grain_threads.emplace_back(
			&ChatRoomWorkerGrain::packet_worker, new ChatRoomWorkerGrain(), h_iocps
		);
	}

	for (int i = 0; i < count_database_grain; ++i) {
		grain_threads.emplace_back(
			&DataBaseWorkerGrain::packet_worker, new DataBaseWorkerGrain(), h_iocps
		);
	}

	for (auto& th : grain_threads) {
		th.join();
	}
}
