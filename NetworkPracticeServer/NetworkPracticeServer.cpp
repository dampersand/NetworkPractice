// NetworkPracticeServer.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#define DEFAULT_PORT "28828"
#define DEFAULT_BUFLEN 512

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsadata;
	int iResult;

	iResult= WSAStartup(MAKEWORD(2,2), &wsadata); // Start Winsock

	if (iResult != 0)
	{
		cout << "Couldn't start winsock." << endl;
		return 1;
	}

	addrinfo *result = NULL, hints; //set some socket address structures
	ZeroMemory(&hints, sizeof(hints)); //clear hints
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; //No idea.  Documentation is prettttty fuzzy.

	iResult = getaddrinfo("0.0.0.0", DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		cout << "Couldn't get LOCAL address info." << endl;
		WSACleanup(); //clean winsock
		return 1;
	}

	SOCKET serverSocket = INVALID_SOCKET;

	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); //set the serverSocket to all the info we got from getaddrinfo.

	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Couldn't create socket from local address info" << endl;
		cout << WSAGetLastError();
		WSACleanup();
		return 1;
	}

	//All socket info gathered
	//////////////////////////////////////////////////////////
	//Begin binding socket

	iResult = bind(serverSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Couldn't bind to socket." << endl << WSAGetLastError();
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result); //Socket is bound, don't need its information anymore.  This clears that addrinfo.

	//Socket Bound
	//////////////////////////////////////////////////////////
	//Listening on socket
	
	cout << "Now listening on socket." << endl;

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) //puts severSocket in a state of 'listen,' where it creates a backlog of attempted connections.
	{
		cout << "Problem listening on the socket." << endl << WSAGetLastError();
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket = INVALID_SOCKET; //dummysocket

	clientSocket = accept(serverSocket, NULL, NULL); //deal with the first attempted connection.
	if (clientSocket == INVALID_SOCKET) 
	{
		cout << "Accept failed." << endl << WSAGetLastError();
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	closesocket(serverSocket);
	//Once call is accepted, continue.
	////////////////////////////////////////////
	//Do shit.

	char recvbuf[DEFAULT_BUFLEN]; //the receive buffer
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN; //length of the receive buffer

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0); //receive information from the client's buffer, stick it in the recvbuf
		if (iResult > 0)
			{
				cout << "I have received this many bytes: " << iResult << endl << endl;
				recvbuf[iResult] = 0;
				cout << recvbuf << endl << endl;

				// Echo the buffer back to the sender
				iSendResult = send(clientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR)
				{
					cout << "Socket problem during connection!" << endl << WSAGetLastError();
					closesocket(clientSocket);
					WSACleanup();
					return 1;
				}
			
				cout << "Buffer returned." << endl;
			}
		else if (iResult == 0)
			cout << "Connection closing...\n";
		else 
		{
			cout << "Did not receive data..." << endl << WSAGetLastError();
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	//End of data reception/send
	/////////////////////////////////////////////////////
	//Shutdown server

	iResult = shutdown(clientSocket, SD_BOTH); //shutdown the socket - I could shut down send or receive, but this shuts down both.
	if (iResult == SOCKET_ERROR)
	{
		cout << "Trouble shutting down..." << endl << WSAGetLastError();
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}

