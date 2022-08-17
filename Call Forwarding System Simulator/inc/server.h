#ifndef __SERVER_H_
#define __SERVER_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include <stdlib.h>

#define BLOCKSIZE 512		// Block Size
#define BUFFERSIZE 1024		// Max Buffer Size
#define SERV_PORT 57322		// Port Number
#define LISTENQ 10		// Maximum Number of Client Connections

#define DELIM ':'		// Delimiter for Tokenizing Client Data

#define SUCCESS 0           	// Returns 0 for Success
#define FAILURE 1 

using namespace std;


class Server {
	
	public:
		Server();	// Default Constructor
		~Server();	// Destructor
	
		// Creation of a Socket
		int OpenSocket();
	
		// Bind Data
		int BindData();
		
		// Listen for Connections
		int Listen();
		
		// Server gets a Socket for an Incoming Client Connection
		int Accept();
				
		// Read Data sent by Client
		int ReadData();

		// Send Data to Client over the Socket
		int SendData(char* data);
		
		// Tokenize Data sent by Client
		int TokenizeData(char* data);
		
		// Search if Mobile Number exists or not
		int SearchMobNum(string data, string mNum);
		
		// Check if Mobile Number is Registered or Not
		int IsNumberRegistered();
		
		// Check if Service is Activated for Mobile Number
		int IsServiceActivated();
		
		// To Process Activation Request for uncondtionalervice
		int ProcessUnconditionalActivationRequest();
		
		// To Process Activation Request for No reply Service
		int ProcessNoReplyActivationRequest(string Number);


		//To Process Activate Request for As Busy Service
		int ProcessAsBusyActivationRequest(string Number);
		
		// To Process Deactivation Request for Mobile Number
		int ProcessDeactivationRequest();
		
		// Process Update Service Request for Mobile Number
		int ProcessUpdateRequest();
		
		// Process Call to another Client
		int ProcessCallRequest(string callNum);
		
		// Write Data to File
		int WriteFile(char* data, char* fname);
	
	
	private:
	    
		int sd;		// Socket Descriptor
		int cd;		// Connection Descriptor
		int fd;     // File Descriptor
		string mobNumber;	// Mobile Number sent by Client
		char databuf[BUFFERSIZE];	// Store Data sent by Client
		vector<string> clientRecord;	// Store Each Client Record
		char clientIpAddress[BUFFERSIZE];   // Store Client IP Address 
		struct sockaddr_in serverAddress;	// Server Address
		struct sockaddr_in clientAddress;	// Client Address
		int client_struct_leno = sizeof(clientAddress);

	
};

void signal_handler(int sig);

#endif

