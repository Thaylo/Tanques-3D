#include "GLDraw.h"

#define Width 800
#define Height 800

GLuint t[2];
GLuint tanque[5];

GLfloat mat_specular[] = { 1.0, 1, 1, 1};
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[4] = { 0.0, 0.0, -10000, 1.0 };

unsigned int getint(FILE *fp)
{
  int c, c1, c2, c3;

  // Pega 4 bytes
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

  return ((unsigned int) c) +
    (((unsigned int) c1) << 8) +
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

unsigned int getshort(FILE *fp)
{
  int c, c1;

  //Pega 2 bytes
  c = getc(fp);
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // Tamanho da imagem em bytes.
    unsigned long i;                    // Contador padrão.
    unsigned short int planes;          // Numero de planos na imagem (deve ser 1).
    unsigned short int bpp;             // Numero de bits por pixel (deve ser 24)
    char temp;                          // Usado para converter cores de bgr para rgb.

    // Certificando-se de que o arquivo está no local.
    if ((file = fopen(filename, "rb")) == NULL) {
      printf("Arquivo nao encontrado: %s\n",filename);
      return 0;
    }

    // Procura atravez do cabeçalho do bmp, parando nas especificações de width/height:
    fseek(file, 18, SEEK_CUR);

    // Não foi feita uma verificação rigorosa de erros.

    // Lendo a largura
    image->sizeX = getint (file);
    printf("Largura de %s: %lu\n", filename, image->sizeX);

    // Lendo a altura
    image->sizeY = getint (file);
    printf("Algura de %s: %lu\n", filename, image->sizeY);

    // Calculo do tamanho (size) (assumindo 24 bits ou 3 bytes por pixel).
    size = image->sizeX * image->sizeY * 3;

    // Le o numero de planos
    planes = getshort(file);
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // Le o numero de bytes por pixel
    bpp = getshort(file);
    if (bpp != 24) {
      printf("Bpp de %s nao e 24: %u\n", filename, bpp);
      return 0;
    }

    // Salta o restante do cabeçalho
    fseek(file, 24, SEEK_CUR);

    // Le os pixels
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Erro alocando memoria para informacoes de cor\n");
	return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Erro lendo informacoes de cor de %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }
    // Terminamos.
    return 1;
}


// Não modifique ainda
GLuint * LoadGLTextures(char *source, GLuint *texture, int pos) {
    // Carrega textura
    Image *image1;

    // Aloca espaço para a textura
    image1 = (Image *) malloc(sizeof(Image));

    if (image1 == NULL){
		printf("Erro alocando espaco para a imagem\n");
		exit(0);
    }

    if (!ImageLoad(source, image1)){
    	exit(1);
    }

    // Cria textura
    glGenTextures(1, &texture[pos]);
    glBindTexture(GL_TEXTURE_2D, texture[pos]);   // Textura 2D (dimensoes x e y)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // reescala linearmente quando a imagem for maior que a textura
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // idem para quando for menor

    // textura 2D, nivel de detalhe 0 (normal), 3 componentes (vermelho, verde, azul), tamanho x da imagem, tamanho y da imagem,
    // borda 0 (normal), data (dados) de cores rgb, unsigned byte data, e finalmente os dados propriamente ditos.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    return texture;
}

void initGl()
{
	int fudido = 0;
	glutInit(&fudido,NULL);

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

	glScalef(1.0/SCALE,1.0/SCALE,1.0/SCALE);
	gluPerspective(30.0f,800.0/800.0,0.1f,10000.0f);



	// allocate a texture name
	glGenTextures( 1, t );

	char source1[20] = "texture.bmp";
	char source2[20] = "sky.bmp";

	int pos = 0;
	LoadGLTextures(source1, t,pos);				// Carrega uma textura

	pos = 1;
	LoadGLTextures(source2, t,pos);				// Carrega uma textura

	char t0[20] = "frente.bmp";
	char t1[20] = "verso.bmp";
	char t2[20] = "lateralDir.bmp";
	char t3[20] = "lateralEsq.bmp";
	char t4[20] = "topo.bmp";

	LoadGLTextures(t0,tanque,0);
	LoadGLTextures(t1,tanque,1);
	LoadGLTextures(t2,tanque,2);
	LoadGLTextures(t3,tanque,3);
	LoadGLTextures(t4,tanque,4);


	glEnable(GL_TEXTURE_2D);			// ativa mapeamento por textura

}


void setColor(float red, float green, float blue)
{
	glColor3f(red, green, blue);
}

void drawLine(float x1, float y1, float x2, float y2)
{
	glBegin(GL_LINES);

		glVertex2f(x1,y1);
		glVertex2f(x2,y2);

	glEnd();
}

void drawRectangle(float x1, float y1, float x2, float y2, float z)
{
	glBegin(GL_QUADS);

		glVertex3f(x1,y1,z);
		glVertex3f(x2,y1,z);
		glVertex3f(x2,y2,z);
		glVertex3f(x1,y2,z);

	glEnd();
}

void drawPlane(float x1, float y1, float x2, float y2, float z)
{
	glBegin(GL_QUADS);

		glVertex3f(x1,y1,0);
		glVertex3f(x2,y1,-1);
		glVertex3f(x2,y2,1);
		glVertex3f(x1,y2,0);

	glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float z)
{
	glBegin(GL_TRIANGLES);

		glVertex3f(x1,y1,z);
		glVertex3f(x2,y2,z);
		glVertex3f(x3,y3,z);

	glEnd();
}



void drawText(float x, float y, char * msg, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, msg);
	vsprintf(buf, msg, args);
	va_end(args);

	int l,i;

	l = strlen(buf);
	glRasterPos2f(x, y);
	for( i=0; i < l; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buf[i]);
	}
}













