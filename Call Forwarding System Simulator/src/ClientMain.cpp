#include<client.h>
#include<User.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<string>


int main()
 {
      	 string portNum,mobNum;
         string ip;


        cout<<"Enter the PORT Number\n";
        cin>>portNum;
        cout<<"Enter the IP Address\n";
        cin>>ip;
        cout<<"Enter the Mobile Number\n";
        cin>>mobNum;

        signal(SIGINT, signal_handler);

    // Validate IP Address
    if (!IsValidIp(ip)) {

        cerr << "Invalid IP Address!" << endl;
                exit(1);
    }

    // Validate Port number
    if (!IsValidPortNum(portNum)) {
    
        cerr << "Invalid Port Number!" << endl;
                exit(2);
    }

    // Validate Mobile Number
    if (!IsValidMobileNum(mobNum)) {
               exit(3);
   }

   // Creating object of Client class
   Client c1(ip,portNum,mobNum);

   // Creation of a Socket
   c1.OpenSocket();

   // Connecting to the Server
   c1.Connect();


   c1.IsMobNumRegistered(mobNum);
   c1.menu();

   c1.MainMenu();

   return SUCCESS;
}

