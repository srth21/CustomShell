#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>


#define tokenSize 100
#define delim " \t>|<"
#define commtok "|<>"
#define MAXSIZE 100

struct historyArray
{
	int processID;
	char command[100];
};

typedef struct historyArray history;
int size=0;
history arr[MAXSIZE];

struct aliasList
{
	char command[MAXSIZE];
	char aliasNames[MAXSIZE][MAXSIZE];
	int currSize;
};

typedef struct aliasList alias;

alias aliasData[MAXSIZE];
int aliasDataSize=0;

char * removeSpaces(char *str)
{
    // To keep track of non-space character count
    char *temp =(char *)malloc(sizeof(char)*20);
    int count = 0;
 	
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            temp[count++] = str[i]; 
    temp[count] = '\0';
    //printf("%s",temp);
    return temp;
}

void printAllAliases()
{
	for(int i=0;i<aliasDataSize;i++)
	{
		printf("Command : %s\n",aliasData[i].command);
		int size=aliasData[i].currSize;
		for(int j=0;j<size;j++)
		{
			printf("Alias Number %d : %s\n",j+1,aliasData[i].aliasNames[j]);
		}
	}
}
void addToAliasList(char *originalCommand, char* newName)
{
	int pos=-1;
	for(int i=0;i<aliasDataSize;i++)
	{
		if(strcmp(aliasData[i].command,originalCommand)==0)
		{
			pos=i;
		}
	}
	if(pos==-1)
	{
		strcpy(aliasData[aliasDataSize].command,originalCommand);
		
		strcpy(aliasData[aliasDataSize].aliasNames[0],newName);
		aliasData[aliasDataSize].currSize=1;
		aliasDataSize+=1;
	}
	else
	{
		strcpy(aliasData[pos].aliasNames[aliasData[pos].currSize],newName);
		aliasData[pos].currSize+=1;
	}
}

void addAlias(char **args)
{
	int i;
	char command[MAXSIZE];
	for(i=0;i<strlen(args[1]);i++)
	{
		command[i]=args[1][i];
	}
	command[i]='\0';
	//printf("%s\n",command);

	char *arg;

  	int pos=0;

  	char *originalCommand;
  	char *newName;
  	originalCommand=strtok(command,"=");
 	newName=strtok(NULL,"=");

 	//printf("%s -> %s\n",originalCommand,newName);

 	addToAliasList(originalCommand,newName);
 	printAllAliases();
}

int checkAliasList(char *arg)
{
	for(int i=0;i<aliasDataSize;i++)
	{
		int j=aliasData[i].currSize;
		for(int k=0;k<j;k++)
		{
			if(strcmp(aliasData[i].aliasNames[k],arg)==0)
			{
				return i;
			}
		}
	}
	return -1;
}
void addToHistory(int id,char *buf)
{
	printf("adding new element...\n");
	arr[size].processID=id;
	strcpy(arr[size].command,buf);
	size+=1;
}
int readLine(char *buf)
{
	
	int size=0;

	while(1)
	{
		int c=getchar();
		if(c==EOF || c=='\n')
		{
			buf[size]='\0';
			return size+1;
		}
		else
		{
			buf[size++]=c;
		}
	}
}

char ** splitIntoArguments(char *buf)
{
	char **args = malloc(tokenSize * sizeof(char*));
  	char *arg;

  	int pos=0;

  	arg=strtok(buf,delim);
 	
 	while(arg!=NULL)
 	{
 		args[pos++]=arg;
 		//printf("args -> %s\n",arg);
 		arg=strtok(NULL,delim);
 	}
 	args[pos]=NULL;

 	return args;

}

void printHistory()
{
	printf("size : %d\n",size);
	for(int i=0;i<size;i++)
	{
		printf("%d : Process ID :%d Command : %s\n",i+1,arr[i].processID,arr[i].command);
	}
}


void exec_order(char *sbuf)
{

	char **commlist = malloc(100 * sizeof(char*));
	char **args;
	char tempbuf[100];
	strcpy(tempbuf,sbuf);
	int ip=0,op=0,pip=0,fdin,fdout;
 	int tempin=dup(0);
 	int tempout=dup(1);	
 	char *ipfile,*opfile;
 	
	int noc=0,i=0; 
	char *comm =strtok(tempbuf, commtok);
	

	while(comm!=NULL)
 	{
 		commlist[i++]=comm;
 		comm=strtok(NULL,commtok);
 		noc++;
 	}
 	char* temp=strstr(sbuf,">");
 	


 	if ((temp!=NULL) && (strstr(temp,"<")==NULL) && (op==0))				//EX: cat <in.txt>out.txt
	{
		printf("\nREDir IAM HERE");
		op=1;
		int j=i;
		opfile=removeSpaces(commlist[j-1]);
		if((strstr(sbuf,"<")!=NULL) && (ip==0))
			{
				ip=1;
				ipfile=removeSpaces(commlist[j-2]);
			}
		i=i-1;
		noc--;
		commlist[i]=NULL;
		if(ip)
		{
			i=i-1;
			noc--;
			commlist[i]=NULL;
		}
	}

	else if ((strstr(sbuf,"<")!=NULL) && (strstr(sbuf,">")!=NULL) && (ip==0) && (op==0))				//EX: cat >out.txt<in.txt
	{
		printf("\nREDir2 IAM HERE");
		ip=1;
		int j=i;
		ipfile=removeSpaces(commlist[j-1]);
		if(strstr(sbuf,">")!=NULL)
			{
				op=1;
				opfile=removeSpaces(commlist[j-2]);
			}
		i=i-1;
		noc--;
		commlist[i]=NULL;
		if(op)
		{
			i=i-1;
			noc--;
			commlist[i]=NULL;
		}
	}

	else if ((strstr(sbuf,"<")!=NULL) && (ip==0) )		// only input EX: cat <in.txt
	{
		ip=1;
		int j=i;
		ipfile=removeSpaces(commlist[j-1]);
		i=i-1;
		noc--;
		commlist[i]=NULL;
	}
	else
		commlist[i]=NULL;
		
 	if (noc==1 && ip==0 && op==0)				// Simple command
 	{
 		printf("\nExecuted simple command");
 		args=splitIntoArguments(sbuf);
		int result=execute(args);
		return;
 	}
 	
	else 										//complex command
	{	
		if(ip)
			{
				fdin = open(ipfile,O_RDONLY);
				if(op)
				{
					
					// For LOOP 
					for(i=0;i<noc;i++)
					{
						
						dup2(fdin,0);
						close(fdin);
						
						if(i==(noc-1))
						{
							fdout=open(opfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);  //fdout=dup(tempout);
						
						}
						else
						{				
							int fdp[2];
							pipe(fdp);
							fdout=fdp[1];
							fdin=fdp[0];
						}
						dup2(fdout,1);
						close(fdout);
					
						args=splitIntoArguments(commlist[i]);
						execute(args);
						
					}
					
				}
				else
				{
					// For LOOP 
					for(i=0;i<noc;i++)
					{
						
						dup2(fdin,0);
						close(fdin);
						
						if(i==(noc-1))
						{
							fdout=dup(tempout);
						
						}
						else
						{				
							int fdp[2];
							pipe(fdp);
							fdout=fdp[1];
							fdin=fdp[0];
						}
						dup2(fdout,1);
						close(fdout);
					
						args=splitIntoArguments(commlist[i]);
						execute(args);
						
					}
					
				}
				
				
			}
		else 
			{
				fdin=dup(tempin);	
				if(op)
				{
					
					// For LOOP 
					for(i=0;i<noc;i++)
					{
						
						dup2(fdin,0);
						close(fdin);
						
						if(i==(noc-1))
						{
							fdout=open(opfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);  //fdout=dup(tempout);
						
						}
						else
						{				
							int fdp[2];
							pipe(fdp);
							fdout=fdp[1];
							fdin=fdp[0];
						}
						dup2(fdout,1);
						close(fdout);
					
						args=splitIntoArguments(commlist[i]);
						execute(args);
						
					}
					
				}
				else
				{
					// For LOOP 
					for(i=0;i<noc;i++)
					{
						
						dup2(fdin,0);
						close(fdin);
						
						if(i==(noc-1))
						{
							fdout=dup(tempout);
						
						}
						else
						{				
							int fdp[2];
							pipe(fdp);
							fdout=fdp[1];
							fdin=fdp[0];
						}
						dup2(fdout,1);
						close(fdout);
					
						args=splitIntoArguments(commlist[i]);
						execute(args);
						
					}
					
				}
			}
			
	dup2(tempin,0);
	dup2(tempout,1);
	close(tempin);
	close(tempout);
	}
}


void cd(char **args)
{
  if(args[1] == NULL) 
  {
  	fprintf(stderr,"expected argument to \"cd\"\n");
  } 
  else 
  {
  	chdir(args[1]);
  }
}



void execute(char **args,char *buf)
{

	int status;
	pid_t pid,wpid;
	pid=fork();

	if(pid==0)
	{	

		if(strstr(buf, ">") !=NULL || strstr(buf, "<") !=NULL)
		{
			printf("%s\n", "eo" );
			exec_order(buf);
		}

		else if(strcmp(args[0],"cd")==0)
		{
			addToHistory(getpid(),buf);
			cd(args);
		}
		else if(strcmp(args[0],"findAll")==0)
		{
			char *command;
			command=(char *)malloc(sizeof(char)*100);
			strcpy(command,"python3 grepinfile.py ");
			int i=1;

			while(args[i]!=NULL)
			{
				strcat(command,strcat(args[i]," "));
				i+=1;
			}

			system(command);
			free(command);
		}
		else if(strcmp(args[0],"mkFile")==0)
		{
			char *command;
			command=(char *)malloc(sizeof(char)*100);
			strcpy(command,"python3 createMakeFile.py ");
			
			int i=1;
			
			char *mainFile;
			char *funcFile;
			char *headerFile;

			mainFile=(char *)malloc(sizeof(char)*100);
			funcFile=(char *)malloc(sizeof(char)*100);
			headerFile=(char *)malloc(sizeof(char)*100);

			strcpy(mainFile,args[1]);
			strcpy(funcFile,args[2]);
			strcpy(headerFile,args[3]);

			strcat(mainFile," ");
			strcat(funcFile," ");
			strcat(headerFile," ");

			strcat(command,mainFile);
			strcat(command,funcFile);
			strcat(command,headerFile);

			system(command);

			free(mainFile);
			free(funcFile);
			free(headerFile);
			free(command);	

		}

		else if(strcmp(args[0],"alias")==0)
		{
			addAlias(args);
		}
		else if (strcmp(args[0],"redit")==0)
		{
			char *command;
			command=(char *)malloc(sizeof(char)*100);
			strcpy(command,"python3 edit.py ");
			system(command);
			free(command);

		}

		else if(strcmp(args[0],"history")==0)
		{
			addToHistory(getpid(),buf);
			printf("Printing history\n");
			printHistory();
		}

		else if(execvp(args[0],args)==-1)
		{ 	
			int x=checkAliasList(args[0]);
			if(x==-1)
			{
				printf("Command not Found.\n");
			}
			else
			{
				strcpy(args[0],aliasData[x].command);
				printf("Replacing with : %s\n",aliasData[x].command);
				execvp(args[0],args);
			}
		}

	// _exit(1);
	}

	
	else
	{	
		// printf("%s\n",buf );

		do
		{
			wpid=waitpid(pid,&status,WUNTRACED);	
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	// return 1;
}



int main(int argc, char const *argv[])
{
	printf("welcome to the shell....\n");
	
	int end=0;
	char *buf;
	char **args;
	
	while(!end)
	{
		buf=(char *)malloc(sizeof(char)*tokenSize);
		printf("\n>>");

		int size=readLine(buf);


		printf("executing command : %s\n",buf);

		if(strcmp(buf,"end")==0)
		{

			end=1;
			kill(0, SIGKILL);
		}
		else
		{	
			args=splitIntoArguments(buf);
			
			execute(args,buf);

		}
		free(buf);
	}	
	return 0;
}