#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

#define tokenSize 10
#define delim " \t"
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
			//printf("here : %s\n",buf );
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
int execute(char **args,char *buf)
{
	pid_t pid,wpid;
	pid=fork();
	int status;

	if(pid==0)
	{
		if(strcmp(args[0],"cd")==0)
		{
			addToHistory(getpid(),buf);
			cd(args);
		}
		else if(strcmp(args[0],"findAll")==0)
		{
			//printf("hello\n");
			char *command;
			command=(char *)malloc(sizeof(char)*100);
			strcpy(command,"python3 searchFiles.py ");
			int i=1;
			//printf("args :%s\n",args[2]);
			while(args[i]!=NULL)
			{
				//printf("args of %d is %s\n",i,args[2]);
				strcat(command,strcat(args[i]," "));
				i+=1;
			}
			//printf("i : %d\n",i);
			//printf("%s\n",command);
			//char *argsNew[]={NULL};
			system(command);
			//printf("does not come here lol\n");
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
			//printf("Command %s\n",command);
			system(command);
			//printf("issue not here 1\n");
			free(mainFile);
			free(funcFile);
			free(headerFile);
			free(command);	
			//printf("issue not here 2\n");
		}
		else if(strcmp(args[0],"alias")==0)
		{
			addAlias(args);
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
	}
	
	else
	{
		do
		{
			wpid=waitpid(pid,&status,WUNTRACED);	
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
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

int main(int argc, char const *argv[])
{
	printf("welcome to the shell....\n");
	
	int end=0;

	char *buf;
	
	char **args;
	
	while(!end)
	{
		buf=(char *)malloc(sizeof(char)*tokenSize);
		printf(">>");
		int size=readLine(buf);
		printf("executing command : %s\n",buf);
		if(strcmp(buf,"end")==0)
		{
			end=1;
			//exit(0);
		}
		else
		{
			args=splitIntoArguments(buf);
			int result=execute(args,buf);
		}
		free(buf);
	}	
	return 0;
}