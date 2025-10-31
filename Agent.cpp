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

void Agent::drawRadar()
{
	double size = 1.0/16;
	if(closest == NULL) return;
	Vector directionToClosest = closest->getPosition() - position;
	double seno = dir.crossProduct(directionToClosest).getLengthVector();
	double cosseno = dir.dotProduct(directionToClosest);

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
	closest = NULL;
	id = 0;
	destroy = false;
	reload = 0;
	position = Vector(0,0,0);
	velocity = Vector(-2,0,0); // Avoids camera initialization problem (workaround).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	firing = false;
}

Agent::Agent(Vector pos)
{
	closest = NULL;
	id = 0;
	destroy = false;
	reload = 0;
	position = pos;
	velocity = Vector(-2,0,0); // Avoids camera initialization problem (workaround).

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
	firing = false;

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

		// Adjustments to fit texture.
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
		drawRadar();

		glTranslatef(position.getX(),position.getY(),position.getZ());




		// Drawing the front of the tank.
		glBindTexture(GL_TEXTURE_2D, tank[0]);
		glBegin(GL_QUADS);
			glNormalT(dir);
			glTexCoord2f(0.0f, 0.0f); glVectorT(P1);
			glTexCoord2f(1.0f, 0.0f); glVectorT(P4);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P3);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P2);
		glEnd();


		// Drawing the back of the tank.
		glBindTexture(GL_TEXTURE_2D, tank[1]);
		glBegin(GL_QUADS);
			glNormalT(dir*(-1.0));
			glTexCoord2f(0.0f, 0.0f); glVectorT(P5);
			glTexCoord2f(1.0f, 0.0f); glVectorT(P8);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P7);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P6);
		glEnd();



		// Drawing the right side of the tank.
		glBindTexture(GL_TEXTURE_2D, tank[2]);
		glBegin(GL_QUADS);
			glNormalT(side*(-1.0));
			glTexCoord2f(0.15f, 0.0f); glVectorT(P5);
			glTexCoord2f(1.0f-0.06f, 0.0f); glVectorT(P1);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P2);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P6);
		glEnd();

		// Drawing the left side of the tank.
		glBindTexture(GL_TEXTURE_2D, tank[3]);
		glBegin(GL_QUADS);
			glNormalT(side);
			glTexCoord2f(0.06f, 0.0f); glVectorT(P4);
			glTexCoord2f(1.0f-0.15f, 0.0f); glVectorT(P8);
			glTexCoord2f(1.0f, 1.0f); glVectorT(P7);
			glTexCoord2f(0.0f, 1.0f); glVectorT(P3);
		glEnd();


		// Drawing the top of the tank.
		glBindTexture(GL_TEXTURE_2D, tank[4]);
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
	double min_dist = dist;
	for(int i = 0; i < gameData->getCount(); i++)
	{
		Agent *ai = gameData->getAgents()[i];
		dist = (ai->getPosition() - position).getLengthVector();
		if(dist < min_dist && ai->getId() != PLAYER_ID)
		{
			Projectile *j = dynamic_cast <Projectile *>(ai);
			if(j == NULL)
			closest = ai;
		}
	}
//	if(id == PLAYER_ID)
//	cout << closest->getId() << "\n";

	reload++;
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

	if(control->space) shoot();

	aceleration = nDir.setVectorLength(MOVABLE_MAX_ACCELERATION) * vert; // The tank always goes in the opposite direction of the motor (therefore dir).

	v_yaw = -hor;


}

void Agent::shoot()
{
	if(reload > ROUNDS_RELOAD)
	{
		reload = 0;
		firing = true;
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

bool Agent::checkFire()
{
	if(firing)
	{
		firing = false;
		return true;
	}
	else return false;
}

Agent::~Agent() {
}
