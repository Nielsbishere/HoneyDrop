
#ifndef WORLD_H_
#define WORLD_H_

using namespace std;

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/freeglut.h>

#include "Entity.h"
#include "Rect.h"
#include "Circle.h"
#include "Shortcut.h"
#include "Resources.h"

#include "FallingBlock.h"
#include "Character.h"
#include "Particle.h"
#include "Coin.h"
#include "EnergyCoin.h"

class Entity;

class World {
private:
	Entity **entities;
	unsigned char *map, *values;
	unsigned int seed;
	int max_entities, width, height, index, spawnTimer, lvl, spawned;
	double camY;
	bool hasChar, clearEntities;
	bool *entitiesb;
	Resources *r;
public:
	int tileSize;
	double dispSize;
	World(int tileSize, double dispSize, unsigned int seed, int width, int height, int level, bool hasChar, Resources *r) {
		this->tileSize = tileSize;
		this->dispSize = dispSize;
		this->width = width;
		this->height = height;
		this->seed = seed;
		spawnTimer = 60;
		lvl = level;
		spawned = 0;
		this->r = r;
		this->camY = 0;
		srand(seed);
		index = 0;
		map = new unsigned char[width*height];
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)map[j*width + i] = i == 0 || i >= width - 2 || j == 0 ? 1 : 0;
		values = new unsigned char[width];
		for (int i = 0; i < width; i++)values[i] = 0;
		entities = new Entity*[max_entities = 512];
		entitiesb = new bool[max_entities];
		this->hasChar = hasChar;
		if (hasChar) {
			entities[0] = new Character(this, r, width / 2.0*tileSize, 2 * tileSize);
			entitiesb[0] = true;
			index++;
		}
		for (int i = hasChar ? 1 : 0; i < max_entities; i++)entitiesb[i] = false;
	}
	~World() {
		for (int i = 0; i < max_entities; i++)if (entitiesb[i])delete entities[i];
		delete[] entities;
		delete[] entitiesb;
		delete[] map;
		delete[] values;
	}
	//Map helpers
	unsigned char get(int i, int j) {
		if (i < 0 || j < 0 || i >= width || j >= height)return 0;
		return map[j*width + i];
	}
	bool set(int i, int j, unsigned char type) {
		if (i < 0 || j < 0 || i >= width || j >= height)return false;
		map[j*width + i] = type;
		return true;
	}
	void addEntity(Entity *e, bool force = false, int entityTimeout = 0) {
		e->setTimeout(entityTimeout);
		for (int i = 0; i < index; i++)if (!entitiesb[i]) {
			entities[i] = e;
			entitiesb[i] = true;
			return;
		}
		if (index >= max_entities) {
			if (force) {
				bool *newEntitiesb = new bool[max_entities += 128];
				Entity **newEntities = new Entity*[max_entities];
				for (int i = 0; i < max_entities - 128; i++) {
					newEntitiesb[i] = entitiesb[i];
					newEntities[i] = entities[i];
				}
				for (int i = max_entities - 128; i < max_entities; i++)newEntitiesb[i] = false;
				delete[] entities;
				delete[] entitiesb;
				entities = newEntities;
				entitiesb = newEntitiesb;
			}
			else {
				delete e;
				return;
			}
		}
		entities[index] = e;
		entitiesb[index] = true;
		index++;
	}
	Particle *spawnParticle(int duration, int fallDuration, double gravity, long long start, long long end, double disp, double x, double y) {
		Particle *p;
		addEntity(p = new Particle(this, start, end, duration, fallDuration, gravity, x, y, disp, disp), true);
		return p;
	}
	FallingBlock *dropBlock(unsigned char data, int x, int y) {
		FallingBlock *fb;
		addEntity(fb = new FallingBlock(this, data, x*tileSize + tileSize / 2, y*tileSize + tileSize / 2), true, 1);
		return fb;
	}

	//Character methods
	bool isAlive(Resources *r) {
		for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->hasSameId(r))return true;
		return false;
	}
	Character *getCharacter(Resources *r) {
		if (!isAlive(r))return NULL;
		for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->hasSameId(r))return (Character*)entities[i];
		return NULL;
	}
	int getCharacterAmount() {
		int j = 0;
		for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i]->getType() == Entity::CHARACTER)j++;
		return j;
	}
	Character **getCharacters() {
		int j = getCharacterAmount();
		Character **c = new Character*[j];
		j = 0;
		for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i]->getType() == Entity::CHARACTER) {
			c[j] = (Character*)entities[i];
			j++;
		}
		return c;
	}
	int findEntity(Entity *e) {
		for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i] == e)return i;
		return -1;
	}
	int getLevel() { return lvl; }
	double getCamY() { return camY / tileSize*dispSize; }
	int getEntities() { return index; }
	bool hasEntity(int i) { return i>=0 && i < index && entitiesb[i]; }
	Entity *getEntity(int i) { if (!hasEntity(i))return NULL; return entities[i]; }
	int getHeight() { return height; }
	int getWidth() { return width; }

	//Common methods
	void update() {
		if (clearEntities) {
			for (int i = 0; i < index; i++)if (entitiesb[i] && entities[i]->getType() != Entity::CHARACTER)execute(i, Entity::Result(Entity::DIE));
			clearEntities = false;
		}
		for (int i = 0; i < index; i++) {
			if (entitiesb[i] && !entities[i]->isInvisible())execute(i, entities[i]->update());
		}

		//Spawning
		if (r->getProperty(0) || !hasChar) {
			if (spawnTimer != 0)spawnTimer--;
			else {
				double perc = 1.0 - spawned / width / height;
				if (perc <= 0.25)perc = 0.25;
				int k = 1 + lvl / 3 + rand() % 3 + (int)(1 / perc);
				if (k >= 5)k = 5;
				int *m = new int[k];
				for (int i = 0; i < k; i++)m[i] = width + 1;
				for (int i = 0; i < k; i++) {
					int wh = rand() % (width - 3) + 1;
					while (Shortcut::contains(m, k, wh))wh = rand() % (width - 3) + 1;
					m[i] = wh;
					double j = 20 - lvl / 2.0;
					for (int k = height - 1; k >= 0; k--) {
						if (map[k*width + wh] != 0) {
							j += k;
							break;
						}
					}
					int c = rand() % 10000;
					if (c < (r->getPerk(10) == 2 ? 9000 : 9500))addEntity(new FallingBlock(this, c < 10 * (lvl - 2) ? 251 : c < 60 * (lvl - 1) ? 250 : values[wh], wh*tileSize + tileSize / 2, j*tileSize + tileSize / 2));
					else {
						c = rand() % 10;
						if (c < 8)addEntity(new Coin(this, (int)((r->getPerk(11) == 2 ? 1.5 : 1)*(rand() % (lvl + 1) + 1)), wh*tileSize + tileSize / 2, j*tileSize + tileSize / 2));
						else addEntity(new EnergyCoin(this, (int)((r->getPerk(11) == 2 ? 1.5 : 1)*(rand() % (lvl + 1) + 1) * 20), wh*tileSize + tileSize / 2, j*tileSize + tileSize / 2));
					}
					if (values[wh] < 249)values[wh]++;
				}
				delete m;
				spawnTimer = (int)((60 - lvl * 2)*perc);
				if (spawnTimer < 40)spawnTimer = 40;
			}
		}

		//Fixing the camera
		if (isAlive(r)) focus(getCharacter(r));
		else {
			int blocks = 0;
			int width = (int)(2.0 / dispSize);
			int height = (int)(1.0 / dispSize);
			for (int i = 0; i < width; i++)
				for (int j = 0; j < height; j++)if (get(i, (int)(camY / tileSize + j)) != 0)blocks++;
			if (blocks >= width * height / 2.0)camY += tileSize / 62.5;
		}
		
	}
	void focus(Entity *e) {
		camY = e->getY() <= (1.0/dispSize+0.5) * tileSize/2 ? 0 : e->getY() - (1.0 / dispSize + 0.5) * tileSize / 2;
	}
	void execute(int i, Entity::Result res) {
		bool leaveParticles = false;
		if (res.a == Entity::INVISIBLE) {
			entities[i]->setInvisible();
			return;
		}
		if (res.a == Entity::INVINCIBLE) entities[i]->setInvincible();
		if (entities[i]->isInvisible())return;
		//Placing and making sure that it leaves particles when it can't
		if (res.a == Entity::PLACE) {
			if (entities[i]->getType() == Entity::FALLINGBLOCK) {
				int x = entities[i]->getBlockX();
				int y = entities[i]->getBlockY();
				if (!set(x, y, ((FallingBlock*)entities[i])->getData() + 1))leaveParticles = true;
			}
			res.a = Entity::DIE;
		}
		//Explosions
		if (res.a == Entity::EXPLODE) {
			if (entities[i]->canExplode()) {
				int radius = res.i; 
				double mid = radius / 2.0;
				//Setting the blocks to 0 where they should be 0
				for (int x = 0; x<radius; x++) {
					for (int y = 0; y<radius; y++) {
						double difX = x - mid;
						double difY = y - mid;
						if (sqrt(pow(difX, 2) + pow(difY, 2)) <= mid && (int)(difX + entities[i]->getBlockX()) > 0 && (int)(difX + entities[i]->getBlockX()) < width-2)set((int)difX + entities[i]->getBlockX(), (int)difY + entities[i]->getBlockY(), 0);
					}
				}
				//Spawning particles; the outside is red-black and the inside is yellow
				double particlePerRadius = 1/8.0*radius;
				int part = 0;
				for(double x=0;x<radius;x+= particlePerRadius)
					for (double y = 0; y<radius; y+= particlePerRadius) {
						double difX = x - mid;
						double difY = y - mid;
						double r = sqrt(pow(difX, 2) + pow(difY, 2));
						if (r > mid)continue;
						part++;
						int duration = (int)((1.0 - r / mid) * 80) + rand()%5;
						long long start = 0xFFA00000 | (long long)((1 - r / mid) * 0xA0) << 8;
						spawnParticle(duration, duration*2, -1 / 1024.0 * tileSize, start, 0xFF0A0A00, particlePerRadius*tileSize, (x-mid)*tileSize+entities[i]->getX(), (y - mid)*tileSize + entities[i]->getY());
					}
				//Making sure that the physics of the blocks above update
				for (int r = 0; r < radius; r++) {
					double difX = r - mid;
					int x = (int)difX + entities[i]->getBlockX();
					for (int j = (int)(entities[i]->getBlockY() - radius / 2.0); j < height; j++) {
						if (x <= 0 || x >= width - 2 || j < 0 || get(x, j-1)!=0)continue;
						unsigned char d = 0;
						int _y = j;
						if ((d = get(x, _y)) != 0) {
							set(x, _y, 0);
							dropBlock(d, x, _y);
						}
					}
				}
			}
			res.a = Entity::DIE;
		}
		//Smelt event; like what happens with magma; it can only smelt a couple blocks before it disappears
		if (res.a == Entity::SMELT) {
			int x = entities[i]->getBlockX();
			int y = entities[i]->getBlockY();
			unsigned char old = get(x, y);
			set(x, y-1, 0);
			//Dropping the block and then destroying it for the particles (Shorter)
			Entity *fb = dropBlock(old, x, y - 1);
			execute(findEntity(fb), Entity::Result(Entity::DAMAGE));
			//Damaging it
			res.a = Entity::DAMAGE;
		}
		if (res.a == Entity::FALL_DAMAGE) {
			int dmg = (int)(entities[i]->getMaxHealth()*0.25);
			if (dmg == 0)dmg = 1;
			if (dmg >= entities[i]->getHealth()) {
				res.a = Entity::DIE;
				leaveParticles = true;
			}else{
				res.a = Entity::DAMAGE;
				res.i = dmg - 1;
			}
			if (entities[i]->getType() == Entity::CHARACTER) {
				Resources *r = ((Character*)entities[i])->getResources();
				if (r->getPerk(4) == 2) {
					r->consume(4);
					res.a = Entity::NONE;
				}
			}
		}
		if (res.a == Entity::DAMAGE && !entities[i]->isInvincible()) {
			if (entities[i]->decreaseHealth(res.i+1)) {
				res.a = Entity::DIE;
				leaveParticles = true;
			}
		}
		//Spawning entity particles and deleting the entity
		if (res.a == Entity::DIE) {
			if (entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->getResources()->getPerk(5) == 2) {
				Character *c = (Character*)entities[i];
				c->getResources()->consume(5);
				c->setInvincible(30);
				clearEntities = true;
				c->addEnergy(c->getMaxEnergy());
				for (int i = 0; i < 4; i++)c->heal();
				c->addEnergy(c->getMaxEnergy());
				return;
			}
			if (entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->getResources()->getPerk(3) == 2)((Character*)entities[i])->getResources()->consume(3);
			if (entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->getResources()->getPerk(10) == 2)((Character*)entities[i])->getResources()->consume(10);
			if (entities[i]->getType() == Entity::CHARACTER && ((Character*)entities[i])->getResources()->getPerk(11) == 2)((Character*)entities[i])->getResources()->consume(11);
			if (leaveParticles) {
				double particlePerEntity = 1 / 8.0;
				if (entities[i]->getType() == Entity::CHARACTER) {
					long long start = ((Character*)entities[i])->getResources()->getColor();
					long long end = 0;
					for (double x = 0; x<entities[i]->getWidth() / tileSize; x += particlePerEntity*(entities[i]->getWidth() / tileSize))
						for (double y = 0; y < entities[i]->getHeight() / tileSize; y += particlePerEntity*(entities[i]->getHeight() / tileSize)) {
							if (rand() % 5 != 0)continue;
							double distX = (x - entities[i]->getWidth() / 2 / tileSize);
							double distY = (y - entities[i]->getHeight() / 2 / tileSize);
							double r = sqrt(pow(distX, 2) + pow(distY, 2));
							spawnParticle((int)(40 - 30 * r / (entities[i]->getWidth() / 2) - rand() % 5), 1, 1 / 64.0 * tileSize, start, end, particlePerEntity*tileSize, distX*tileSize + entities[i]->getX(), distY*tileSize + entities[i]->getY());
						}
				}
				else if (entities[i]->getType() == Entity::FALLINGBLOCK) {
					unsigned char data = ((FallingBlock*)entities[i])->getData();
					long long r = (long long)((240 - 142) * (1 - data / 249.0) + 142);
					long long g = (long long)((147 - 87) * (1 - data / 249.0) + 87);
					long long b = (long long)((53 - 31) * (1 - data / 249.0) + 31);
					long long start = (0xFF << 24) | (r << 16) | (g << 8) | b;
					long long end = start & 0xFFFFFF;
					for(double x=0;x<entities[i]->getWidth()/tileSize;x+=particlePerEntity*(entities[i]->getWidth()/tileSize))
						for (double y = 0; y < entities[i]->getHeight() / tileSize; y += particlePerEntity*(entities[i]->getHeight() / tileSize)) {
							if (rand() % 10 != 0)continue;
							double distX = (x - entities[i]->getWidth() / 2 / tileSize);
							double distY = (y - entities[i]->getHeight() / 2 / tileSize);
							double r = sqrt(pow(distX, 2) + pow(distY, 2));
							spawnParticle((int)(40 - 30*r/(entities[i]->getWidth()/2) - rand()%5), 1, 1 / 64.0 * tileSize, start, end, particlePerEntity*tileSize, distX*tileSize + entities[i]->getX(), distY*tileSize + entities[i]->getY());
						}
				}
			}
			delete entities[i];
			entitiesb[i] = false;
		}
	}
	void draw(Resources *r) {
		if (!hasChar) Rect::quad(-1, -1, 2, 2, 0x2000000);
		else {
			if (!isAlive(r))Rect::quad(-1, -1, 2, 2, 0x80C00000);
		}
		for (int i = 0; i < max_entities; i++)if (entitiesb[i] && !entities[i]->isInvisible()) {
			if (entities[i]->getY() < camY - (1.0 / dispSize / 2 + 1)*tileSize || entities[i]->getY() > camY + (2.0 / dispSize / 2 + 1)*tileSize)continue;
			entities[i]->draw();
		}

		double rem = (camY / tileSize) - ((int)camY / tileSize);

		for (int i = 0; i <= (int)(2.0 / dispSize) + 1; i++) {
			for (int j = 0; j <= (int)(1.0 / dispSize) + 1; j++) {
				int x = i;
				int y = (int)(j + camY / tileSize);
				if (x<0 || x >= width || y >= height || y<0)continue;
				unsigned char data = map[y*width + x];
				if (data == 0)continue;
				if (data>0)data -= 1;
				FallingBlock::sdraw(i*dispSize - 1.0f, j*dispSize * 2 - 1.0f - rem*dispSize, dispSize, dispSize * 2, data);
			}
		}
		if (isAlive(r)) {
			double green = getCharacter(r)->getHealth() / getCharacter(r)->getMaxHealth();
			double red = getCharacter(r)->getMaxHealth() / 10;
			Rect::quad((GLfloat)-0.2, (GLfloat)0.9, (GLfloat)0.4, (GLfloat)(GLfloat)0.1, 0xFF0A0A0A);
			Rect::quad((GLfloat)-0.2, (GLfloat)0.9, (GLfloat)(0.4 * red), (GLfloat)0.1, 0xFFA00000);
			Rect::quad((GLfloat)-0.2, (GLfloat)0.9, (GLfloat)(0.4 * green * red), (GLfloat)0.1, 0xFF00A000);
			green = getCharacter(r)->getEnergy() / getCharacter(r)->getMaxEnergy();
			Rect::quad((GLfloat)0.6, (GLfloat)0.9, (GLfloat)0.4, (GLfloat)0.1, 0xFF0A0A0A);
			Rect::quad((GLfloat)0.6, (GLfloat)0.9, (GLfloat)(0.4 * green), (GLfloat)0.1, 0xFF0080FF);
		}
	}
};

//Drawing particle
void Particle::draw() {
	Entity::quad(start);
	int ftimer = timer - fallDuration < 0 ? 0 : timer - fallDuration;
	double ftime = (double)ftimer / max;
	ftime /= ftime;
	Entity::quad(end & 0xFFFFFF | (255-(long long)(ftime*255)) << 24);
}
//Drawing coin
void Coin::draw() {
	GLfloat _x = (GLfloat)(x / world->tileSize*world->dispSize - 1);
	GLfloat _y = (GLfloat)(y / world->tileSize*world->dispSize * 2 - 1 - world->getCamY() * 2);
	GLfloat dispSizeX = (GLfloat)(world->dispSize*width / world->tileSize);
	GLfloat dispSizeY = (GLfloat)(world->dispSize*height / world->tileSize * 2);
	Circle::draw(_x, _y, (GLfloat)(dispSizeX*0.5), (GLfloat)(dispSizeY*0.5), 0xFF808000);
	Circle::draw(_x, _y, (GLfloat)(dispSizeX*0.4), (GLfloat)(dispSizeY*0.4), 0xFFA0A000);
}
void EnergyCoin::draw() {
	GLfloat _x = (GLfloat)(x / world->tileSize*world->dispSize - 1);
	GLfloat _y = (GLfloat)(y / world->tileSize*world->dispSize * 2 - 1 - world->getCamY() * 2);
	GLfloat dispSizeX = (GLfloat)(world->dispSize*width / world->tileSize);
	GLfloat dispSizeY = (GLfloat)(world->dispSize*height / world->tileSize * 2);
	Circle::draw(_x, _y, (GLfloat)(dispSizeX*0.5), (GLfloat)(dispSizeY*0.5), 0xFF0080FF);
	Circle::draw(_x, _y, (GLfloat)(dispSizeX*0.4), (GLfloat)(dispSizeY*0.4), 0xFF3B9AF8);
}

//Declarations of fallingblock
void Entity::quad(long long argb) {
	glBegin(GL_QUADS);
	GLfloat _x = (GLfloat)(x / world->tileSize*world->dispSize - 1 - width/2.0/world->tileSize*world->dispSize);
	GLfloat _y = (GLfloat)(y / world->tileSize*world->dispSize * 2 - 1 - height / 2.0 / world->tileSize*world->dispSize - world->getCamY()*2);
	GLfloat dispSizeX = (GLfloat)(world->dispSize*width / world->tileSize);
	GLfloat dispSizeY = (GLfloat)(world->dispSize*height / world->tileSize * 2);
	Rect::quad(_x, _y, dispSizeX, dispSizeY, argb);
}

void FallingBlock::validateSize() {
	if (world->tileSize != width) {
		width = world->tileSize;
		height = width;
	}
}

void FallingBlock::init() {
	if (data == 251)maxHealth = health = (world->getLevel() * 2);
	this->gravity *= 0.1+world->getLevel()/32.0;
	this->maxGravity = this->gravity * 60;
}

void FallingBlock::draw() {
	if (data < 250) {
		Entity::quad(0xFFF09335);
		Entity::quad((long long)((1.0f - data / 249.0f)*0.1f * 255) << 24);
	}
	else if (data == 250)Entity::quad(0xFF4C4C4C);	//Bomb
	else if (data == 251)Entity::quad(0xFFA32828);	//Magma
}
//Character drawing
void Character::draw() {
	bool b = y <= (1.0 / world->dispSize + 0.5) * world->tileSize / 2;
	glTranslatef(0, (GLfloat)(-height/world->tileSize*world->dispSize / (b ? 2 : 4)), 0);
	//Main body
	Entity::quad(r->getColor());
	Entity::quad(0x50000000);
	Entity::translatedQuad(1/128.0, 1 / 16.0, 11 / 16.0, 22 / 32.0, r->getColor());
	//Eyes
	Entity::translatedQuad(-1 / 4.0, 3 / 8.0, -11 / 32.0 * eyeScale, 22 / 64.0 * eyeScale, 0xFF0A0A0A);
	Entity::translatedQuad(1 / 4.0, 3 / 8.0, 11 / 32.0 * eyeScale, 22 / 64.0 * eyeScale, 0xFF0A0A0A);
	Entity::translatedQuad(-2 / 8.0, 7 / 16.0, -7 / 32.0 * eyeScale, 14 / 64.0 * eyeScale, 0xFFF0F0F0);
	Entity::translatedQuad(2 / 8.0, 7 / 16.0, 7 / 32.0 * eyeScale, 14 / 64.0 * eyeScale, 0xFFF0F0F0);
	//Invincible animation
	if (invincible) {
		if(invincibleTime%20<15)Entity::quad(0x20FFFFFF);
	}
	glTranslatef(0, (GLfloat)(height / world->tileSize*world->dispSize / (b ? 2 : 4)), 0);
}
//Character fixing tileSize
void Character::fixBounds() {
	width = height = world->tileSize/32.0*25;
}
void Character::init() {
	maxEnergy = energy = (120.0 * world->getLevel());
}

//Entity getters and setters using world related functions
void Entity::setGravity(World *w) {
	gravity = w->tileSize / 62.5;
	maxGravity = gravity * 25;
}

int Entity::getBlockX() { return (int)(x / world->tileSize); }
int Entity::getBlockY() { return (int)(y / world->tileSize); }
double Entity::getY() { return y; }

//Entity collision

unsigned char Entity::calculateCorners(double x, double y) {
	int leftTile = (int)(x - width / 2) / world->tileSize;
	int rightTile = (int)(x + width / 2 - 1) / world->tileSize;
	int topTile = (int)(y - height / 2) / world->tileSize;
	int bottomTile = (int)(y + height / 2 - 1) / world->tileSize;

	bool tl = world->get(leftTile, topTile) != 0;
	bool tr = world->get(rightTile, topTile) != 0;
	bool bl = world->get(leftTile, bottomTile) != 0;
	bool br = world->get(rightTile, bottomTile) != 0;

	return tl << 3 | tr << 2 | bl << 1 | (br?1:0);
}

Entity::Result Character::collide(Entity::Direction dir, int i) {
	if (dir == Entity::ENTITY_UP)return Entity::Result(Entity::FALL_DAMAGE);
	if (dir == Entity::DOWN && y >= world->tileSize * world->getHeight() && (x <= world->tileSize || x>=(world->getWidth()-3)*world->tileSize)) return Entity::Result(Entity::INVISIBLE);
	return Entity::Result();
}

Entity::Result FallingBlock::collide(Entity::Direction dir, int i) {
	if ((dir == Entity::DOWN || dir == Entity::ENTITY_BELOW || dir == Entity::VOID_BELOW) && data == 250)return Entity::Result(Entity::EXPLODE, (int)(getRadius()*(rand() % 5 + 7) / 10.0));
	if (dir == Entity::DOWN && data == 251)return Entity::Result(Entity::SMELT);
	if (data == 251 && dir == Entity::VOID_BELOW)return Entity::Result(Entity::DIE);
	if ((dir == Entity::DOWN || dir == Entity::VOID_BELOW))return Entity::Result(Entity::PLACE);
	if (dir == Entity::ENTITY_BELOW) {
		if (world->hasEntity(i) && world->getEntity(i)->getType() == Entity::CHARACTER) {
			Resources *r = ((Character*)world->getEntity(i))->getResources();
			int k = (int)(((Character*)world->getEntity(i))->getHealth() * 0.25);
			if (k == 0)k = 1;
			if (r->getPerk(4) == 2 || ((Character*)world->getEntity(i))->getHealth()>=k)return Entity::Result(Entity::DIE);
		}
		return Entity::Result(Entity::NONE);
	}
	return Entity::Result();
}

//Updating for coins and energycoins
Entity::Result Coin::update() {
	Circle circ = Circle(5 * world->tileSize, x, y);
	int characters = world->getCharacterAmount();
	if (characters > 0) {
		Character **c = world->getCharacters();
		Character *closest = NULL;
		double clos = 0;
		for (int i = 0; i < characters; i++) {
			if (c[i]->getResources()->getPerk(3) != 2)continue;
			if (circ.intersects(c[i]->getRect())) {
				double r = sqrt(pow(c[i]->getX() - x, 2) + pow(c[i]->getY() - y, 2));
				if (clos == 0 || clos < r) {
					clos = r;
					closest = c[i];
				}
			}
		}
		if (clos != 0) {
			if (clos < 3 * world->tileSize) {
				if (closest->getY() > y)mY += 5 - 0.5 * clos / world->tileSize;
				else mY -= 5 - 0.5 * clos / world->tileSize;
			}
			if (closest->getX() > x)mX += 5 - clos / world->tileSize;
			else mX -= 5 - clos / world->tileSize;
		}
		delete[] c;
	}
	return Entity::update();
}

//Map collision
Entity::Result Entity::collision() {
	Result result = Result();

	int bx = (int)x / world->tileSize;
	int by = (int)y / world->tileSize;

	double xdest = x + mX;
	double ydest = y + mY;

	double xtemp = x;
	double ytemp = y;

	bool topLeft = false, topRight = false, bottomLeft = false, bottomRight = false;

	unsigned char c = calculateCorners(x, ydest);
	bottomRight = (c & 0b1) == 1;
	bottomLeft = (c & 0b10) == 2;
	topRight = (c & 0b100) == 4;
	topLeft = (c & 0b1000) == 8;
	if (mY < 0) {
		if (topLeft || topRight) {
			mY = 0;
			ytemp = by * world->tileSize + height / 2;
			onGround = true;
			Result r = collide(DOWN);
			if (r.a > result.a)result = r;
		}
		else {
			ytemp += mY;
			onGround = false;
		}
	}
	else if (mY > 0 && type!=Entity::PARTICLE) {
		if (bottomLeft || bottomRight) {
			mY = 0;
			ytemp = (mY + 1) * world->tileSize - height / 2;
			Result r = collide(UP);
			if (r.a > result.a)result = r;
		}
		else ytemp += mY;
		onGround = false;
	}

	c = calculateCorners(xdest, y);
	bottomRight = (c & 0b1) == 1;
	bottomLeft = (c & 0b10) == 2;
	topRight = (c & 0b100) == 4;
	topLeft = (c & 0b1000) == 8;
	if (mX < 0) {
		if (topLeft || bottomLeft) {
			mX = 0;
			xtemp = bx * world->tileSize + width / 2;
			Result r = collide(LEFT);
			if (r.a > result.a)result = r;
		}
		else xtemp += mX;
	}
	else if (mX > 0) {
		if (topRight || bottomRight) {
			mX = 0;
			xtemp = (bx + 1) * world->tileSize - width / 2;
			Result r = collide(RIGHT);
			if (r.a > result.a)result = r;
		}
		else xtemp += mX;
	}

	if (onGround) {
		c = calculateCorners(x, ydest - 1);
		bottomRight = (c & 0b1) == 1;
		bottomLeft = (c & 0b10) == 2;
		topRight = (c & 0b100) == 4;
		topLeft = (c & 0b1000) == 8;
		if (!topLeft && !topRight) {
			onGround = false;
			Result r = collide(DOWN);
			if (r.a > result.a)result = r;
		}
		else onGround = true;
	}

	//Entity collision
	int id = -1;
	for (int i = 0; i < world->getEntities(); i++)if (world->hasEntity(i) && world->getEntity(i) == this) {
		break;
		id = i;
	}

	for (int i = 0; i < world->getEntities(); i++)if (world->hasEntity(i)) {
		Entity *e = world->getEntity(i);
		if (e == this || e->isInvisible() || (e->getType() == Entity::PARTICLE && !(type==Entity::CHARACTER||isInvincible())))continue;

		bool yDown = mY < 0 && ytemp - height / 2 < e->y + e->height / 2 && y - height / 2 > e->y - e->height / 2;
		bool yUp = mY > 0 && ytemp + height / 2 > e->y - e->height / 2 && y - height / 2 < e->y + e->height / 2;
		bool xLeft = mX < 0 && xtemp - width / 2 < e->x + e->width / 2 && x - width / 2 > e->x - e->width / 2;
		bool xRight = mX > 0 && xtemp + width / 2 > e->x - e->width / 2 && x - width / 2 < e->x + e->width / 2;

		bool hy = yDown || yUp;
		bool hx = xLeft || xRight;

		if (hy && !hx)hx = (x - width/2 <= e->x - width / 2 && x + width/2 > e->x - width/2) || (x + width / 2 > e->x + width / 2 && x - width / 2 < e->x + width / 2);
		if (hx && !hy)hy = (y - height / 2 <= e->y - height / 2 && y + height / 2 > e->y - height / 2) || (y + height / 2 > e->y + height / 2 && y - height / 2 < e->y + height / 2);

		if (!(hx&&hy))continue;

		if (e->getType() == Entity::COIN && type == Entity::CHARACTER) {
			int worth = ((Coin*)e)->getWorth();
			world->execute(i, e->collide(Entity::ENTITY_UP, id));
			((Character*)this)->getResources()->addCoins(worth);
			continue;
		}else if (type == Entity::COIN && e->getType() == Entity::CHARACTER) {
			int worth = ((Coin*)this)->getWorth();
			((Character*)e)->getResources()->addCoins(worth);
			return Result(DIE);
		}
		if (e->getType() == Entity::ENERGY_COIN && type == Entity::CHARACTER) {
			int worth = ((EnergyCoin*)e)->getWorth();
			world->execute(i, e->collide(Entity::ENTITY_UP, id));
			((Character*)this)->addEnergy(worth);
			continue;
		}
		else if (type == Entity::ENERGY_COIN && e->getType() == Entity::CHARACTER) {
			int worth = ((EnergyCoin*)this)->getWorth();
			((Character*)e)->addEnergy(worth);
			return Result(DIE);
		}

		if (e->getType() == Entity::PARTICLE && type == Entity::CHARACTER) {
			int h = (int)(maxHealth*0.3);
			if (h < 1)h = 1;
			if(!invincible)health -= h;
			setInvincible(10);
			Entity::Result res = Entity::Result(health<0?Entity::DIE:Entity::NONE);
			if (res.a > result.a)result = res;
			continue;
		}

		if (yDown) {
			Result r = collide(Entity::ENTITY_BELOW, i);
			if (r.a >= result.a)result = r;
			mY = 0;
			ytemp = e->y + e->height / 2 + height / 2;
			world->execute(i, e->collide(Entity::ENTITY_UP, id));
		}
		else if (yUp) {
			Result r = collide(Entity::ENTITY_UP, i);
			if (r.a >= result.a)result = r;
			mY = 0;
			ytemp = e->y - e->height / 2 - height / 2;
			world->execute(i, e->collide(Entity::ENTITY_BELOW, id));
		} 
		if (world->hasEntity(i) && world->getEntity(i) == e) {
			if (xLeft) {
				Result r = collide(Entity::ENTITY_LEFT, i);
				if (r.a >= result.a)result = r;
				mX = 0;
				xtemp = e->x + e->width / 2 + width / 2;
				world->execute(i, e->collide(Entity::ENTITY_RIGHT, id));
			}
			else if (xRight) {
				Result r = collide(Entity::ENTITY_RIGHT, i);
				if (r.a >= result.a)result = r;
				mX = 0;
				xtemp = e->x - e->width / 2 - width / 2;
				world->execute(i, e->collide(Entity::ENTITY_LEFT, id));
			}
		}
	}

	//Adjusting movement

	if (jumping && onGround) {
		double megaj = (megaJumping ? 1.1 + world->getLevel()*0.1 : 1);
		if (megaj > 1.3)megaj = 1.3;
		mY = gravity * 20 * jumpHeight * megaj;
		onGround = false;
	}

	if (!onGround) {
		mY += -gravity;

		if (mY > 0) {
			jumping = false;
			megaJumping = false;
		}
		if (mY < -maxGravity) mY = -maxGravity;
	}
	if (horizontal) {
		double spr = (sprinting ? 1.5 + 0.1*world->getLevel() : 1);
		if (spr > 2)spr = 2;
		if (absf(mX) < world->tileSize / 7.5*speed*spr) {
			if (sprinting)handleSprinting();
			mX += (!right ? -1 : 1)*world->tileSize / 30.0*(!onGround ? 2 : 1);
		}
	}
	else if (mX != 0) {
		mX += (mX>0 ? -1 : 1)*world->tileSize / 20.0*(onGround ? 2 : 1);
		if (mX<0 && right)mX = 0;
		else if (mX>0 && !right)mX = 0;
	}
	if (y - height / 2 < 0) {
		Result r = collide(Entity::VOID_BELOW);
		if (r.a > NONE)return r;
	}
	if (y + height/2 < 0)return Result(DIE);

	//Finish collision
	x = xtemp;
	y = ytemp;

	return result;
}

#endif