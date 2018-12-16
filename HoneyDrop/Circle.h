#ifndef CIRCLE_H_
#define CIRCLE_H_

#define DEG2RAD 3.14159/180.0

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

#include "ColorManager.h"
#include "Rect.h"
#include <cmath>

class Rect;

class Circle {
private:
	double radius, rX, rY;
public:
	Circle(double r, double rx, double ry) : radius(r), rX(rx), rY(ry) {}
	bool intersects(double x, double y) {
		if (x<rX - radius || x>rX + radius || y<rY - radius || y>rY + radius)return false;
		return sqrt(pow(x - rX, 2) + pow(y - rY, 2)) <= radius;
	}
	bool intersects(Rect r);
	static void draw(GLfloat x, GLfloat y, GLfloat w, GLfloat h, long long color) {
		glTranslatef(x, y, 0);
		ColorManager::setColor((long)color);
		drawEllipse(w, h);
		glTranslatef(-x, -y, 0);
	}
	static void drawEllipse(float radiusX, float radiusY)
	{
		int i;
		glBegin(GL_LINES);
		for (i = 0; i<360; i++){
			double rad = i*DEG2RAD;
			glVertex2f(0, 0);
			glVertex2f((GLfloat)(cos(rad)*radiusX), (GLfloat)(sin(rad)*radiusY));
		}
		glEnd();
	}
};

#endif