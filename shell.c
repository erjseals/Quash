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

#define INPUT_CHAR_MAX 256
#define INPUT_ARG_MAX 10

void printInit();
void printAndGet(char* input);
void formatInput(char* input, char** inputArgs);
int tryMyCommands(char** inputArgs);
void setCommand(char** inputArgs);
void cdCommand(char** inputArgs);
void jobsCommand(char** inputArgs);
void printPath();
void printUser();

int main(int argc, char **argv, char **envp)
{
  printInit();

  //String that user input is placed inside.
  char input[INPUT_CHAR_MAX];
  char* inputArgs[INPUT_ARG_MAX];

  int quit = 1;
  int myCommandsFlag = 0;

  //Shell loop
  while(quit){

    printAndGet(input);

    formatInput(input, inputArgs);

    myCommandsFlag = tryMyCommands(inputArgs);

    if(myCommandsFlag == 1)
      quit = 0;
    }
  return 0;
}

void printInit()
{
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("* * * * * * * * * Eric Seals made a shell * * * * * * * * *\n");
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("\n");
}

void printAndGet(char* input)
{
  char* user = getenv("USER");
  char* pwd  = getenv("PWD");
  char* temp = "";

  printf("\n%s:%s$", user, pwd);

  size_t lengthOfInput = 0;
  ssize_t charsRead;

  charsRead = getline(&temp, &lengthOfInput, stdin);

  strcpy(input, temp);
  free(temp);

  //printf("Retrieved line of length %zu :\n", charsRead);
  //printf("%s", input);
}

void formatInput(char* input, char** inputArgs)
{
  //first add a space to the end of the input incase the user did not.
  int inputLength;
  inputLength = strlen(input); /* its length */
  printf("inputLength: %d\n", inputLength);
  input[inputLength] = ' '; /* insert a space */
  input[inputLength+1] = ' ';
  input[inputLength+2]= '\0'; /* strings need to be terminated in a null */
  inputLength = strlen(input);
  printf("inputLength: %d\n", inputLength);

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
}

int tryMyCommands(char** inputArgs)
{
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
    case 1:
      cdCommand(inputArgs);
      return 0;
    case 2:
      jobsCommand(inputArgs);
      return 0;
    case 3:
      return 1;
    case 4:
      return 1;
    case 5:
      printPath();
      return 0;
    case 6:
      printUser();
      return 0;
    default:
      break;
  }
  return 0;
}

void setCommand(char** inputArgs)
{
  //Need to seperate the input. Valid form is PATH=xxx
  char* envVariable;
  char* envCommand;
  int i;

  envVariable = strsep(&inputArgs[1], "=");
  envCommand = strsep(&inputArgs[1], "\0");

  setenv(envVariable, envCommand, 1);

  //printf("Congrats, you're in setCommand\n");
}

void cdCommand(char** inputArgs)
{
  printf("Congrats, you're in cdCommand\n");
}

void jobsCommand(char** inputArgs)
{
  printf("Congrats, you're in jobsCommand\n");
}

void printPath()
{
  char* path = getenv("PATH");
  printf("%s", path);
}

void printUser()
{
  char* user = getenv("USER");
  printf("Current User: %s", user);
}
