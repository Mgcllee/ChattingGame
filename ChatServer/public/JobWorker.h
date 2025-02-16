#pragma once

#include <unordered_set>
#include <cstdlib>

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Client.h"
#include "ChatServer/public/ChatRoomSession.h"

static std::mutex chat_log_mutex;
extern std::mutex login_user_mutex;

static std::unordered_set<std::wstring> login_user_list;

class NetSocketWorker
{
	HANDLE h_iocp;

	SOCKET& server_socket;
	SOCKET& accept_client_socket;
	OverlappedExpansion* accept_overlapped_expansion;

	std::atomic<int>& ticket_number;

	std::wofstream& chat_log_file;

	std::unordered_map<std::wstring, ChatRoomSession>& room_list;

public:
	NetSocketWorker(
		SOCKET& in_server_socket,
		SOCKET& in_accept_client_socket,
		OverlappedExpansion* in_accept_overlapped_expansion
	);
	~NetSocketWorker();

	void job_worker(HANDLE h_iocp);
	bool check_exist_job(OverlappedExpansion* exoverlapped, BOOL GQCS_result, int client_ticket);

	void recv_client_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(int player_ticket, short* packet);

	void write_to_chat_log(const std::wstring& chat);
};