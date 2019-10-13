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


void printInit();

int main(int argc, char*argv[])
{
  printInit();

}

void printInit()
{
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("* * * * * * * * * Eric Seals made a shell * * * * * * * * *\n");
  printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
  printf("\n");

  char* user = getenv("USER");
  char* path = getenv("PATH");
  char* home = getenv("HOME");

  printf("%s@swifter:~$\n", user);
  printf("The current path is : %s\n", path);
  printf("And home is : %s\n", home);

}
