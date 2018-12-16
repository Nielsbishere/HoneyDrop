#ifndef _SHORTCUT_H_
#define _SHORTCUT_H_

using namespace std;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

#include <iostream>
#include <sstream>

//Easier way to do things, like combining strings, drawing strings, etc.
class Shortcut {
public:
	static unsigned char *getFromString(string s) {
		unsigned char *val = new unsigned char[s.size() + 1];
		strcpy_s((char *)val, s.size()+1, s.c_str());
		return val;
	}
	static void drawString(string s, double x, double y, double scale, long long color) {
		unsigned char *txt = getFromString(s);
		drawString(txt, x, y, scale*0.0006, color);
	}
	static void drawString(unsigned char* c, double x, double y, double scale, long long color) {
		glPushMatrix();
		glTranslatef((GLfloat)x, (GLfloat)y, 0);
		glScalef((GLfloat)scale, (GLfloat)scale, 0);
		glColor4f(((color & 0xFF0000) >> 16) / 255.0f, ((color & 0xFF00) >> 8) / 255.0f, (color & 0xFF) / 255.0f, ((color & 0xFF000000) >> 24) / 255.0f);
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, c);
		glPopMatrix();
	}
	static bool contains(int *arr, int length, int what) {
		for (int i = 0; i<length; i++)if (arr[i] == what)return true;
		return false;
	}
	static string asString(int n) {
		ostringstream ss;
		ss << n;
		return ss.str();
	}
	static string append(int n, const char* s, bool reverse = false) {
		ostringstream ss;
		if (!reverse)ss << n << s;
		else ss << s << n;
		return ss.str();
	}
	static string combine(string n, const char* s, bool reverse=false) {
		ostringstream ss;
		if(!reverse)ss << n << s;
		else ss << s << n;
		return ss.str();
	}
};

#endif