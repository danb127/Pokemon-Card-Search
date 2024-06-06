all: parser search

parser: parser.c
	gcc -g -o parser parser.c -std=gnu11 -Wall -Werror

search: search.c
	gcc -g -o search search.c -std=gnu11 -Wall -Werror

clean:
	rm -rf abilities.bin indexes.bin pokemon.bin parser search