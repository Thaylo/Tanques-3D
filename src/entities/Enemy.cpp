/*
 * Enemy.cpp
 *
 *  Created on: 19/04/2013
 *      Author: thaylo
 */

#include "Enemy.h"

namespace std {

Enemy::Enemy() {
	control = NULL;
	target = NULL;
}

Enemy::Enemy(Agent *targetAgent) {
	control = NULL;
	target = targetAgent;
}

void Enemy::controlAction()
{
	if(target == NULL)
	{
		cout << "Target not initialized for enemy\n";
		return;
	}
	Vector A = target->getPosition() - position;
	double seno = A.crossProduct(dir).getLengthVector();
	double cosseno = A.dotProduct(dir);
	double theta = atan2(seno,cosseno);


	if(theta > 0.02 && theta < M_PI)
	{
		v_yaw = 1;
	}
	else if(theta < -0.02 && theta >= M_PI)
	{
		v_yaw = -1;
	}
	else
	{
		v_yaw = 0;
	}

	Vector nDir = dir;
	if(A.getLengthVector() > 7)
	{
		aceleration = nDir.setVectorLength(MOVABLE_MAX_ACCELERATION);
	}
	else
	{
		aceleration = 0;
		if(A.getLengthVector() < 13 && fabs(theta) < M_PI/15) shoot();
	}
}

Enemy::~Enemy() {
}

void Enemy::shoot()
{
	if (reload > ROUNDS_RELOAD * ROUNDS_RELOAD_HANDICAP_FOR_AI)
	{
		Agent::shoot();
	}
}

} /* namespace std */
