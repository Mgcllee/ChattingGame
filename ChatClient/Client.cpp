#include "Client.h"

Client::~Client()
{
	m_socket->disconnect();
}

sf::Socket::Status Client::connect_to_server(string addr, unsigned short port)
{
	sf::Socket::Status status = m_socket->connect(addr, port);
	m_socket->setBlocking(false);

	/*if (status != sf::Socket::Done) {
		printf("������ ������ �� �����ϴ�.\n");
	}*/

	return status;
}

void Client::disconnect_to_server()
{
	m_socket->disconnect();
}

void Client::communicate_server()
{
	random_device rd;  // �ϵ����� ���� �ѹ��� ����
	mt19937 eng(rd()); // �����⸦ �õ�� �ʱ�ȭ
	uniform_int_distribution<> distr(1, 5); // 1���� 5������ ���� ����

	int order = distr(eng);
	if (order >= 3 && m_socket->getRemoteAddress() != sf::IpAddress::None) {
		order = 1;
	}

	switch (order) {
	case 1: {	// connect 
		m_socket->disconnect();
		connect_to_server(SERVER_ADDR, PORT_NUM);
		break;
	}
	case 2: {	// disconnect
		m_socket->disconnect();
		break;
	}
	case 3: {	// send chat and recv new chat
		send_chatting();
		// recv_chatting();
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
}

void Client::send_chatting()
{
	// [Work I/O]: �޸���(ReadOnly)���� �Ϻ� �����Ͽ� �а� �� ������ ����

	PACKET packet;
	
	// [TODO]: update read io work
	packet.content = "Hello World!";
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
	
	printf("%s\n", packet->content.c_str());
}

void Client::request_chat_log()
{
	
}