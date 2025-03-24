#pragma once

#include "stdafx.h"

class Client
{
public:
	Client(const Client&) = delete;

	Client() : m_socket(SOCKET()) {
		eng = mt19937(rd());
		distr_job = uniform_int_distribution<>(JOB_TYPE::SEND_CHAT, JOB_TYPE::USER_LOGOUT);
		distr_name = uniform_int_distribution<>(0, MAX_SENTENCE - 1);
		distr_chat = uniform_int_distribution<>(0, MAX_SENTENCE - 1);
	}
	Client(Client&& other) noexcept : m_socket(other.m_socket) {
		eng = mt19937(rd());
		distr_job = uniform_int_distribution<>(JOB_TYPE::SEND_CHAT, JOB_TYPE::USER_LOGOUT);
		distr_name = uniform_int_distribution<>(0, MAX_SENTENCE - 1);
		distr_chat = uniform_int_distribution<>(0, MAX_SENTENCE - 1);
	}	
	~Client() {

	}


	Client& operator=(const Client&) = delete;

	Client& operator=(Client&& other) noexcept {
		if (this != &other) {
			m_socket = move(m_socket);
		}
		return *this;
	}


	void connect_to_server(string addr, unsigned short port, int i);
	void disconnect_to_server();
	
	void communicate_server(int i);
	void send_packet(BASIC_PACK& packet);
	template<typename T>
	void recv_packet(T& packet);
	
	void login_server();
	bool process_login_result();
	void request_logout();
	
	void send_chatting();
	void request_chat_log();

private:
	SOCKET m_socket;

	random_device rd;
	mt19937 eng;
	uniform_int_distribution<> distr_job;
	uniform_int_distribution<> distr_name;
	uniform_int_distribution<> distr_chat;

	wstring id = L"";
	wstring pw = L"";
};
