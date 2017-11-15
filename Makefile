PROG = d20-maker

CC   = gcc
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf


SRC_PATH=./src
OBJS = vertex pixels lines mouse chain screen-triangles net-builder sha256 storage fader button
HEAD =  mouse_app font colours d20
SRCS = main

OBJECTS = $(OBJS:%=$(SRC_PATH)/%.c) $(OBJS:%=$(SRC_PATH)/%.h)
HEADERS = $(HEAD:%=$(SRC_PATH)/%.h)
SOURCES = $(SRCS:%=$(SRC_PATH)/%.c)


default : clean $(PROG)

clean :
		rm -f *.o $(PROG) *~

$(PROG) : $(HEADERS) $(SOURCES) $(OBJECTS)
		$(CC) $(FLAGS) $^ $(LIBS) -o $@

