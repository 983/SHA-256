tests: sha256.c sha256.h tests.c test_vectors.h
	gcc -O3 -Wall -Wextra -pedantic tests.c sha256.c -o tests

example: sha256.c sha256.h example.c
	gcc -O3 -Wall -Wextra -pedantic example.c sha256.c -o example
