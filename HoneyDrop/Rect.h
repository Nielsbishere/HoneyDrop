#ifndef RECT_H_
#define RECT_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

#include "Circle.h"

class Rect {
public:
	double x, y, width, height;
	Rect(double _x, double _y, double _width, double _height) : x(_x), y(_y), width(_width), height(_height) {}
	Rect() : x(0), y(0), width(0), height(0) {}
	bool intersects(Rect r) {
		return r.width > 0 && r.height > 0 && width > 0 && height > 0 && r.x < x + width && r.x + r.width > x && r.y < y + height && r.y + r.height > y;
	}
	bool intersects(int x, int y) {
		return x >= this->x && x <= this->x + width && y >= this->y && y <= this->y + height;
	}
	static void quad(double _x, double _y, double _width, double _height, long long fill, long long stroke) {
		GLfloat x = (GLfloat)_x;
		GLfloat y = (GLfloat)_y;
		GLfloat width = (GLfloat)_width;
		GLfloat height = (GLfloat)_height;
		quad(x, y, width, height, fill);
		glColor4f(((stroke & 0xFF0000) >> 16) / 255.0f, ((stroke & 0xFF00) >> 8) / 255.0f, (stroke & 0xFF) / 255.0f, ((stroke & 0xFF000000) >> 24) / 255.0f);
		glLineWidth(2.5);
		glBegin(GL_LINES);
		glVertex2f(x, y); glVertex2f(x + width, y);
		glVertex2f(x, y + height); glVertex2f(x + width, y + height);
		glVertex2f(x, y); glVertex2f(x, y + height);
		glVertex2f(x + width, y); glVertex2f(x + width, y + height);
		glEnd();
	}
	static void quad(GLfloat x, GLfloat y, GLfloat width, GLfloat height, long long fill) {
		glBegin(GL_QUADS);
		glColor4f(((fill & 0xFF0000) >> 16) / 255.0f, ((fill & 0xFF00) >> 8) / 255.0f, (fill & 0xFF) / 255.0f, ((fill & 0xFF000000) >> 24) / 255.0f);
		glVertex2f(x, y); glVertex2f(x + width, y); glVertex2f(x + width, y + height); glVertex2f(x, y + height);
		glEnd();
	}
	bool intersects(double r, double rx, double ry) {
		return Circle(rx, ry, r).intersects(Rect(x, y, width, height));
	}
};

bool Circle::intersects(Rect r) {
	return intersects(r.x, r.y) || intersects(r.x + r.width, r.y) || intersects(r.x, r.y+r.height) || intersects(r.x+r.width, r.y+r.height) || intersects(r.x+r.width/2.0, r.y+r.height/2.0);
}

#endif