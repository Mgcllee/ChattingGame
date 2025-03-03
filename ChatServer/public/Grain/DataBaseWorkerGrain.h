#pragma once

#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/stdafx.h"

class DataBaseWorkerGrain : public IGrain
{
public:
	DataBaseWorkerGrain();
	virtual ~DataBaseWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};

