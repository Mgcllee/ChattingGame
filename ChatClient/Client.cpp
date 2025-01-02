#include "Client.h"

Client::~Client()
{
	// m_socket->disconnect();
}

sf::Socket::Status Client::connect_to_server(string addr, unsigned short port, int i)
{
	sf::Socket::Status status = m_socket->connect(addr, port);
	m_socket->setBlocking(false);

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
	uniform_int_distribution<> distr(2, 3);

	int order = distr(eng);
	
	if (order == 1) order = 2;

	switch (order) {
	case 1: {	// connect 
		if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
			connect_to_server(SERVER_ADDR, PORT_NUM, i);
		}
		break;
	}
	case 2: {	// disconnect
		if (m_socket->getRemoteAddress() != sf::IpAddress::None) {
			request_logout();
			//m_socket->disconnect();
		}
		break;
	}
	case 3: {	// send chat and recv new chat
		if (m_socket->getRemoteAddress() != sf::IpAddress::None) {
			send_chatting();
			// recv_chatting();
		}
		break;
	}
	case 4: {	// request all chat

		break;
	}
	case 5: {	// request my information

		break;
	}
	default: {
		break;
	}
	}

	if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
		printf("None %d\n", i);
		exit(true);
	}
}

void Client::send_chatting()
{
	// [Work I/O]: �޸���(ReadOnly)���� �Ϻ� �����Ͽ� �а� �� ������ ����

	const char* str = "Hello World!";
	
	
	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = std::strlen(str) + 1;
	strncpy_s(packet.str, str, packet.length);
	packet.size = sizeof(packet);

	size_t sent;
	if (sf::Socket::Done != m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent)) {
		printf("Ŭ���̾�Ʈ �۽� ����\n");
	}
}

void Client::recv_chatting()
{
	// [Work I/O]: ���Ӱ� ���� ä���� �׷������� ���(�ϴ� ����̶�� ����)

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
	// [Work I/O]: �޸���(ReadOnly)���� �Ϻ� �����Ͽ� �а� �� ������ ����

	const char* str = "Goodbye Server i'm leave";

	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = std::strlen(str) + 1;
	strncpy_s(packet.str, str, packet.length);
	packet.size = sizeof(packet);

	size_t sent;
	if (sf::Socket::Done != m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent)) {
		printf("Ŭ���̾�Ʈ �۽� ����\n");
	}
}
