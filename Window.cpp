#include "Window.h"

// Static member initialization
void (*window::s_keyPressFunc)(int code) = NULL;
void (*window::s_keyReleaseFunc)(int code) = NULL;
void (*window::s_mouseFunc)(int type, int button, int x, int y) = NULL;

// GLUT callback for regular keyboard keys (press)
void window::keyboardCallback(unsigned char key, int x, int y)
{
	if (s_keyPressFunc)
		s_keyPressFunc((int)key);
}

// GLUT callback for regular keyboard keys (release)
void window::keyboardUpCallback(unsigned char key, int x, int y)
{
	if (s_keyReleaseFunc)
		s_keyReleaseFunc((int)key);
}

// GLUT callback for special keys (press)
void window::specialKeyCallback(int key, int x, int y)
{
	if (s_keyPressFunc)
		s_keyPressFunc(key);
}

// GLUT callback for special keys (release)
void window::specialKeyUpCallback(int key, int x, int y)
{
	if (s_keyReleaseFunc)
		s_keyReleaseFunc(key);
}

// GLUT callback for mouse events
void window::mouseCallback(int button, int state, int x, int y)
{
	if (s_mouseFunc)
	{
		int type = (state == GLUT_DOWN) ? 4 : 5; // ButtonPress=4, ButtonRelease=5 in X11
		s_mouseFunc(type, button, x, y);
	}
}

window::window()
{
	// GLUT window creation is handled in Main.cpp during glutInit
	// This is just a placeholder to maintain compatibility with existing code

	// Create GLUT window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tanques 3D - C++/OpenGL");

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
}

bool window::processWindow(void (*mouseFunc)(int type, int button, int x, int y),
                           void (*keyPress)(int code),
                           void (*keyRelease)(int code))
{
	// Store callback functions
	s_mouseFunc = mouseFunc;
	s_keyPressFunc = keyPress;
	s_keyReleaseFunc = keyRelease;

	// Set GLUT callbacks
	glutKeyboardFunc(keyboardCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialKeyCallback);
	glutSpecialUpFunc(specialKeyUpCallback);
	glutMouseFunc(mouseCallback);

	// Process events
	glutMainLoopEvent();

	return true;
}

void window::showWindow()
{
	glutSwapBuffers();
}
