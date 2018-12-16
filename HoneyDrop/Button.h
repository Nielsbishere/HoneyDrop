#ifndef _BUTTON_H
#define _BUTTON_H

using namespace std;

#include "Shortcut.h"
#include "Rect.h"
#include "State.h"

class State;

class Button {
private:
	double posX, posY, width, height;
	long long fill, stroke, txtColor;
	unsigned char* txt;
	int maxW, maxH;
	int length;
	int oldX, oldY;
	Rect r;
	void (State::*run)(int);
	int amount;
	double tscale;
	bool canSee, exists;
public:
	Button(double x, double y, double w, double h, string text, long long fillColor, void (State::*func)(int), int am, double tscale = 1, long long strokeColor = 0xFFA0A0A0, long long textColor = 0xFFFFFFFF, bool canSee = true, int pw = 0, int ph = 0) : posX(x), posY(y), width(w), height(h) {
		fill = fillColor;
		stroke = strokeColor;
		txtColor = textColor;
		maxW = pw;
		maxH = ph;
		txt = Shortcut::getFromString(text);
		oldX = 0;
		oldY = 0;
		length = text.size();
		run = func;
		exists = true;
		amount = am;
		this->tscale = tscale;
		this->canSee = canSee;
	}
	Button(bool exists) {
		posX = posY = width = height = maxH = maxW = length = oldX = oldY = 0;
		fill = stroke = txtColor = 0;
		this->exists = exists;
		string text = string("Non existent");
		txt = Shortcut::getFromString(text);
		r = Rect();
		amount = 0;
		tscale = 1;
		run = nullptr;
		this->canSee = false;
	}
	//Making sure that you get a rectangle that is pixel accurate, so that you can draw it and check if the pixel clicked on the screen matches this button.
	Rect getRect(int width, int height) {
		//posX, posY, width and height are percentages of the screen width and height
		//While maxW and maxH are the maximum pixel length of the rectangle.
		int sX = (int)((posX / 100)*width * 2);
		int sY = (int)((posY / 100)*height * 2);

		int rW = (int)((this->width / 100)*width * 2);
		if (maxW != 0 && rW > maxW)rW = maxW;
		sX -= rW / 2;							//Centering the position, so a button looks 100% centered.

		int rH = (int)((this->height / 100)*height * 2);
		if (maxH != 0 && rH > maxH)rH = maxH;
		sY -= rH / 2;							//Centering

		return Rect(sX, sY, rW, rH);
	}

	//Drawing; if '!canSee' that means that only the text will show, handy for titles
	void draw(int screenWidth, int screenHeight) {
		if (screenWidth != oldX || screenHeight != oldY) {
			r = getRect(screenWidth, screenHeight);
			oldX = screenWidth;
			oldY = screenHeight;
		}
		if (canSee)Rect::quad((double)r.x / screenWidth - 1, (double)r.y / screenHeight - 1, (double)r.width / screenWidth, (double)r.height / screenHeight, fill, stroke);
		Shortcut::drawString(txt, (double)r.x / screenWidth - 1 + r.width / 2.0 / screenWidth - (length*120.0*tscale*screenWidth / 1920) / 2 / screenWidth, (double)r.y / screenHeight - 1 + 0.15*r.height / screenHeight / tscale, 0.0006*tscale, txtColor);
	}

	//Converting a screen position to a pixel position and checking it if this button contains it.
	bool contains(double screenX, double screenY, int screenWidth, int screenHeight) {
		return getRect(screenWidth, screenHeight).intersects((int)((screenX + 1)*screenWidth), (int)((screenY + 1)*screenHeight));
	}
	//Activating the ButtonListener
	bool click(State &st, double screenX, double screenY, int screenWidth, int screenHeight) {
		if (exists && contains(screenX, screenY, screenWidth, screenHeight)) {
			activate(st);
			return true;
		}
		return false;
	}
	void activate(State &st) {
		if (exists)(st.*run)(amount);
	}
	void setStroke(long c) { stroke = c; }
	void setText(string text) {
		delete[] txt;
		txt = Shortcut::getFromString(text);
	}
	~Button() {
		delete[] txt;
	}
};

#endif