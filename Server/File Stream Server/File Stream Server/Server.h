#ifndef SERVER_H
#define SERVER_H

#pragma comment (lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define SCK_VERSION2 0x0202

#define BufferSize 1500

using namespace std;

class Server{
private:
	string directory;
	string ip_addr;
	int port;

	long SUCCESSFUL;
	WSAData WinSockData;
	WORD DLLVERSION;

	SOCKET sock_LISTEN;
	SOCKET sock_CONNECTION;
	SOCKADDR_IN ADDRESS;
	int AddressSize;
	
	char* buffer = new char[BufferSize];
	char* ack = new char[1];

	bool send_file(string filename, int ImageLength, stringstream & temp, ifstream & StreamImage);			//Sends file to server

public:
	Server(string _directory, string _ip_addr, int _port);			//Constructor sets up the Directory, IP_Address, and Port # of the Server
	~Server();
	bool transmit(string filename);									//Opens socket and listens for connection ("Send File" or "Test Connection")
	void resetConnection();											//Closes socket and reopens it for later sending
	//bool connected();												//Returns TRUE if connection to server is established - MAYBE USE??
	string getIP();													//Returns the client IP Address
};



#endif //CLIENT_H