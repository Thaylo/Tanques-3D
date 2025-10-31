/*
 * Projectile.cpp
 *
 *  Created on: 18/04/2013
 *      Author: thaylo
 */

#include "Projectile.h"
#include "GameData.h"


extern GameData* gameData;

Projectile::Projectile()
{
	initialPos = position;
	distance = 0;
	position = Vector(0,0,0);
	velocity = Vector(0.0001,0,0); // Avoids camera initialization problem (workaround).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	firing = false;
}


Projectile::Projectile(Agent *shooter)
{
	initialPos = position;
	distance = 0;
	position = shooter->getPosition();
	velocity = shooter->getDir(); // Avoids camera initialization problem (workaround).
	velocity.setVectorLength(MOVABLE_MAX_VELOCITY*3);

	dir = shooter->getDir();

	side = shooter->getSide();
	up = shooter->getUp();
	firing = false;
}

void Projectile::draw()
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

void Projectile::iterate()
{
	Agent::iterate();
	//velocity = dir;
	velocity.setVectorLength(MOVABLE_MAX_VELOCITY );

	Agent **ag = gameData->getAgents();
	int count = gameData->getCount();
	for(int i = 0; i < count; i++)
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



Projectile::~Projectile() {
}
