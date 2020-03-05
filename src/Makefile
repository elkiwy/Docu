#!make

WARNS = -Wpedantic -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion
OPTIONS = -g -Og -fsanitize=address -fsanitize=undefined

objdir = obj
srcs = $(wildcard *.c)
objs = $(patsubst %.c,$(objdir)/%.o,$(srcs))


main: $(objs)
	cc -std=c99 -DDEBUG $(WARNS) $(OPTIONS) -o $@ $^

$(objdir)/%.o: %.c
	cc -std=c99 -DDEBUG $(WARNS) $(OPTIONS) -c $< -o $@

$(objdir):
	mkdir $(objdir)

run: main
	./main ../testProject

clean: 
	rm $(objs)

