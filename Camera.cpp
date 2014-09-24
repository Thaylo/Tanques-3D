#include "Camera.h"


Camera::Camera()
{
	tracked = NULL;
	position = Vector(0,0,1);
	velocity = Vector(0,0,-1);
	dir = Vector(1,0,0);
	up = Vector(0,1,0);
	side = Vector(0,0,1);

}

Camera::Camera(Movable *track)
{
	tracked = track;
	position = Vector(0,0,1);
	velocity = Vector(0,0,-1);
	dir = Vector(1,0,0);
	up = Vector(0,1,0);
	side = Vector(0,0,1);

}

void Camera::draw()
{
	glTranslatef(position.getX(),position.getY(),position.getZ());
	glColor3f(0,0,1);
	drawRectangle(-0.1,-0.1,0.1,0.1,0);
	glTranslatef(-position.getX(),-position.getY(),-position.getZ());

}

void Camera::posiciona()
{
   gluLookAt(
	position.getX(),position.getY(),position.getZ(),
	tracked->getPosition().getX(),tracked->getPosition().getY(),tracked->getPosition().getZ(),
	up.getX(),up.getY(),up.getZ());

}

//Movable *Camera::getTracked()
//{
//	return tracked;
//}

void Camera::iterate()
{

	double factor = 0.6;
	position = tracked->getPosition() - tracked->getDir().setVectorLength(factor) - tracked->getVelocity().setVectorLength(factor) + Vector(0,0,factor);

	dir = tracked->getDir().setVectorLength(1.0) + tracked->getVelocity().setVectorLength(factor);
	up = tracked->getUp();
	side = dir.crossProduct(up).setVectorLength(1.0);
}

Camera::~Camera()
{

}
