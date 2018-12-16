
#ifndef ENTITY_H_
#define ENTITY_H_

using namespace std;

#include "World.h"
#include "Rect.h"

class World;

class Entity {
public:
	static double absf(double x) {
		return x < 0 ? -x : x;
	}
	enum Action {
		NONE, INVISIBLE, INVINCIBLE, DAMAGE, FALL_DAMAGE, PLACE, SMELT, EXPLODE, DIE
	};
	struct Result {
		int i;
		Action a;
		Result(Action ac = NONE, int r = 0) : a(ac), i(r) {}
	};
	enum Direction { UP, DOWN, LEFT, RIGHT, ENTITY_UP, ENTITY_UP_TOUCH, ENTITY_RIGHT, ENTITY_LEFT, ENTITY_BELOW, ENTITY_BELOW_TOUCH, VOID_BELOW };
	enum EntityType { CHARACTER, FALLINGBLOCK, PARTICLE, COIN, ENERGY_COIN};
protected:
	double x, y, mX, mY, width, height, maxGravity, gravity, speed, jumpHeight;
	EntityType type;
	double health, maxHealth, radius;
	int timeout, invincibleTime;
	bool onGround, horizontal, right, jumping, invisible, invincible, sprinting, megaJumping;
public:
	World *world;
	Entity(World *w, double wh, double h, EntityType et, double he, double _x = 0, double _y = 0) : world(w), width(wh), height(h), x(_x), y(_y), mX(0), mY(0), type(et), onGround(false), horizontal(false), right(false), jumping(false), maxGravity(16), gravity(32 / 62.5), speed(1), jumpHeight(1), maxHealth(he), health(he), radius(0), timeout(0), invisible(false), invincible(false), invincibleTime(0), sprinting(false), megaJumping(false) {
		setGravity(w);
	}
	virtual ~Entity() {}

	//Getters and setters
	EntityType getType() { return type; }
	double getRadius() { return radius; }
	int getBlockX();
	int getBlockY();
	double getY();
	double getX() {return x; }
	double getWidth() { return width; }
	double getHeight() { return height; }
	double getHealth() { return health; }
	double getMaxHealth() { return maxHealth; }
	bool isOnGround() { return onGround; }
	bool canTakeDamage() { return type != PARTICLE; }
	Rect getRect() { return Rect(x-width/2,y-height/2,width,height); }

	bool canExplode() { return radius > 0; }

	void setExplosion(double radius) {
		if(radius>=0)this->radius = radius;
	}
	void setTimeout(int time) { timeout = time; }

	bool decreaseHealth(int i) {
		if ((health -= i) <= 0)return true;
		return false;
	}

	//Drawing
	void quad(long long argb);
	void translatedQuad(double percentageX, double percentageY, double percentageWidth, double percentageHeight, long long argb) {
		double oldX = x;
		double oldY = y;
		double oldHeight = height;
		double oldWidth = width;

		x += percentageX*width;
		y += percentageY*height;
		width *= percentageWidth;
		height *= percentageHeight;

		quad(argb);

		x = oldX;
		y = oldY;
		height = oldHeight;
		width = oldWidth;
	}

	//Gravity
	void setGravity(World *w);
	void setInvisible() { invisible = true; }
	void setInvincible() { invincible = true; }
	bool isInvisible() { return invisible; }
	bool isInvincible() { return invincible; }
	void setInvincible(int i) {
		setInvincible();
		invincibleTime = i;
	}

	//Collision; thanks to ForeignGuyMike, I tried to do collision myself, didn't work out, so I interpreted his code and rewrote it for C++ instead of Java
	//The problem with my collision was; it worked awful when an object was moving two directions and it was less efficient than his collision

	unsigned char calculateCorners(double x, double y);
	Result collision();

	//Overridable methods
	virtual void draw();
	virtual Result update();
	virtual Result collide(Direction d, int i=-1);
	virtual void setSprinting(bool sprint);
	virtual void handleSprinting() {}
};

void Entity::setSprinting(bool sprint) {
	sprinting = sprint;
}

Entity::Result Entity::update() {
	if (invincibleTime > 0) {
		invincibleTime--;
		if (invincibleTime == 0)invincible = false;
	}
	if (timeout > 0) {
		timeout--;
		return Result();
	}
	return collision();
}
void Entity::draw() {}

Entity::Result Entity::collide(Entity::Direction d, int i) { return Result(); }


#endif