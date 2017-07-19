generate: generate.o
	cc -o generate generate.o 

generate.o: src/generate.c 
	cc -c src/generate.c

.PHONY: clean
clean:
	-rm *.o generate
