/*
* core.cpp
*
*  (Created on: Feb 1, 2016)
*  Last modified on: Apr 19, 2016
*      Author: Niels Brunekreef
*/


//Honeydrop; made by Niels Brunekreef (Nielsbwashere) (Osomi)
//APIS: Windows (OS), openGL (graphics), freeglut (screen),  SOIL (png loading)
//Help:
//ForeignGuyMike: Collision tutorial
//Creative Film: Sounds; explosion (https://www.youtube.com/watch?v=-Yh-f_i9BTo)
//TheXpender: Logo and background (and testing)
//Sanja, Djorno, Thomas, Daan: Testing


//OS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//Main library freeglut = opengl + extra functionality
#include <GL/freeglut.h>

//Basic includes
#include <iostream>
#include <ctime>

//Icon

#include "resource.h"

//Shortcuts and stuff
#include "ColorManager.h"
#include "TextureSet.h"

#include "Resources.h"
#include "State.h"
#include "MenuState.h"
#include "GameState.h"

using namespace std;

//Run handler
static int MAX_FPS = 60;
static int FRAME_PERIOD = 1000 / MAX_FPS;
int behind=0, skip = 500, timesSkipped=0;
bool init = false;

int handle = 0;

//Screen and rendering
double dispSize = 0.05;
int tileSize = 32;
unsigned int seed = (unsigned int)time(0);

int timeOut = 10;

//The current state and resource
Resources *resources = new Resources(0);
TextureSet *ts = new TextureSet();
State *s = new MenuState(tileSize, dispSize, resources, seed, ts);

//The thing run before the draw method
void update() {
	if (timeOut>0)timeOut--;
	s->update();
}
//The thing run after the update method
void draw() {
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	s->render(m_viewport[2], m_viewport[3]);
}
//The thing run on keypress
void keypress(unsigned char c, int x, int y) {
	if (timeOut != 0)return;
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, c, true);
}
void keynotpress(unsigned char c, int x, int y) {
	if (timeOut != 0)return;
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, c, false);
}
void specialkey(int c, int x, int y) {
	if (timeOut != 0)return;
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	if(c==GLUT_KEY_UP)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'w', true);
	else if (c == GLUT_KEY_LEFT)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'a', true);
	else if (c == GLUT_KEY_RIGHT)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'd', true);
	else if (c == GLUT_KEY_DOWN)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 's', true);
}
void specialkeynot(int c, int x, int y) {
	if (timeOut != 0)return;
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	if (c == GLUT_KEY_UP)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'w', false);
	else if (c == GLUT_KEY_LEFT)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'a', false);
	else if (c == GLUT_KEY_RIGHT)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 'd', false);
	else if (c == GLUT_KEY_DOWN)s->press((double)x / m_viewport[2] * tileSize, (1.0 - (double)y / m_viewport[3])*tileSize, 's', false);
}
//The thing run after a mouse click
void click(int button, int state, int x, int y) {
	if (timeOut != 0)return;
	bool left = button == GLUT_LEFT_BUTTON;
	bool right = button == GLUT_RIGHT_BUTTON;
	bool middle = button == GLUT_MIDDLE_BUTTON;
	int scroll = button == 3 ? -1 : button == 4 ? 1 : 0;
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	s->click(left, right, middle, scroll, (double)x / m_viewport[2] * 2 - 1, (2.0 - (double)y / m_viewport[3] * 2) - 1, m_viewport[2], m_viewport[3]);
}
//The thing initializing the map
void initialize() {
	ts->load("img/logo.png");
}
//The basic GLUT method that clears the screen and calls the draw function
void display() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glFlush();
}
//A check to switch between states
void executeState() {
	if (s->reload() && s->getType() == State::GAME) {
		int lvl = ((GameState*)s)->getLevel();
		delete s;
		s = new GameState(tileSize, dispSize, resources, (unsigned int)time(0), lvl, ts);
		timeOut = 10;
	}
	else if (s->kill() && s->getType() == State::MENU) {
		glutDestroyWindow(handle);
	}
	else if (s->menu() && s->getType() != State::MENU) {
		delete s;
		s = new MenuState(tileSize, dispSize, resources, seed = (unsigned int)time(0), ts);
		timeOut = 10;
	}
	else if (s->getGame() != 0 && s->getType() == State::MENU) {
		int lvl = s->getGame();
		delete s;
		s = new GameState(tileSize, dispSize, resources, (unsigned int)time(0), lvl, ts);
		timeOut = 10;
	}
}
//The thing called on an update tick of the window
void loop() {
	if (!init) {
		init = true;
		initialize();
	}
	long time = clock();
	update();
	glutPostRedisplay();
	executeState();
	long interval = clock() - time;
	if (interval >= FRAME_PERIOD) {
		behind += interval - FRAME_PERIOD;
		if (behind >= skip) {
			long ticks = (long)(0.4 * behind);
			cout << "Skipping " << ticks << " ticks! (" << timesSkipped << ")" << endl;
			behind -= ticks;
			timesSkipped++;
		}
	}
	else {
		if (timesSkipped > 0)timesSkipped--;
		if (behind>0) {
			long sleep = FRAME_PERIOD - interval;
			if (behind <= sleep) {
				sleep -= behind;
				behind = 0;
			}
			else {
				behind -= sleep;
				sleep = 0;
			}
			Sleep(sleep);
		}
		else Sleep(FRAME_PERIOD - interval);
	}
}
void exit() {
	delete resources;
	delete s;
	delete ts;
}
//Initiating the window on start
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {		//release
	glutInit(&__argc, __argv);
//int main(int argc, char **argv){
	//glutInit(&argc, argv);
	//Glut
	glutCreateWindow("HoneyDrop (Osomi Lunar)");
	glutInitWindowSize(320, 320);
	glutInitWindowPosition(0, 0);
	glutDisplayFunc(display);
	glutIdleFunc(loop);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keynotpress);
	glutSpecialFunc(specialkey);
	glutSpecialUpFunc(specialkeynot);
	glutMouseFunc(click);
	handle = __argc;
	//handle = argc;
	glutWMCloseFunc(exit);
	glutMainLoop();
	return 0;
}
