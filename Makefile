STUDENT_ID=2630235

all: quash

quash:
	gcc -Wall -g shell.c -o quash

clean:
	rm -f quash

tar:
	make clean
	mkdir $(STUDENT_ID)-quash
	cp -r buildInstructions.txt Makefile shell.c $(STUDENT_ID)-quash
	tar cvzf $(STUDENT_ID)-quash.tar.gz $(STUDENT_ID)-quash
	rm -rf $(STUDENT_ID)-quash
