#pragma once

#include "IGrain.h"

class DataBaseWorkerGrain : public IGrain
{
	// h_iocp_database ������� ����� ����
	std::wofstream chat_log_file;
	xlnt::workbook wb;
	xlnt::worksheet ws;
	// =================================
};

