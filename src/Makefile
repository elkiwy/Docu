#!make

WARNS = -Wpedantic -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion
OPTIONS = -g -Og -fsanitize=address -fsanitize=undefined

objdir = obj
srcs = $(wildcard *.c)
objs = $(patsubst %.c,$(objdir)/%.o,$(srcs))


main: $(objs)
	ld -r -b binary -o obj/doculang_c.o lang/c.doculang 
	ld -r -b binary -o obj/doculang_gisp.o lang/gisp.doculang 
	cc -std=c99 -D_DEFAULT_SOURCE -DDEBUG $(WARNS) $(OPTIONS) -o $@ $^ obj/doculang_c.o obj/doculang_gisp.o

$(objdir)/%.o: %.c
	cc -std=c99 -D_DEFAULT_SOURCE -DDEBUG $(WARNS) $(OPTIONS) -c $< -o $@

$(objdir):
	mkdir $(objdir)

runhtml: main
	./main -i ../testProject -m HTML -s ../teststyle.css
runorg: main
	./main -i ../testProject -m ORG
rundebug: main
	./main -i ../testProject -m DEBUG

clean: 
	rm $(objs)

