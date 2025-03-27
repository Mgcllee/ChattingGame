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

	int flag = TRUE;
	if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0) {
		WSACleanup();
		return;
	}

	u_long mode = 1;
	if (ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR) {
		wprintf(L"Faile to set non-blocking socket [Error Code: %d]\n", GetLastError());
		return;
	}

	SOCKADDR_IN server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &server_address.sin_addr);
	server_address.sin_port = htons(PORT_NUM);

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
	closesocket(m_socket);
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

	DWORD  bytesSent;
	WSABUF wsabuf;
	wsabuf.buf = reinterpret_cast<char*>(&packet);
	wsabuf.len = packet.size;

	auto startTime = std::chrono::steady_clock::now();
	auto currentTime = startTime;
	while (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() < 10) {
		int result = WSASend(m_socket, &wsabuf, 1, &bytesSent, 0, NULL, NULL);
		if (result == SOCKET_ERROR) {
			if (result == WSAEWOULDBLOCK) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				currentTime = std::chrono::steady_clock::now();
				continue;
			}
			wprintf(L"WSASend failed with error [error_code: %d]\n", GetLastError());
			return;
		}
		return;
	}
	wprintf(L"Send timeout after 10 seconds.\n");
}

template<typename T>
void Client::recv_packet(T& packet)
{
	if (m_socket == NULL)
		return;

	DWORD bytesReceived;
	DWORD flags = 0;

	wchar_t buffer[1024];
	WSABUF wsabuf;
	wsabuf.buf = reinterpret_cast<CHAR*>(buffer);
	wsabuf.len = sizeof(buffer);

	auto startTime = std::chrono::steady_clock::now();
	auto currentTime = startTime;
	while (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() < 10) {
		int result = WSARecv(m_socket, &wsabuf, 1, &bytesReceived, &flags, NULL, NULL);

		if (result == SOCKET_ERROR) {
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				currentTime = std::chrono::steady_clock::now();
				continue;
			}
			else {
				wprintf(L"WSARecv failed with error [error_code: %d]\n", GetLastError());
				return;
			}
		}

		if (bytesReceived > 0) {
			memcpy(&packet, &buffer, sizeof(T));
			return;
		}
	}
	wprintf(L"Receive timeout after 10 seconds.\n");
	return;
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
		send_packet(login_packet);
		
		S2C_LOGIN_RESULT_PACK login_result_packet{};
		recv_packet(login_result_packet);
		if (login_result_packet.size <= 0) continue;

		wstring result(login_result_packet.result);
		if (result.find(L"로그인 성공!") != wstring::npos) {
			id = user_id[target];
			break;
		}
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
	recv_packet(result_packet);

	if (result_packet.size <= 0) return;

	wstring result(result_packet.result);
	if (result.find(L"로그아웃 성공") != wstring::npos) {
		disconnect_to_server();
		return;
	}
	else return;
}
