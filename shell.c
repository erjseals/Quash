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

    printf("%s", input);

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

  printf("%s:%s$", user, pwd);

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
  int i;
  for(i = 0 ; i < INPUT_ARG_MAX ; i++)
  {
    inputArgs[i] = strsep(&input, " ");\
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
  int numberOfCommands = 5;
  int matchedString = -1;
  char* myCommands[numberOfCommands];


  myCommands[0] = "set";
  myCommands[1] = "cd";
  myCommands[2] = "jobs";
  myCommands[3] = "quit";
  myCommands[4] = "exit";

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
    default:
      break;
  }
  return 0;
}

void setCommand(char** inputArgs)
{
  printf("Congrats, you're in setCommand\n");
}

void cdCommand(char** inputArgs)
{
  printf("Congrats, you're in cdCommand\n");
}

void jobsCommand(char** inputArgs)
{
  printf("Congrats, you're in jobsCommand\n");
}
