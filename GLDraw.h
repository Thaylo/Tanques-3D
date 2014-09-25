#ifndef GLDRAW_H_
#define GLDRAW_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdarg.h>
#include "Constants.h"

typedef struct Image_{
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
} Image;

unsigned int getint(FILE *fp);

unsigned int getshort(FILE *fp);
int ImageLoad(char *filename, Image *image);

GLuint * LoadGLTextures(char *source, GLuint *texture);


// Esta funcao deve ser chamada antes de qualquer outra funcao para desenhar na tela
void initGl();

// Desenha uma linha ligando o ponto (x1,y1) ao ponto (x2,y2)
void drawLine(float x1, float y1, float x2, float y2);

// Desenha um retangulo com vertices nos pontos (x1,y1) e (x2,y2)
void drawRectangle(float x1, float y1, float x2, float y2,float z);

void drawPlane(float x1, float y1, float x2, float y2, float z);

// Desenha um triangulo com vertices nos pontos (x1,y1) e (x2,y2) e (x3,y3)
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float z);

// Desenha tudo depois desse comando na cor passada, ate que outro comando deste seja passado.
// red,green e blue devem ser valores entre 0.0 e 1.0
void setColor(float red, float green, float blue);

// Desenha um texto na tela nas posicoes x e y, os parametros sao passados exatamente como a
// funcao print de c, mas com as coordenadas x e y no inicio.
void drawText(float x, float y, char * msg, ...);


#endif
