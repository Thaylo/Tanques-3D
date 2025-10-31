# Detect operating system
UNAME_S := $(shell uname -s)

# Target executable name
TARGET=jogoThaylo

# Compiler
CXX=g++

# Platform-specific flags
ifeq ($(UNAME_S),Darwin)
    # macOS
    CPPFLAGS=-framework OpenGL -framework GLUT -Wno-deprecated
else ifeq ($(UNAME_S),Linux)
    # Linux
    CPPFLAGS=-lm -lGLU -lGL -lglut -lX11
else
    # Default to Linux flags
    CPPFLAGS=-lm -lGLU -lGL -lglut -lX11
endif

# Object files
OBJECTS=Main.o \
Agent.o \
Enemy.o \
Terrain.o \
Projetil.o \
Window.o \
Control.o \
Controlable.o \
GLDraw.o \
joystick.o \
Ground.o \
Camera.o \
GameData.o \
Vector.o \
Timer.o \
oDrawable.o \
Movable.o \
Matter.o

all: ${TARGET}

%.o: %.cpp
	${CXX} -c $<

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${CPPFLAGS}

clean:
	rm -f ${TARGET} ${OBJECTS}

.PHONY: all clean
