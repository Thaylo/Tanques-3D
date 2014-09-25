/*
 * Projetil.h
 *
 *  Created on: 18/04/2013
 *      Author: thaylo
 */

#include "Agent.h"

#ifndef PROJETIL_H_
#define PROJETIL_H_


class Projetil : public Agent{
	int distance;
	Vector initialPos;
public:
	Projetil();
	Projetil(Vector pos);
	Projetil(Agent *atirador);
	void draw();
	void iterate();
	virtual ~Projetil();
};

#endif /* PROJETIL_H_ */
