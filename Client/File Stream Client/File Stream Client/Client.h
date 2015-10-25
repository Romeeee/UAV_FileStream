#ifndef CLIENT_H
#define CLINET_H

#pragma comment (lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>

#define SCK_VERSION2 0x0202
#define BufferSize 1500

using namespace std;

class Client{
private:
	string directory;
	string ip_addr;
	int port;

	long SUCCESSFUL;
	WSAData WinSockData;
	WORD DLLVERSION;
	
	SOCKET sock;
	SOCKADDR_IN ADDRESS;

	char MESSAGE[BufferSize]; 
	char* ack = new char[1];

public:
	Client(string _directory, string _ip_addr, int _port);	//Constructor sets up the Directory, IP_Address, and Port # of the Server
	~Client();
	bool recieve();											//Recieves files from the server
	bool connected();										//Returns TRUE if connection to server is established - MAYBE USE??
	string getIP();
};



#endif //CLIENT_H