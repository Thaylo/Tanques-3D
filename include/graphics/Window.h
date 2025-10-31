#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

using namespace std;

class window
{
private:
	static void (*s_keyPressFunc)(int code);
	static void (*s_keyReleaseFunc)(int code);
	static void (*s_mouseFunc)(int type, int button, int x, int y);

	static void keyboardCallback(unsigned char key, int x, int y);
	static void keyboardUpCallback(unsigned char key, int x, int y);
	static void specialKeyCallback(int key, int x, int y);
	static void specialKeyUpCallback(int key, int x, int y);
	static void mouseCallback(int button, int state, int x, int y);

public:
	window();
	void showWindow();
	~window(){	}
	bool processWindow(void (*mouseFunc)(int type, int button, int x, int y), void (*keyPress)(int code), void (*keyRelease)(int code));
};

// Funcao cria a janela
window* initWindow();

// Efetivamente mostra na tela o conteudo
void showWindow(window* w);

#endif
