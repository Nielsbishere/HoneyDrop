#ifndef FALLING_BLOCK_
#define FALLING_BLOCK_

#include "Entity.h"
#include "World.h"

#include "Rect.h"

class World;

class FallingBlock : public Entity {
private:
	unsigned char data;
public:
	FallingBlock(World *w, unsigned char data, double x, double y) : Entity(w, 32, 32, FALLINGBLOCK, 1, x, y) {
		validateSize();
		this->data = data;
		if (data == 250)setExplosion(5.0);
		init();
	}
	//To make sure that the entity is really meant to be 32x32, because it can be that the world->tileSize was changed
	//Declaration is in World
	void validateSize();
	void init();

	//Quick draw methods

	static void sdraw(double _x, double _y, double dispSizeX, double dispSizeY, unsigned char data) {
		glBegin(GL_QUADS);
		Rect::quad((GLfloat)_x, (GLfloat)_y, (GLfloat)dispSizeX, (GLfloat)dispSizeY, 0xFFF09335);
		Rect::quad((GLfloat)_x, (GLfloat)_y, (GLfloat)dispSizeX, (GLfloat)dispSizeY, (long long)((1.0f - data / 249.0f)*0.1f*255)<<24);
	}

	//Getters and setters
	unsigned char getData() { return data; }

	//Overridable methods
	virtual void draw();
	virtual Entity::Result update();
	virtual Entity::Result collide(Entity::Direction dir, int i=-1);
};
Entity::Result FallingBlock::update() {
	return Entity::update();
}

#endif