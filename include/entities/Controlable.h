/*
 * Controlable.h
 *
 *  Created on: 16/10/2012
 *      Author: thaylo
 */

#ifndef CONTROLABLE_H_
#define CONTROLABLE_H_

#include "Movable.h"
#include "Control.h"
#include "Constants.h"
#include  <cstdlib>

class Controlable : public Movable {
protected:
	Control *control;
public:
	Controlable();
	void setController(Control *control_);
	virtual void controlAction() = 0;
	virtual ~Controlable();
};

#endif /* CONTROLABLE_H_ */
