// NetworkPractice.cpp : Defines the entry point for the console application.
//

/*
Hello, future Dan.  Are you looking at using this program to help you remember how to network?
Well, I'm here to warn you about something really quick:
You probably want to use winsock2.h in your code... and if you're using windows.h in your code too,
You're probably getting an error.  That's 'cause windows.h includes the old Winsock.h, which conflicts
with winsock2.h.  Google "win32_lean_and_mean," it's a macro that should fix your problem.
*/

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>

#define DEFAULT_BUFFER_SIZE 512
#define DEFAULT_PORT "28828"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsadata; //declare an empty WSADATA (a winsock specific structure).
	int iResult; //int that serves as our error message-runner.
	PCSTR argv;
	string IPin;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) //checks for winsock 2.2 or lower.  Essentially, this will turn "on" certain parts of winsock.
	{
		cout << "WinSock done fucked up at WSAStartup." << endl; // no problems should return 0.  For a list of problems, see MSDN::WSAstartup.
		return 1;
	}

	addrinfo *result = NULL, *ptr = NULL, hints; //declare a couple pointers and an addrinfo class - winsock specific structure

	ZeroMemory(&hints, sizeof(hints)); //windows macro, fills hints up with 0s.
	hints.ai_family = AF_UNSPEC; //set hints to be either ipv4 or ipv6.
	hints.ai_socktype = SOCK_STREAM; //set the hints socket to be streaming (that's TCP, dummy)
	hints.ai_protocol = IPPROTO_TCP; //tells the socket to use TCP.

	cout << "Kay, we all here.  Where do you wanna go?\n";
	cin.sync();
	getline(cin, IPin);
	argv = IPin.c_str();


	iResult = getaddrinfo(argv, DEFAULT_PORT, &hints, &result); //when you call getaddrinfo, you call it with an IP address (argv[1] here), port number, hints, and result.  The hints tell getaddrinfo what sort of port it should expect to find; the result is a pointer to whatever info you got from the IP:port.

	if (iResult != 0)
	{
		cout << "NAAAAAAAAAH, G! getaddrinfo done fucked up." << endl;
		WSACleanup(); //Closes winsock.
		return 1;
	}


	SOCKET connectMe = INVALID_SOCKET; //bitch, I don't know what this is.  There's no documentation anywhere on the socket object, just the socket function.

	//for(ptr=result; ptr != NULL; ptr=ptr->ai_next) //give ptr all the info that getaddrinfo found.  Check each address for a connection.
	//	{
	ptr = result;
		connectMe = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol); //call the socket function to turn connectMe into a socket with whatever family, socket type, and protocol we got from getaddrinfo.

		if (connectMe == INVALID_SOCKET)
		{
			cout << "Nope.  Couldn't connect this bitch, the socket function gave up." << endl;
			cout << "The last error that winsock saw was" << WSAGetLastError;
			WSACleanup();
			return 1;
		}

		//Socket info gathered
		//////////////////////////////////////////
		//Connect to dis bitch
		iResult = connect(connectMe,ptr->ai_addr,ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			cout << WSAGetLastError;
			closesocket(connectMe);
			connectMe = INVALID_SOCKET;
		}
	//}

	freeaddrinfo(result);

	if (connectMe == INVALID_SOCKET)
	{
		cout << "Could not connect..." << endl << WSAGetLastError();
		WSACleanup();
		return 1;
	}

	char *sendbfr;
	int bfrsize = DEFAULT_BUFFER_SIZE;
	int echoBfr;
	string message;

	do
	{
		cout << "Type what you wanna send." << endl << ">";
		cin.sync();
		getline(cin, message);
		cout << endl << endl;
		sendbfr = (char*)message.c_str();

		iResult = send(connectMe, sendbfr, message.size(), NULL);

		if (iResult > 0)
		{
			cout << "Sent bytes: " << message.size() << endl;

			echoBfr = recv(connectMe, sendbfr, bfrsize, NULL);
			if (echoBfr > 0)
				cout << "Received bytes as echo: " << echoBfr << endl;
			if (echoBfr == SOCKET_ERROR)
			{
				cout << "Server sent back an error..." << endl << WSAGetLastError();
				closesocket(connectMe);
				WSACleanup();
				return 1;
			}

		}
		else if (iResult == 0)
		{
			cout << "All information sent!" << endl;
		}
		else if (iResult == SOCKET_ERROR)
		{
			cout << "Some socket error, connection will close." << endl << WSAGetLastError();
			closesocket(connectMe);
			WSACleanup();
			return 1;
		}
		else
		{
			cout << "No data..." << endl << WSAGetLastError();
			closesocket(connectMe);
			WSACleanup();
			return 1;
		}
	}while (iResult > 0);

	iResult = shutdown(connectMe, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown Error..." << endl << WSAGetLastError();
		closesocket(connectMe);
		WSACleanup();
		return 1;
	}

	closesocket(connectMe);
	WSACleanup();

	return 0;

}

