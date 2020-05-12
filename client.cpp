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
int attendance = 0;
void saveUserData(string,string,string);

string getResponse(char*buf,int sock){
    //        Wait for response
              memset(buf, 0, 4096);
              int bytesReceived = recv(sock, buf, 4096, 0);
              if (bytesReceived == -1)
              {
                  cout << "error getting response from server\n";
                  return "Error in getting response";
              }
              else
              {
                  //        Display response
                //  cout << "SERVER> " << string(buf, bytesReceived) << "\r\n";
                  return string(buf, bytesReceived);
              }
    
    
}

int connectToServer(string accountCredentials){
    //    Create a socket
    bool loggedIn = false;
       int sock = socket(AF_INET, SOCK_STREAM, 0);
       if (sock == -1)
       {
           return 1;
       }

       //    Create a hint structure for the server we're connecting with
       int port = 50140;
       string ipAddress = "0.0.0.0";

       sockaddr_in hint;
       hint.sin_family = AF_INET;
       hint.sin_port = htons(port);
       inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

       //    Connect to the server on the socket
       int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
       if (connectRes == -1)
       {
           return 1;
       }

      
       char buf[4096];
       string userInput;

//Getting initial connection response from server
     cout<<getResponse(buf,sock);
    
    
       do {
            if(loggedIn){
           cout<<"See your information saved on server - 1"<<endl;
             cout<<"Attend a class of your department-2"<<endl;
            
                string choice;;
           
           cout << "> ";
           cin>>choice;
    
           
           
           if(choice == "1"){
               //See user information saved on server
               userInput = "myinfo";//server command
           }
           else if(choice == "2"){
               //See user information saved on server
               cout<<"Subject Code"<<endl;
               string code = "";
               cin>>code;
               
               
               cout<<"You are currently Attending a class. Press any key To Exit the class and your attendanc will be marked"<<endl;
               //The attendance will be updated on the server
               getchar();
               
               userInput = "updateattendance";//server command
               attendance += 1;
               string atten = to_string(attendance);
               saveUserData(accountCredentials,atten,code);
             
           }
           else if(choice == "3"){
               cout<<"Closing your connection from server"<<endl;
               break;
           }
           
           else{
              //Direct server command - not given in the choice
               userInput = choice;
           }
           
           
         
            
            }else{
                userInput = accountCredentials;
                              loggedIn = true;
            }
           

           //        Send to server
        
           int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
           if (sendRes == -1)
           {
               cout << "Could not send to server\n";
               continue;
           }

           cout<<getResponse(buf,sock);
       } while(true);

       //    Close the socket
      
    close(sock);

       return 0;
}

int clientActions(){
    /*
    //MARK:- Actions that can be taken by client are -
     
     Create Account
     Attend classes -> Attendance will be marked for those classes
            - This attendance will be updated on the server and stored locally on a file on client
     See your information stored in server
     Disconnect from server
     
    */
    
    return 0;
}



int main()
{
    
//    //Create Account
    cout<<"Need to create your account.Your activity will be monitored by the server "<<endl;
    string name,roll,dept;
    cout<<"Enter your name"<<endl;
    cin>>name;
    cout<<"Enter your rollNo"<<endl;
    cin>>roll;
    cout<<"Enter your department"<<endl;
    cin>>dept;
    
    string accountCredentials = name+"_"+roll+"_"+dept;
    
  
    
    
    connectToServer(accountCredentials);
}

void saveUserData(string userCred,string attendance,string subject){
    fstream stream;
    
    stream.open(userCred+".txt",ios::out);
    string data = userCred +" : " +"Subject : "+subject+ " Attendance : "+attendance+"\n";
    stream>>data;
    stream.close();
}

//Done5
