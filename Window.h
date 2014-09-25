#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

class window
{
private:
	Display* g_pDisplay;
	Window g_window;
	int g_bDoubleBuffered;
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
