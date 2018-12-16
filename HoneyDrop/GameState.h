#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "State.h"
#include "Resources.h"

#include "Shortcut.h"

#include "Shortcut.h"
#include "TextureSet.h"

class GameState : public State {
private:
	bool hasWon, wonScreen;
	int step, stepTimer;
public:
	GameState(int tileSize, double dispSize, Resources *r, unsigned int seed, int level, TextureSet *ts) : State(tileSize, dispSize, seed, r, level, StateType::GAME, ts, true) {
		initButtons();
		hasWon = wonScreen = false;
		step = r->getProperty(0)?7:0;
		stepTimer = 60*10;
	}
	virtual ~GameState() {

	}
	virtual void update();
	virtual void render(int width, int height);
	virtual void click(bool left, bool right, bool middle, int scroll, double x, double y, int width, int height);
	virtual void press(double x, double y, unsigned char key, bool down);
	virtual void initButtons();
};

void GameState::update() {
	if (step < 7 && w->isAlive(r)) {
		if (stepTimer > 0)stepTimer--;
		else {
			stepTimer = 60 * 10;
			step++;
		}
	}
	State::update();
	if (isGameOver())enableButtons();
	else if (w->isAlive(r)) {
		if (w->getCharacter(r)->isInvisible() && !hasWon && !wonScreen)hasWon = true;
		if (hasWon && !wonScreen) {
			wonScreen = true;
			setButtons(3);
			addButton(new Button(50, 70, 25, 5, Shortcut::combine(r->getWinText(w->getLevel()), "Congrats! You won", true), 0xFF909090, &GameState::nothing, 0, 0.75, 0xFFA0A0A0, 0xFF3030CF, false));
			addButton(new Button(50, 52.5, 25, 5, string("Shop"), 0xFF909090, &GameState::shop, 0));
			addButton(new Button(50, 45, 25, 5, string("Menu"), 0xFF909090, &GameState::success, 0));
			enableButtons();
		}
	}
}
void GameState::render(int width, int height) {
	State::render(width, height);
	if (step<6) {
		if (step == 0)Shortcut::drawString("Use A and D (or the left and right arrow keys) to move", -0.85, -0.8, 0.5, 0xFF0080FF);
		else if (step == 1)Shortcut::drawString("Use S or the down arrow key whilst moving to sprint", -0.75, -0.8, 0.5, 0xFF0080FF);
		else if (step == 2)Shortcut::drawString("Use W or the up arrow key to jump higher", -0.6, -0.8, 0.5, 0xFF0080FF);
		else if (step == 3)Shortcut::drawString("Use E to heal", -0.2, -0.8, 0.5, 0xFF0080FF);
		else if (step == 4){
			Shortcut::drawString("Dodge falling blocks", -0.25, -0.8, 0.5, 0xFF0080FF);
			r->setProperty(0, true);
		}
		else if (step == 5) {
			Shortcut::drawString("But watch your energy and health", -0.45, 0.8, 0.5, 0xFF0080FF);
		}
		else {
			Shortcut::drawString("Good luck!", -0.25, -0.8, 0.5, 0xFF0080FF);
		}
	}
}
void GameState::click(bool left, bool right, bool middle, int scroll, double x, double y, int width, int height) {
	State::click(left, right, middle, scroll, x, y, width, height);
}
void GameState::press(double x, double y, unsigned char key, bool down) {
	State::press(x, y, key, down);
}
void GameState::initButtons() {
	if (hasWon) {
		wonScreen = true;
		setButtons(3);
		addButton(new Button(50, 70, 25, 5, Shortcut::combine(r->getWinText(w->getLevel()), "Congrats! You won", true), 0xFF909090, &GameState::nothing, 0, 0.75, 0xFFA0A0A0, 0xFF3030CF, false));
		addButton(new Button(50, 52.5, 25, 5, string("Shop"), 0xFF909090, &GameState::shop, 0));
		addButton(new Button(50, 45, 25, 5, string("Menu"), 0xFF909090, &GameState::success, 0));
		enableButtons();
		return;
	}
	setButtons(3);
	addButton(new Button(50, 60, 25, 5, string("Respawn"), 0xFF909090, &GameState::reload, 0));
	addButton(new Button(50, 52.5, 25, 5, string("Shop"), 0xFF909090, &GameState::shop, 0));
	addButton(new Button(50, 45, 25, 5, string("Menu"), 0xFF909090, &GameState::exit, 0));
	if (w->isAlive(r))disableButtons();
}


#endif