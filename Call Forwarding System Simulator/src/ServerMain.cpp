#include<server.h>
#include<User.h>

// Main Function
int main() 
{

    signal(SIGINT, signal_handler);	//calling the signal handle function to handle the interruption

	// Creating object of Server class
	Server s1;
	
	// Creation of a Socket
	s1.OpenSocket();

	// Bind Data
	s1.BindData();
	
	// Listen for Connections
	s1.Listen();
	
	return SUCCESS;
}

