#pragma once

#include "IGrain.h"

class DataBaseWorkerGrain : public IGrain
{
public:
	DataBaseWorkerGrain();
	~DataBaseWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	std::wofstream chat_log_file;
	xlnt::workbook wb;
	xlnt::worksheet ws;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};

