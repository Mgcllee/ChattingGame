#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/OverlappedExpansion.h"

class GameServerSilo {
private:
	HANDLE h_iocp_network;
	HANDLE h_iocp_clients;
	HANDLE h_iocp_chatroom;
	HANDLE h_iocp_database;

	SOCKET server_socket;
	SOCKET accept_client_socket;
	OverlappedExpansion* accept_overlapped_expansion;

public:
	GameServerSilo();
	~GameServerSilo();

	void run_game_logic_grains();
};