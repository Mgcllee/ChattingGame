#pragma once

class ClientWorkerGrain
{
	// h_iocp_clients ��� ����
	std::atomic<int> ticket_number;
	std::unordered_map<int, Client> clients;
	// ======================

};