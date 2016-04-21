PROG	= d20-maker

CC	= gcc
FLAGS= -Wall
LIBS	= -lm -lSDL2 -lSDL2_image -lSDL2_ttf


default	: clean $(PROG)

clean	:
	rm -f *.o $(PROG) *~

$(PROG)	: 
	$(CC) $(FLAGS) -o $@ $^ $(LIBS) $@.c
