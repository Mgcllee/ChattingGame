#pragma once

#include "IGrain.h"

class DataBaseWorkerGrain : public IGrain
{
	// h_iocp_database 담당으로 변경될 예정
	std::wofstream chat_log_file;
	xlnt::workbook wb;
	xlnt::worksheet ws;
	// =================================
};

