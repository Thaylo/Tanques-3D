#include "GameData.h"

extern int level;


GameData::GameData()
{
	jogador = NULL;
	quant = 0;
	control = initializeControl();

	insertPlayer();
	int quantInimigos = level; // Setado pelo usuário.
	for(int i = 0; i < quantInimigos; i++)
	{
		Enemy *e = new Enemy(jogador);
		e->setId(0);
		e->setPosition(Vector((rand()%100)/4.0 - 25.0,((rand()%100)/4.0 - 25.0),0.0));
		agents[quant++] = e;
	}


}

void GameData::insertPlayer()
{
	jogador = new Agent(Vector(0,0,0.0));
	jogador->setController(&control);
	jogador->setId(PLAYER_ID);
	agents[quant++] = jogador;

	c = Camera(jogador);
	g = Ground(jogador);
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

	for(int i = 0; i < quant; i++)
	{
		aux = agents[i];
		aux->iterate();
		if(aux->checkDisparo())
		{
			Projetil *disparo = new Projetil(aux);

			disparo->setId(aux->getId());
			agents[quant++] = disparo;
		}
		if(aux->isToDestroy())
		{
			if(aux->getId() == PLAYER_ID)
			{
				Projetil *isTiro = dynamic_cast<Projetil *>(aux);
				if (!isTiro)
				{
					std::cout << "Game Over: LOSER!" << std::endl;
					getControl()->keyEsc = TRUE;
				}
			}
			delete aux;
			agents[i] = agents[quant - 1];
			--quant;
			--i;
		}
	}
	if (1 == quant && jogador == agents[0])
	{
		std::cout << "Game Over: WINNER!" << std::endl;
		getControl()->keyEsc = TRUE;
	}
}



GameData::~GameData()
{
	for(int i = 0; i < quant; i++)
	{
		delete agents[i];
	}
}

void GameData::drawGame()
{


	glClearColor(0,0,0,0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	//glutSolidSphere (0.04, 10, 8);

// Preparando pra mexer na camera!



	glPushMatrix();
// Aqui deve entrar o "jogo de Câmera"!!!


	c.posiciona();

// Aqui termina o "jogo de câmera"!!!

	g.draw();
	//c.draw();


	for(int i = 0; i < quant; i++)
	{
		agents[i]->draw();
	}

	glPopMatrix();

}







