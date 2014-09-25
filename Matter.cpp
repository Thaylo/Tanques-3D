/*
 * Matter.cpp
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#include "Matter.h"

namespace std {

Matter::Matter() {
	charge = mass = 1;
}

double Matter::getMass() const
{
	return mass;
}

double Matter::getCharge() const
{
	return charge;
}

void Matter::setMass(double mass_)
{
	mass = mass_;
}

void Matter::setCharge(double charge_)
{
	charge = charge_;
}

} /* namespace std */
