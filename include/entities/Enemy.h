/*
 * Enemy.h
 *
 *  Created on: 19/04/2013
 *      Author: thaylo
 */

#ifndef ENEMY_H_
#define ENEMY_H_

#include "Agent.h"

namespace std {

class Enemy: public std::Agent {
private:
	Agent *target;
public:
	Enemy();
	Enemy(Agent *targetAgent);
	void controlAction();
	virtual ~Enemy();
	virtual void shoot();
};

} /* namespace std */
#endif /* ENEMY_H_ */
