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

	u_long on = 1;
	if (ioctlsocket(m_socket, FIONBIO, &on) == INVALID_SOCKET) {
		wprintf(L"fail to non-blocking socket\n");
		return;
	}

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
		connect_to_server(SERVER_ADDR, PORT_NUM, key);
		return;
	}
	else if (id.empty()) {
		login_server();
		return;
	}
	
	wprintf(L"%d\t", key);
	// TODO: Cleanup job_type
	send_chatting();
	return;

	BASIC_PACK recv_pack{};
	recv_packet(recv_pack);
	if (recv_pack.type == S2C_PACKET_TYPE::RESPONSE_EXIST_CLIENTS) {

	}

	


	int job_type = distr_job(eng);
	switch (job_type) {
	case JOB_TYPE::SEND_CHAT: {
		send_chatting();
		break;
	}
	case JOB_TYPE::USER_LOGOUT: {
		request_logout();
		break;
	}
	}
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

template<typename T>
void Client::recv_packet(T& packet)
{
	if (m_socket == NULL)
		return;

	DWORD bytesReceived = 0;
	WSABUF wsabuf;
	wchar_t* buffer = new wchar_t[MAX_PACKET_SIZE];
	int result;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_socket, &readfds);

	struct timeval timeout;
	// 타임아웃 설정 (5초)
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	// select 호출
	result = select(0, &readfds, NULL, NULL, &timeout);
	if (result == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
		return;
	}
	else if (result == 0) {
		printf("Timeout occurred, no data received.\n");
		return; // 타임아웃 발생 시 계속 루프
	}

	// 데이터 수신
	if (FD_ISSET(m_socket, &readfds)) {
		wsabuf.len = MAX_PACKET_SIZE;
		wsabuf.buf = reinterpret_cast<char*>(buffer);

		result = WSARecv(m_socket, &wsabuf, 1, &bytesReceived, NULL, NULL, NULL);
		if (result == SOCKET_ERROR) {
			printf("WSARecv() failed: %d\n", WSAGetLastError());
			return;
		}

		// 수신된 데이터 처리
		if (bytesReceived > 0) {
			memcpy(&packet, buffer, sizeof(T));
		}
		else {
			printf("Connection closed by the server.\n");
		}
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
		Sleep(10);
		wcscpy_s(login_packet.id, user_id[target].c_str());
		wcscpy_s(login_packet.pw, user_id[target].c_str());

		send_packet(login_packet);

		if (process_login_result()) {
			id = user_id[target];
			pw = user_id[target];
			break;
		}
	}
}

bool Client::process_login_result() {
	S2C_LOGIN_RESULT_PACK packet{};
	
	recv_packet(packet);
	// recv(m_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	if (packet.size <= 0) return false;

	wstring result(packet.result);
	if(result.find(L"로그인 성공!") != wstring::npos) {
		return true;
	}
	else return false;
}

void Client::send_chatting() {
	int target = distr_chat(eng);

	C2S_SEND_CHAT_PACK packet;
	packet.size = static_cast<short>(sizeof(packet));
	packet.type = C2S_PACKET_TYPE::SEND_CHAT_PACK;
	wcscpy_s(packet.str, chat_sentences[target].c_str());

	wprintf(L"%s: %s\n", id.c_str(), packet.str);

	send_packet(packet);

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
	// recv_packet(result_packet);
	recv(m_socket, reinterpret_cast<char*>(&result_packet), sizeof(result_packet), 0);


	if (result_packet.size <= 0) return;

	wstring result(result_packet.result);
	if (result.find(L"로그아웃 성공") != wstring::npos) {
		disconnect_to_server();
		return;
	}
	else return;
}
