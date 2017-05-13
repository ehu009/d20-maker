PROG = d20-maker

CC   = gcc
HEAD = pixels.h lines.h drawn-triangles.h mouse.h chain.h intstack.h mouse_app.h net_slot.h net_model.h net-builder.h
SRCS = pixels.c lines.c drawn-triangles.c mouse.c chain.c integer_stack.c net_slot.c net-builder.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf

default : clean $(PROG)

clean :
	rm -f *.o $(PROG) *~

$(PROG) : $(HEAD)
	$(CC) $(FLAGS) $(SRCS) $^ $(LIBS) main.c -o $@
