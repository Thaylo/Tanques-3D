/*
 * Ground.cpp
 *
 *  Created on: 28/03/2013
 *      Author: thaylo
 */

#include "Ground.h"
extern GLuint t[2];

Ground::Ground() {
	tracked = NULL;

}

Ground::Ground(Agent *a) {
	tracked = a;

}

void Ground::iterate()
{
	position = tracked->getPosition();
}

void Ground::draw()
{
	// begin drawing a cube
	double aresta = 60;

	glBindTexture(GL_TEXTURE_2D, t[0]);   // Escolhe a textura a ser usada.
		glBegin(GL_QUADS);
		glColor4f(1,1,1,0);
		glNormal3f(0,0,1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-aresta, -aresta,  0);	// Bottom Left Of The Texture and Quad

		glNormal3f(0,0,1);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( aresta, -aresta,  0);	// Bottom Right Of The Texture and Quad

		glNormal3f(0,0,1);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( aresta,  aresta,  0);	// Top Right Of The Texture and Quad

		glNormal3f(0,0,1);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-aresta,  aresta,  0);	// Top Left Of The Texture and Quad
		glEnd();

		glTranslatef(tracked->getPosition().getX(),tracked->getPosition().getY(),tracked->getPosition().getZ());

		aresta = 100;

		glBindTexture(GL_TEXTURE_2D, t[1]);   // Escolhe a textura a ser usada.
		glBegin(GL_QUADS);		                // begin drawing a cube
			glNormal3f(0,0,1);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-aresta, -aresta,  5);	// Bottom Left Of The Texture and Quad

			glNormal3f(0,0,1);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( aresta, -aresta,  5);	// Bottom Right Of The Texture and Quad

			glNormal3f(0,0,1);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( aresta,  aresta,  5);	// Top Right Of The Texture and Quad

			glNormal3f(0,0,1);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-aresta,  aresta,  5);	// Top Left Of The Texture and Quad
		glEnd();
		glTranslatef(-tracked->getPosition().getX(),-tracked->getPosition().getY(),-tracked->getPosition().getZ());


}

Ground::~Ground() {
}
