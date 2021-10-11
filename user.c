//Takes hostname and port number as argument

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_DATA 4096 

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno;
	size_t ret;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[MAX_DATA];
	char* ptr;
	char* ptr1;
	char temp[MAX_DATA];
	size_t bytes_read; 
	int msgsize = 0;
	

    if (argc < 3) {
      fprintf(stderr,"usage %s userid hostname port\n", argv[0]);
      exit(0);
    }
    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    server = gethostbyname(argv[2]);
    if (server == NULL) error ("ERROR, no such host");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
      error("ERROR connecting");
	
	
	printf("User %s socket %d with port %d is used.\n",argv[1],sockfd,ntohs(serv_addr.sin_port));
	
	
	//send user id to server (login)
	ret = write (sockfd, argv[1], (int)sizeof(argv[1]));
    if (ret < 0) error ("ERROR writing to socket");
    
	
	
	while(1)
	{
		//input
		if(ret >1) 
		{
			bzero (buffer, MAX_DATA);
			printf ("ucmd> ");
			fgets(buffer, sizeof(buffer), stdin);
			
			
			ptr1=strtok(buffer,"\0");
			if(strcmp(ptr1,"\n")==0) 
			{
				printf("Enter a command!\n");
				continue;
			}
			
			//append user id with ucmd
			char part[]=";";
			strcpy(temp,argv[1]);
			strcat (temp,part);
			ptr1= strtok(buffer,"\n");
			strcat (temp,ptr1);
			
			strcpy(buffer,temp);
			//write to server
			ret = write (sockfd, buffer, (int)sizeof(buffer));
			printf("\nwrite ret= %zu\n",ret);
		}
		if (ret < 0) error ("ERROR writing to socket");
		
		
		
		//read from server
		
		char in_buff[MAX_DATA];
		ret = read(sockfd, in_buff,MAX_DATA);
		if (ret < 0) error ("ERROR reading from socket");
		//print in_buff
		if(sizeof(in_buff))
		{	
			printf("\n%s\n",in_buff);
			//printf("\nread ret= %zu\n",ret);
			bzero(in_buff,MAX_DATA);
		}
		
		
		//fflush(stdout);
		//fflush(stdin);
		
		
			
		/*
		ptr	= strtok(in_buff,"\n");
		
		if(strcmp(ptr, "logout") == 0) 
		{
			printf("User %s socket %d has been closed, process terminating.\n",argv[1],sockfd);
		
			break;	
		}
		*/
		
		//printf("ptr=%s\n",ptr);
		//printf ("Size of ptr=%d\n\n",(int)strlen(ptr));
		
		//sleep(1);
    
	}
	
	close(sockfd);
	return 0;
}
