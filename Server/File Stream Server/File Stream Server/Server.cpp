#include "Server.h"

using namespace std;

Server::Server(string _directory, string _ip_addr, int _port){
	directory = _directory;
	const char *temp = _ip_addr.c_str();
	ip_addr = _ip_addr;
	port = _port;

	DLLVERSION = MAKEWORD(2, 1);

	//Start Winsock
	SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	//Create Socket Structure
	AddressSize = sizeof(ADDRESS);
	
	//Initialize Connection Socket
	sock_CONNECTION = socket(AF_INET, SOCK_STREAM, NULL); // socket(USE INTERNET DOMAIN, TYPE SOCKET STREAM (vs UDP - DATAGRAM), NULL)
	ADDRESS.sin_addr.s_addr = inet_addr(temp); //Change IP according to server ip???
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(port); //htons converts port number to number the function can understand. htons(PORT#);

	//Initialize Listening Socket
	//sock_LISTEN = socket(AF_INET, SOCK_STREAM, NULL);

	//Initialize Listening Socket
	sock_LISTEN = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sock_LISTEN, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	listen(sock_LISTEN, SOMAXCONN);

}

Server::~Server(){
	closesocket(sock_CONNECTION);
}

//Need to figure out how to tell difference between checking if client is online and actual file transfer
/*bool Server::connected(){
	int status = connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	closesocket(SUCCESSFUL);
	if (status == 0){
		return true;
	}
	else
		return false;
}*/

bool Server::transmit(string filename){						//Will change output to output different error codes later

	ifstream StreamImage(directory + filename, ios::in | ios::binary);					//Open file to stream 
	StreamImage.seekg(0, StreamImage.end);	//Moves seekg to the end of the stream
	int ImageLength = StreamImage.tellg();	//Sets the length of the character stream to the position of the last character in the stream (Determines string length)
	StreamImage.seekg(0, StreamImage.beg);	//Moves seekg back to the beginning of the stream

	stringstream temp;
	temp << filename;

	cout << "Size of file: " << ImageLength << " bytes" << endl;

	if (ImageLength <= 0){																		//If the file size is < 0, the file does not exist 
		cout << "Error: file does not exist" << endl;
		return false;
	}


	//Send file or connection status here
	bool sent = false;

	cout << "Waiting for incoming connection..." << endl;
	do{
		if (sock_CONNECTION = accept(sock_LISTEN, (SOCKADDR*)&ADDRESS, &AddressSize)){				//Wait until the client has accepted our connection
			//See if can interrupt when needed^^^!!!!!!!!!!!!!!!!!!!!
			if (sock_CONNECTION == INVALID_SOCKET){		//Check if send is fail
				cout << "Error: INVALID IP ADDRESS" << endl; //Maybe?
				return false; //Maybe?
			}
			SUCCESSFUL = recv(sock_CONNECTION, ack, sizeof(ack), NULL);
			cout << "Recieved request: ";
			if (ack[0] == 'T'){							//Test connection
				cout << "Test Connection" << endl;
				resetConnection();						//Reset the Connection
			}
			else if (ack[0] == 'S'){					//Send file
				cout << "Send File" << endl;
				sent = send_file(filename, ImageLength, temp, StreamImage);
				if (sent == false){
					return false;
				}
			}
		}
	} while (ack[0] == 'T');	//If testing connection, go back to trying to send. 
	return true; //Need to determine how to quit after some time, or know when to give up on sending
}


bool Server::send_file(string filename, int ImageLength, stringstream & temp, ifstream & StreamImage){

	int remaining_bytes = 0;																//Integer to show how many bytes are left to send

	temp.seekg(0, temp.end);																//---
	int info_length = temp.tellg();															//--->	Determine length of file name (Probably easier way of doing this)
	temp.seekg(0, temp.beg);																//---

	temp.read(buffer, info_length);															//Read file information into the buffer
	unsigned long long imagesize = (unsigned long long)ImageLength;
	SUCCESSFUL = send(sock_CONNECTION, (char *)&info_length, sizeof(int), NULL);			//Send size of file name
	SUCCESSFUL = recv(sock_CONNECTION, ack, sizeof(ack), NULL);
	SUCCESSFUL = send(sock_CONNECTION, buffer, info_length, NULL);							//Send file name
	SUCCESSFUL = recv(sock_CONNECTION, ack, sizeof(ack), NULL);
	SUCCESSFUL = send(sock_CONNECTION, (char *)&imagesize, sizeof(unsigned long long), NULL);			//Send size of file
	SUCCESSFUL = recv(sock_CONNECTION, ack, sizeof(ack), NULL);
	StreamImage.read(buffer, BufferSize);													//Reads buffer into StreamImage: (file, # of characters to read) 

	cout << "Sending Files..." << endl;
	remaining_bytes = ImageLength;
	while (remaining_bytes > 0){
		SUCCESSFUL = send(sock_CONNECTION, buffer, BufferSize, NULL);		//Send packets to the client, BufferSize determines how many bytes are sent
		remaining_bytes -= BufferSize;										//How many bytes are left
		StreamImage.read(buffer, BufferSize);								//Read new data intothe buffer
		SUCCESSFUL = recv(sock_CONNECTION, ack, sizeof(ack), NULL);			//Receive acknowledgment that the client has recieved data
		if (SUCCESSFUL == SOCKET_ERROR){
			cout << "Error: Socket Connection Terminated" << endl;
			resetConnection();												//Reset the connection
			return false;
		}
	}

	cout << "Done" << endl;
	resetConnection();

	return true;
}
void Server::resetConnection(){
	shutdown(sock_CONNECTION, SD_BOTH);						//Shutdown connection to the client: Let it know we're done sending
	closesocket(sock_CONNECTION);							//Close socket
	sock_CONNECTION = socket(AF_INET, SOCK_STREAM, NULL);	//Re-open socket for next file to send
}

string Server::getIP(){
	return ip_addr;
}