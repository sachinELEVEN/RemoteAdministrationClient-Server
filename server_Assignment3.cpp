#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include<fstream>
using namespace std;

int initialiseServer();
void* serverActions(void * );
void* checkForAbnormalActivities(void*);
int listening = -1;
string removeThisComputer = "";

//MARK:- MESSAGE SYSTEM CLASSES
class User {
   
    public:
    
    User(string ip,string port,string name,string rollNo,string department,int socket){
      
        this->port = port;
         this->ip = ip;
        this->name = name;
        this->rollNo = rollNo;
        this->department = department;
        this->socket = socket;
      
    }
    
    
    User(){
        
    }

    
    string port;
    string ip;
    int socket = -1;
    
    //User Details
    string name;
    string rollNo;
    string department;
    int attendance=0;
    bool abnormaActivityDetected = false;
    
    
    void markAttendance(){
        //Whenever clients joins a class(function in client program) his attendance is increase
        this->attendance += 1;
        
        //save this attendance in a file of this user in the server
        fstream stream;
        string fileName = this->rollNo + ".txt";
        stream.open(fileName,ios::out);
        
        stream<<this->attendance;
        stream.close();
        
    }
    
  
};


class Users{
public:
 User users[10];
    int emptyPos = 0;
    
    
    
    
    //USE THIS METHOD TO ADD A NEW USER
    User addNewUser(string ip,string port,string name,string rollNo,string department,int socket){
        
        User newUser = User(ip, port, name,rollNo,department,socket);
        
        users[emptyPos] = newUser;
        emptyPos += 1;
       
        return newUser;
        
    }
    
    
   
    string getAllUserDetails(){
        string result = "No Users are there";
        for(int i = 0;i<10;++i){
            
            if(users[i].name == ""){
                continue;
            }
            
            if(result == "No Users are there"){
                result = "";
            }
            
            result += "IP : "+users[i].ip +" | Port : " +users[i].port + " Name : "+ users[i].name +" | RollNo : "+ users[i].rollNo +  "  | Department:" + users[i].department +" | Attendance : " +  to_string(users[i].attendance)+" | Abnormal Activity Detected "+  (users[i].abnormaActivityDetected ? "YES" : "NO");
            result += "\n";
        }
        
        return result;
    }
    
    
    
 
    User* getUserByPort(string key){
        
        for(int i = 0;i<10;++i){
            
            if(users[i].port == key){
                return &users[i];
            }
            
        }
        
        return NULL;
        
    }
    
    string getUserDetailsByPort(string key){
        string result = "";
          for(int i = 0;i<10;++i){
              
              if(users[i].port == key){
                    result += "IP : "+users[i].ip +" | Port : " +users[i].port +  " Name : "+ users[i].name +" | RollNo : "+ users[i].rollNo +  "  | Department:" + users[i].department +" | Attendance : " +  to_string(users[i].attendance)+" | Abnormal Activity Detected "+ (users[i].abnormaActivityDetected ? "YES" : "NO");
                             result += "\n";
                  
                  return result;
              }
              
          }
          
          return "Not Found";
          
      }
    
    
    string getIPandPortList(){
        string result = "No connected computers";
        for(int i = 0;i<10;++i){
          
            if (users[i].socket == -1){
                continue;
            }
            
            if(result == "No connected computers") {
                result = "";
            }
            
            result += "IP : " +users[i].ip + " | Port : "+ users[i].port;
            result += "\n";
            
        }
        
        return result;
    }
    
    void removeUserFromActiveList(string key){
        
      User* removeUser = this->getUserByPort(key);
        removeUser->name = "";
         removeUser->rollNo = "";
         removeUser->port = "";
        removeUser->ip = "";
         removeUser->socket = -1;
        
        
    }
    
    
    
}SystemUsers;

 


//MARK:- CODE BELOW
void *handleSingleClient(void *threadid){
    
           
           sockaddr_in client;
            socklen_t clientSize = sizeof(client);
           
      // cout<<"Waiting for connection"<<endl;
       int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
      // cout<<"Connection established";
    //Give this task to a new thread
          
               
       char host[NI_MAXHOST];      // Client's remote name
       char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
    
       memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
       memset(service, 0, NI_MAXSERV);
    
       if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
       {
          // cout << host << " \nconnected on port " << service << endl;
       }
       else
       {
           inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
         //  cout << host << " connected on port " << ntohs(client.sin_port) << endl;
       }
    
      
    
    //CREATING CLIENT ACCOUNT - Message
    bool accountCreated = false;
    string clientName = "";
    
    string clientHandle1 = "\n\n*********************** REMOTE ADMINISTRATION *********************************\n\n";
    string clientHandle2 = "Register - follow the below instructions\n";
    string clientHandle3 = "\nType in your details in the order NAME_ROLLNO_DEPARTMENT\n\n";
    
    string clientHandle = clientHandle1 + clientHandle2 + clientHandle3;
    
     char *mess = &clientHandle[0];
      send(clientSocket, mess,clientHandle.length(), 0);
     
      User senderUser = User();
     
     // While loop: accept and echo message back to client
        char buf[4096];
    string quit = "quit\n";
     
        while (true)
        {
            
            if (string(service) == removeThisComputer){
                cout<<"Removing computer at IP"<<host<<" and port"<<service<<endl;
                string mess = "Server terminated your connection\n";
                send(clientSocket, &mess[0],mess.length(), 0);
                break;
            }
            
            
            memset(buf, 0, 4096);
     
            // Wait for client to send data
            int bytesReceived = recv(clientSocket, buf, 4096, 0);
            if (bytesReceived == -1)
            {
                cout << "Error in recv(). Quitting" << endl;
                break;
            }
     
            if (bytesReceived == 0)
            {
                cout << "Client disconnected " << endl;
                break;
            }
            
          //  bytesReceived += -1;//to remove \n
     
            //Responses to client from server
            string clientReq = string(buf,0,bytesReceived);
            
            
            //PRINTING CLIENT MESSAGE
            cout<<"**************************************"<<endl;
            cout<<" From Client | IP "<<host<<" | Port "<<service<<endl;
            cout<<string(buf, 0, bytesReceived)<< endl;
            cout<<"**************************************\n"<<endl;
         
          
            if (!accountCreated){
                //CREATING USER ACCOUNT
          //Parsing the client response to get name rollno department
                
                string userLoginInfo[3];//0->Name,1->roll,2->department
                int currentPos = 0;
                for(int i = 0;i<bytesReceived;++i){
                    if(clientReq[i]=='_'){
                        
                        currentPos += 1;
                        continue;
                    }else{
                        userLoginInfo[currentPos] += clientReq[i];
                    }
                }
                
                
                
                
                
                
                
                //
          senderUser  =  SystemUsers.addNewUser(string(host), string(service), userLoginInfo[0],userLoginInfo[1],userLoginInfo[2],clientSocket);
                accountCreated = true;
             
                clientName = senderUser.name;
                cout<<"New Client Logged In"<<"|  IP : "<<host<<" | Port : "<<service<<endl;
                //User is connected to the server
                 string notif = " You are now Logged In and Connected to the server, " +clientName+ " Your Activity will be monitored by the server\n";
                
                    send(clientSocket,&notif[0],notif.length(),0);
                
                
                
                continue;
                
            }
            
           
            if (clientReq.compare(quit)==0){
                string closingMess = "Client wants to close the connection. - CLOSING";
                char *mess = &closingMess[0];
                send(clientSocket, mess,closingMess.length(), 0);
                cout<<closingMess<<endl;
                break;
            }
            
            else if (clientReq.compare("myinfo\n")==0){
                //Client wants to see its information stored on server
              string userData =  SystemUsers.getUserDetailsByPort(string(service));
             
                send(clientSocket,&userData[0],userData.length(),0);
            }
            else if (clientReq.compare("myactions\n")==0){
                         
                        string userActions = ">myinfo -> To get your details stored on server\n>myactions -> List of sctions\n>update->To Update Your information\n";
                       
                          send(clientSocket,&userActions[0],userActions.length(),0);
                      }//markAttendance
            else if (clientReq.compare("updateattendance\n")==0){
                
                //This is not an explicit action-> whenver user attends a class(function of client program) his/her
                //attendance is automatically updated
                        
                User* user = SystemUsers.getUserByPort(service);
                user->markAttendance();
                cout<<user->name<<"| Port "<<user->port<<" has updated attendance. Changes have been saved in a file named : "<<user->rollNo<<".txt on server"<<endl;
                
                string response = "done\n";
                               send(clientSocket,&response[0],response.length(),0);
                                    
                                 }
            
            
            else{
                //here send something to client if you want
                string invalidRequest = "Invalid Request -> Please type 'myactions' to checks valid server requests\n";
                send(clientSocket,&invalidRequest[0],invalidRequest.length(),0);
            }
            
            
          
        }
       
    //Remove client from active list
    SystemUsers.removeUserFromActiveList(service);
    
       // Close the socket
       close(clientSocket);
       
       
    return 0;
       
}


int main(){
    //MARK:- This program handles multiple clients using multithreading,each client can send messages to some other client of his/her wish, client can also disconnect from server by just typing 'quit' command.
    /*
    
     //MARK:- FEATURES OF THE PROGRAM
     SEND PERSONAL MESSAGES
     SEND GROUP MESSAGES
     GET NOTIFIED OF MESSAGES THEY RECIEVE
     GET NOTIFIED WHENEVER A NEW USER JOINS THE CHAT ROOM
     SIMPLE AND EASY TO USE. JUST @USERNAME/@GROUPNAME TO SEND MESSAGE TO THAT USER OR GROUP - EG - HEY HOW ARE YOU @MESSI
     SEE A LIST OF ALL USERS
     USERS CREATE ACCOUNT WHENEVER THEY JOIN THE CHATROOM
     PROGRAM LOCALLY SAVES USERNAME,IP,PORT,SOCKET,GROUP NAME
     MESSAGE,ITS SENDER,RECIEVER IS ALSO STORED LOCALLY
     ALL THE ABOVE IS ACHEIVED THROUGH CLASSES - SO CAN BE EASILY SAVED TO A FILE
    
    */

    
  
    
     initialiseServer();
   
    
    
    
return 0;
}
 

int initialiseServer()
{
   
   //WORK OF PARENT THREAD
    // Create a socket
     listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cout << "Can't create a socket! Quitting" << endl;
        return -1;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8080);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    cout<<"Starting to listening"<<endl;
    listen(listening, SOMAXCONN);
    cout<<"Started listnening"<<endl;
    // Wait for a connection
 
    
 
    //MARK:- Handling multiple client using mutli threading
    int maxClients = 10;
    pthread_t threads[maxClients+1];
      int rc;
      int i;
      
      for( i = 0; i < maxClients; i++ ) {
       //  cout << "main - : creating thread, " << i << endl;
         rc = pthread_create(&threads[i], NULL,  handleSingleClient, (void *)i);
         
         if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
         }
      }
    
    //Creating thread to handle server actions
   rc = pthread_create(&threads[i], NULL,  serverActions, (void *)i);
            
            if (rc) {
               cout << "Error:unable to create thread," << rc << endl;
               exit(-1);
            }
    
     
    
    //Creating thread to detect abnormal activities
    rc = pthread_create(&threads[i], NULL,  checkForAbnormalActivities, (void *)i);
               
               if (rc) {
                  cout << "Error:unable to create thread," << rc << endl;
                  exit(-1);
               }
    
    pthread_exit(NULL);
   
 
    return 0;
}


void* serverActions(void * threadid){
   
    /*
    //MARK:- Various Actions that can be performed from the server
    
     Get list of IP and Port of active computer
     Get list and details of all users logged in different machines
     Get details of a particular user
     Shutdown a remote computer - disconnect user's computer from server
     */
    
    while(true){
        cout<<"************************ SERVER ACTIONS ***********************************"<<endl;
    cout<<"Get list of IP and Port of active computer - 1\nGet list and details of all users logged in different machines - 2\nGet details of a particular user - 3\nShutdown a remote computer - disconnect user's computer from server - 4"<<endl;
         cout<<"***********************************************************"<<endl;
        
    
        int choice = -1;
        cin>>choice;
        
        if(choice == 1){
            //Note - This information will be updated as clients join the server or change their details
             cout<<"Here is the list of active computers"<<endl;
           cout<<SystemUsers.getIPandPortList()<<endl;
           
        }
        else  if(choice == 2){
            cout<<SystemUsers.getAllUserDetails()<<endl;
        }
        else  if(choice == 3){
            cout<<"Here is the list of active computers"<<endl;
             cout<<SystemUsers.getIPandPortList()<<endl;
            cout<<"See from the above list and Type in Port of the computer whose details you want to see"<<endl;
            string reqPort = "";
            cin>>reqPort;
            cout<<SystemUsers.getUserDetailsByPort(reqPort)<<endl;
            
               }
        else  if(choice == 4){
            cout<<"Here is the list of active computers"<<endl;
            cout<<SystemUsers.getIPandPortList()<<endl;
            cout<<"See from the above list and Type in Port of the computer that you want to REMOVE "<<endl;
            string reqPort = "";
            cin>>reqPort;
            removeThisComputer = reqPort;
            //Thread handling this client will be end
               }
        else if(choice == 5){
            cout<<"Enter Port"<<endl;
            string reqPort = "";
            cin>>reqPort;
          User* user = SystemUsers.getUserByPort(reqPort);
            user->markAttendance();
        }
   
    
    
    }
    
    
    return 0;
}

void* checkForAbnormalActivities(void* threadid){
    //Finds users who are doing abnormal activities like - too many logins and logouts
    
    
    while(true){
        //Monitors attendance
        for(int i = 0;i<10;++i){
            if(SystemUsers.users[i].attendance > 2 && SystemUsers.users[i].abnormaActivityDetected == false){
                cout<<"*********** Abnormal Activity Detected ***************"<<endl;
                SystemUsers.users[i].abnormaActivityDetected = true;
                cout<<"Abnormal Activity is detected at machine with these details - "<<SystemUsers.getUserDetailsByPort(SystemUsers.users[i].port)<<endl;
                cout<<"*************************************************"<<endl;
            }
        }
        
        
    }
    
    
    return 0;
}

//Done169


