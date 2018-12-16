/*
* ColorManager.h
*
*  Created on: Apr 6, 2016
*      Author: Niels
*/
#ifndef COLORMANAGER_H_
#define COLORMANAGER_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

class ColorManager {
public:
	static void setColor(long argb) {
		glColor4f(((argb & 0xFF0000) >> 16) / (GLfloat)255.0, ((argb & 0xFF00) >> 8) / (GLfloat)255.0, (argb & 0xFF) / (GLfloat)255.0, ((argb & 0xFF000000) >> 24) / (GLfloat)255.0);
	}
	static void setColor(long rgb, unsigned char alpha) {
		setColor((alpha << 24) | rgb);
	}
};

#endif