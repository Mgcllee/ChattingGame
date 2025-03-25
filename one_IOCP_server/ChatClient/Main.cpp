#pragma once 

#include "Client.h"
#include <thread>
#include <windows.h>

vector<wstring> user_id;
vector<wstring> chat_sentences;
vector<Client> clients;

void read_files();

int main() {
	read_files();

	clients.reserve(MAX_CLIENT);
	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, 0);
		clients[i].login_server();
	}
	wprintf(L"Complete all client connect to server\n");

	while (true) {
		for (int i = 0; i < MAX_CLIENT; ++i) {
			Sleep(500);
			clients[i].communicate_server(i);
		}
	}

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].disconnect_to_server();
	}
	return 0;
}

void read_files() {
	locale::global(locale(".UTF-8"));
	wifstream sentence_file(L"mixed_sentences.txt");
	if (sentence_file) {
		wstring sentence;
		while (getline(sentence_file, sentence)) {
			if (sentence.empty()) break;
			chat_sentences.emplace_back(sentence);
		}
		sentence_file.close();
	}
	else {
		wprintf(L"Can't read mixed_sentences file...\n");
		return;
	}

	wifstream name_file(L"valid_game_names_with_korean_words.txt");
	if (name_file) {
		wstring sentence;
		while (getline(name_file, sentence)) {
			if (sentence.empty()) break;
			user_id.emplace_back(sentence);
		}
		name_file.close();
	}
	else {
		wprintf(L"Can't read valid_game_names_with_korean_words file...\n");
		return;
	}
}
