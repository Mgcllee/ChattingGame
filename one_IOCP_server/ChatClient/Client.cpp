#include "Client.h"


void Client::connect_to_server(string addr, unsigned short port, int i)
{
	WSADATA wsadata;

	if(WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		wprintf(L"WSAStartup Error\n");
		return;
	}

	if((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		wprintf(L"Socket Error\n");
		return;
	}

	// Nagle 알고리즘 끄기
	int flag = 1;
	if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0) {
		WSACleanup();
		return;
	}

	u_long mode = FALSE;
	ioctlsocket(m_socket, FIONBIO, &mode);

	SOCKADDR_IN server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &server_address.sin_addr);
	server_address.sin_port = ::htons(PORT_NUM);

	while (true) {
		if (SOCKET_ERROR == connect(m_socket, (SOCKADDR*)&server_address, sizeof(server_address))) {
			int error_type = WSAGetLastError();
			if (error_type == WSAEWOULDBLOCK) {
				continue;
			}
			else if (error_type == WSAEISCONN) {
				break;
			}
			else {
				wprintf(L"connection Error! (error_type: %d)\n", error_type);
				return;
			}
		}
	}
}

void Client::disconnect_to_server() {
	id.clear();
	pw.clear();
}

void Client::communicate_server(int key) {
	if (m_socket == NULL) {
		connect_to_server(SERVER_ADDR, PORT_NUM, 0);
		return;
	}
	else if (id.empty()) {
		login_server();
		return;
	}

	send_chatting();
}

void Client::send_packet(BASIC_PACK& packet) {
	if (packet.size <= 0 || m_socket == NULL)
		return;

	WSABUF wsabuf;
	wsabuf.buf = reinterpret_cast<char*>(&packet);
	wsabuf.len = packet.size;

	DWORD  bytesSent;
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(m_socket, &writefds);

	int result = select(0, NULL, &writefds, NULL, NULL);
	if (result > 0) {
		result = WSASend(m_socket, &wsabuf, 1, &bytesSent, 0, NULL, NULL);
		if (result == SOCKET_ERROR) {
			wprintf(L"WSASend failed: %d\n", WSAGetLastError());
		}
		else {
			// Success
		}
	}
	else if (result == 0) {
		wprintf(L"Timeout occurred while waiting for socket to be writable.\n");
	}
	else {
		wprintf(L"select failed: %d\n", WSAGetLastError());
	}
}

void Client::login_server() {
	if (m_socket == NULL)
		return;

	C2S_LOGIN_PACK login_packet;
	login_packet.size = static_cast<short>(sizeof(login_packet));
	login_packet.type = C2S_PACKET_TYPE::LOGIN_PACK;
	
	for (int time = 0; time < 10; ++time) {
		int target = distr_name(eng);
		wcscpy_s(login_packet.id, user_id[target].c_str());

		S2C_LOGIN_RESULT_PACK packet{};
		
		// send_packet(login_packet);
		send(m_socket, reinterpret_cast<const char*>(&login_packet), sizeof(login_packet), 0);
		recv(m_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
		
		if (packet.size <= 0) continue;

		wstring result(packet.result);
		if (result.find(L"로그인 성공!") != wstring::npos) {
			id = user_id[target];
			wprintf(L"%s\n", result.c_str());
			break;
		}
		else wprintf(L"%s\n", result.c_str());
	}
}

void Client::send_chatting() {
	C2S_SEND_CHAT_PACK packet;
	packet.size = static_cast<short>(sizeof(packet));
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;

	int chat_idx = distr_chat(eng);
	wcscpy_s(packet.str, chat_sentences[chat_idx].c_str());

	send_packet(packet);
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
	recv(m_socket, reinterpret_cast<char*>(&result_packet), sizeof(result_packet), 0);


	if (result_packet.size <= 0) return;

	wstring result(result_packet.result);
	if (result.find(L"로그아웃 성공") != wstring::npos) {
		disconnect_to_server();
		return;
	}
	else return;
}
