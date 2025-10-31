/*
 * Matter.cpp
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#include "Matter.h"
#include <iostream>

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
	// Validate mass (must be positive)
	if (mass_ <= 0.0) {
		cerr << "ERROR: Invalid mass value: " << mass_ << endl;
		cerr << "       Mass must be positive. Setting to 1.0 as fallback." << endl;
		mass = 1.0;
		return;
	}
	mass = mass_;
}

void Matter::setCharge(double charge_)
{
	charge = charge_;
}

} /* namespace std */
