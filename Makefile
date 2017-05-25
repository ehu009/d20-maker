PROG = d20-maker

CC   = gcc
HEAD = vertex.h pixels.h lines.h mouse.h net-builder.h mouse_app.h
SRCS = vertex.c pixels.c lines.c mouse.c net-builder.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf

default : clean $(PROG)

clean :
	rm -f *.o $(PROG) *~

$(PROG) : $(HEAD)
	$(CC) $(FLAGS) $(SRCS) $^ $(LIBS) main.c -o $@
