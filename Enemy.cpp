/*
 * Enemy.cpp
 *
 *  Created on: 19/04/2013
 *      Author: thaylo
 */

#include "Enemy.h"

namespace std {

Enemy::Enemy() {
	// TODO Auto-generated constructor stub
	control = NULL;
	target = NULL;
}

Enemy::Enemy(Agent *alvo) {
	// TODO Auto-generated constructor stub
	control = NULL;
	target = alvo;
}

void Enemy::controlAction()
{
	if(target == NULL)
	{
		cout << "Alvo nao inicializado para o inimigo\n";
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
		if(A.getLengthVector() < 13 && fabs(theta) < M_PI/15) atirar();
	}
}

Enemy::~Enemy() {
	// TODO Auto-generated destructor stub
}

virtual void Enemy::atirar()
{
	if (recarga > ROUNDS_RECARGA * ROUNDS_RECARGA_HANDICAP_FOR_IA)
	{
		Agent::atirar();
	}
}

} /* namespace std */
