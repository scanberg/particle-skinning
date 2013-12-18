main 	= main.cpp
src		= Animation.cpp Body.cpp Camera.cpp crc32.cpp Entity.cpp GPUParticleSystem.cpp Material.cpp Model.cpp ParticleSkinnedModel.cpp Shader.cpp StringHash.cpp Texture2D.cpp Transform.cpp

# use some O flag to speed up when shit is working
cFlags 	= -c -Wall -g -std=c++11
ldFlags = -lassimp
objDir  = obj/
srcDir  = src/
binDir 	=
bin  	= Program

CC = g++

UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
	ldFlags += -lglfw -lGL -lGLEW
else ifeq ($(UNAME), mingw)
	ldFlags += -lglfw3 -lglew32 -lopengl32
else ifeq ($(UNAME), Darwin)
	cFlags 	+= -I/usr/local/include
	ldFlags += -I/usr/local/include
	ldFlags += -L/usr/local/lib
	ldFlags += -framework Cocoa -framework OpenGL -lglfw3 -lGLEW
	CC = clang++
endif

RM = rm

objects = $(src:.cpp=.o)
cMain 	= $(addprefix $(srcDir),$(main))
cSrc 	= $(addprefix $(srcDir),$(src))
cObj 	= $(addprefix $(objDir),$(objects))
cBin 	= $(addprefix $(binDir),$(bin))

all: $(cBin)
	./$(cBin) data/hellknight/hellknight.md5mesh data/hellknight/walk7.md5anim

$(cBin): $(cMain) $(cObj)
	$(CC) -std=c++11 $^ $(ldFlags) -o $@

$(objDir)%.o: $(srcDir)%.cpp $(srcDir)%.h
	$(CC) $(cFlags) $< -o $@

clean:
	$(RM) $(cObj)
