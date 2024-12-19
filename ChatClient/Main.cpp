#pragma once 

#define MAX_CLIENT 1

#include "Client.h"

vector<Client> clients;

int main() {
	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
	}

	// communicate server
	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].communicate_server();
	}

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].disconnect_to_server();
	}
	return 0;
}

