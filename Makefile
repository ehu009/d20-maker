PROG = d20-maker

CC	 = gcc
HEAD = pixels.h lines.h trekanter.h
SRCS = pixels.c lines.c trekanter.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf


default	: clean $(PROG)

clean	:
	rm -f *.o $(PROG) *~

$(PROG)	: $(HEAD)
	$(CC) $(FLAGS) $(SRCS) -o $@ $^ $(LIBS) $@.c
