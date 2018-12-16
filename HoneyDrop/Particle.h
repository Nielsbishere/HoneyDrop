#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "Entity.h"
#include "World.h"

#include "Rect.h"

class World;

class Particle : public Entity {
private:
	long long start, end;
	int timer, max;
	int fallDuration;
public:
	Particle(World *w, long long start, long long end, int time, int fallDuration, double gravity, double x, double y, double sizeX, double sizeY) : Entity(w, sizeX, sizeY, PARTICLE, 1, x, y) {
		this->start = start;
		this->end = end;
		timer = time+fallDuration;
		max = timer;
		this->fallDuration = fallDuration;
		this->gravity = gravity / 5.0;
		this->maxGravity = gravity;
	}

	//Overridable methods
	virtual void draw();
	virtual Entity::Result update();
};
Entity::Result Particle::update() {
	if (timer == fallDuration) {
		gravity *= -64 / 5.0;
		maxGravity *= -64;
	}
	if (timer > 0)timer--;
	else return Result(Entity::DIE);
	return Entity::update();
}

#endif