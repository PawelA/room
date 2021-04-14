MAIN = main
ARCH = x11

CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lm -lGL -lX11

all: $(MAIN)
%.o: %.vert %.frag
	ld -r -b binary $^ -o $@
clean:
	rm -f *.o arch/*.o $(MAIN)

include deps.mk
