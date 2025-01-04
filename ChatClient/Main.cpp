#pragma once 

#define MAX_CLIENT 50'000

#include "Client.h"

vector<Client> clients;

int main() {
	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
	}
	printf("Complete all client connect to server\n");

	while (true) {
		sf::sleep(sf::seconds(1.f));
		for (int i = 0; i < MAX_CLIENT; ++i) {
			clients[i].communicate_server(i);
		}
	}

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].disconnect_to_server();
	}
	return 0;
}

