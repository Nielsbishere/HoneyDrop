#ifndef _STATE_H
#define _STATE_H

#include "Button.h"
#include "World.h"
#include "Resources.h"

class State {
public:
	enum StateType {
		MENU, GAME, MULTIPLAYER
	};
protected:
	World *w;
	Resources *r;
	TextureSet *ts;
private:
	Button **buttons;
	int bts, timer, game, currButton;
	StateType type;
	bool shouldGoToMenu, shouldReload, shouldKill, btsb, hasButtons, pauzed, drawLogo;
public:
	//Things like constructors, cleanups and copy; just to make sure we don't get bad pointers!
	State(int tileSize, double dispSize, unsigned int seed, Resources *r, int level, StateType type, TextureSet *ts, bool hasChar, bool drawLogo = false) {
		w = new World(tileSize, dispSize, seed, (int)(2.0 / dispSize + 1), (type == GAME) ? (int)(0.125 / 1.3 / 2 / dispSize + 1)*(level + 6) : (int)(1.0 / dispSize + 1), level, hasChar, r);
		bts = timer = game = currButton = 0;
		shouldGoToMenu = shouldReload = shouldKill = btsb = hasButtons = pauzed = false;
		buttons = NULL;
		this->r = r;
		this->type = type;
		this->ts = ts;
		this->drawLogo = drawLogo;
	}
	void clearButtons() {
		for (int i = 0; i < bts && i < currButton; i++)delete buttons[i];
		delete[] buttons;
	}
	void cleanup() {
		delete w;
		if (hasButtons)clearButtons();
	}
	virtual ~State() {
		cleanup();
	}
	void copy(const State &st, bool needsCleanup = true) {
		if (needsCleanup)cleanup();
		w = new World(*st.w);
		buttons = new Button*[bts = st.bts];
		timer = st.timer;
		btsb = st.btsb;
		hasButtons = st.hasButtons;
		shouldKill = st.shouldKill;
		shouldReload = st.shouldReload;
		shouldGoToMenu = st.shouldGoToMenu;
		game = st.game;
		currButton = st.currButton;
		type = st.type;
		r = st.r;
	}
	State(const State& st) {
		copy(st, false);
	}
	State& operator=(const State& st) {
		if (this != &st)copy(st);
		return *this;
	}

	//Button handler
	void setButtons(int i) {
		if (hasButtons)clearButtons();
		else hasButtons = true;
		buttons = new Button*[bts = i];
		currButton = 0;
	}
	void addButton(Button *b) {
		if (currButton >= bts)return;
		buttons[currButton] = b;
		currButton++;
	}
	void callButton(int i, State &st) {
		if (i >= bts || !btsb || !hasButtons || timer != 0)return;
		buttons[i]->activate(st);
		timer = 30;
	}
	void enableButtons() { btsb = true; }
	void disableButtons() { btsb = false; }

	//Logo
	void drawlogo() { drawLogo = true; }

	//Button actions

	void refresh(int i) { initButtons(); if (pauzed)pauzed = false; }
	void exit(int i) { if (type == GAME && w->isAlive(r))w->execute(w->findEntity(w->getCharacter(r)), Entity::DIE); shouldGoToMenu = true; }
	void reload(int i) { if (type == GAME && w->isAlive(r))w->execute(w->findEntity(w->getCharacter(r)), Entity::DIE); shouldReload = true; }
	void kill(int i) { shouldKill = true; }
	void goToGame(int i) { game = i + 1; }
	void nothing(int i) {}
	void success(int i) {
		r->undoWin();
		r->winLevel(w->getLevel()-1);
		exit(i);
	}
	void pauzeScreen() {
		setButtons(3);
		addButton(new Button(50, 60, 25, 5, string("Back"), 0xFF909090, &State::refresh, 0));
		addButton(new Button(50, 52.5, 25, 5, string("Restart"), 0xFF909090, &State::reload, 0));
		addButton(new Button(50, 45, 25, 5, string("Menu"), 0xFF909090, &State::exit, 0));
		enableButtons();
	}

	//Advanced menu responses; for things like purchasing upgrades and such

	void shop(int i);
	void skins(int i);
	void powerup(int i);
	void select(int i);

	//Purchase methods
	void purchase(int i);
	void purchase2(int i);
	void play(int i);
	
	//Indicators
	int getGame() { return game; }
	bool menu() { return shouldGoToMenu; }
	bool reload() { return shouldReload; }
	bool kill() { return shouldKill; }
	
	bool isGameOver() { return !w->isAlive(r); }

	StateType getType() { return type; }
	int getLevel() { return w->getLevel(); }

	//Overridable functions
	virtual void update();
	virtual void render(int width, int height);
	virtual void click(bool left, bool right, bool middle, int scroll, double x, double y, int width, int height);
	virtual void press(double x, double y, unsigned char key, bool down);
	virtual void initButtons();
};

//Basic methods, that update the timer and click/render the buttons when needed

void State::update() {
	if (pauzed)return;
	if (timer>0)timer--;
	w->update();
}
void State::render(int w, int h) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->w->draw(r);
	glBegin(GL_QUADS);
	glColor4f(20 / 255.0f, 25 / 255.0f, 25 / 255.0f, 50 / 255.0f);
	glVertex2f(-1, -1); glVertex2f(1, -1); glVertex2f(1, 1); glVertex2f(-1, 1);
	glEnd();
	if (hasButtons && btsb && bts != 0) {
		for (int i = 0; i < bts && i < currButton; i++)buttons[i]->draw(w, h);
	}
	Shortcut::drawString(Shortcut::append(r->getCoins(), " coins"), -1, 0.9, 0.5, 0xFFF0F000);
	if(drawLogo)ts->draw("img/logo.png", -0.25, 0.2, 0.5, 2 * 0.22083333);
	glDisable(GL_BLEND);
}
void State::click(bool left, bool right, bool mid, int scroll, double x, double y, int w, int h) {
	if (hasButtons && btsb && bts != 0 && timer == 0) {
		for (int i = 0; i < bts && i < currButton; i++) {
			if (buttons[i]->click(*this, x, y, w, h)) {
				timer = 10;
				break;
			}
		}
	}
}
void State::press(double x, double y, unsigned char key, bool down) {
	if (w->isAlive(r)) {
		Character *c = w->getCharacter(r);
		if (key == 32 && down)c->jump();
		if (key == 'a' || key == 'd')c->turn(key != 'a', down);
		if (key == 's')c->setSprinting(down);
		if (key == 'w' && down)c->megaJump();
		if (key == 'e' && down)c->heal();
		if (key == 27 && down && !pauzed) {
			pauzeScreen();
			pauzed = true;
		}
	}
	else {
		if (down && (key == 32 || key == 13))callButton(0, *this);
	}
}
void State::initButtons() {}

//Advanced menu responses

void State::shop(int i) {
	drawLogo = false;
	setButtons(3);
	addButton(new Button(50, 60, 30, 5, string("Back"), 0xFF909090, &State::refresh, 0, 0.8));
	addButton(new Button(50, 52.5, 30, 5, string("Skins"), 0xFF909090, &State::skins, 0, 0.8));
	addButton(new Button(50, 45, 30, 5, string("Power-ups"), 0xFF909090, &State::powerup, 0, 0.8));
}

void State::skins(int z) {
	setButtons(3 * 4 + 1);
	addButton(new Button(50, 80, 40, 5, string("Back to shop"), 0xFF909090, &State::shop, 0));
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++) {
			int k = j * 3 + i;
			long long l = r->getColorAt(k);
			int y = r->getPrice(k);
			addButton(new Button(35 + 15 * i, 80 - 12.5*(j + 1), 12, 5, !r->getColor(k) ? Shortcut::asString(y) : string(""), l, &State::purchase, k, 0.5, r->getColor() == l ? 0xFF800000 : 0xFFA0A0A0, k == 2 ? 0xFF000000 : 0xFFFFFFFF));
		}
}

void State::powerup(int i) {
	setButtons(1 + 2 + 3 * 3 * 2);
	addButton(new Button(50, 90, 40, 5, string("Back to shop"), 0xFF909090, &State::shop, 0));
	addButton(new Button(50, 80, 40, 5, string("Perks"), 0xFF909090, &State::nothing, 0, 1, 0xFFA0A0A0, 0xFFFFFFFF, false));
	addButton(new Button(50, 80 - 13.5 * 4, 40, 5, string("Debuffs"), 0xFF909090, &State::nothing, 0, 1, 0xFFA0A0A0, 0xFFFFFFFF, false));
	for (i = 0; i<3; i++)
		for (int j = 0; j<4; j++) {
			int k = j * 3 + i;
			if (j == 2)continue;
			addButton(new Button(25 + 55 / 3.0*i*1.5, 80 - 13.5*(j + 1) - (j / 3)*12.5, 24, 5, r->getAt(k), r->perkAt(k) ? 0xFF00A000 : 0xFFA00000, &State::purchase2, k, 0.45, 0xFFA0A0A0, 0xFFFFFFFF));
			addButton(new Button(25 + 55 / 3.0*i*1.5, 80 - 13.5*(j + 1) - (j / 3)*12.5 + 5, 24, 5, r->getNameAt(k), r->perkAt(k) ? 0xFF00A000 : 0xFFA00000, &State::purchase2, k, 0.35, 0xFFA0A0A0, 0xFFFFFFFF, false));
		}
}

void State::select(int i) {
	drawLogo = false;
	setButtons(3 * 4 + 1);
	addButton(new Button(50, 80, 40, 5, string("Back to menu"), 0xFF909090, &State::refresh, 0));
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++) {
			int k = j * 3 + i;
			addButton(new Button(35 + 15 * i, 80 - 12.5*(j + 1), 12, 5, r->getLevelText(k), 0xFF909090, &State::play, k, 0.35));
		}
}

//Purchase things

void State::purchase(int k) {
	r->buy(k, r->getPrice(k));
	if (r->getColor(k)) {
		r->setColor(r->getColorAt(k));
	}
	skins(0);
}

void State::purchase2(int k) {
	if (r->getType(k) == 1)r->purchase(k);
	else if (r->getType(k) == 0) {
		if (r->getPerk(k) < 10)r->purchase(k);
	}
	else r->toggle(k);
	powerup(0);
}

void State::play(int i) {
	if (!r->hasUnlocked(i))r->unlock(i);
	else {
		goToGame(i);
		return;
	}
	select(0);
}

#endif