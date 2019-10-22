#include <stdio.h>
#include <unistd.h>

int main(){

	int i;
	i = 0;
	while(i < 3){
	sleep(5);
	printf("running\n");
	i++;
	}
}
