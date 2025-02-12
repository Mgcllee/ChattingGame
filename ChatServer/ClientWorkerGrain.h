#pragma once

class ClientWorkerGrain
{
	// h_iocp_clients 담당 예정
	std::atomic<int> ticket_number;
	std::unordered_map<int, Client> clients;
	// ======================

};