#pragma once
class JobWorker
{
};

void process_packet(HANDLE h_iocp) {
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	printf("Running Server...\n");

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &overlapped, INFINITY);
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (exoverlapped == nullptr) continue;
		if (FALSE == GQCS_result) {
			if (exoverlapped->socket_type == ACCEPT) {
				printf("Accept error\n");
			}
			else {
				int player_ticket = static_cast<int>(key);
				string error_message = "GQCS Error on client[";
				error_message += to_string(player_ticket);
				error_message += "]";
				printf("%s\n", error_message.c_str());
			}
			continue;
		}

		switch (exoverlapped->socket_type) {
		case SOCKET_TYPE::ACCEPT: {

			// printf("new clinet accept\n");

			int ticket = ticket_number.load();
			ticket_number.fetch_add(1);
			clients[ticket] = Client(client_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, ticket, 0);
			clients[ticket].recv_packet();

			ZeroMemory(&accept_overlapped_expansion->overlapped,
				sizeof(accept_overlapped_expansion->overlapped));
			client_socket =
				WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server_socket, client_socket,
				accept_overlapped_expansion->packet_buffer, 0, addr_size + 16, addr_size + 16,
				0, &accept_overlapped_expansion->overlapped);
			break;
		}
		case SOCKET_TYPE::RECV: {
			int ticket = static_cast<int>(key);
			int remain_data = num_bytes + clients[ticket].remain_packet_size;

			char* p = exoverlapped->packet_buffer;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {

					if (ticket % 374 == 0) {
						PACKET* chat_packet = reinterpret_cast<PACKET*>(p);
						printf("[%d]: %s\n", ticket, chat_packet->content);
					}

					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else {
					break;
				}
			}

			clients[ticket].remain_packet_size = remain_data;
			if (remain_data > 0) {
				memcpy(exoverlapped->packet_buffer, p, remain_data);
			}
			clients[ticket].recv_packet();
			break;
		}
		case SOCKET_TYPE::SEND: {

			break;
		}
		default: {
			break;
		}
		}
	}
}