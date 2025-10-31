/*
 * Movable.cpp
 *
 *  Created on: 14/10/2012
 *      Author: thaylo
 */

#include "Movable.h"

using namespace std;

Movable::Movable(){
	position = velocity = aceleration = 0;



	roll = pitch = yaw = v_roll = v_pitch = v_yaw = 0;

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);

}

Movable::Movable(const Vector &positionv)
{
	position = positionv;
	velocity = Vector(0,0,0);
	aceleration = Vector(0,0,0);
	roll= pitch= yaw = v_roll= v_pitch= v_yaw = 0;

	dir = Vector(1,0,0);
	side = Vector(0,1,0);
	up = Vector(0,0,1);
}

Vector Movable::getPosition() const
{
	return position;
}

Vector Movable::getVelocity() const
{
	return velocity;
}

Vector Movable::getAceleration() const
{
	return aceleration;
}

Vector Movable::getUp() const
{
	return up;
}

Vector Movable::getDir() const
{
	return dir;
}

Vector Movable::getSide() const
{
	return side;
}

void Movable::setPosition(const Vector &pos)
{
	position = pos;
}

void Movable::setVelocity(const Vector &vel)
{
	velocity = vel;
}

void Movable::setAcelerration(const Vector &acel)
{
	aceleration = acel;
}

void Movable::setUp(const Vector &upSet)
{
	up = upSet;
}

void Movable::setRoll(const double &rollRef)
{
	roll = rollRef;
}

void Movable::setPitch(const double &pitchRef)
{
	pitch = pitchRef;
}

void Movable::setYaw(const double &yawRef)
{
	yaw = yawRef;
}

double Movable::getRoll() const
{
	return roll;
}

double Movable::getPitch() const
{
	return pitch;
}

double Movable::getYaw() const
{
	return yaw;
}

void Movable::setVRoll(const double &vrollRef)
{
	v_roll = vrollRef;
}

void Movable::setVPitch(const double &vpitchRef)
{
	v_pitch = vpitchRef;
}

void Movable::setVYaw(const double &vyawRef)
{
	v_yaw = vyawRef;
}

void Movable::iterate()
{

	if( velocity.getLengthVector() > MOVABLE_MAX_VELOCITY)
	{
		velocity.setVectorLength(MOVABLE_MAX_VELOCITY);
	}


	// Aqui começam os códigos de atualização das variáveis de estado.

	roll += v_roll * TIME_STEP/500.0;
	pitch += v_pitch * TIME_STEP/500.0;
	yaw += v_yaw * TIME_STEP/500.0;


	position = position + velocity*TIME_STEP/1000.0;
	velocity = velocity + aceleration*TIME_STEP/1000.0 - velocity*MOVABLE_LINEAR_FRICTION*TIME_STEP/1000.0;


	up = Vector(0,0,1);
	side = side.rotateVector(up,v_yaw* TIME_STEP/1000.0).setVectorLength(1.0);
	dir = up.crossProduct(side).setVectorLength(1.0);

}
