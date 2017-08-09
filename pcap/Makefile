all : test1.c test2.c test3.c simplesniffer.c
	gcc -g  -o test1 test1.c -lpcap
	gcc -g  -o test2 test2.c -lpcap
	gcc -g  -o test3 test3.c -lpcap
	gcc -g  -o simplesniffer simplesniffer.c -lpcap

test1 : test1.c
	gcc -g  -o test1 test1.c -lpcap

test2 : test2.c
	gcc -g  -o test2 test2.c -lpcap

test3 : test3.c
	gcc -g -o test3 test3.c -lpcap

simplesniffer : simplesniffer.c
	gcc -g  -o simplesniffer simplesniffer.c -lpcap

.PHONY : clean
clean:
	-rm -rf *.o test1 test2 test3 simplesniffer ./core.*
