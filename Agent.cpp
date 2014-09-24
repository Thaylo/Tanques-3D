/*
 * Agent.cpp
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#include "Agent.h"
#include "GameData.h"

using namespace std;

extern GameData* gameData;

void Agent::glVectorT(Vector v)
{
	glVertex3f(v.getX(),v.getY(),v.getZ());
}

void Agent::desenhaRadar()
{
	double size = 1.0/16;
	if(maisProximo == NULL) return;
	Vector direcaoMaisProx = maisProximo->getPosition() - position;
	double seno = dir.crossProduct(direcaoMaisProx).getLengthVector();
	double cosseno = dir.dotProduct(direcaoMaisProx);

	double phi;
	if(cosseno == 0) phi = M_PI/2;
	else phi = atan2(seno,cosseno);


	glTranslated(position.getX(),position.getY(),position.getZ()+0.2);

	Vector local_dir = dir;
	Vector local_side = side;
	Vector local_up = up;

	local_dir = dir.rotateVector(up,phi).setVectorLength(1.0);
	local_side = up.crossProduct(dir);

	glBegin(GL_TRIANGLES);
	glVectorT(local_dir*2*size);
	glVectorT(local_side*(-size/2)-local_up*(size/5));
	glVectorT(local_side*(size/2)-local_up*(size/5));
	glEnd();

	glBegin(GL_TRIANGLES);
	glVectorT(local_dir*2*size);
	glVectorT(local_side*(-size/2)+local_up*(size/5));
	glVectorT(local_side*(size/2)+local_up*(size/5));
	glEnd();

	glBegin(GL_QUADS);
		glVectorT(local_side*(-size/2)+local_up*(size/5));
		glVectorT(local_side*(size/2)+local_up*(size/5));
		glVectorT(local_side*(size/2)-local_up*(size/5));
		glVectorT(local_side*(-size/2)-local_up*(size/5));
	glEnd();

	glTranslated(-position.getX(),-position.getY(),-position.getZ()-0.2);
	//glRotatef(-phi*180/M_PI,0,0,1);
}

Agent::Agent()
{
	maisProximo = NULL;
	id = 0;
	destroy = false;
	recarga = 0;
	position = Vector(0,0,0);
	velocity = Vector(-2,0,0); // Evita problema de inicialização na camera (GAMB, POG).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	disparando = false;
}

Agent::Agent(Vector pos)
{
	maisProximo = NULL;
	id = 0;
	destroy = false;
	recarga = 0;
	position = pos;
	velocity = Vector(-2,0,0); // Evita problema de inicialização na camera (GAMB, POG).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	disparando = false;

}

void Agent::setId(int idx)
{
	id = idx;
}

int Agent::getId()
{
	return id;
}

void glNormalT(Vector v)
{
	glNormal3f(v.getX(),v.getY(),v.getZ());
}

 void Agent::draw()
{
		double size = 1/8.0;
		Vector dirl = dir*size*(1+sqrt(5))/2.0; // Razão de ouro!
		Vector sidel = side*size;
		Vector upl = up*size;



		//glColor4f(1,1,1,0);



		Vector P1 = dirl - sidel;
		Vector P2 = P1 + upl;
		Vector P3 = P2 + sidel * 2;
		Vector P4 = P3 - upl;

		Vector P5 = P1 - dirl * 2;
		Vector P6 = P2 - dirl * 2;
		Vector P7 = P3 - dirl * 2;
		Vector P8 = P4 - dirl * 2;

		// Ajustes para caber na textura.
		P5 = P5 + dirl*0.23;
		P8 = P8 + dirl*0.23;

//		char t0[20] = "frente.bmp";
//		char t1[20] = "verso.bmp";
//		char t2[20] = "lateralDir.bmp";
//		char t3[20] = "lateralEsq.bmp";
//		char t4[20] = "frente.bmp""topo.bmp";
//



		glColor3f(0.2,0.7,0.1);

		if(id == PLAYER_ID)
		desenhaRadar();

		glTranslatef(position.getX(),position.getY(),position.getZ());




		// Fazendo a frente do tanque.
		glBindTexture(GL_TEXTURE_2D, tanque[0]);
		glBegin(GL_QUADS);
			glNormalT(dir);
			glTexCoord2f(0.0f, 0.0f); glVectorT(P1);
			glTexCoord2f(1.0f, 0.0f); glVectorT(P4);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P3);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P2);
		glEnd();


		// Fazendo o verso do tanque.
		glBindTexture(GL_TEXTURE_2D, tanque[1]);
		glBegin(GL_QUADS);
			glNormalT(dir*(-1.0));
			glTexCoord2f(0.0f, 0.0f); glVectorT(P5);
			glTexCoord2f(1.0f, 0.0f); glVectorT(P8);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P7);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P6);
		glEnd();



		// Fazendo a lateral direita do tanque.
		glBindTexture(GL_TEXTURE_2D, tanque[2]);
		glBegin(GL_QUADS);
			glNormalT(side*(-1.0));
			glTexCoord2f(0.15f, 0.0f); glVectorT(P5);
			glTexCoord2f(1.0f-0.06f, 0.0f); glVectorT(P1);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P2);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P6);
		glEnd();

		// Fazendo a lateral esquerda do tanque.
		glBindTexture(GL_TEXTURE_2D, tanque[3]);
		glBegin(GL_QUADS);
			glNormalT(side);
			glTexCoord2f(0.06f, 0.0f); glVectorT(P4);
			glTexCoord2f(1.0f-0.15f, 0.0f); glVectorT(P8);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P7);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P3);
		glEnd();


		// Fazendo o topo do tanque.
		glBindTexture(GL_TEXTURE_2D, tanque[4]);
		glBegin(GL_QUADS);
			glNormalT(up);
			glTexCoord2f(0.0f, 0.0f); glVectorT(P6);
			glTexCoord2f(1.0f, 0.0f); glVectorT(P2);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P3);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P7);
		glEnd();


		glTranslatef(-position.getX(),-position.getY(),-position.getZ());
}

void Agent::iterate()
{


	double dist = 1000;
	double menor_dist = dist;
	for(int i = 0; i < gameData->getQuant(); i++)
	{
		Agent *ai = gameData->getAgents()[i];
		dist = (ai->getPosition() - position).getLengthVector();
		if(dist < menor_dist && ai->getId() != PLAYER_ID)
		{
			Projetil *j = dynamic_cast <Projetil *>(ai);
			if(j == NULL)
			maisProximo = ai;
		}
	}
//	if(id == PLAYER_ID)
//	cout << maisProximo->getId() << "\n";

	recarga++;
	controlAction();
	Controlable::iterate();
}

void Agent::controlAction()
{
	if(!control) return;

	double vert = 0, hor = 0;

	if(control->arrowUp) vert += 1;

	if(control->arrowDown) vert -= 1;

	if(control->arrowRight) hor += 1;

	if(control->arrowLeft) hor -= 1;

	Vector nDir = dir;

	if(control->space) atirar();

	aceleration = nDir.setVectorLength(MOVABLE_MAX_ACCELERATION) * vert; // O tanque sempre vai na direção oposta ao motor (portanto dir).

	v_yaw = -hor;


}

void Agent::atirar()
{
	if(recarga > ROUNDS_RECARGA)
	{
		recarga = 0;
		disparando = true;
	}
}

void Agent::destroyNow()
{
	destroy = true;
}

bool Agent::isToDestroy()
{
	return destroy;
}

bool Agent::checkDisparo()
{
	if(disparando)
	{
		disparando = false;
		return true;
	}
	else return false;
}

Agent::~Agent() {
	// TODO Auto-generated destructor stub
}
