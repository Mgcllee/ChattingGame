#pragma once 

#define MAX_CLIENT 10
#define MAX_SENTENCE 100'000

#include "Client.h"

vector<wstring> chat_sentences;
vector<Client> clients;

int main() {
	locale::global(locale(""));

	wifstream readFile("mixed_sentences.txt");
	readFile.imbue(locale(readFile.getloc(), new codecvt_utf8<wchar_t>));
	if (readFile.is_open()) {
		wstring sentences;

		for (int i = 0; i < MAX_SENTENCE; ++i) {
			getline(readFile, sentences);
			chat_sentences.push_back(sentences);
		}
		readFile.close();
	}
	else {
		printf("Can't read txt file...\n");
		return 0;
	}

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

