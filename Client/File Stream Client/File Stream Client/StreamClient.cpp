//This is a program that reads a file and sends it through a socket stream
#include "Client.h"

using namespace std;

int main(){
	string directory = "";
	string ip_addr = "127.0.0.1";
	int port = 666;
	Client object(directory, ip_addr, port);
	while (!cin.eof()){
		cout << "\n Connect to SERVER? (Y/N)?";
		string RESPONSE;
		cin >> RESPONSE;
		RESPONSE[0] = tolower(RESPONSE[0]);

		//Start Connection
		if (RESPONSE[0] == 'n'){
			cout << "\n QUITTING";
		}
		else{
			bool connected = object.connected();
			if (connected) cout << "Connected to server" << endl;
			else cout << "No Connection to server" << endl;
			connected = object.recieve();
			system("pause");
		}
	}
	return 0;
}