#include <server.h>
#include <User.h>

#define PORT 4056

void signal_handler(int sig) 
{ 
    cout << "Ctrl+C Sent: " << endl << "Server EXIT..."<<endl;  
    exit(EXIT_FAILURE);
}

Server::Server() {
    
    	fstream logfile;
	logfile.open("/home/veeru/project/dat/serverlog.txt", ios::out|ios::app);

	sd = 0;
	cd = 0;
	fd = 0;
	mobNumber = "";
	databuf[0] = '\0';
	clientIpAddress[0] = '\0';
	
	// Populating Own address structure to bind it to the Socket
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4056);	// PORT NO
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP ADDRESS

	
}

int Server::OpenSocket() {

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {

		perror("SOCKET OPEN FAILURE");
        	exit(0);
	}
	
	    return SUCCESS;
}

int Server::BindData() {

    int status = 0;
	
	// Now Binding
	status = bind(sd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (status < 0) {
		perror("BIND FAILURE");
		close(sd);
        	exit(0);
	}

	return SUCCESS;
}

int Server::Listen() {

    int status = 0;
	
	cout << "Server Running... I am waiting for Connections..." << endl;
	
	// Listen
	status = listen(sd, LISTENQ);
	if (status < 0) {
		perror("LISTEN FAILURE");
		close(sd);
        exit(0);
	}
	
	// Now waiting for Accepting Connection Request
	Accept();
	
	return SUCCESS;
}

int Server::Accept() {

	memset(&clientAddress, 0, sizeof(clientAddress));
	
	socklen_t clientLength = sizeof(clientAddress);

	// Accept Connection
	cd = accept(sd, (struct sockaddr*) &clientAddress, &clientLength);
	if (cd < 0) {
		perror("ACCEPT FAILURE");
		close(sd);
        exit(0);
	}
	
	// Printing the IP Address of the Client
	cout << endl << "Request from - Client IP Address: " << inet_ntop(AF_INET, &clientAddress.sin_addr, clientIpAddress, sizeof(clientIpAddress)) << endl;	
	
	// Read Data sent by Client
	ReadData();

	return SUCCESS;
}

int Server::ReadData() {

    int count = 0;
	
	while(1) {

		// Receive Data from Client
		count = recv(cd, databuf, BUFFERSIZE, 0);
		if (count <= 0) {
			perror("DATA NOT RECEIVED FROM CLIENT");
			close(cd);
			close(sd);
            exit(0);
		}
	
		databuf[count] = '\0';
	
		// cout << "Data Received from Client: " << databuf << endl;
		cout << databuf << endl;
	
		if (strcmp(databuf, "Exit") == 0) {
			break;
		}
		
		// Tokenize Data sent by Client
		TokenizeData(databuf);		
	}
	
	// Listen for New Connections
	Listen();
	
	return SUCCESS;
}

int Server::SendData(char* data) {

    int count = 0, len = 0;
	len = strlen(data);

	count = send(cd, data, len, 0);
	if (count < 0) {
		perror("DATA NOT SENT TO CLIENT");
		close(cd);
		close(sd);
	    exit(0);
	}
	
	// cout << "Data: '" << data << "' is sent to Client successfully!" << endl << endl;
	return SUCCESS;
}	

int Server::TokenizeData(char* data) {	

	string str = data;
	
	// Split the string into tokens
	vector<string> tokens;
	
	stringstream ss(str);
	string intermediate;
	
	while(getline(ss, intermediate, DELIM)) {
		tokens.push_back(intermediate);
	}
	
	// Store Case (tokens[1])
	int choice = atoi(tokens[1].c_str());
	
	// Store Mobile Number
	mobNumber = tokens[0];
	
	switch(choice) {

		case 1	: 	// Check if Mobile Number is Registered
					IsNumberRegistered();
					break;
					
		case 2	:	// Check if Service is Activated for Mobile Number
					IsServiceActivated();
					break;
					
		case 3	:	// Process Activation Request for Uncondtional call forward Service
					ProcessUnconditionalActivationRequest();
					break;

		case 4	:	// Process Activation Request for No-Reply Call forward Service 
					ProcessNoReplyActivationRequest(tokens[2]);
					break;
					
		case 5	:	// Process Deactivation Request for Mobile Number
					ProcessDeactivationRequest();
					break;
					
		case 6	:	// Process Update Service Request for Mobile Number
					ProcessUpdateRequest();
					break;
					
		case 7	:	// Process Call to another Client
					ProcessCallRequest(tokens[2]);
					break;

		case 8	:	//Process Activation Request for As Busy Call forward service
					ProcessAsBusyActivationRequest(tokens[20]);
					break;

	
		default	:	cout << "Invalid Choice!" << endl;
					break;				
	}

    return SUCCESS;
}

int Server::IsNumberRegistered() {

    int status = 0, isRegistered = 1;
	string msg;
	char buffer[BUFFERSIZE];
    string file ="/home/veeru/project/dat/RegisteredClients.txt";
    const char* clientFile = file.c_str();
    
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAIL!") ;
        exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
        exit(0);
	}	

	stringstream ss(buffer);
	string mnumber;
	
	// Compare Mobile Number with each Number in File
	while (getline(ss, mnumber)) {
		
		if(mobNumber == mnumber) {
			isRegistered = 0;
		}
	}
	
	if (isRegistered == 0) {
		
		// If Number is Registered
		msg = "Mobile Number is already Registered!";
		SendData(const_cast<char*>(msg.c_str()));
	
	} else if (isRegistered == 1) {

		mobNumber.append("\n");
	
		// If Number is Not Registered
		string file ="/home/veeru/project/dat/RegisteredClients.txt";
		WriteFile(const_cast<char*>(mobNumber.c_str()), const_cast<char*>(file.c_str()));
//		cout<< "Mobile Number is successfully registred into the Database \n";

		msg = "Mobile Number is successfully Registered into Database!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Close File Descriptor
	close(fd);

	return SUCCESS;
}

int Server::IsServiceActivated() {

	int status = 0, isActivated = 1;
	string msg;
	char buffer[BUFFERSIZE] = {0};
	string file = "/home/veeru/project/dat/ActivatedClients.txt";
    const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!");
        exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		isActivated = SearchMobNum(intermediate, mobNumber);
	}
	
	// Check if Service is Activated
	if (isActivated == 0) {
		msg = "Yes";
		cout<<" service is activated\n";
		SendData(const_cast<char*>(msg.c_str()));
	} else if (isActivated == 1){
		msg = "No";
		SendData(const_cast<char*>(msg.c_str()));
	}

	return SUCCESS;
}

int Server::SearchMobNum(string data, string mNum) {

	string msg;
	
	// Split the Data into Client Number
	vector<string> clientNum;
	
	stringstream ss(data);
	string intermediate;
	
	while(getline(ss, intermediate, ',')) {
		clientNum.push_back(intermediate);
	}
	
	// Return SUCCESS if Mobile Number exists
	if (clientNum[0] == mNum) {
		return SUCCESS;
	}
	
	return FAILURE;
}

int Server::ProcessUnconditionalActivationRequest() {

	string msg;
	string file = "/home/veeru/project/dat/ActivatedClients.txt";
	string data = mobNumber + ",UN," + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
//	cout <<"call forward service successfuly Activated\n";
	msg = "Unconditional CFSS Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessNoReplyActivationRequest(string Number) {

    	string msg;
	string file = "/home/veeru/project/dat/ActivatedClients.txt";
	string data = mobNumber + ",NR," + Number + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	msg = "No Reply CFSS Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessAsBusyActivationRequest(string Number){
	string msg;
	string file = "/home/veeru/project/dat/ActivatedClients.txt";
	string data = mobNumber + ",B," + Number + "\n";

	WriteFile(const_cast<char*>(data.c_str()),const_cast<char*>(file.c_str()));

	msg = "Busy CFSS Service Successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}


int Server::ProcessDeactivationRequest() {

	int status = 0;
	string msg = "";
	string data = "";
	char buffer[BUFFERSIZE] = {0};
	string file = "/home/veeru/project/dat/ActivatedClients.txt";
    	const char* clientFile = file.c_str();

	// Clear Previous Records
	clientRecord.clear();
	
	// Opening the File in Read Mode
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
        exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		clientRecord.push_back(intermediate);
	}
	
	// Delete Record of Client if Mobile Number Exists
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		if (SearchMobNum(clientRecord[i], mobNumber) == 0) {

			// clientRecord[i].erase();
			clientRecord.erase(clientRecord.begin() + i);
		}
	}
	
	// Close File Descriptor
	close(fd);
	
	// Append Updated Data
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		data.append(clientRecord[i]);
		data.append("\n");
	}
	
	// Open File in Truncate Mode
	fd = open(clientFile, O_TRUNC);
	if (fd < 0) {
		perror("FILE OPENING FAILED!");
		exit(0);
	}
	
	// Close File Descriptor
	close(fd);
		
	// Write Updated Data to File
	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
//	cout <<"call forward service is Deactivated\n";

	// Send Message to Client
	msg = "Call Forward Service is successfully Deactivated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessUpdateRequest() {

	string msg = "";
	
	// Send Message to Client
	msg = "Call Forward Service is successfully Updated!";
	SendData(const_cast<char*>(msg.c_str()));
	
    return SUCCESS;
}

int Server::ProcessCallRequest(string callNum) {
	
	int status = 0, isNumPresent = 1, isActivated = 1;
	string msg = "", serviceType = "";
	char buffer[BUFFERSIZE] = {0};
	string file ="/home/veeru/project/dat/ActivatedClients.txt";
    	const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		
		isActivated = SearchMobNum(intermediate, callNum);
		
		// Check if Call Forward Service is Activated for Receiver
		if (isActivated == 0) {
		
			// Split the Record
			vector<string> record;
			record.clear();
		
			stringstream st(intermediate);
			string data;
			
			while (getline(st, data, ',')) {
				record.push_back(data);
			}
		
			// Store Call Forward Service Type
			serviceType = record[1];
		
			// forward all Calls if Service Type is unconditional
			if (serviceType == "UN") {
			
			msg = "Unconditional call forwarded as '" + callNum + "' Receiver has subscribed for Unconditional call forward Services!";
			SendData(const_cast<char*>(msg.c_str()));
			break;
				
			// forward all Calls if Service Type is NoReply
			} else if (serviceType == "NR") {
				
				for (unsigned int i = 2; i < record.size(); i++) {
					if (record[i] == mobNumber) {
						isNumPresent = 0;
						msg = "No-Reply call forwarded as'" + callNum + "' Receiver has subscribed for No-Reply call forward Services!";
						SendData(const_cast<char*>(msg.c_str()));
						break;
					}
				}
				break;
			}
			//forward all Calls if Service Type is Busy
			else if(serviceType == "B"){
				for(unsigned int i = 0;i < record.size(); i++)
				if(record[i] == mobNumber){
				isNumPresent = 0;

				msg = "'As Busy call forward as'" + callNum + "'Reveiver has subscribed for No Reply call forward Services!'";
				SendData(const_cast<char*>(msg.c_str()));
				break;
			}
		}break;
		
		}
				
	}

	
	// Establish Call if Call Forward Service isn't Activated for Receiver
	if (isActivated == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Establish Call if Receiver hasn't listed Caller Number in case of Selective
	if (serviceType == "UN" && serviceType == "NR" && serviceType =="B" && isNumPresent == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	return SUCCESS;
}

int Server::WriteFile(char* data, char* fname) {

    	int status = 0;
    
	// Opening the File
	fd = open(fname, O_WRONLY|O_APPEND, S_IRWXU);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
        exit(0);
	}

    // Write to File
    status = write(fd, data, strlen(data));
	if (status < 0) {
	 	perror("FILE WRITE FAILURE!");
        exit(0);
	}
	
	// Close File Descriptor
	close(fd);
	
    return SUCCESS;
}

Server::~Server() {

	int sstatus = 0, cstatus = 0;

	// Closing the Connection Descriptor
	cstatus = close(cd);
	if (cstatus < 0) {
		perror("CONNECTION CLOSE FAILURE");
        exit(0);
	}

	// Closing the Socket Descriptor
	sstatus = close(sd);
	if (sstatus < 0) {
		perror("SOCKET CLOSE FAILURE");
		exit(0);
	}

}

