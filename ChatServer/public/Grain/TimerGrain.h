#pragma once

#include "ChatServer/public/Interface/IGrain.h"


class TimerGrain : public IGrain
{
public:
	TimerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};

