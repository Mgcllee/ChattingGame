#pragma once

#include <tuple>
#include "ChatServer/public/OverlappedExpansion.h"

class IGrain {
public:
	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) = 0;
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) = 0;
};