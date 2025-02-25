#pragma once

class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();

	void OnReceive(int nErrorCode);
};


