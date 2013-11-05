Sources		= main.cpp

CFlags 		= -c -Wall -g -O2
LDFlags 	= 
ObjectDir 	= obj/
SourceDir 	= src/
Executable  = Program
BinDir 		= 

SYS := $(shell gcc -dumpmachine)
ifneq (, $(findstring linux, $(SYS)))
	LDFlags += -lglfw -lGL -lGLEW
else ifneq (, $(findstring mingw, $(SYS)))
	LDFlags += -lglfw3 -lglew32 -lopengl32
else ifneq (, $(findstring darwin, $(SYS)))
	LDFlags += -framework Cocoa -framework OpenGL -lglfw3 -lGLEW
endif

CC = g++
RM = rm

Objects 	= $(Sources:.cpp=.o)
CSources 	= $(addprefix $(SourceDir),$(Sources))
CObjects 	= $(addprefix $(ObjectDir),$(Objects))
CExecutable = $(addprefix $(BinDir),$(Executable))

all: $(CSources) $(CExecutable)
	./$(CExecutable)

$(CExecutable): $(CObjects)
	$(CC) $(CObjects) $(LDFlags) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp
	$(CC) $(CFlags) $< -o $@

clean:
	$(RM) $(CObjects)
