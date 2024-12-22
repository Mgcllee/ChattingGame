#include "NetworkSetting.h"

NetworkSetting::NetworkSetting()
{
	WSADATA WSAData;
	int error_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	int option;
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	error_code = setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	error_code = bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	error_code = listen(server_socket, SOMAXCONN);


	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
}

NetworkSetting::~NetworkSetting()
{

}


void NetworkSetting::create_iocp_handle() {
	accept_overlapped_expansion = new OverlappedExpansion();
	accept_overlapped_expansion->socket_type = SOCKET_TYPE::ACCEPT;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, (ULONG_PTR)accept_overlapped_expansion, 0);
}

void NetworkSetting::accept_clients()
{
	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);

	AcceptEx(server_socket, client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);
}
