#pragma once

#include "stdafx.h"
#include "Client.h"

atomic<int> ticket_number;

unordered_map<int, Client> clients;

int main() {
	NetworkSetting network_setting;
	network_setting.create_iocp_handle();
	network_setting.accept_clients();

	ticket_number.store(0);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(process_packet, h_iocp);
	for (auto& th : worker_threads)
		th.join();

	return 0;
}