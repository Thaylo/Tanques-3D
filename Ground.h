/*
 * Ground.h
 *
 *  Created on: 28/03/2013
 *      Author: thaylo
 */

#ifndef GROUND_H_
#define GROUND_H_

#include "oDrawable.h"
#include "Movable.h"
#include "Vector.h"
#include "Agent.h"
#include <list>



class Ground: public oDrawable, public Movable {
private:
	Agent *tracked;
public:
	Ground();
	Ground(Agent *a);
	void draw();
	void iterate();

	virtual ~Ground();


};

#endif /* GROUND_H_ */
