MAIN = room
ARCH = win32

CC = gcc
CFLAGS = -Wall -g

ifeq ($(ARCH), x11)
LDLIBS = -lm -lGL -lX11
clean:
	rm -f *.o $(MAIN)
endif

ifeq ($(ARCH), win32)
LDLIBS = -lm -lopengl32 -lgdi32
endif

all: $(MAIN)
$(MAIN):
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
%.o: %.vert %.frag
	ld -r -b binary $^ -o $@
	


include deps.mk
