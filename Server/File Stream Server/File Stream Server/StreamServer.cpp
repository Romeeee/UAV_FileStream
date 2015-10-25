//This is a program that reads a file and sends it through a socket stream
#include "Server.h"
#include <sstream>
#include <string>

#define BufferSize 1500

using namespace std;

int main(){

	string directory = "C:/";
	string ip_addr = "127.0.0.1";
	int port = 666;
	
	cout << "Enter File Name: ";
	string name;
	cin >> name;
	stringstream temp;
	temp << name;
	Server object(directory, ip_addr, port);
	bool sent = false;
	do{
		sent = object.transmit(name);
	} while (!sent);
	system("pause");
}