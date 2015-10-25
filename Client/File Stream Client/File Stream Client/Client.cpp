#include "Client.h"

using namespace std;

Client::Client(string _directory, string _ip_addr, int _port){
	directory = _directory;
	const char *temp = _ip_addr.c_str();
	ip_addr = _ip_addr;
	port = _port;

	DLLVERSION = MAKEWORD(2, 1);

	//Start Winsock
	SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	//Initialize Connection Socket
	

	sock = socket(AF_INET, SOCK_STREAM, NULL); // socket(USE INTERNET DOMAIN, TYPE SOCKET STREAM (vs UDP - DATAGRAM), NULL)

	//const char *temp = _ip_addr.c_str();
	ADDRESS.sin_addr.s_addr = inet_addr(temp);
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(port); //htons converts port number to number the function can understand. htons(PORT#);
}

Client::~Client(){
	delete ack;
	closesocket(sock);
}

//Need to figure out how to tell difference between checking if client is online and actual file transfer
bool Client::connected(){
	int status = connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	ack[0] = 'T';
	SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);
	shutdown(sock, SD_BOTH);															//Shutdown connection
	closesocket(sock);																	//Close socket
	sock = socket(AF_INET, SOCK_STREAM, NULL);
	if (status == 0){
		return true;
	}
	else
		return false;
}

bool Client::recieve(){																	//Will change output to output different error codes later
	ofstream ImageRecieved;																//Output file stream
	int thing = connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));					//Initiate connection with the server
	ack[0] = 'S';
	SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);
	cout << "Connect Status: " << thing << endl;										//Anything other than 0 is an error
	if (thing != 0){	
		cout << "Error: could not connect to server" << endl;
		return false;
	}

	//int filesize;																		//Use to determine how many bytes to write to the file
	unsigned long long sizefile;
	int namesize;																		//Use to determine how many bytes to use for the name


	SUCCESSFUL = recv(sock, (char *)&namesize, sizeof(int), NULL);						//Recieve size of name - test.exe is 8 characters 
	SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);
	SUCCESSFUL = recv(sock, MESSAGE, BufferSize, NULL);									//Recieve name
	//SUCCESSFUL = recv(sock, (char *)&filesize, sizeof(int), NULL);						//Recieve size of file
	SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);
	SUCCESSFUL = recv(sock, (char *)&sizefile, sizeof(unsigned long long), NULL);						//Recieve size of file
	SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);
	int filesize = (int)sizefile;
	if (SUCCESSFUL < 0){																//Check again for connection: -1 is error or 0 if end of stream
		cout << "Error: could not connect to server" << endl;
		return false;
	}

	stringstream name;																	//Get name from buffer
	for (int i = 0; i < namesize; i++){
		name << MESSAGE[i];
	}
	string file;																		//Create a string: file
	name >> file;																		//Set file = "name"

	cout << "size of name: " << namesize << endl;										//Output stuff...
	cout << "size of file " << (int)filesize << endl;										//...............
	cout << "filename : " << file << endl;												//...............

	ImageRecieved.open(directory + file, ios::binary);									//Open file given directory and file name
	int bytes_remaining = filesize;														
	cout << "Downloading ... " << endl;
	int count = 0;
	do{
		SUCCESSFUL = recv(sock, MESSAGE, BufferSize, NULL);								//Recieve data in chuncks of "BufferSize" bytes
		//If connection has be closed, SUCCESSFUL will return 0
		//If error, SUCCESSFUL will return -1
		//If neither, SUCCESSFUL will return # of bytes recieved
		if (bytes_remaining < BufferSize){												//Everything else after the file size is jibberish, want to ignore those 
			ImageRecieved.write(MESSAGE, bytes_remaining);								//Only write what was saved from the server
		}
		else{
			ImageRecieved.write(MESSAGE, BufferSize);									//Write the entire buffer to the file
		}
		if (count % (int)(filesize/BufferSize/30) == 0){
			cout << string(10, '\b');
			cout << fixed <<setw(5) << setprecision(2) << (float)(filesize - bytes_remaining) / (float)filesize * 100 << "%";
		}
		bytes_remaining -= BufferSize;													
		SUCCESSFUL = send(sock, ack, sizeof(ack), NULL);								//Send acknowledgement packet to server
		count++;
	} while (SUCCESSFUL > 0);
	cout << string(7, '\b');
	cout << "100.00%" << endl;
	shutdown(sock, SD_BOTH);															//Shutdown connection
	closesocket(sock);																	//Close socket
	ImageRecieved.close();

	sock = socket(AF_INET, SOCK_STREAM, NULL);											//Reopen Socket
	//THIS SEEMS TO WORK TO ALLOW MULTIPLE DOWNLOADS AFTER CLOSE...NOT SURE WHY

	return true;																		//Return true if file successfuly sent
}

string Client::getIP(){
	return ip_addr;
}