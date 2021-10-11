/* A simple server in the internet domain using TCP
   The PORT NUMBER and SLEEP TIME is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>


#define ERROR -1
#define MAX_CLIENTS 10
#define MAX_DATA 4096
int T;
//int T=atoi(argv[2]);
	


void *handle_client(void *pclient);
void parseCmd(char* cmd, char** params,char* delim);
int count_char( char *cmd, char *ch);
void SIGINT_handler(int sig);
void SIGQUIT_handler(int sig);
void SIGRTMAX_handler(int sig);
void SIGRTMIN_handler(int sig);


int main (int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[MAX_DATA];
    struct sockaddr_in serv_addr, cli_addr;
    //int sockaddr_len = sizeof(struct sockaddr_in);
	int data_len;
	char data[MAX_DATA];
	int ret;
	 
    if (argc < 2)
    { fprintf (stderr, "Arg : portno, T(sleep)\n");
      exit(1);
    }
	
	printf("UM's pid = %d\n",getpid());	
	
	portno = atoi(argv[1]); //take port number from argument passed
    int T = atoi(argv[2]); //take sleep time from argument passed
	//T =sleep_time();
	printf("Port no = %d\n",portno);
	printf("Sleep time = %d\n",T);
	
	
    //create socket
	/*sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
       error("ERROR opening socket");*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
		{
			perror("server socket: ");
			exit(-1);
		}
   
	//bzero:writing zeroes (bytes containing '\0')
     bzero ((char *) &serv_addr, sizeof(serv_addr));
     
	 
	 //server data structure	
	 serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //listen on all interfaces on host(local machine) for connections
     serv_addr.sin_port = htons(portno);
     
	 //bind
	 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
       //error("ERROR binding");
		{
			perror("bind: ");
			exit(-1);
		}
	
	printf("UM server socket ready. \n\n");
	
	
	//listen
     //listen(sockfd,5);
	 if((listen(sockfd,10))==-1)
	{
		perror("listen: ");
		exit(-1);
	}
	 
	 
	while(1)
		{	
		 
		//admin signal handlers
		//printf("Sigint = %d\n",SIGINT);
		if (signal(SIGINT, SIGINT_handler) == SIG_ERR) 
		{
			printf("SIGINT install error\n");
		}
		if (signal(SIGQUIT, SIGQUIT_handler) == SIG_ERR) 
		{
			printf("SIGQUIT install error\n");
		}
		if (signal(SIGRTMAX, SIGRTMAX_handler) == SIG_ERR) 
		{
			printf("SIGRTMAX install error\n");
		}
		if (signal(SIGRTMIN, SIGRTMIN_handler) == SIG_ERR) 
		{
			printf("SIGRTMIN install error\n");
		}
		
		
		//continue socket functions
		
		clilen = sizeof(cli_addr);
		 
		 //accept
		if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))==-1) //cli=socket descriptor
			{
				perror("accept: ");
				exit(-1);
			}
		 
		//printf("Accept client socket %d, %d\n",newsockfd, (int)cli_addr.sin_port);
		
		ret = read (newsockfd, buffer, MAX_DATA);
		if (ret < 0) perror ("ERROR reading from socket");
		
		printf("New User %s logs in, thread created, socket %d with port %d is used.\n",buffer,newsockfd,ntohs(cli_addr.sin_port));
		
		
		//printf("New client connected from port#%d and IP%s\n",ntohs(cli_addr.sin_port),inet_ntoa(cli_addr.sin_addr));
		
		
		//handle_client(newsockfd);
		pthread_t t;
		int *pclient=malloc(sizeof(int));
		*pclient=newsockfd;
		pthread_create(&t,NULL,handle_client,pclient);
		 
	}
	
	return 0; 
}

void *handle_client(void *pclient)
{
	int newsockfd=*((int*)pclient);
	free(pclient);//don't need any more
	
	char buffer[MAX_DATA];
	int ret;
	char* ptr1;
	char* uid;
	ret=1;
	
	char cmd[100],cmd1[100];
    char* params[10];
    int c,j=0,Cmd_Count = 0;
	
	printf("Shsh process with thread id  %ld has been created by process %d\n",(long)pthread_self(),getpid());
	
	
	while(ret)
	{	
		
		
		bzero(buffer,MAX_DATA);
		//read ucmd
		ret = read (newsockfd, buffer, MAX_DATA);
		if (ret < 0) perror ("ERROR reading from socket");
		 
		 
		 
		if (ret>100)
		{	 
			//printf("ret =%d\n",ret);
			printf ("Received from Client: %s\n", buffer);
			
			//check blank entry
			char buffertest[MAX_DATA];
			strcpy(buffertest,buffer);
			char* blank;
			blank=strtok(buffertest,";");
			blank=strtok(NULL,"\0");
			if(strcmp(blank,"\n")==0) 
			{
				printf("Enter a command!\n");
				continue;
			}
			
			uid	= strtok(buffer,";");
			printf("Processing user # %s\n",uid);
			ptr1	= strtok(NULL,"\n");
			
			
			//printf("ptr1=%s\n",ptr1);
			//printf ("Size of ptr1=%d\n\n\n\n",(int)strlen(ptr1));
			
			
			strcpy(cmd,ptr1);
			strcpy(cmd1,cmd);
			
			
			
			
			//printf("cmd=%s\n",cmd);
			//printf ("Size of cmd=%d\n\n\n\n",(int)strlen(cmd));
			
			//if logout
			if(strcmp(ptr1, "logout") == 0) 
			{
				printf("Logout\n");
				printf("Shsh process %d terminates \n",getpid());
				printf("User %s logs out, socket %d has been closed, thread is terminating.\n",uid,newsockfd);
				ret = write (newsockfd, ptr1,(int)strlen(ptr1));
				if (ret < 0) perror ("ERROR writing to socket");
				break;	
			}
			
			
			//count no of ";" in string cmd
			int c=count_char(cmd,";");
			
			
			//first word
			char *ptr,*first,*temp;
			ptr	= strtok(cmd," "); //first delim is already used as "SPACE"
			// printf("First word - %s\n\n", ptr);
			first=ptr;
			if ((strcmp (first,"exit")==0) || (strcmp (first,"cmd")==0) || (strcmp (first,"pipe")==0)){}
			else	{printf("Wrong Command! \n");}
			//if exit
			if(strcmp(first, "exit") == 0) 
			{
				printf("Shsh process %d terminates \n",getpid());
				break;
			}
			
			fflush(stdout);
			// redirect stdout
			int saved_stdout=dup(1);
			dup2(newsockfd,1); 
				
		
			//cmd function starts here
			if (strcmp (first, "cmd" )==0)
			{ 
				ptr = strtok(NULL,"\n"); //ptr=second word to end of line
				
				//printf("CMD command = %s\n",ptr);
				system(ptr);
								
				//printf("\n\n CMD ends \n");
				
			}// cmd ends
			
			
			//pipe function starts
			if (strcmp (first, "pipe" )==0)
			{ 
				ptr = strtok(NULL,"\n"); //ptr=second word to end of line
				
				//printf("PIPE command = %s\n",ptr);
							
				//separate the commands on";"
				parseCmd(ptr,params,";");
				//run pipe commands
				int i=0;	
				int fd[2];
				pid_t pid;
				int fdd = 0;			//Backup file descriptor

				while (params[i]!=NULL) 
				{
					//printf("Inside while loop. \n\n");
					pipe(fd);
					if ((pid = fork()) == -1) {
						perror("fork");
						exit(1);
					}
					else if (pid == 0) {
						dup2(fdd, 0);
						if (params[i+1] != NULL) 
						{
							 //printf("Inside Child:  %s ; PID = %d\n", params[i], getpid());
							 printf("Shsh forked %d for %s with in-pipe %d and out-pipe %d\n",getpid(),cmd1,fd[0],fd[1]);
							
							dup2(fd[1], 1);
						}
						close(fd[0]);
						system(params[i]);
						//execvp((*cmd)[0], *cmd);
						exit(1);
					}
					else {
						//printf("Inside Parent:  %s ; PID = %d\n", params[i], getpid());
						wait(NULL); 		// Collect childs
						close(fd[1]);
						fdd = fd[0];
						i++;
					}
				}
			}//pipe ends
			
			
			
			else printf("\n");
		
		// restoring stdout
		dup2(saved_stdout,1); 
		close(saved_stdout);
		fflush(stdout);
		fflush(stdin);
		sleep(1);
		}//if (ret>100) (buffer =256)
	}//while(ret) ends
	printf("Client disconnected.\n\n");
	close(newsockfd); //close client socket
	return NULL;
}



// Split cmd into array of parameters
void parseCmd(char* cmd, char** params,char* delim)
{       
    
	for(int i = 0; i < 20; i++) 
	{
        params[i] = strsep(&cmd,delim);
        if(params[i] == NULL) break;
 
	}
}


//count the character in a string
int count_char(char* cmd, char* ch)
{	int i,count=0;
	
	//printf("inside count char\n");
	//printf("CMD string = %s\n",cmd);
	//printf("Char to search = '%s'\n",ch);
	for (i = 0; cmd[i] != '\0'; ++i) 
	{	//printf(" Cmd %d = %c\n",i,cmd[i]);
		char c=cmd[i];
        if (*ch == c)
            ++count;
		//printf("Count = %d\n",count);
    }return count;
}


//admin signal handlers
void SIGINT_handler(int sig)
{
    printf("From SIGINT: just got a signal # %d\n", sig);
	printf("UM receives the sleep signal, going to sleep\n");
	//printf("t=%d\n",T);
	//printf("argv2=%d\n",atoi(argv[2]));
	sleep(T);
}
void SIGQUIT_handler(int sig)
{
    printf("From SIGQUIT: just got a signal # %d\n", sig);
	printf("UM terminated on Admin request with 2 active users\n");

}
void SIGRTMAX_handler(int sig)
{
    printf("From SIGRTMAX: just got a signal # %d\n", sig);

}
void SIGRTMIN_handler(int sig)
{
    printf("From SIGRTMIN: just got a signal # %d\n", sig);
}
