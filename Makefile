main 	= main.cpp
src		= Shader.cpp

cFlags 	= -c -Wall -g -O2
ldFlags =
objDir  = obj/
srcDir  = src/
binDir 	=
bin  	= Program

UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
	ldFlags += -lglfw -lGL -lGLEW
else ifeq ($(UNAME), mingw)
	ldFlags += -lglfw3 -lglew32 -lopengl32
else ifeq ($(UNAME), Darwin)
	ldFlags += -framework Cocoa -framework OpenGL -lglfw3 -lGLEW
endif

CC = g++
RM = rm

objects = $(src:.cpp=.o)
cMain 	= $(addprefix $(srcDir),$(main))
cSrc 	= $(addprefix $(srcDir),$(src))
cObj 	= $(addprefix $(objDir),$(objects))
cBin 	= $(addprefix $(binDir),$(bin))

all: $(cBin)
	./$(cBin)

$(cBin): $(cMain) $(cObj)
	$(CC) $^ $(ldFlags) -o $@

$(objDir)%.o: $(srcDir)%.cpp $(srcDir)%.h
	$(CC) $(cFlags) $< -o $@

clean:
	$(RM) $(cObj)
