#ifndef CHARACTER_H_
#define CHARACTER_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

#include "Rect.h"

#include "Entity.h"
#include "Resources.h"

#include <cmath>

class Character : public Entity {
private:
	Resources *r;
	double eyeScale;
	double maxEnergy, energy;
	int healTimer;
public:
	Character(World *w, Resources *r, double x, double y) : Entity(w, 25, 25, Entity::CHARACTER, r->getPerk(2)+1, x, y) {
		this->r = r;
		speed = 0.1*sqrt(r->getPerk(0) + 1) + 0.5;
		jumpHeight = 0.15*sqrt(r->getPerk(1) + 1) + 0.6;
		if (r->getPerk(9) == 2)speed *= 0.4;
		if (r->getPerk(10) == 2 && health>=2)maxHealth = health/=2;
		if (r->getPerk(11) == 2)jumpHeight *= 0.85;
		eyeScale = 0.75;
		fixBounds();
		init();
		healTimer = 0;
	}
	//Generic input methods
	void Character::jump() {
		if (!jumping && onGround)jumping = true;
	}
	void Character::megaJump() {
		if (!jumping && onGround && energy >= 30) {
			jumping = true;
			megaJumping = true;
			energy -= 30;
		}
	}
	void Character::heal() {
		if (energy >= maxEnergy*0.25 && health!=maxHealth && healTimer==0) {
			energy -= maxEnergy*0.25;
			double heal = maxHealth*0.5;
			if (heal < 1)heal = 1;
			health += heal;
			if (health > maxHealth)health = maxHealth;
			healTimer = 1500;
		}
	}
	void Character::addEnergy(double i) {
		if (i < 0)return;
		energy += i;
		if (energy > maxEnergy)energy = maxEnergy;
	}
	void init();
	void Character::turn(bool right, bool horizontal) {
		this->right = right;
		this->horizontal = horizontal;
	}

	//Fixing the tileSize
	void fixBounds();

	//Getters and setters
	bool hasSameId(Resources *r) {
		return this->r->hasSameId(r);
	}
	double getEnergy() { return energy; }
	double getMaxEnergy() { return maxEnergy; }

	Resources *getResources() { return r; }

	//Overridable methods
	virtual Entity::Result update();
	virtual void draw();
	virtual Entity::Result collide(Entity::Direction dir, int i=-1);
	virtual void setSprinting(bool sprint);
	virtual void handleSprinting();
};

Entity::Result Character::update() {
	if (healTimer > 0)healTimer--;
	Entity::Result res = Entity::update();
	if (res.a == Entity::DIE) {
		if (r->getPerk(10) == 2)r->consume(10);
		if (r->getPerk(11) == 2)r->consume(11);
	}
	return res;
}

void Character::setSprinting(bool sprint) {
	if (sprint&&energy <= 0)return;
	sprinting = sprint;
}
void Character::handleSprinting() {
	energy -= 0.5;
	if (energy <= 0)energy = 0;
}

#endif