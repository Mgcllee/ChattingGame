#pragma once

#include "stdafx.h"

class Client
{
private:
	unique_ptr<sf::TcpSocket> m_socket;

public:
	wchar_t id[MAX_BUF] = L"";
	wchar_t pw[MAX_BUF] = L"";

public:
	Client() : m_socket(make_unique<sf::TcpSocket>()) {}
	Client(const Client&) = delete;
	Client(Client&& other) noexcept : m_socket(move(other.m_socket)) {}
	
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
