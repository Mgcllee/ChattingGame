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
		wprintf(L"Error first connect_to_server function!\n");
		exit(true);
	}

	return status;
}

void Client::disconnect_to_server()
{
	id.clear();
	pw.clear();
	m_socket->disconnect();
}

void Client::communicate_server(int key) {
	if (m_socket->getRemoteAddress() == sf::IpAddress::None) {
		connect_to_server(SERVER_ADDR, PORT_NUM, key);
		return;
	}

	if (id.empty()) {
		login_server();
		return;
	}

	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(JOB_TYPE::USER_LOGIN, JOB_TYPE::USER_LOGOUT);

	int order = distr(eng);

	BASIC_PACK recv_pack{};
	recv_packet(recv_pack);
	if (recv_pack.type == S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS) {

	}

	switch (order) {
	case JOB_TYPE::SEND_CHAT: {
		send_chatting();
		break;
	}
	case JOB_TYPE::USER_LOGOUT: {
		// request_logout();
		break;
	}
	}
}

void Client::send_packet(BASIC_PACK& packet) {
	if (packet.size <= 0 || m_socket->getRemoteAddress() == sf::IpAddress::None)
		return;

	size_t sent;
	sf::Socket::Status ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);
	if (sf::Socket::Status::NotReady == ret) {
		for (int second = 1; second <= 10; ++second) {
			sf::sleep(sf::seconds(0.01f));
			ret = m_socket->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent);

			if (ret != sf::Socket::Status::NotReady)
				break;
		}
	}

	if (sf::Socket::Done != ret) {
		wprintf(L"클라이언트 송신 오류 %d\n", static_cast<int>(ret));
	}
}

template<typename T>
void Client::recv_packet(T& packet)
{
	if (m_socket->getRemoteAddress() == sf::IpAddress::None)
		return;

	size_t recv_size;
	sf::Socket::Status ret = m_socket->receive(&packet, sizeof(packet), recv_size);
	if (sf::Socket::Status::NotReady == ret) {
		for (int second = 1; second <= 10; ++second) {
			sf::sleep(sf::seconds(0.1f));
			ret = m_socket->receive(&packet, sizeof(packet), recv_size);

			if (ret != sf::Socket::Status::NotReady)
				break;
		}
	}

	if (packet.type == S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS || sf::Socket::NotReady == ret) {
		return;
	}
	if (sf::Socket::Done != ret) {
		wprintf(L"클라이언트 수신 오류 %d\n", static_cast<int>(ret));
	}
}

void Client::login_server() {
	if (m_socket->getRemoteAddress() == sf::IpAddress::None)
		return;

	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, MAX_SENTENCE);

	C2S_LOGIN_PACK login_packet;
	login_packet.size = static_cast<short>(sizeof(login_packet));
	login_packet.type = C2S_PACKET_TYPE::LOGIN_PACK;
	
	for (int time = 0; time < 10; ++time) {
		int target = distr(eng);
		wcsncpy_s(login_packet.id, sizeof(login_packet.id) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);
		wcsncpy_s(login_packet.pw, sizeof(login_packet.pw) / sizeof(wchar_t), user_id[target].c_str(), _TRUNCATE);
	
		send_packet(login_packet);
		sf::sleep(sf::seconds(0.02f));
		wprintf(L"%s 로그인 시도\n", user_id[target].c_str());

		if (process_login_result()) {
			id = user_id[target];
			pw = user_id[target];
			wprintf(L"%s님 로그인 성공!\n", id.c_str());
			break;
		}
		else {
			wprintf(L"동일한 ID(%s)때문에 로그인 실패\n", user_id[target].c_str());
			sf::sleep(sf::seconds(0.01f));
		}
	}
}

bool Client::process_login_result() {
	S2C_LOGIN_RESULT_PACK packet{};
	recv_packet(packet);

	if (packet.size <= 0) return false;

	// wcout << packet.result << "\n";
	
	wstring result(packet.result);
	if(result.find(L"로그인 성공!") != wstring::npos) {
		return true;
	}
	else return false;
}

void Client::send_chatting() {
	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, 99'999);
	int target = distr(eng);

	C2S_SEND_CHAT_PACK packet;
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	packet.length = static_cast<short>(chat_sentences[target].length() + 1);
	wcsncpy_s(packet.str, sizeof(packet.str) / sizeof(wchar_t), chat_sentences[target].c_str(), _TRUNCATE);
	packet.size = static_cast<short>(sizeof(packet));

	send_packet(packet);
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

void Client::request_logout() {
	if (sizeof(id) <= 0 && sizeof(pw) <= 0) return;

	C2S_LOGOUT_PACK logout_packet;
	logout_packet.size = sizeof(logout_packet);
	logout_packet.type = C2S_PACKET_TYPE::LOGOUT_PACK;
	wcsncpy_s(logout_packet.id, sizeof(logout_packet.id) / sizeof(wchar_t), id.c_str(), _TRUNCATE);
	wcsncpy_s(logout_packet.pw, sizeof(logout_packet.pw) / sizeof(wchar_t), pw.c_str(), _TRUNCATE);
	send_packet(logout_packet);

	S2C_LOGOUT_RESULT_PACK result_packet{};
	recv_packet(result_packet);

	if (result_packet.size <= 0) return;

	wstring result(result_packet.result);
	if (result.find(L"로그아웃 성공") != wstring::npos) {
		wprintf(L"%s\n", result_packet.result);
		disconnect_to_server();
		return;
	}
	else return;
}
