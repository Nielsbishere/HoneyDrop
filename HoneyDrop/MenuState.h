#ifndef MENUSTATE_H_
#define MENUSTATE_H_

#include "State.h"
#include "Resources.h"

#include "TextureSet.h"

class MenuState : public State {
public:
	MenuState(int tileSize, double dispSize, Resources *r, unsigned int seed, TextureSet *ts) : State(tileSize, dispSize, seed, r, 7, StateType::MENU, ts, false, true) {
		initButtons();
		enableButtons();
	}
	virtual ~MenuState() {
	
	}
	virtual void update();
	virtual void render(int width, int height);
	virtual void click(bool left, bool right, bool middle, int scroll, double x, double y, int width, int height);
	virtual void press(double x, double y, unsigned char key, bool down);
	virtual void initButtons();
};

void MenuState::update() {
	State::update();
}
void MenuState::render(int width, int height) {
	State::render(width, height);
}
void MenuState::click(bool left, bool right, bool middle, int scroll, double x, double y, int width, int height) {
	State::click(left, right, middle, scroll, x, y, width, height);
}
void MenuState::press(double x, double y, unsigned char key, bool down) {
	State::press(x, y, key, down);
}
void MenuState::initButtons() {
	setButtons(3);
	addButton(new Button(50, 55, 25, 5, string("Play"), 0xFF909090, &MenuState::select, 0));
	addButton(new Button(50, 47.5, 25, 5, string("Shop"), 0xFF909090, &MenuState::shop, 0));
	addButton(new Button(50, 40, 25, 5, string("Exit"), 0xFF909090, &MenuState::kill, 0));
	drawlogo();
}

#endif