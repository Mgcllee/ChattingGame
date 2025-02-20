#pragma once 

#include "Client.h"

vector<wstring> user_id;
vector<wstring> chat_sentences;
vector<Client> clients;

void read_files();


int main() {
	read_files();

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
	}
	wprintf(L"Complete all client connect to server and login success\n");

	while (true) {
		sf::sleep(sf::seconds(0.01f));
		for (int i = 0; i < MAX_CLIENT; ++i) {
			clients[i].communicate_server(i);
		}
	}

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].disconnect_to_server();
	}

	return 0;
}


void read_files() {
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
		wprintf(L"Can't read mixed_sentences file...\n");
		return;
	}

	readFile.open("valid_game_names_with_korean_words.txt");
	readFile.imbue(locale(readFile.getloc(), new codecvt_utf8<wchar_t>));
	if (readFile.is_open()) {
		wstring sentences;

		for (int i = 0; i < MAX_SENTENCE; ++i) {
			getline(readFile, sentences);
			user_id.push_back(sentences);
		}
		readFile.close();
	}
	else {
		wprintf(L"Can't read valid_game_names_with_korean_words file...\n");
		return;
	}
}