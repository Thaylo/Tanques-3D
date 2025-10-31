#include "Window.h"
#include "GLDraw.h"
#include "Timer.h"
#include "GameData.h"
#include "Camera.h"

void mouseFunc(int type, int button, int x, int y);
void keyPress(int code);
void keyRelease(int code);

void processLogic();
GameData* gameData;
window *w;
int level;


int main(int argc, char** argv)
{
	// Initialize GLUT first (required for macOS/GLUT)
	glutInit(&argc, argv);

	if(argc >= 2)
		level = atoi(argv[1]);
	else
	{
		printf("You can specify the number of opponents when starting, for example:\n\"./jogoThaylo 15\"\n");
		level = 3;
	}

	w = new window();
	gameData = new GameData();

	initGl();

	long lastTime = getCurrentTime();

	// Set up GLUT callbacks
	w->processWindow(mouseFunc, keyPress, keyRelease);

	// Set up display callback
	auto displayFunc = []() {
		gameData->drawGame();
		w->showWindow();
	};
	glutDisplayFunc(displayFunc);

	// Set up idle callback for game logic
	auto idleFunc = []() {
		static long lastTime = getCurrentTime();
		long currentTime = getCurrentTime();

		if(currentTime - lastTime > TIME_STEP)
		{
			lastTime += TIME_STEP;
			processLogic();
		}

		glutPostRedisplay();
	};
	glutIdleFunc(idleFunc);

#ifdef __APPLE__
	// On macOS, use GLUT's main loop
	glutMainLoop();
#else
	// On Linux, use original game loop
	while(true)
	{
		w->showWindow();
		if(!w->processWindow(mouseFunc, keyPress, keyRelease))
		{
			break;
		}

		long currentTime = getCurrentTime();

		if(currentTime - lastTime > TIME_STEP)
		{
			lastTime += TIME_STEP;
			processLogic();
		}

		gameData->drawGame();
	}
	delete w;
	delete gameData;
#endif

	return 0;
}

void processLogic()
{
	if(gameData->getControl()->keyEsc)
	{
		exit(0);
	}
	gameData->iterateGameData();
}

void mouseFunc(int type, int button, int x, int y)
{
	printf("mouse - type: %d, button: %d, x: %d, y: %d\n", type, button, x, y);
}

void keyPress(int code)
{
	printf("key: %d, type: press\n", code);
	Control *control = gameData->getControl();

	switch(code)
	{

		case TURBINE_INC:
		{
			control->power_inc = true;

		} break;

		case TURBINE_DEC:
		{
			control->power_dec = true;

		} break;

		case ARROW_UP:
		{
			control->arrowUp = true;
		} break;

		case ARROW_DOWN:
		{
			control->arrowDown = true;
		} break;

		case ARROW_RIGHT:
		{
			control->arrowRight = true;
		} break;

		case ARROW_LEFT:
		{
			control->arrowLeft = true;
		} break;
		case KEY_ESC:
		{
			control->keyEsc = true;
			break;
		}
		case KEY_SPACE:
		{
			control->space = true;
			break;
		}
	}	
}

void keyRelease(int code)
{
	printf("key: %d, type: release\n", code);
	Control *control = gameData->getControl();

	switch(code)
	{
		case TURBINE_INC:
		{
			control->power_inc = false;

		} break;

		case TURBINE_DEC:
		{
			control->power_dec = false;

		} break;

		case ARROW_UP:
		{
			control->arrowUp = false;
		} break;

		case ARROW_DOWN:
		{
			control->arrowDown = false;
		} break;

		case ARROW_RIGHT:
		{
			control->arrowRight = false;
		} break;

		case ARROW_LEFT:
		{
			control->arrowLeft = false;
		} break;

		case KEY_ESC:
		{
			control->keyEsc = false;
			break;
		}
		case KEY_SPACE:
		{
			control->space = false;
			break;
		}
	}	
}




