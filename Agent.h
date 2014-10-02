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

extern GLuint tanque[5];


namespace std {

class Agent : public Controlable, public oDrawable, public Matter{
private:
	int id;
	bool destroy;
public:
	bool disparando;
	int recarga;
	Agent *maisProximo;

	Agent();
	Agent(Vector x);
	void setId(int idx);
	int getId();
	void desenhaRadar();
	void draw();
	void controlAction();

	virtual void atirar();
	bool checkDisparo();

	void destroyNow();
	bool isToDestroy();
	void iterate();
	void glVectorT(Vector v);
	~Agent();
};

void glVectorT(Vector v);


} /* namespace std */
#endif /* AGENT_H_ */
