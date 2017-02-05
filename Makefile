PROG = d20-maker

CC   = gcc
HEAD = pixels.h lines.h drawn-triangles.h mouse.h net-builder.h mouse_app.h
SRCS = pixels.c lines.c drawn-triangles.c mouse.c net-builder.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf


default : clean $(PROG)

clean :
	rm -f *.o $(PROG) *~

$(PROG) : $(HEAD)
	$(CC) $(FLAGS) $(SRCS) main.c -o $@ $^ $(LIBS)
