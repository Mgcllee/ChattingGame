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
	random_device rd;  // �ϵ����� ���� �ѹ��� ����
	mt19937 eng(rd()); // �����⸦ �õ�� �ʱ�ȭ
	uniform_int_distribution<> distr(2, 3); // 1���� 5������ ���� ����

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

	PACKET packet;
	
	// [TODO]: update read io work
	 strncpy_s(packet.content, "Hello World!", sizeof("Hello World!"));
	packet.size = sizeof(packet);

	size_t sent;
	if (sf::Socket::Done != m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent)) {
		printf("Ŭ���̾�Ʈ �۽� ����\n");
	}
}

void Client::recv_chatting()
{
	// [Work I/O]: ���Ӱ� ���� ä���� �׷������� ���(�ϴ� ����̶�� ����)

	PACKET* packet = new PACKET();
	size_t recved_size;
	m_socket->receive(reinterpret_cast<void*>(packet), (size_t)sizeof(packet), recved_size);
	
	printf("%s\n", packet->content);
}

void Client::request_chat_log()
{
	
}

void Client::request_logout()
{
	// [Work I/O]: �޸���(ReadOnly)���� �Ϻ� �����Ͽ� �а� �� ������ ����

	PACKET packet;

	// [TODO]: update read io work

	const char* str = "Goodbye Server i'm leave";;
	strncpy_s(packet.content, str, sizeof(str));
	packet.size = sizeof(packet);

	size_t sent;
	if (sf::Socket::Done != m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent)) {
		printf("Ŭ���̾�Ʈ �۽� ����\n");
	}
}
