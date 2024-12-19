#pragma once

#include "stdafx.h"

class Client
{
private:
	unique_ptr<sf::TcpSocket> m_socket;

public:
	Client() : m_socket(make_unique<sf::TcpSocket>()) {}
	~Client();
	
	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;

	Client(Client&& other) noexcept : m_socket(move(other.m_socket)) {}
	Client& operator=(Client&& other) noexcept {
		if (this != &other) {
			m_socket = move(other.m_socket);
		}
		return *this;
	}


	sf::Socket::Status connect_to_server(string addr, unsigned short port);
	void disconnect_to_server();

	void communicate_server();

	void send_chatting();
	void recv_chatting();

	void request_chat_log();
};

