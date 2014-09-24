/*
 * Projetil.cpp
 *
 *  Created on: 18/04/2013
 *      Author: thaylo
 */

#include "Projetil.h"
#include "GameData.h"


extern GameData* gameData;

Projetil::Projetil()
{
	initialPos = position;
	distance = 0;
	position = Vector(0,0,0);
	velocity = Vector(0.0001,0,0); // Evita problema de inicialização na camera (GAMB, POG).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	disparando = false;
}


Projetil::Projetil(Agent *atirador)
{
	initialPos = position;
	distance = 0;
	position = atirador->getPosition();
	velocity = atirador->getDir(); // Evita problema de inicialização na camera (GAMB, POG).
	velocity.setVectorLength(MOVABLE_MAX_VELOCITY*3);

	dir = atirador->getDir();

	side = atirador->getSide();
	up = atirador->getUp();
	disparando = false;
}

void Projetil::draw()
{

	glColor3f(1,0,0);
	glTranslated(position.getX(),position.getY(),position.getZ());

	double size = 1/5.0;

	glBegin(GL_TRIANGLES);
	glVectorT(dir*2*size);
	glVectorT(side*(-size/2)-up*(size/5));
	glVectorT(side*(size/2)-up*(size/5));
	glEnd();


	glBegin(GL_TRIANGLES);
	glVectorT(dir*2*size);
	glVectorT(side*(-size/2)+up*(size/5));
	glVectorT(side*(size/2)+up*(size/5));
	glEnd();

	glBegin(GL_TRIANGLES);
	glVectorT(dir*2*size);
	glVectorT(side*(-size/2)+up*(size/5));
	glVectorT(side*(-size/2)-up*(size/5));
	glEnd();

	glBegin(GL_TRIANGLES);
	glVectorT(dir*2*size);
	glVectorT(side*(size/2)+up*(size/5));
	glVectorT(side*(size/2)-up*(size/5));
	glEnd();

	glTranslated(-position.getX(),-position.getY(),-position.getZ());

}

void Projetil::iterate()
{
	Agent::iterate();
	//velocity = dir;
	velocity.setVectorLength(MOVABLE_MAX_VELOCITY );

	Agent **ag = gameData->getAgents();
	int quant = gameData->getQuant();
	for(int i = 0; i < quant; i++)
	{
		double dist = (ag[i]->getPosition()-position).getLengthVector();
		if(dist < 0.5 && (this->getId() != ag[i]->getId()))
		{
			ag[i]->destroyNow();
		}
		if((position - initialPos).getLengthVector()> 40)
		{
			destroyNow();
		}
	}
}



Projetil::~Projetil() {
	// TODO Auto-generated destructor stub
}
