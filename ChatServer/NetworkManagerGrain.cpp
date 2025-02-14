#include "NetworkManagerGrain.h"

NetworkManagerGrain::NetworkManagerGrain(std::string accpet_addr, HANDLE& h_iocp_network)
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

	accept_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	option = TRUE;
	setsockopt(accept_client_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	accept_overlapped_expansion = new OverlappedExpansion();
	accept_overlapped_expansion->socket_type = SOCKET_TYPE::ACCEPT;

	h_iocp_network = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp_network, (ULONG_PTR)accept_overlapped_expansion, 0);

	SOCKADDR_IN client_addr;
	int addr_size = sizeof(client_addr);
	AcceptEx(server_socket, accept_client_socket,
		accept_overlapped_expansion->packet_buffer, 0, addr_size + 16,
		addr_size + 16, 0, &accept_overlapped_expansion->overlapped);
}

std::tuple<SOCKET&, SOCKET&, OverlappedExpansion*> NetworkManagerGrain::get_socket_variable()
{
    return std::tuple<SOCKET&, SOCKET&, OverlappedExpansion*>
        (server_socket, accept_client_socket, accept_overlapped_expansion);
}

void NetworkManagerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{

}

bool NetworkManagerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	return false;
}
