PROG = d20-maker

CC   = gcc
FLAGS= -Wall
LIBS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf
WLIBS = -lmingw32 -lSDL2main $(LIBS)

SRC_PATH=./src

OBJS = vertex pixels lines mouse list screen-triangles net-builder sha256 storage fader button d20 memory
HEAD = mouse_app font colours
SRCS = main

OBJECTS = $(OBJS:%=$(SRC_PATH)/%.c) $(OBJS:%=$(SRC_PATH)/%.h)
HEADERS = $(HEAD:%=$(SRC_PATH)/%.h)
SOURCES = $(SRCS:%=$(SRC_PATH)/%.c)

default : clean $(PROG)

clean :
	rm -f *.o $(PROG) *~

win : $(HEADERS) $(SOURCES) $(OBJECTS)
	$(CC) $(FLAGS) -I ./include -L ./lib $^ $(WLIBS) -o d20-maker.exe

$(PROG) : $(HEADERS) $(SOURCES) $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LIBS) -o $@
