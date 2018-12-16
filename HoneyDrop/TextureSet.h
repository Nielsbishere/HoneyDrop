#ifndef TEXTURESET_H_
#define TEXTURESET_H_

using namespace std;
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//OpenGL
#include <GL/freeglut.h>

//Loading PNG
#include <SOIL.h>

class TextureSet {
private:
	vector<GLint> textures;
	vector<string> textureNames;
public:
	~TextureSet() {
		textures.clear();
		textureNames.clear();
	}
	void load(string texture) {
		cout << "Loading " << texture << endl;
		textureNames.push_back(texture);
		textures.push_back(SOIL_load_OGL_texture(texture.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_NTSC_SAFE_RGB));
		cout << "Loaded " << texture << endl;
	}
	int getTextureId(string text) {
		return std::distance(textureNames.begin(), std::find(textureNames.begin(), textureNames.end(), text));
	}
	bool hasTexture(string text) {
		return std::find(textureNames.begin(), textureNames.end(), text) != textureNames.end();
	}
	GLint getTexture(string text) {
		if (!hasTexture(text))return 0;
		return textures.at(getTextureId(text));
	}
	void draw(string txt, double x, double y, double width, double height) {
		if (!hasTexture(txt))return;
		glBindTexture(GL_TEXTURE_2D, getTexture(txt));
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2f(x, y);
		glTexCoord2i(1, 0); glVertex2f(x+width, y);
		glTexCoord2i(1, 1); glVertex2f(x+width, y+height);
		glTexCoord2i(0, 1); glVertex2f(x, y+height);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

#endif