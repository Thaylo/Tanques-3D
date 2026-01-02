/**
 * Main.cpp - Game entry point
 *
 * Tanques 3D - A tank battle game
 * Uses GLUT callback-driven architecture for macOS compatibility
 */

#include "core/Timer.h"
#include "game/GameData.h"
#include "rendering/Camera.h"
#include "rendering/GLDraw.h"
#include "rendering/Window.h"

// Globals
GameData* gameData = nullptr;
int level = 3;
long lastTime = 0;

/**
 * DRY helper: Sets control key state based on key code.
 */
void setControlKey(Control* control, int code, bool pressed) {
    switch (code) {
        case TURBINE_INC:  control->power_inc = pressed; break;
        case TURBINE_DEC:  control->power_dec = pressed; break;
        case ARROW_UP:     control->arrowUp = pressed; break;
        case ARROW_DOWN:   control->arrowDown = pressed; break;
        case ARROW_RIGHT:  control->arrowRight = pressed; break;
        case ARROW_LEFT:   control->arrowLeft = pressed; break;
        case KEY_ESC:      control->keyEsc = pressed; break;
        case KEY_SPACE:    control->space = pressed; break;
    }
}

// GLUT Callbacks
void displayCallback() {
    if (gameData) {
        gameData->drawGame();
    }
    glutSwapBuffers();
}

void idleCallback() {
    long currentTime = getCurrentTime();
    
    if (currentTime - lastTime > TIME_STEP) {
        lastTime += TIME_STEP;
        
        if (gameData) {
            if (gameData->getControl()->keyEsc) {
                exit(0);
            }
            gameData->iterateGameData();
        }
    }
    
    glutPostRedisplay();
}

void mouseCallback(int button, int state, int x, int y) {
    printf("mouse - button: %d, state: %d, x: %d, y: %d\n", button, state, x, y);
}

void keyboardCallback(unsigned char key, int x, int y) {
    printf("key: %d, type: press\n", key);
    if (gameData) {
        setControlKey(gameData->getControl(), key, true);
    }
}

void keyboardUpCallback(unsigned char key, int x, int y) {
    printf("key: %d, type: release\n", key);
    if (gameData) {
        setControlKey(gameData->getControl(), key, false);
    }
}

void specialCallback(int key, int x, int y) {
    printf("special key: %d, type: press\n", key);
    if (gameData) {
        setControlKey(gameData->getControl(), key, true);
    }
}

void specialUpCallback(int key, int x, int y) {
    printf("special key: %d, type: release\n", key);
    if (gameData) {
        setControlKey(gameData->getControl(), key, false);
    }
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tanques 3D");

    // Parse command line
    if (argc >= 2) {
        level = atoi(argv[1]);
    } else {
        printf("You can specify the number of opponents when starting, for example:\n"
               "\"./Tanques3D 15\"\n");
    }

    // Initialize OpenGL and game
    initGl();
    gameData = new GameData();
    lastTime = getCurrentTime();

    // Register GLUT callbacks
    glutDisplayFunc(displayCallback);
    glutIdleFunc(idleCallback);
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialCallback);
    glutSpecialUpFunc(specialUpCallback);

    printf("Starting game with %d enemies...\n", level);
    printf("Controls: Arrow keys to move, Space to fire, ESC to quit\n");

    // Enter GLUT main loop (this is blocking but handles the window properly)
    glutMainLoop();

    // Cleanup (won't reach here normally as glutMainLoop doesn't return)
    delete gameData;

    return 0;
}
