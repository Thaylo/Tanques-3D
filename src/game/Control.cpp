#include "Control.h"

Control initializeControl()
{
	Control control;
	
	control.power_dec = control.power_inc = 0;

	control.arrowUp = 0;
	control.arrowDown = 0;
	control.arrowLeft  = 0;
	control.arrowRight  = 0;
	
	control.newRightPressed = 0;
	control.rightPressed  = 0;
	control.newLeftPressed = 0;
	control.leftPressed  = 0;

	control.keyEsc = 0;

	control.scrollPressed  = 0;
	
	control.space = 0;
	control.pressedPos = Vector(0,0,0);
	control.releasedPos = Vector(0,0,0);

	
	return control;
}
