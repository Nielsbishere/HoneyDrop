#ifndef COIN_H_
#define COIN_H_

#include <iostream>

#include "Entity.h"
#include "World.h"

#include "Circle.h"
#include "Rect.h"

class World;

class Coin : public Entity {
private:
	int worth;
public:
	Coin(World *w, int worth, double x, double y) : Entity(w, 25, 25, COIN, 1, x, y) {
		this->worth = worth>5?5:worth;
	}
	//Getters and setters
	unsigned char getWorth() { return worth; }

	//Overridable methods
	virtual void draw();
	virtual Entity::Result update();
	virtual Entity::Result collide(Entity::Direction dir, int i = -1);
};

Entity::Result Coin::collide(Entity::Direction dir, int i) {
	if (dir==Entity::ENTITY_UP_TOUCH|| dir == Entity::ENTITY_UP || dir == Entity::ENTITY_BELOW_TOUCH || dir == Entity::ENTITY_BELOW || dir == Entity::ENTITY_LEFT || dir == Entity::ENTITY_RIGHT)return Entity::Result(Entity::DIE);
	return Entity::Result();
}

#endif