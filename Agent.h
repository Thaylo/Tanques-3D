/*
 * Agent.h
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "Movable.h"
#include "oDrawable.h"
#include "Matter.h"
#include "Controlable.h"

extern GLuint tank[5];


namespace std {

class Agent : public Controlable, public oDrawable, public Matter{
private:
	int id;
	bool destroy;
public:
	bool firing;
	int reload;
	Agent *closest;

	Agent();
	Agent(Vector x);
	void setId(int idx);
	int getId();
	void drawRadar();
	void draw();
	void controlAction();

	virtual void shoot();
	bool checkFire();

	void destroyNow();
	bool isToDestroy();
	void iterate();
	void glVectorT(Vector v);
	~Agent();
};

void glVectorT(Vector v);


} /* namespace std */
#endif /* AGENT_H_ */
