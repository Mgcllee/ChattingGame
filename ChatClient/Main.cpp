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

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
	}
	wprintf(L"Complete all client connect to server and login success\n");

	vector<thread> threads;
	for (int i = 0; i < MAX_THREAD; ++i) {
		threads.emplace_back(run_clients_communication, 
			MAX_CLIENT / MAX_THREAD * i + 1, 
			MAX_CLIENT / MAX_THREAD * (i + 1) + 1);
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
	wifstream readFile(L"mixed_sentences.txt");
	if (readFile) {
		wstring sentence;
		while (getline(readFile, sentence)) {
			chat_sentences.emplace_back(sentence);
		}
		readFile.close();
	}
	else {
		wprintf(L"Can't read mixed_sentences file...\n");
		return;
	}

	readFile.open("valid_game_names_with_korean_words.txt");
	if (readFile) {
		wstring sentences;
		while (getline(readFile, sentences)) {
			user_id.emplace_back(sentences);
		}
		readFile.close();
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