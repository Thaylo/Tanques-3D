/*
 * Vector.h
 *
 *  Created on: 09/10/2012
 *      Author: thaylo
 */


#ifndef VECTOR_H
#define VECTOR_H 1

#include <iostream>
#include <cmath>

using namespace std;


class Vector{

public:

	Vector();
	Vector(double xv, double yv, double zv);
	double getX()const ;
	double getY()const ;
	double getZ()const ;

	double setX(double xv);
	double setY(double yv);
	double setZ(double yv);

	Vector operator+(const Vector &v2)const ;
	Vector operator=(const Vector &v2);
	Vector operator=(double k);
	Vector operator-(const Vector &v2)const ;


	Vector operator*(double k)const ;
	Vector operator/(double k)const ;

	double getLengthVector()const ;
	Vector setVectorLength(double length);

	double distanceVector(const Vector &v2)const ;



	double dotProduct(const Vector &B)const ;
	Vector getNormalVector()const ;
	Vector crossProduct(const Vector &v) const;
	double angleVectors(const Vector &v) const;
	Vector rotateVector(const Vector &v, double theta_rad);
	double getArea3Vectors(const Vector &vec2, const Vector &vec3)const ;

	~Vector();

private:
	double x,y,z;
};

ostream& operator<<(ostream& output, const Vector& p);

#endif // Vector.h
