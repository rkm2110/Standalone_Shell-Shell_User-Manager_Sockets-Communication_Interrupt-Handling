#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>

void parseCmd(char* cmd, char** params,char* delim);
int count_char( char *cmd, char *ch);

int main()
{
    char cmd[100],cmd1[100],cmd2[100];
    char* params[10];
	int c,j=0,Cmd_Count = 0;
	
	printf("Shsh process %d has been created by process %d\n",getpid(),getppid());
		
    while(1) 	//repeat continuously
	{
        
		printf("\ncmd> "); // display prompt 
		
        // Read command from standard input, exit on Ctrl+D
        if(fgets(cmd, sizeof(cmd), stdin) == NULL) break;
		
		char* blank;
		strcpy(cmd2,cmd);
		blank=strtok(cmd2,"\0");
		if(strcmp(blank,"\n")==0) 
		{
			printf("Enter a command!\n");
			continue;
		}
		
		printf("\nShsh processing %s\n",cmd);
		
		//count no of ";" in string cmd
		int c=count_char(cmd,";");		
		//printf("No of commands = %d\n",c+1);
		
		
        // Remove trailing newline character, if any
        if(cmd[strlen(cmd)-1] == '\n') {
            cmd[strlen(cmd)-1] = '\0';
        }
		
		strcpy(cmd1,cmd);

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
		
	
		//cmd function starts here
		if (strcmp (first, "cmd" )==0)
		{ 
			ptr = strtok(NULL,"\n"); //ptr=second word to end of line
			
			//printf("CMD command = %s\n",ptr);
			system(ptr);
			//printf("\n\n CMD ends \n");
			
		}


	
		
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
				//i++;
			}
			
			//printf("\n \n Pipe Ends \n\n");
			
			
		}
		
    }

    return 0;
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



