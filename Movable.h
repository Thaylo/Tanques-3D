/*
 * Movable.h
 *
 *  Created on: 14/10/2012
 *      Author: thaylo
 */

#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "Vector.h"
#include "Constants.h"
namespace std {

class Movable {
protected:
	Vector position;
	Vector velocity;
	Vector aceleration;

	Vector up;
	Vector dir;
	Vector side;

	double roll, pitch, yaw; // http://www.toymaker.info/Games/assets/images/yawpitchroll.jpg
	double v_roll, v_pitch, v_yaw;

public:

	Movable();
	Movable(const Vector &positionv);

	virtual void iterate();



	Vector getPosition() const;
	Vector getVelocity() const;
	Vector getAceleration() const;

	Vector getUp() const;
	Vector getDir() const;
	Vector getSide() const;


	void setPosition(const Vector &pos);
	void setVelocity(const Vector &vel);
	void setAcelerration(const Vector &acel);
	void setUp(const Vector &upSet);


	void setRoll(const double &rollRef);
	void setPitch(const double &pitchRef);
	void setYaw(const double &yawRef);

	double getRoll() const;
	double getPitch() const;
	double getYaw() const;

	void setVRoll(const double &vrollRef);
	void setVPitch(const double &vpitchRef);
	void setVYaw(const double &vyawRef);

	virtual ~Movable(){};
};

} /* namespace std */
#endif /* MOVABLE_H_ */
