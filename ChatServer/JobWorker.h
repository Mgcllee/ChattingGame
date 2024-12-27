#pragma once

#include "stdafx.h"
#include "Client.h"

static mutex chat_log_mutex;

class JobWorker
{
	HANDLE h_iocp;

	std::unordered_map<int, Client>& clients;

	SOCKET server_socket;
	SOCKET accept_client_socket;

	OverlappedExpansion* accept_overlapped_expansion;

	std::atomic<int>& ticket_number;

	std::ofstream& chat_log_file;

public:
	JobWorker(
		SOCKET in_server_socket,
		SOCKET in_accept_client_socket,
		OverlappedExpansion* in_accept_overlapped_expansion,
		atomic<int>& in_ticket_number,
		unordered_map<int, Client>& in_clients,
		ofstream& in_chat_log_file
	);
	~JobWorker();

	void job_worker(HANDLE h_iocp);
	bool check_exist_job(OverlappedExpansion* exoverlapped, BOOL GQCS_result, int client_ticket);

	void recv_client_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(int player_ticket, char* packet);

	void write_to_chat_log(const string& chat);
};