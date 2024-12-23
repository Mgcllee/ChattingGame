#pragma once

#include "JobWorker.h"

class GameSession
{
	SOCKET server_socket;
	SOCKET accept_client_socket;

	OverlappedExpansion* accept_overlapped_expansion;
	HANDLE h_iocp;

	atomic<int> ticket_number;
	unordered_map<int, Client> clients;

public:
	GameSession();
	~GameSession();

	void run_game_session();

};