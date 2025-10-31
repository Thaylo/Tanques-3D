#ifndef CONTROL_H_
#define CONTROL_H_

#include "Vector.h"

typedef struct Control {
	bool arrowUp;
	bool arrowDown;
	bool arrowLeft;
	bool arrowRight;

	bool power_inc; // Potencia na turbina
	bool power_dec; // Potencia na turbina

	// Right e left só devem ser mudados na função de callback do mouseFunc, mas os "new"s podem ser mudados em qualquer lugar do codigo.
	bool rightPressed; bool newRightPressed;
	bool leftPressed; bool newLeftPressed;
	bool scrollPressed;

	bool space;

	bool keyEsc;
	
	Vector pressedPos;
	Vector releasedPos;
} Control;

Control initializeControl();

#endif
