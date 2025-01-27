#pragma once

#include "JobWorker.h"
#include "ChatRoomSession.h"

class GameSession
{
	SOCKET server_socket;
	SOCKET accept_client_socket;

	OverlappedExpansion* accept_overlapped_expansion;
	HANDLE h_iocp;

	std::atomic<int> ticket_number;
	std::unordered_map<int, Client> clients;

	std::wofstream chat_log_file;
	xlnt::workbook wb;
	xlnt::worksheet ws;
	std::vector<std::vector<int>> map;

	std::unordered_map<std::wstring, ChatRoomSession> room_list;

public:
	GameSession();
	~GameSession();

	void run_game_session();

};