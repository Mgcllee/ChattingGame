#pragma once

#include "JobWorker.h"

class GameServerSilo {
private:
	// Accept, Recv, Send, ((가제)Exit) 처리 담당
	HANDLE h_iocp_network;
	// 클라이언트 요청 사항 처리 담당
	HANDLE h_iocp_clients;
	// 각 채팅룸에서 받는 요청 처리 담당
	HANDLE h_iocp_chatroom;
	// 데이터베이스 처리 담당
	HANDLE h_iocp_database;

public:
	GameServerSilo();
	~GameServerSilo();

	void run_game_logic_grains();
};