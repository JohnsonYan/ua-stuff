all : test1.c test2.c test3.c simplesniffer.c arpsniffer.c tcpsyndos.c
	gcc -g -o test1 test1.c -lpcap
	gcc -g -o test2 test2.c -lpcap
	gcc -g -o test3 test3.c -lpcap
	gcc -g -o simplesniffer simplesniffer.c -lpcap
	gcc -g -o arpsniffer arpsniffer.c -lpcap
	gcc -g -o tcpsyndos tcpsyndos.c -lpcap

test1 : test1.c
	gcc -g -o test1 test1.c -lpcap

test2 : test2.c
	gcc -g -o test2 test2.c -lpcap

test3 : test3.c
	gcc -g -o test3 test3.c -lpcap

simplesniffer : simplesniffer.c
	gcc -g -o simplesniffer simplesniffer.c -lpcap

arpsniffer : arpsniffer.c
	gcc -g -o arpsniffer arpsniffer.c -lpcap

tcpsyndos : tcpsyndos.c
	gcc -g -o tcpsyndos tcpsyndos.c -lpcap

.PHONY : clean
clean:
	-rm -rf *.o ./core.* test1 test2 test3 simplesniffer arpsniffer tcpsyndos
