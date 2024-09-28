#pragma once


#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <WinSock2.h>


// Q: why do we need this class ?
// A: this is the main class which holds all the resources,
// accept new clients and handle them.
class Communicator
{
public:
	Communicator();
	~Communicator();
	void serve();
	void startHandleRequests();

private:

	void bindAndListen();
	void handleNewClient(const SOCKET client_socket);
	SOCKET _m_serverSocket;
};

