CPPFLAGS=-lm -lGLU -lGL -lglut -lX11
TARGET=jogoThaylo

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
	g++ -c $<

${TARGET}: ${OBJECTS}
	g++ -o ${TARGET} ${OBJECTS} ${CPPFLAGS}

clean:
	rm -f ${TARGET} ${OBJECTS}
