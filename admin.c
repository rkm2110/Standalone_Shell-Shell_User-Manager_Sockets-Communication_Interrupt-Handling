//Takes pid of UM as argument

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    int pid;
	char buffer[20];
	char *cmd;

    if (argc < 2) {
      fprintf(stderr,"Enter PID as argument\n");
      exit(0);
    }
    
	pid = atoi(argv[1]);
	printf("PID = %d\n",pid);

	while(1)
	{
		bzero(buffer,sizeof(buffer));
		printf("Admin : ");
		fgets(buffer, (int)sizeof(buffer), stdin);
		int ret=10;
		
		cmd=strtok(buffer,"\0");
		if(strcmp(cmd,"\n")==0) 
		{
			printf("Enter a command!\n");
			continue;
		}	
		//if(strncmp(buffer,"\n", 1)) break;
		
		cmd=strtok(cmd,"\n");
		//printf("cmd = %s\n",cmd);
		
		if(strcmp("terminate",cmd)==0)
		{
			ret = kill(pid, SIGQUIT);
			printf("Sent a SIGQUIT signal\n");
		}
		else if(strcmp("sleep",cmd)==0)
		{
			ret = kill(pid, SIGINT);
			printf("Sent a SIGINT signal\n");
		}
		else if(strcmp("infor",cmd)==0)
		{
			ret = kill(pid, SIGRTMAX);
			printf("Sent a SIGRTMAX signal\n");
		}
		else if(strcmp("listuser",cmd)==0)
		{
			ret = kill(pid, SIGRTMIN);
			printf("Sent a SIGRTMIN signal\n");
		}
		else 
		{
			printf("Wrong command!\n\n");
		}
		//printf("ret = %d\n",ret);
		
	}//while loop end
	return 0;
}
