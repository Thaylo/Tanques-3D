#ifndef CAMERA_H_
#define CAMERA_H_

#include "Movable.h"
#include "GLDraw.h"
#include "oDrawable.h"

extern GLfloat light_position[];

class Camera: public Movable, public oDrawable{
public:
	Camera();
	Camera(Movable *track);
	void iterate();
	void draw();
	void posiciona();
	//Movable *getTracked();
	~Camera();
private:
	Movable *tracked;



};


#endif
