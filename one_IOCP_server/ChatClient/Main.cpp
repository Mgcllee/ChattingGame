#pragma once 

#include "Client.h"
#include <thread>
#include <windows.h>

vector<wstring> user_id;
vector<wstring> chat_sentences;
vector<Client> clients;

void read_files();
void run_clients_communication(int start, int end);


int main() {
	read_files();

	clients.emplace_back();
	for (int i = 1; i <= MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
	}
	wprintf(L"Complete all client connect to server and login success\n");

	vector<thread> threads;
	for (int i = 0; i < MAX_THREAD; ++i) {
		threads.emplace_back(run_clients_communication, 
			(MAX_CLIENT / MAX_THREAD) * i + 1,
			(MAX_CLIENT / MAX_THREAD) * (i + 1) + 1);
	}
	for (auto& th : threads) {
		th.join();
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

void run_clients_communication(int start, int end) {
	while (true) {
		for (int i = start; i < end; ++i) {
			clients[i].communicate_server(i);
		}
	}
}