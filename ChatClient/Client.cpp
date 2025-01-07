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

	// TODO: 사용자 이름 중복 확인
	order = 3;

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
	case 3: {
		if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
			break;
		}

		login_server();
		if (login_result()) {
			
		}
		break;
	}
	}
}

void Client::login_server() {
	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, 99'999);
	int target = distr(eng);

	
	// TODO: 사용자 이름 중복 확인
	target = 0;


	C2S_LOGIN_PACK login_packet;
	login_packet.size = static_cast<short>(sizeof(login_packet));
	login_packet.type = C2S_PACKET_TYPE::LOGIN_PACK;
	wcsncpy_s(login_packet.id, sizeof(login_packet.id) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);
	wcsncpy_s(login_packet.pw, sizeof(login_packet.pw) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);
	
	size_t sent;
	sf::Socket::Status ret = m_socket->send(reinterpret_cast<const void*>(&login_packet), (size_t)login_packet.size, sent);
	if (sf::Socket::Status::NotReady == ret) {
		for (int second = 1; second <= 10; ++second) {
			sf::sleep(sf::seconds(1.f));
			target = distr(eng);
			wcsncpy_s(login_packet.id, sizeof(login_packet.id) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);
			wcsncpy_s(login_packet.pw, sizeof(login_packet.pw) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);

			ret = m_socket->send(reinterpret_cast<const void*>(&login_packet), (size_t)login_packet.size, sent);

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

bool Client::login_result()
{
	S2C_LOGIN_RESULT_PACK packet;
	size_t recv_size;
	m_socket->receive(&packet, sizeof(packet), recv_size);

	if (packet.result == L"로그인 성공! 어서오세요!\n") {
		return true;
	}

	wcout << packet.result;
	return false;
}

void Client::send_chatting()
{
	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, 99'999);
	int target = distr(eng);

	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = static_cast<short>(chat_sentences[target].length() + 1);
	wcsncpy_s(packet.str, sizeof(packet.str) / sizeof(wchar_t), chat_sentences[target].c_str(), _TRUNCATE);
	packet.size = static_cast<short>(sizeof(packet));

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

	S2C_SEND_CHAT_LOG_PACK* packet = new S2C_SEND_CHAT_LOG_PACK();
	size_t recved_size;
	m_socket->receive(reinterpret_cast<void*>(packet), (size_t)sizeof(packet), recved_size);
}

void Client::request_chat_log()
{
	
}

void Client::request_logout()
{
	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, 99'999);
	int target = distr(eng);

	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK; 
	packet.length = static_cast<short>(chat_sentences[target].length() + 1);
	wcsncpy_s(packet.str, sizeof(packet.str) / sizeof(wchar_t), chat_sentences[target].c_str(), _TRUNCATE);
	packet.size = static_cast<short>(sizeof(packet));

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
