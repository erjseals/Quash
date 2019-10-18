// this is a simple program to run
// for infinity. Used to test the
// background process of the Quash.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
  int i = 0;
  while(i < 5){
    printf("Hi, ");
    sleep(310);
    printf("still here.\n");
    //i++;
  }
  return 0;
}
