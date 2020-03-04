#!make

WARNS = -Wpedantic -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion
OPTIONS = -g -Og -fsanitize=address -fsanitize=undefined

main: main.c
	cc -std=c99 -DDEBUG $(WARNS) $(OPTIONS) main.c -o main

run:  
	cc -std=c99 -DDEBUG $(WARNS) $(OPTIONS) main.c -o main
	./main ../testProject

