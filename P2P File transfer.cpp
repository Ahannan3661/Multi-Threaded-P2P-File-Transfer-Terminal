#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>  
#include <errno.h>
#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>  
#include <sys/ioctl.h>
#include <errno.h>
#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stddef.h>
#include <dirent.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

using namespace std;

#define max_length 500
int serverports = 12200;
int clientports = 12100;
int noOfThreads = 1;
string file_name = "";
char serverIp[15];
int startPort = 5500;
string c_name = "./client_data";
char* extension;
char tokens[3] = {'0', '0', '0'};
string path = "";
string file_name_server = "";
bool serverTokens[3] = {false, false, false};
bool connected = false;
int exit_status = 0;

struct Data
{
	int TID;
	char *read_data;
};

class Server_socket{
    FILE *f1;

    int PORT;
    
    int general_socket_descriptor;
    int new_socket_descriptor;

    struct sockaddr_in address;
    int address_length;

    public:
    void become_socket(int p){
	PORT = p;
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(general_socket_descriptor < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(general_socket_descriptor, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    //listen for up to 5 requests at a time
    listen(general_socket_descriptor, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    new_socket_descriptor = accept(general_socket_descriptor, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(new_socket_descriptor < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
        }

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
                perror("[ERROR] : Socket failed");
                exit(EXIT_FAILURE);
            }
        }

        void bind_socket(){
            if (bind(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address))<0) {
                perror("[ERROR] : Bind failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Bind Successful.\n";
        }

        void set_listen_set(){
            if (listen(general_socket_descriptor, 3) < 0) {
                perror("[ERROR] : Listen");
                exit(EXIT_FAILURE);
            }
        }

        void accept_connection(){
            if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) { 
                perror("[ERROR] : Accept");
                exit(EXIT_FAILURE);
            }
        }


long send_size()
{
	FILE *f1;
	long file_size;
	f1 = fopen(file_name_server.c_str(), "rb");
	if(f1 == NULL) 
	{
    	printf("Error Opening File"); 
		exit(0); 
	}
   	fseek(f1, 0, SEEK_END);
   	file_size = ftell(f1);
	write(new_socket_descriptor, (void *)&file_size, sizeof(long));
	fclose(f1);
}

void receive_file_name()
{
	char file_buf [100];
	memset(&file_buf, 0, sizeof(file_buf));
	read(new_socket_descriptor, (void *)&file_buf, sizeof(file_buf));
	string temp(file_buf);
	file_name_server = temp;
	FILE *f1;
	f1 = fopen(file_name_server.c_str(), "rb");
    if(f1)
		{
			fclose(f1);
        }
    else{
         	cout<<"[ERROR] : File loading failed, Exititng.\n";
         	exit(EXIT_FAILURE);
        }
}

void receive_noOfThreads()
{
	read(new_socket_descriptor, (void *)&noOfThreads, sizeof(int));
}


string openDirectory(string directory, int indent)
{
	DIR  *dp;
	struct dirent *ep;
    	struct stat thestat;
        //will be used to determine the file owner & group
    	struct passwd *tf; 
    	struct group *gf;
	char buf[512];
	char indiv[512];
	string re = "";
	dp = opendir (directory.c_str());
	if (dp != NULL)
    	{ 	
		while (ep = readdir (dp))
      	 	{	
			sprintf(buf, "%s/%s", directory.c_str(), ep->d_name);
			if(serverTokens[1] == true)
			{
				if(serverTokens[0] == false && ep->d_name[0] == '.')
				{
					continue;
				}
					string type = "";
					string permissions = "";
					stat(buf, &thestat);
					switch (thestat.st_mode & S_IFMT) 
					{
				            case S_IFBLK:  type+="b"; break;
				            case S_IFCHR:  type+="c"; break; 
				            case S_IFDIR:  type+="d"; break; //It's a (sub)directory 
				            case S_IFIFO:  type+="p"; break; //fifo
				            case S_IFLNK:  type+="l"; break; //Sym link
				            case S_IFSOCK: type+="s"; break;
				            //Filetype isn't identified
				            default:       type+="-"; break;
                			}
        				permissions += (thestat.st_mode & S_IRUSR) ? "r" : "-";
        				permissions += (thestat.st_mode & S_IWUSR) ? "w" : "-";
        				permissions += (thestat.st_mode & S_IXUSR) ? "x" : "-";
        				permissions += (thestat.st_mode & S_IRGRP) ? "r" : "-";
        				permissions += (thestat.st_mode & S_IWGRP) ? "w" : "-";
        				permissions += (thestat.st_mode & S_IXGRP) ? "x" : "-";
        				permissions += (thestat.st_mode & S_IROTH) ? "r" : "-";
        				permissions += (thestat.st_mode & S_IWOTH) ? "w" : "-";
        				permissions += (thestat.st_mode & S_IXOTH) ? "x." : "-.";
					int hard_links = thestat.st_nlink;
					tf = getpwuid(thestat.st_uid);
				        string owner = "";
					owner += tf->pw_name;
					gf = getgrgid(thestat.st_gid);
					string group = " ";
				        group += gf->gr_name;
					int size = thestat.st_size;
				        string file_name = ep->d_name;
				        string modified = ctime(&thestat.st_mtime);
					sprintf(indiv, "%s %s  %d %s %s %d %s %s\n", type.c_str(), permissions.c_str(), 					hard_links, owner.c_str(), group.c_str(), size, modified.c_str(),file_name.c_str());
					for(int i=0 ; i<indent ; i++) re += "\t";
					re += indiv;
			            	if(ep->d_type==DT_DIR && serverTokens[2] == true && ep->d_name[0] != '.')
					{
						re += openDirectory(buf, indent+1);
					}
			}
			else
			{
				char type[10];
				if(serverTokens[0] == false && ep->d_name[0] == '.')
				{
					continue;
				}
					if(ep->d_type==DT_DIR) strcpy(type,"Dir"); else strcpy(type,"File");
					for(int i=0 ; i<indent ; i++) re += "\t";
	          			re += strcat(strcat(type," : "), strcat(ep->d_name,"\n"));
					if(ep->d_type==DT_DIR && serverTokens[2] == true && ep->d_name[0] != '.')
					{
						re += openDirectory(buf, indent+1);
					}


			}	

	  	}
      		closedir (dp);
   	}
  	else   re = "Couldn't open the directory.";
	return re;
}


string receive_command()
{
	char recv_buf[10];
	read(new_socket_descriptor, (void *)&recv_buf, sizeof(recv_buf));
	string temp(recv_buf);
	return temp;
}


void send_ls()
{
	char msg[5000];
    int bytesRead, bytesWritten = 0;
    cout << "Waiting for client to Enter Access Key..." << endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytesRead += recv(new_socket_descriptor, (char*)&msg, sizeof(msg), 0);
    string data = "";
    cout << "Client: " << msg << endl;
	if(strcmp(msg, "123")==0) 
	{
		memset(&msg, 0, sizeof(msg)); //clear the buffer
		strcpy(msg, "y");
	}
	else 
	{
		memset(&msg, 0, sizeof(msg)); //clear the buffer
		strcpy(msg, "n");	
	}
    bytesWritten += send(new_socket_descriptor, (void*)&msg, strlen(msg), 0);
    //we don't want to share files
    if(strcmp(msg, "n")==0)
    {
	cout << "Client gave incorrect Access Key." << endl;
    }
    //we will share files
    else if(strcmp(msg, "y")==0)
    {
	memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(new_socket_descriptor, (char*)&msg, sizeof(msg), 0);

	for(int i=0 ; i<strlen(msg) ; i++)
	{
		if(msg[i] == '1')
		serverTokens[i] = true;  
	}
	data = "";
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytesRead += recv(new_socket_descriptor, (char*)&msg, sizeof(msg), 0);
	string directory(msg);
	if(strcmp(msg, "cd") == 0) directory = "./";
	data = openDirectory(directory, 0);
	memset(&msg, 0, sizeof(msg)); //clear the buffer
    strcpy(msg, data.c_str());
	bytesWritten += send(new_socket_descriptor, (void*)&msg, strlen(msg), 0);
	cout << "Successfully shared files info." << endl;
    }
}

void transmit_file(int TID)
   {
   FILE *f1;
   int size, read_size, stat, packet_index;
   long start_point, end_point;
   char send_buffer[10240], read_buffer[256];
   packet_index = 1;

   f1 = fopen(file_name_server.c_str(), "rb");   

   if(f1 == NULL) {
        printf("Error Opening File"); exit(0); } 

   fseek(f1, 0, SEEK_END);
   size = ftell(f1)/noOfThreads;
   start_point = TID*size;
   if(TID == noOfThreads-1) size += ftell(f1)%noOfThreads;
   fseek(f1, start_point, SEEK_SET);
   end_point = start_point + size;
   printf("Total thread#%d transmission size: %i\n",TID, size);

   //Send thread transmission Size
   printf("Sending transmission Size\n");
   write(new_socket_descriptor, (void *)&size, sizeof(int));

   //Send file as Byte Array
   printf("Sending file as Byte Array\n");

   while(ftell(f1)<(end_point)) {
      //Read from the file into our send buffer
	  if(ftell(f1)+10240 < end_point)
      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, f1);
	  else read_size = fread(send_buffer, 1, end_point - ftell(f1), f1);
      //Send data through our socket
      do{ 
      stat = write(new_socket_descriptor, send_buffer, read_size);  
	} while (stat <0);
      //while(stat < read_size) stat += write(new_socket_descriptor, &send_buffer[stat], read_size-stat);
      printf("Packet Number: %i\n",packet_index);
      printf("Packet Size Sent: %i\n",read_size);
      printf("From Thread: %d\n",TID);     
      printf(" \n");
      printf(" \n");
      packet_index++;  

      //clear Send buffer
      bzero(send_buffer, sizeof(send_buffer));

     }
      printf("Thread#%d Finished\n", TID);
    }
};
class Client_socket{
    FILE *file;

    int PORT;
    
    int general_socket_descriptor;
	
    struct sockaddr_in address;
    int address_length;

    public:
    void become_socket(int p){
    PORT = p;
	struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
    inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(PORT);
    general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(general_socket_descriptor, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
	    exit(0);
    }
    cout << "Connected to the server!" << endl;
        }

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
                perror("[ERROR] : Socket failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void create_connection(){
            if (connect(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address)) < 0) { 
                perror("[ERROR] : connection attempt failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connection Successfull.\n";
        }

long receive_size()
{
	long file_size;
	read(general_socket_descriptor, &file_size, sizeof(long));
	return file_size;
}

void send_file_name()
{
	char write_buffer[256];
	memset(&write_buffer, 0, sizeof(write_buffer));
	strcpy(write_buffer, file_name.c_str());
	write(general_socket_descriptor, (void*)write_buffer, strlen(write_buffer));
}

void send_noOfThreads()
{
	write(general_socket_descriptor, &noOfThreads, sizeof(int));
}

void send_command(string command)
{
	char send_buf[10];
	strcpy(send_buf, command.c_str());
	write(general_socket_descriptor, (void*)&send_buf, strlen(send_buf));
}

void receive_ls()
{
	int bytesWritten = 0, bytesRead = 0;
	char msg[5000];
	string data = "";
    memset(&msg, 0, sizeof(msg));//clear the buffer
    cout << "Enter Access Key: ";
    cin >> data;
    strcpy(msg, data.c_str());
    bytesWritten += send(general_socket_descriptor, (void*)&msg, strlen(msg), 0);
    memset(&msg, 0, sizeof(msg));//clear the buffer
    bytesRead += recv(general_socket_descriptor, (char*)&msg, sizeof(msg), 0);
    if(strcmp(msg, "y") == 0)
	{
		data = "";
		data += tokens[0];
		data += tokens[1];
		data += tokens[2];
		memset(&msg, 0, sizeof(msg));//clear the buffer
		strcpy(msg, data.c_str());
		bytesWritten += send(general_socket_descriptor, (void*)&msg, strlen(msg), 0);
		if(strcmp(path.c_str(),"empty") == 0) data = "cd";
        	else data = path;
		memset(&msg, 0, sizeof(msg));
        	//clear the buffer
        	strcpy(msg, data.c_str());
		bytesWritten += send(general_socket_descriptor, (void*)&msg, strlen(msg), 0);
        	cout << "Awaiting server response..." << endl;
        	memset(&msg, 0, sizeof(msg));//clear the buffer
        	bytesRead += recv(general_socket_descriptor, (char*)&msg, sizeof(msg), 0);
		cout << "Output:-" << endl << msg << endl;
	}
    else
	{
		cout << "Incorrect Access Key" << endl;
	}	
}

void receive_file(int TID, char** read_data)
{
int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

char filearray[10239];
FILE *file;

//Find the size of data to read
do{
stat = read(general_socket_descriptor, &size, sizeof(int));
}while(stat<0);
char total_data[size];
int total_pointer = 0;
char buffer[] = "Got it";

//Loop while we have not received the above established amount of data
struct timeval timeout = {10,0};

fd_set fds;
int buffer_fd=1, buffer_out;

	while(recv_size < size) 
	{
	memset(&filearray, 0, sizeof(filearray));

		do{
        		read_size = read(general_socket_descriptor,filearray, 10239);
		} while(read_size<0);

        //Write the currently read data into a temporary array that will hold all the contents tranfered via this thread
		for(int i=0 ; i < read_size ; i++)
		{
			total_data[total_pointer] = filearray[i];
			total_pointer++;
		}
             //Increment the total number of bytes read
             recv_size += read_size;
             packet_index++;
	}
    memcpy(*read_data, total_data, sizeof(total_data));
    }
};

void clientThread(void * input)
{
	Data* data = (Data*) input;
}

struct serverData
{
    int TID;
};

void serverThread()
{
}

void * startServer(void * input)
{
	Server_socket temp;
	temp.become_socket(serverports);
	Server_socket S;
	S.become_socket(serverports+1);
	Server_socket templs;
	templs.become_socket(serverports+10);
	while(1)
	{
	if(exit_status != 0) break;
	//creating temporary socket to send file size
	string command = temp.receive_command();

	if(strcmp(command.c_str(),"ls") == 0)
	{
		templs.send_ls();
	}
	else if(strcmp(command.c_str(),"connect") == 0)
	{
		
	}
	else if(strcmp(command.c_str(),"cp") == 0)
	{
		temp.receive_file_name();
		temp.send_size();
		//initializing data structs for each thread and creating/joining threads
		S.transmit_file(0);
	}
	else if(strcmp(command.c_str(),"exit") == 0)
	{
		exit_status = 1;
		cout << "Client disconnected" << endl;
	}
	else
	{
		cout << "Invalid Command" << command << endl;
	}
	}
    return 0;
}

int main()
{
	pthread_t server_thread;
	pthread_create(&server_thread, NULL, startServer, (void*)NULL);
	Client_socket C;
	Client_socket sock;
	Client_socket sockls;
	while(1)
	{
    cout << "Enter command in the following format - command@ipaddress@path\n";
	cout << "Type exit@ipaddress to end program\n";
	cout << "Type connect@ipaddress to establish connection\n";
    char input[max_length];
	string temp;
	cin.getline(input, max_length);
	string handler(input);
	if(strcmp(handler.substr(0,7).c_str(), "connect")==0)
	{
		if(connected) 
		{
			cout << "Already connected. (exit this connection first)" << endl;
			continue;
		}
		connected = true;
		int count = 0;
		string com = "";
    	char * token;
    	token = strtok(input, "@");
		while(token != NULL)
		{
			if(count == 0) com = token;
			if(count == 1) 
			{
				string ipadd = token;
				const char* ptr = ipadd.c_str();
				strcpy(serverIp, ptr);
			}
			token  = strtok(NULL, "@");
			count++;
		}
		sock.become_socket(clientports);
		C.become_socket(clientports+1);
		sockls.become_socket(clientports+10);
		//cout << "Connected to peer with IP: " << serverIP <<endl;
	}
	if(strcmp(handler.substr(0,4).c_str(), "exit")==0)
	{
		if(!connected)
		{
			cout << "Establish a connection first" << endl;
			continue;
		}
		int count = 0;
		string com = "";
    	char * token;
    	token = strtok(input, "@");
		while(token != NULL)
		{
			if(count == 0) com = token;
			if(count == 1) 
			{
				string ipadd = token;
				const char* ptr = ipadd.c_str();
				strcpy(serverIp, ptr);
			}
			token  = strtok(NULL, "@");
			count++;
		}
		sock.send_command(input);
		cout << "Program Terminated" << endl;
		break;
	}
    string holder[3];
    holder[2] = "empty";
    int count = 0;

    char * token;
    token = strtok(input, "@");
    while(token != NULL){
	holder[count] = (string)token;			
	token  = strtok(NULL, "@");
	count++;
    }
	const char* ptr = holder[1].c_str();
	strcpy(serverIp, ptr);
 	char command[200];
	strcpy(command, holder[0].c_str()); 
    token = strtok(command, " ");
	if(strcmp(token,"ls") == 0)
	{		
		if(!connected)
		{
			cout << "Establish a connection first" << endl;
			continue;
		}
		// run ls on local pc
		if(strcmp(holder[2].c_str(),"empty") == 0)
		{

		}
		//run ls on remote pc
		else
		{
			sock.send_command(token);
	    	while(token != NULL){
			if(strcmp(token, "-a") == 0) tokens[0] = '1';
			if(strcmp(token, "-l") == 0) tokens[1] = '1';
			if(strcmp(token, "-r") == 0) tokens[2] = '1';			
			token  = strtok(NULL, " ");
    		}
			path = holder[2];
			sockls.receive_ls();
		}

	}
	else if(strcmp(token,"connect") == 0)
	{
		continue;
		//sock.send_command(token);
	}
	else if(strcmp(token,"cp") == 0)
	{
		if(!connected)
		{
			cout << "Establish a connection first" << endl;
			continue;
		}
		// run cp on remote pc
		if(gethostbyname(serverIp) != NULL)
		{
		string command = token;
		FILE* file;
		extension = strchr(const_cast<char*>(holder[2].c_str()), '.');
		if(strchr(extension, '.') != NULL)
		{
			extension = strchr(&extension[1], '.');
		}
		c_name.append(extension);
		file = fopen(c_name.c_str(), "wb");
        if(file)
		{
			fclose(file);
			file_name = holder[2];
			sock.send_command(command);
			sock.send_file_name();
			long file_size = sock.receive_size();
			int thread_load = file_size/noOfThreads;
			int remainder = file_size%noOfThreads;
			Data threadData;
        	threadData.TID = 0;
			threadData.read_data = (char*)malloc((thread_load * sizeof(char))+remainder);
			C.receive_file(0, &threadData.read_data);
			FILE *file;
			file = fopen(c_name.c_str(), "wb");
		    fwrite(threadData.read_data, 1, thread_load+remainder, file); 
			cout << "File transfer ended" << endl;
			cout << "Press Enter to continue." << endl;
        }
        else
		{
	        cout<<"[ERROR] : File creation failed, Exititng.\n";
            exit(EXIT_FAILURE);
        }
		}
		//run cp on local pc
		else
		{

	  	}
	}
	else
	{
		cout << "Invalid Command: " << token << endl;
	}
	cin.get();
	}

    return 0;
}
