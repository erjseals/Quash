/*
  Author: Eric Seals
  Date: October 18 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define INPUT_CHAR_MAX  512
#define PATH_MAX        1024
#define INPUT_ARG_MAX   100
#define MAX_BACKGROUND  10

void  printInit();
void  printPath();
void  printUser();
void  printAndGet(char* input);
void  cdCommand(char** inputArgs);
void  setCommand(char** inputArgs);
void  jobsCommand(char** inputArgs);
int   tryMyCommands(char** inputArgs);
int   formatInput(char* input, char** inputArgs);
pid_t execAnExec(char** inputArgs, int runInBack);
void  checkOnBackground(pid_t* backgroundProcess, int* numBackground);



//
//
//
//
//
int main(int argc, char **argv, char **envp)
{
  //String that user input is placed inside.
  int i=0;
  int runInBack=0;
  int shutdown = 1;
  int numBackground = 0;
  int myCommandsFlag = 0;
  char input[INPUT_CHAR_MAX];
  char* inputArgs[INPUT_ARG_MAX];
  pid_t backgroundProcess[MAX_BACKGROUND];

  for (i = 0 ; i < MAX_BACKGROUND ; i++)
    backgroundProcess[i] = 0;

  printInit();

  //Shell loop
  while(shutdown){

    checkOnBackground(backgroundProcess, &numBackground);
    printAndGet(input);
    runInBack = formatInput(input, inputArgs);

    myCommandsFlag = tryMyCommands(inputArgs);

    if(myCommandsFlag == 1){
      shutdown = 0;
    }

    if(myCommandsFlag == 2){
      pid_t pid = 0;
      pid = execAnExec(inputArgs, runInBack);

      if( runInBack == 1 && pid > 0 && numBackground<MAX_BACKGROUND ){
        //finds available Job id
        for(i = 0 ; i < MAX_BACKGROUND ; i++)
          if(backgroundProcess[i]!=0);
          else
            break;

        backgroundProcess[i] = pid;
        numBackground++;
        printf("[%d] %d running in background\n", numBackground, pid);
      }
    }
  }
  return 0;
}



//
//
//
//
//
void printInit()
{
  printf("\n");
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("* * * * * * * * * Eric Seals made a shell * * * * * * * * *\n");
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("\n");
}



//
//
//
//
//
void printAndGet(char* input)
{
  char* user = getenv("USER");

  char pwd[PATH_MAX];
  char* cwd = getcwd(pwd, PATH_MAX);

  char* temp = "";

  printf("%s:%s$", user, cwd);

  size_t lengthOfInput = 0;
  ssize_t charsRead;

  charsRead = getline(&temp, &lengthOfInput, stdin);

  strcpy(input, temp);
  free(temp);

  //printf("Retrieved line of length %zu :\n", charsRead);
  //printf("%s", input);
}



//
//
//
//
//
int formatInput(char* input, char** inputArgs)
{
  //This first line removes the "\n" from the end of input
  input[strcspn(input, "\n")] = 0;

  if(strcmp(input, "")==0)
    strcpy(input, " \0");

  int i;
  for(i = 0 ; i < INPUT_ARG_MAX ; i++)
  {
    inputArgs[i] = strsep(&input, " ");
    //When no delimiter is found, the token is taken to be the whole String
    //and the return value is NULL. This means the parsing is complete.
    if(inputArgs[i] == NULL)
      break;
    //Incase there are two spaces after one another, we ignore the space
    //as an argument.
    if(strlen(inputArgs[i]) == 0)
      i--;
  }

  i = 0;
  while(inputArgs[i] != NULL){
    i++;
  }

  int runInBack = 0;
  if(strcmp(inputArgs[i-1], "&")== 0){
    runInBack = 1;
    inputArgs[i-1] = NULL;
  }
  return runInBack;
}



//
//
//
//
//
int tryMyCommands(char** inputArgs)
{
  if(inputArgs[0] == NULL)
    return 0;

  int numberOfCommands = 7;
  int matchedString = -1;
  char* myCommands[numberOfCommands];

  myCommands[0] = "set";
  myCommands[1] = "cd";
  myCommands[2] = "jobs";
  myCommands[3] = "quit";
  myCommands[4] = "exit";
  myCommands[5] = "PATH";
  myCommands[6] = "USER";

  int i;
  for(i=0;i<numberOfCommands;i++)
  {
    if(strcmp(myCommands[i], inputArgs[0])==0)
      matchedString = i;
  }

  switch (matchedString){
    case 0:
      setCommand(inputArgs);
      return 0;
      break;
    case 1:
      cdCommand(inputArgs);
      return 0;
      break;
    case 2:
      jobsCommand(inputArgs);
      return 0;
      break;
    case 3:
      return 1;
      break;
    case 4:
      return 1;
      break;
    case 5:
      printPath();
      return 0;
      break;
    case 6:
      printUser();
      return 0;
      break;
    default:
      break;
  }
  return 2;
}



//
//
//
//
//
void setCommand(char** inputArgs)
{
  if(inputArgs[1] == NULL)
    ;
  else{
    //Need to seperate the input. Valid form is PATH=xxx
    char* envVariable;
    char* envCommand;
    int i;

    envVariable = strsep(&inputArgs[1], "=\n\0");
    envCommand = strsep(&inputArgs[1], "\n\0");

    if(strcmp(envVariable, "PATH")==0)
    {
      if(envCommand==NULL || strcmp(envCommand, "")==0)
        ;
      else
      {
        printf("Setting PATH to %s\n", envCommand);
        setenv(envVariable, envCommand, 1);
      }
    }
  }
}



//
//
//
//
//
void cdCommand(char** inputArgs)
{
  if(inputArgs[1] == NULL){
    char* home = getenv("HOME");
    chdir(home);
  }
  else{
    chdir(inputArgs[1]);
  }
}



//
//
//
//
//
void jobsCommand(char** inputArgs)
{
  printf("Congrats, you're in jobsCommand\n");
}



//
//
//
//
//
void printPath()
{
  char* path = getenv("PATH");
  printf("%s\n", path);
}



//
//
//
//
//
void printUser()
{
  char* user = getenv("USER");
  printf("Current User: %s\n", user);
}



//
//
//
//
//
pid_t execAnExec(char** inputArgs, int runInBack)
{
  //Use execvp because that passes the environment
  pid_t pid;
  int status;

  pid = fork();

  if(pid < 0){
    fprintf(stderr, "fork failed\n");
  }
  else if(pid == 0){
    // Child process!
    execvp(inputArgs[0], inputArgs);
    fprintf(stderr, "Executable Failed\n");
  }
  else{
    // Parent Process!

    //if Child is a Background process
    if(runInBack == 1){
      return pid;
    }
    //else Child runs in foreground.
    else {
      waitpid(pid, &status, 0);
    }
  }
  return -1;
}



//
// Returns Nothing
// Takes the array of background processes PIDs
// and the current number of background processes
//
void checkOnBackground(pid_t* backgroundProcess, int* numBackground){
  if(*numBackground == 0)
    ;
  else{
    int status;
    int i;
    pid_t pidReturned;

    for(i = 0 ; i < MAX_BACKGROUND ; i++){
      if(backgroundProcess[i]==0)
        ;
      else{
        pidReturned = waitpid(backgroundProcess[i], &status, WNOHANG);
        if(pidReturned == -1){
          fprintf(stderr, "Error with waitpid!\n");
          backgroundProcess[i] = 0;
          *numBackground--;
        }else if (pidReturned == 0){
          printf("Job %d with PID %d is still running\n", i, backgroundProcess[i]);
        }else if (pidReturned == backgroundProcess[i]){
          printf("Job %d with PID %d has finished\n", i, backgroundProcess[i]);
          backgroundProcess[i] = 0;
          *numBackground--;
        }
      }
    }
  }
}
