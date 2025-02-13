#pragma once

#include "JobWorker.h"

class GameServerSilo {
private:
	// Accept, Recv, Send, ((����)Exit) ó�� ���
	HANDLE h_iocp_network;
	// Ŭ���̾�Ʈ ��û ���� ó�� ���
	HANDLE h_iocp_clients;
	// �� ä�÷뿡�� �޴� ��û ó�� ���
	HANDLE h_iocp_chatroom;
	// �����ͺ��̽� ó�� ���
	HANDLE h_iocp_database;

public:
	GameServerSilo();
	~GameServerSilo();

	void run_game_logic_grains();
};