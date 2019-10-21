STUDENT_ID=2630235

all: quash

quash:
	gcc -Wall -g shell.c -o quash

clean:
	rm -f quash
