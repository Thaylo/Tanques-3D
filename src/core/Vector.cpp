/*
 * Vector.cpp
 *
 *  Created on: 09/10/2012
 *      Author: thaylo
 */

#include "Vector.h"

Vector::Vector()
{
	x = y = z = 0;
}

Vector::Vector(double xv, double yv, double zv)
{
	x = xv;
	y = yv;
	z = zv;
}


ostream& operator<<(ostream& output, const Vector& p) {
    output << "(" <<  p.getX() << ", " << p.getY() << ", " << p.getZ() << ")" ;
    return output;  // for multiple << operators.
}

double Vector::getX()const {
	return x;
}

double Vector::getY()const {
	return y;
}

double Vector::getZ()const {
	return z;
}

double Vector::setX(double xv){
	return(x = xv);
}


double Vector::setY(double yv){
	return(y = yv);
}

double Vector::setZ(double zv){
	return(z = zv);
}

Vector Vector::operator=(const Vector &v2){
	Vector r;
	r.x = x = v2.x;
	r.y = y = v2.y;
	r.z = z = v2.z;
	return r;
}

Vector Vector::operator=(double k){
	if(k != 0)
	{
		cout << "Atribuicao invalida de double -> Vector\n";
	}
	Vector r;
	r.x = x = 0;
	r.y = y = 0;
	r.z = z = 0;
	return r;
}

Vector Vector::operator+(const Vector &v2)const {
	Vector r;
	r.x = x + v2.x;
	r.y = y + v2.y;
	r.z = z + v2.z;
	return r;
}

Vector Vector::operator-(const Vector &v2)const {
	Vector r;
	r.x = x - v2.x;
	r.y = y - v2.y;
	r.z = z - v2.z;
	return r;
}


Vector Vector::operator*(double k)const {
	Vector r;
	r.x = x*k;
	r.y = y*k;
	r.z = z*k;
	return r;
}

Vector Vector::operator/(double k)const {
	Vector r;
	r.x = x/k;
	r.y = y/k;
	r.z = z/k;
	return r;
}

double Vector::getLengthVector()const
{
	return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
}

Vector Vector::setVectorLength(double length)
{
	Vector r;
	double oldLength = this->getLengthVector();
	if(oldLength != 0)
	{
		x *= length/oldLength;
		y *= length/oldLength;
		z *= length/oldLength;
	}
	else {
		//cout << "\nWarning, null vector in setVectorLength() call!!!\n";
	}
	r = *this;
	return r;
}

double Vector::distanceVector(const Vector &v2)const
{
	Vector temp = (*this - v2);
	return temp.getLengthVector();
}

double Vector::dotProduct(const Vector &B)const
{
	return x*B.x + y*B.y + z*B.z;
}

Vector Vector::getNormalVector()const
{
	Vector r(-y,x,0);
	r.setVectorLength(1.0);
	return r;
}

Vector Vector::crossProduct(const Vector &v) const
{
	double i,j,k;
	i = y * v.z - z*v.y;
	j =	z * v.x - x*v.z;
	k = x * v.y - y*v.x;
	return Vector(i,j,k);
}

double Vector::angleVectors(const Vector &v) const
{
	Vector a = *this;
	Vector b = v;
	return atan2(a.crossProduct(b).getLengthVector(), a.dotProduct(b));
}

// IN RADIANS
Vector Vector::rotateVector(const Vector &v, double phi)
{
	double R[3][3]; // http://en.wikipedia.org/wiki/Rotation_matrix
	Vector u = v;
	u.setVectorLength(1.0);

	R[0][0] = cos(phi) + u.x*u.x*(1-cos(phi));
	R[0][1] = u.x*u.y*(1-cos(phi)) - u.z*sin(phi);
	R[0][2] = u.x*u.z*(1-cos(phi)) + u.y*sin(phi);

	R[1][0] = u.y*u.x*(1-cos(phi)) + u.z*sin(phi);
	R[1][1] = cos(phi) + u.y*u.y*(1-cos(phi));
	R[1][2] = u.y*u.z*(1-cos(phi)) - u.x*sin(phi);

	R[2][0] = u.z*u.x*(1-cos(phi)) - u.y*sin(phi);
	R[2][1] = u.z*u.y*(1-cos(phi)) + u.x * sin(phi);
	R[2][2] = cos(phi) + u.z*u.z*(1-cos(phi));

	double unrotated[3];
	unrotated[0] = this->x;
	unrotated[1] = this->y;
	unrotated[2] = this->z;

	double rotated[3] = {0.0,0.0,0.0};
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			rotated[i] += R[i][j]*unrotated[j];
		}
	}

	return Vector(rotated[0],rotated[1],rotated[2]);
}

Vector::~Vector()
{

}
