/*
 * Controlable.cpp
 *
 *  Created on: 16/10/2012
 *      Author: thaylo
 */

#include "Controlable.h"

Controlable::Controlable()
{
	Movable();
	control = NULL;
}

void Controlable::setController(Control *control_)
{
	control = control_;
}


Controlable::~Controlable() {
	// TODO Auto-generated destructor stub
}

