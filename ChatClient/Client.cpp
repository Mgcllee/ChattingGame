#include "Client.h"

Client::~Client()
{
	// m_socket->disconnect();
}

sf::Socket::Status Client::connect_to_server(string addr, unsigned short port, int i)
{
	sf::Socket::Status status = m_socket->connect(addr, port);
	m_socket->setBlocking(false);

	if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
		printf("Error first connect_to_server function!\n");
		exit(true);
	}

	return status;
}

void Client::disconnect_to_server()
{
	m_socket->disconnect();
}

void Client::communicate_server(int i)
{
	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(1, 2);

	int order = distr(eng);
	switch (order) {
	case 1: {	// disconnect
		if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
			break;
		}
		
		request_logout();
		//m_socket->disconnect();
		break;
	}
	case 2: {	// send chat and recv new chat
		if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
			break;
		}
		
		send_chatting();
		// recv_chatting();
		break;
	}
	}
}

void Client::send_chatting()
{
	// [Work I/O]: 메모장(ReadOnly)에서 일부 발췌하여 읽고 그 내용을 전송

	const char* str = "Hello World!";
	
	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = std::strlen(str) + 1;
	strncpy_s(packet.str, str, packet.length);
	packet.size = sizeof(packet);

	size_t sent;
	sf::Socket::Status ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);
	if (sf::Socket::Status::NotReady == ret) {
		for (int second = 1; second <= 10; ++second) {
			sf::sleep(sf::seconds(1.f));
			ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);
			
			if (ret != sf::Socket::Status::NotReady)
				break;

			printf("Try again...\n");
		}
	}

	if (sf::Socket::Done != ret) {
		printf("클라이언트 송신 오류 %d\n", static_cast<int>(ret));
		exit(true);
	}
}

void Client::recv_chatting()
{
	// [Work I/O]: 새롭게 받은 채팅을 그래픽으로 출력(하는 모습이라고 가정)

	S2C_SEND_CHAT_LOG* packet = new S2C_SEND_CHAT_LOG();
	size_t recved_size;
	m_socket->receive(reinterpret_cast<void*>(packet), (size_t)sizeof(packet), recved_size);
	
	// printf("%s\n", packet->content);
}

void Client::request_chat_log()
{
	
}

void Client::request_logout()
{
	// [Work I/O]: 메모장(ReadOnly)에서 일부 발췌하여 읽고 그 내용을 전송

	const char* str = "Goodbye Server i'm leave";

	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = std::strlen(str) + 1;
	strncpy_s(packet.str, str, packet.length);
	packet.size = sizeof(packet);

	size_t sent;
	sf::Socket::Status ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);
	if (sf::Socket::Status::NotReady == ret) {
		for (int second = 1; second <= 10; ++second) {
			sf::sleep(sf::seconds(1.f));
			ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);

			if (ret != sf::Socket::Status::NotReady)
				break;
			
			printf("Try again...\n");
		}
	}

	if (sf::Socket::Done != ret) {
		printf("클라이언트 송신 오류 %d\n", static_cast<int>(ret));
		exit(true);
	}
}
