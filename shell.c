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
#include <errno.h>

#define INPUT_MAX       512
#define PATH_MAX        1024
#define MAX_BACKGROUND  10
#define READ  0
#define WRITE 1

void  printInit();
void  printPath();
void  printUser();
void  printHome();
void  clear(char** inputArgs);
void  printAndGet(char* input);
void  cdCommand(char** inputArgs);
void  setCommand(char** inputArgs);
void  handlePipes(char** inputArgs, int* pipeDivide, int* totalArgs, int* totalPipes);
int   tryMyCommands(char** inputArgs);
void  formatInput(char* input, char** inputArgs, int* runInBack);
pid_t execAnExec(char** inputArgs, int runInBack);
pid_t execSomePipes(char** inputArgs, int pipeDivide, int totalArgs);
void  jobsCommand(pid_t* backgroundProcess, int numBackground);
int   checkOnBackground(pid_t* backgroundProcess, int* numBackground);



//
//
//
//
//
int main(int argc, char **argv, char **envp)
{
  //String that user input is placed inside.
  int i=0;
  int totalArgs=0;
  int runInBack=0;
  int pipeDivide=0;
  int totalPipes=0;
  int shutdown = 1;
  int numBackground = 0;
  char input[INPUT_MAX];
  int myCommandsFlag = 0;
  char* inputArgs[INPUT_MAX];
  pid_t backgroundProcess[MAX_BACKGROUND];

  for (i = 0 ; i < MAX_BACKGROUND ; i++)
    backgroundProcess[i] = 0;

  printInit();

  //Shell loops
  while(shutdown){

    totalArgs=0; pipeDivide=0; totalPipes=0;

    numBackground = checkOnBackground(backgroundProcess, &numBackground);
    printAndGet(input);
    formatInput(input, inputArgs, &runInBack);
    handlePipes(inputArgs, &pipeDivide, &totalArgs, &totalPipes);

    //if greater than 0, there are pipes to handles
    if(pipeDivide > 0)
      myCommandsFlag = 4;
    else
      myCommandsFlag = tryMyCommands(inputArgs);


    switch (myCommandsFlag) {
      case 1:
        shutdown = 0;
        break;

      case 2:;
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
        break;

      case 3:
        jobsCommand(backgroundProcess, numBackground);
        break;

      case 4:
        execSomePipes(inputArgs, pipeDivide, totalArgs);
        break;

      default:
        break;
    }
    clear(inputArgs);
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
  getline(&temp, &lengthOfInput, stdin);

  strcpy(input, temp);
  free(temp);
}



//
//
//
//
//
void formatInput(char* input, char** inputArgs, int* runInBack)
{

  //This first line removes the "\n" from the end of input
  input[strcspn(input, "\n")] = 0;

  if(strcmp(input, "")==0)
    strcpy(input, " \0");

  int i;
  for(i = 0 ; i < INPUT_MAX ; i++)
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


  *runInBack = 0;
  if(i == 0);
  else{
    if(strcmp(inputArgs[i-1], "&")== 0){
      *runInBack = 1;
      inputArgs[i-1] = NULL;
    }
  }
}

//
//
//
//
//
void handlePipes(char** inputArgs, int* pipeDivide, int* totalArgs, int* totalPipes){
  int i = 0;
  *totalPipes = 0;
  while(inputArgs[i] != NULL ){
    if(strcmp(inputArgs[i], "|")==0 ){
      *pipeDivide = i;
      i++;
      *totalPipes++;
    }
    else{
      //strcpy(beforePipe[i], inputArgs[i]);
      i++;
    }
  }
  *totalArgs = i;
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

  int numberOfCommands = 8;
  int matchedString = -1;
  char* myCommands[numberOfCommands];

  myCommands[0] = "set";
  myCommands[1] = "cd";
  myCommands[2] = "jobs";
  myCommands[3] = "quit";
  myCommands[4] = "exit";
  myCommands[5] = "PATH";
  myCommands[6] = "USER";
  myCommands[7] = "HOME";

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
      return 3;
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
    case 7:
      printHome();
      return 0;
      break;
    default:
      break;
  }
  return 2;
}



//
// set Command
// supports set PATH and set HOME
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

    if(strcmp(envVariable, "HOME")==0)
    {
      if(envCommand==NULL || strcmp(envCommand, "")==0)
        ;
      else
      {
        printf("Setting HOME to %s\n", envCommand);
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
void jobsCommand(pid_t* backgroundProcess, int numBackground)
{
  int runningInBackground = numBackground;
  if(runningInBackground == 0);
  else{
    int i;

    for(i = 0 ; i < MAX_BACKGROUND ; i++){
      if(backgroundProcess[i]==0)
        ;
      else
        printf("[%d] %d running in background\n", i, backgroundProcess[i]);
    }
  }
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
void printHome()
{
  char* home = getenv("HOME");
  printf("%s\n", home);
}



//
// exec call for single input
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
    fprintf(stderr, "\nExecutable Failed\n");
    _exit(EXIT_FAILURE);
  }
  else{
    // Parent Process!

    //if Child is a Background process
    if(runInBack == 1){
      pid_t returned;
      returned = waitpid(pid, &status, WNOHANG);

      if(returned < 0)
        return -1;
      else
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
// exec calls for the piped situation
//
//
//
pid_t execSomePipes(char** inputArgs, int pipeDivide, int totalArgs){
  int status;
  pid_t pid_1, pid_2;

  int fds1[2];

  char* beforePipe[INPUT_MAX];
  char* afterPipe[INPUT_MAX];

  int i;
  for(i = 0 ; i < pipeDivide ; i++)
    beforePipe[i] = inputArgs[i];

  i++;

  int j = 0;
  while(inputArgs[i]!=NULL){
    afterPipe[j] = inputArgs[i];
    j++;
    i++;
  }

  if( pipe(fds1) < 0){
    perror("pipes failed");
  }

  pid_1 = fork();

  if(pid_1 == 0){
    //in the first child with arguments 0 to pipeDivide

    close(fds1[READ]);
    dup2(fds1[WRITE], STDOUT_FILENO);

    execvp(beforePipe[0], beforePipe);
    fprintf(stderr, "\nExecutable Failed\n");
    _exit(EXIT_FAILURE);
  }

  close(fds1[WRITE]);

  pid_2 = fork();

  if(pid_2 == 0){
    //in the second child with arguments pipeDivide+1 to null

    dup2(fds1[READ], STDIN_FILENO);
    close(fds1[WRITE]);

    execvp(afterPipe[0], afterPipe);
    fprintf(stderr, "\nExecutable Failed\n");
    _exit(EXIT_FAILURE);
  }

  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return 0;
}



//
// Returns Nothing
// Takes the array of background processes PIDs
// and the current number of background processes
//
int checkOnBackground(pid_t* backgroundProcess, int* numBackground){
  int runningInBackground = *numBackground;
  if(runningInBackground == 0);
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
          runningInBackground--;
        }
        else if (pidReturned == 0){
          //printf("Job %d with PID %d is still running\n\n", i, backgroundProcess[i]);
        }
        else if (pidReturned == backgroundProcess[i]){
          printf("Job %d with PID %d has finished\n\n", i, backgroundProcess[i]);
          backgroundProcess[i] = 0;
          runningInBackground--;
        }
      }
    }
  }
  return runningInBackground;
}




//
// Not sure if this is entirely necessary
// Probably just needlessly slows down the shell
// but I was having unexpected outputs and this fixed them
//
void clear(char** inputArgs){
  int i;
  for(i=0 ; i < INPUT_MAX ; i++)
    inputArgs[0] = NULL;
}
