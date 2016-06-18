PROG = d20-maker

CC	 = gcc
HEAD = pixels.h
SRCS = pixels.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf


default	: clean $(PROG)

clean	:
	rm -f *.o $(PROG) *~

$(PROG)	: $(HEAD)
	$(CC) $(FLAGS) $(SRCS) -o $@ $^ $(LIBS) $@.c
