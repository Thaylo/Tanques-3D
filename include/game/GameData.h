#ifndef GAMEDATA_H_
#define GAMEDATA_H_ 1


#include "Constants.h"
#include "Control.h"
#include <stdlib.h>
#include "Agent.h"
#include "Ground.h"
#include "GLDraw.h"
#include "Camera.h"
#include <list>
#include <vector>
#include "Projectile.h"
#include "Enemy.h"

extern GLfloat mat_specular[];
extern GLfloat mat_shininess[];
extern GLfloat light_position[];

class GameData
{
private:
	Control control;
	Agent *player;
	Agent *agents[4000];
	int count;
	Ground g;
	Camera c;
	int gameState;


public:
	int *getJoypad();
	GameData();
	void insertPlayer();
	Control *getControl();
	void iterateGameData();
	void drawGame();
	int getCount(){return count;}
	Agent **getAgents() { return agents; }
	~GameData();

};





#endif /*GAMEDATA_H_*/
