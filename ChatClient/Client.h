#pragma once

#include "stdafx.h"

class Client
{
private:
	unique_ptr<sf::TcpSocket> m_socket;

	random_device rd;
	mt19937 eng;
	uniform_int_distribution<> distr;

public:
	wstring id = L"";
	wstring pw = L"";

public:
	Client() : m_socket(make_unique<sf::TcpSocket>()) {
		eng = mt19937(rd());
		distr = uniform_int_distribution<>(JOB_TYPE::USER_LOGIN, JOB_TYPE::USER_LOGOUT);
	}
	Client(const Client&) = delete;
	Client(Client&& other) noexcept : m_socket(move(other.m_socket)) {
		eng = mt19937(rd());
		distr = uniform_int_distribution<>(JOB_TYPE::USER_LOGIN, JOB_TYPE::USER_LOGOUT);
	}
	
	~Client();

	Client& operator=(const Client&) = delete;
	Client& operator=(Client&& other) noexcept {
		if (this != &other) {
			m_socket = move(other.m_socket);
		}
		return *this;
	}


	sf::Socket::Status connect_to_server(string addr, unsigned short port, int i);
	void disconnect_to_server();
	
	void communicate_server(int i);
	void send_packet(BASIC_PACK& packet);
	
	template<typename T>
	void recv_packet(T& packet);
	
	void login_server();
	bool process_login_result();
	void send_chatting();
	void recv_chatting();

	void request_chat_log();
	void request_logout();


};
