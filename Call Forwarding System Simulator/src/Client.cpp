#include <client.h>
#include <User.h>

void signal_handler(int sig) {
	
    cout << "Ctrl+C Sent: " << endl << "Client EXIT..."<<endl;  
    exit(EXIT_FAILURE);
}

bool IsNumber(const string & str) {
    // std::find_first_not_of searches the string for the first character 
    // that does not match any of the characters specified in its arguments

    return !str.empty () && 
    (str.find_first_not_of ("[0123456789]") == std::string::npos);	
}

bool IsValidIp(string ip) {
    
    //Split the string into tokens
    vector<string> tokens;
	
    stringstream ss(ip);
    string intermediate;
	
    while (getline(ss, intermediate, DELIM)) {
	    tokens.push_back(intermediate);
	}

	// Return false if tokens size is not equal to four
	if (tokens.size() != 4) {
	    return false;
	}

	// Validate each token
	for (unsigned int i = 0; i < tokens.size(); i++) {
	    
	// Verify that string is number or not and the numbers are in the valid range
	if (!IsNumber(tokens[i]) || atoi(tokens[i].c_str()) > 255 || atoi(tokens[i].c_str()) < 0) {
	        return false;
	    }
	}

	return true;
}


bool IsValidPortNum(string portNum) {

    // Verify that Port Number is number or not
    if (!IsNumber(portNum)) { 
        return false;
    }

    return true;
}

bool IsValidMobileNum(string mobNum) {
    

    int len = 0;
    
    // Verify that Mobile Number contains digit or not
    if (!IsNumber(mobNum)) { 
        return false;
    }
    
    // Mobile number should be of 10 digits
    len = strlen(mobNum.c_str());
    if (len != 10) 
    {
        return false;
    }
    
//    else if(len !=13)
  //  {
//	 return false;
  //  }

    
    // Mobile number should start from 7,8 or 9
    if (mobNum[0] < '7'|| mobNum[0] >'8') 
    { 
        
	    return false;    
    }
    
    return true;
}

Client::Client() 
{
	sd = 0;
	mobileNumber = "";
}


Client::Client(string IpAddress, string PortNo, string mobNum) {
    
	fstream logfile;  
    	logfile.open("/home/veeru/project/dat/Clientlog.txt", ios::out|ios::app);

	// Populate the Server Address structure
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(PortNo.c_str())); 	// PORT NO
	serverAddress.sin_addr.s_addr = inet_addr(IpAddress.c_str());	// IP ADDRESS	
	mobileNumber = mobNum;	// MOBILE NUMBER
}


int Client::OpenSocket() {

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("SOCKET OPEN FAILURE");

		exit(0);
	}
	
	return SUCCESS;
}

int Client::Connect() {
	
	int status = 0;
	
	// Connect
	status = connect(sd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (status < 0) {
		perror("CONNECTION FAILURE");
		close(sd);

		exit(0);
	}
	
	cout << "Successfully Connected to the Server!" << endl;
	
	return SUCCESS;
}

int Client::IsMobNumRegistered(string mobNum) {

	mobileNumber = mobNum;
	
	// '1' if Mobile Number is Registered or Not
	mobNum += ":1";
	
	SendData(const_cast<char*>(mobNum.c_str()));
	
	// Read the Data sent by Server
	ReadData();

	return SUCCESS;
}

int Client::SendData(char* data) {

	int count = 0, bufferLen = 0;
	char buffer[BUFFERSIZE];
	
	strcpy(buffer, data);
	bufferLen = strlen(buffer);
	
	// Send Data
	count = send(sd, buffer, bufferLen, 0);
	if (count < 0 ) {
		perror("DATA NOT SENT TO SERVER");
		close(sd);

		exit(0);
	}
	
	// cout << "Data: '" <<  buffer << "' is sent to Server successfully!" << endl;
	
	return SUCCESS;
}

int Client::ReadData() {
	
  //  	socklen_t length;
	char buffer[BUFFERSIZE];
//	length = sizeof(serverAddress);
	int count = 0;

	// Receive Data
	count = recv(sd, buffer, BLOCKSIZE, 0);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);

		exit(0);
	}

	buffer[count] = '\0';
	
	// cout << "Data Received from Server: " << buffer <<  endl;
	cout << buffer << endl;
	return SUCCESS;
}

int Client::menu(){
	
	int ch = 0;
	
	do {
		cout <<"\n\n\t\t-----HomePage---------\n";
		cout <<"\n1. Register \t\t\t 2. Login \n ";
		cout <<"\n Please choose and enter the number ";
		cin >> ch;

		if(ch == 1)
		{
			Register();
		}
		
		else if(ch == 2)
		{
			Login();
		}

		else if(ch < 1 || ch > 2)
		{
			cout <<"\n Enter the valide number";
		}

	}
	while(ch != 2);
	system("pause");
	return 0;
}

void Client::Register(){
	
	cin.ignore();
	cout <<"\n\n" << endl;
	
	cout <<"New UserName: ";
	getline(cin, username);

	cout <<"\nNew Password: ";
	getline(cin, password);

	cout <<"\nRegistered Successfully!" << endl;

//	string msg;
	
//	msg =" Client Registered Successfully!";
//	SendData(const_cast<char*>(msg.c_str()));


	ofstream c("clients.txt", ios::app);
	
	if(!c.is_open()){
		
		cout <<"Could not open file\n";
	}

	c << username <<endl;
	c << password <<endl;
	c << '\n';
	c.close();

	ofstream l("login.txt", ios::app);

	if(!l.is_open()){

		cout <<"could not open file\n";

	}

	l << username << " " << password << endl;
	l.close();

}

void Client::Login(){

	do
	{
		cout <<"\n Username : ";
		cin >> loguser;

		cout <<"\n Password : ";
		cin >> logpassword;

		if(CheckCredentials(loguser, logpassword) == true)
		{
			cout <<"\n Login Successful!" << endl;
			cout <<"\n Welcome, " << loguser << endl;
			MainMenu();
			
		//	string msg = "";
		//	msg ="Client Logined Successfully!";
		//	SendData(const_cast<char*>(msg.c_str()));

		}

		else{
			cout <<"\n Invalide Username ot Password." << endl;
		}
	
	}
	while(CheckCredentials(loguser, logpassword) != true);

}

bool Client::CheckCredentials(string loguser, string logpassword)
{
	string u, p;

	bool status = false;

	ifstream f;
	f.open("login.txt");

	if(!f.is_open())
	{
		cout <<"unable to open file!\n";
	}
	else if(f)
	{
		while(!f.eof())
		{
			f >> u >> p;
			if(loguser == u && logpassword == p)
			{
				status = true;
			}
			else
			{
				status = false;
			}
		}
	}

	f.close();

	return status;
}


int Client::MainMenu() {
	
	// Choice
	int choice = 0;
	
	// Exit Message
	string msg = "Exit";
	
	// Performing Operation as per Client Choice
	do {
	    
	    cout << endl;
	    cout << "************ MENU ************" << endl << endl;

	    cout << "1. ACTIVATE Call Forward SERVICE" << endl;
	    cout << "2. DEACTIVATE Call Forward SERVICE" << endl;
	    cout << "3. UPDATE Call Forward SERVICE" << endl;
	    cout << "4. CALL ANOTHER CLIENT" << endl;
	    cout << "5. EXIT" << endl << endl;

	    cout << "******************************" << endl << endl;
	
	    cout << "Hi, which service would you like to avail? (1-5) : ";
	    cin >> choice;
	    
	    cout << endl;
	
	    switch(choice) {
	    
	        case 1	:	// cout << "Activate" << endl;
	                    	ActivateService();
	                    	break;
						
	        case 2	:	// cout << "Deactivate" << endl;
	                    	DeactivateService();
	                    	break;
	                    
	        case 3	:	// cout << "Update" << endl;
	                    	UpdateService();
	                    	break;

		case 4	:	CallClient();
				break;

	                    
	                    
	        case 5	:	SendData(const_cast<char*>(msg.c_str()));
						break;
	                    
	        default :   	cout << "Invalid Choice!" << endl; 
	                    	break;
	    
	    }
	    
	} while (choice != 5);
	
	return SUCCESS;
}

int Client::IsServiceActivated() {
	
	string mobNum = "";
	char data[BUFFERSIZE];
    
	// '2' if Service is Activated or Not
	mobNum = mobileNumber + ":2";
	
	// Check at Server Side in Database
	// If Service is already Activated for the Mobile Number 
	SendData(const_cast<char*>(mobNum.c_str()));
	
	// Receive Data from Server
	int count = recv(sd, data, BLOCKSIZE, 0);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);
		exit(0);
	}
	data[count] = '\0';

	// cout << "Yes/No : " << data << endl;
	
	if (strcmp(data, "Yes") == 0) {
		return SUCCESS;
		
	} else if (strcmp(data, "No") == 0) {
		return FAILURE;
	}
	
	return FAILURE;
}

int Client::ActivateService() 
{

	int serviceType = 0;
	string mobNum = "";
    	int isActivated = 1;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Activated
	if (isActivated == 0)
	{
		cout << "Can't Proceed as Service is Already Activated!" << endl;
		return FAILURE;

	} 
	else 
	{
		// Else Display Menu and Proceed Further
		cout << "***** TYPES OF SERVICES *****" << endl;
		cout << "1. Unconditional Call Forward Service" << endl;
		cout << "2. As Busy Call Forward Service" << endl;
		cout << "3. No Reply Call Forward Service" <<endl;
		cout << "*****************************" << endl << endl;
			
		cout << "Which type of Service would you like to select?" << endl;
		cin >> serviceType;

		while(serviceType < 1 || serviceType > 4) 
		{ 
			cout << "Invalid Choice! Please Enter Again : ";
			cin >> serviceType;
		}
		
		cout << endl;

		// Unconditional call forward activation is requested
		mobNum = "";
		mobNum = mobileNumber + ":3"+" for Uncondtional";

//		char service[BUFFERSIZE] = {0};
		string ch = "";
		string Number = "", num = "";

		switch(serviceType){

			case 1	:
					do{
					//input from user
					cout << "Enter the Number you want to establish service:";
					cin >> num;

					while(!IsValidMobileNum(num)){
						cout << "Invalid Mobile Number!" << endl;
						cout << "Please Enter a valid Number: ";
						cin >> num;
					}

					Number.append(num);
					
					cout <<"Do you wanto to establish Unconditional CFSS to another number?(Y/N) :";
					cin >>ch;

					while(ch != "Y"	&& ch != "N"){
						cout <<"Invalide choice! please enter Y or N :";
						cin >> ch;
					}
					
					if(ch == "Y"){
						Number.append(",");
					}
					
				}while(ch != "N");

				// No Reply service Request 
				mobNum = "";
                                mobNum = mobileNumber + ":3" ; 
                                SendData(const_cast<char*>(mobNum.c_str()));
                                ReadData();
                                break;

			case 2	:	// Send Activation Request of No Reply Service to Server
						do {
							
							// Input from the user the number he wishes to Block
							cout  << "Enter the Number you want to establish service: ";
							cin >> num;
				
							while(!IsValidMobileNum(num)) {	
								cout << "Invalid Mobile Number!" << endl;
								cout << "Please Enter a Valid Number: ";
								cin >> num;
							}	
						
							Number.append(num);
						
							cout << "Do you want to Establish No Reply CFSS to another number?(y/n) : ";
							cin >> ch;
							
							while (ch != "Y" && ch != "N") {
								cout << "Invalid Choice! Please Enter Y or N : ";
								cin >> ch;
							}
							
							if (ch == "Y") {
								Number.append(",");
							}

						} while (ch != "N");
						
						//  No reply Service Activation is Requested
						mobNum = "";
						mobNum = mobileNumber + ":4" + ":" + Number;
						SendData(const_cast<char*>(mobNum.c_str()));
						ReadData();
						break;
			 case 3  :       // Send Activation Request of 'busy Service' to Server
                                                do {

                                                        // Input from the user the number he wishes to Block
                                                        cout  << "Enter the Number you want to busy: ";
                                                        cin >> num;

                                                        while(!IsValidMobileNum(num)) {
                                                                cout << "Invalid Mobile Number!" << endl;
                                                                cout << "Please Enter a Valid Number: ";
                                                                cin >> num;
                                                        }

                                                        Number.append(num);

                                                        cout << "Do you want to Establish Busy to another number?(Y/N) : ";
                                                        cin >> ch;

                                                        while (ch != "Y" && ch != "N") {
                                                                cout << "Invalid Choice! Please Enter Y or N : ";
                                                                cin >> ch;
                                                        }

                                                        if (ch == "Y") {
                                                                Number.append(",");
                                                        }

                                                } while (ch != "N");

                                                //  As Busy Service Activation is Requested
                                                mobNum = "";
                                                mobNum = mobileNumber + ":8" + ":" + Number;
                                                SendData(const_cast<char*>(mobNum.c_str()));
                                                ReadData();
                                                break;			


			default :   cout << "Invalid Choice!" << endl;
						break;
		}
	} 
	return SUCCESS;

}	
int Client::DeactivateService() {
	
	string data = "";
	string msg = "";
	int isActivated = 0;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Deactivated
	if (isActivated == 1) {
		cout << "Can't Proceed as Service is Already Deactivated!" << endl;
		
		return FAILURE;

	} else {
		
		// '5' if Deactivation of Service is Requested
		msg = mobileNumber + ":5";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}
	
	return SUCCESS;
}

int Client::UpdateService() {

	string msg = "";
	int isDeactivated = 1;
	
	// Deactivate Service
	isDeactivated = DeactivateService();

	if (isDeactivated == 0) {
	
		// Activate Service
		ActivateService();

		// '6' if Update of Service is Requested
		msg = mobileNumber + ":6";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}

    return SUCCESS;
}

int Client::CallClient() {

	string msg = "";
	string callNum = "";
	
	// Input from the user the number he wishes to Call
	cout << "Enter which Number you want to Call: ";
	cin >> callNum;
	
	while(!IsValidMobileNum(callNum)) {	
		cout << "Invalid Mobile Number!" << endl;
		cout << "Please Enter a Valid Number: ";
		cin >> callNum;
	}	
	
	// '7' if Call to Client is Requested
	msg = mobileNumber + ":7" + ":" + callNum;
	SendData(const_cast<char*>(msg.c_str()));
	ReadData();
	
	return SUCCESS;
}

Client::~Client() 
{

	int status = 0;
	
	// Closing the Socket Descriptor
	close(sd);
	if (status < 0) {
	    perror("SOCKET CLOSE FAILURE");
	    exit(0);
	}
}


