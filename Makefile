PROG = d20-maker
SRC_PATH=./src

CC   = gcc

HEAD = vertex pixels lines mouse chain screen-triangles net-builder mouse_app
SRCS = vertex pixels lines mouse chain screen-triangles net-builder main
#HEAD = vertex.h pixels.h lines.h mouse.h chain.h screen-triangles.h net-builder.h mouse_app.h
#SRCS = vertex.c pixels.c lines.c mouse.c chain.c screen-triangles.c net-builder.c
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf

SOURCES = $(SRCS:%=$(SRC_PATH)/%.c)
HEADERS = $(HEAD:%=$(SRC_PATH)/%.h)

default : clean $(PROG)

clean :
	rm -f *.o $(PROG) *~

$(PROG) :$(HEADERS)
	$(CC) $(FLAGS) $^ $(SOURCES) $(LIBS) -o $@

