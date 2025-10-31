/*
 * Projectile.h
 *
 *  Created on: 18/04/2013
 *      Author: thaylo
 */

#include "Agent.h"

#ifndef PROJECTILE_H_
#define PROJECTILE_H_


class Projectile : public Agent{
	int distance;
	Vector initialPos;
public:
	Projectile();
	Projectile(Vector pos);
	Projectile(Agent *shooter);
	void draw();
	void iterate();
	virtual ~Projectile();
};

#endif /* PROJECTILE_H_ */
