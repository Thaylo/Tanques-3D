/*
 * oDrawable.h
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#ifndef ODRAWABLE_H_
#define ODRAWABLE_H_

#include "GLDraw.h"

using namespace std;

class oDrawable {
public:
	oDrawable();
	virtual void draw() = 0;
	virtual ~oDrawable(){};
};

#endif /* ODRAWABLE_H_ */
