#include "GameData.h"

extern int level;


GameData::GameData()
{
	player = NULL;
	count = 0;
	control = initializeControl();

	insertPlayer();
	int enemyCount = level; // Set by user.
	for(int i = 0; i < enemyCount; i++)
	{
		Enemy *e = new Enemy(player);
		e->setId(0);
		e->setPosition(Vector((rand()%100)/4.0 - 25.0,((rand()%100)/4.0 - 25.0),0.0));
		agents[count++] = e;
	}


}

void GameData::insertPlayer()
{
	player = new Agent(Vector(0,0,0.0));
	player->setController(&control);
	player->setId(PLAYER_ID);
	agents[count++] = player;

	c = Camera(player);
	g = Ground(player);
}

Control *GameData::getControl()
{
	return &control;
}

void GameData::iterateGameData()
{
	Agent *aux;

	g.iterate();
	c.iterate();

	for(int i = 0; i < count; i++)
	{
		aux = agents[i];
		aux->iterate();
		if(aux->checkFire())
		{
			Projectile *shot = new Projectile(aux);

			shot->setId(aux->getId());
			agents[count++] = shot;
		}
		if(aux->isToDestroy())
		{
			if(aux->getId() == PLAYER_ID)
			{
				Projectile *isShot = dynamic_cast<Projectile *>(aux);
				if (!isShot)
				{
					std::cout << "Game Over: LOSER!" << std::endl;
					getControl()->keyEsc = TRUE;
				}
			}
			delete aux;
			agents[i] = agents[count - 1];
			--count;
			--i;
		}
	}
	if (1 == count && player == agents[0])
	{
		std::cout << "Game Over: WINNER!" << std::endl;
		getControl()->keyEsc = TRUE;
	}
}



GameData::~GameData()
{
	for(int i = 0; i < count; i++)
	{
		delete agents[i];
	}
}

void GameData::drawGame()
{


	glClearColor(0,0,0,0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	//glutSolidSphere (0.04, 10, 8);

// Preparing to work with camera!



	glPushMatrix();
// Here the "camera game" enters!!!


	c.posiciona();

// Here the "camera game" ends!!!

	g.draw();
	//c.draw();


	for(int i = 0; i < count; i++)
	{
		agents[i]->draw();
	}

	glPopMatrix();

}







