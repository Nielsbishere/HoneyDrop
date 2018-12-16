#ifndef _RESOURCES_H
#define _RESOURCES_H

using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

#include "Shortcut.h"
#include "ObjectWriter.h"
#include "ObjectReader.h"

class Resources {
	long long color;
	int coins;
	int *perks;
	bool *colors;
	bool *levels;
	bool *wonLevels;
	bool *properties;
	int charId;
	bool winScreen;
public:
	//Constructors and deconstructors
	~Resources() {
		write("resources.sav");
		delete[] properties;
		delete[] perks;
		delete[] colors;
		delete[] levels;
		delete[] wonLevels;
	}
	Resources(int c) : color(0xFFA0A0A0), coins(c) {
		perks = new int[12];
		colors = new bool[12];
		levels = new bool[12];
		wonLevels = new bool[12];
		properties = new bool[12];
		for (int i = 0; i < 12; i++) {
			colors[i] = i == 1;
			perks[i] = 0;
			levels[i] = i == 0;
			wonLevels[i] = false;
			properties[i] = false;
		}
		winScreen = false;
		srand((unsigned int)time(0));
		charId = rand();
		read("resources.sav");
	}
	//Getters and setters
	long long getColor() { return color; }
	bool getColor(int i) { if (i >= 12)return false; return colors[i]; }
	void setColor(long long l) { color = l; }
	int getCoins() { return coins; }
	int getPerk(int k) { if (k >= 12)return 0; return perks[k]; }

	bool hasSameId(Resources *r) {
		return charId == r->charId;
	}
	bool hasWonLevel(int k) {
		if (k < 0)return true;
		if (k >= 12)return false;
		return wonLevels[k];
	}
	void winLevel(int k) {
		if (k >= 12)return;
		wonLevels[k] = true;
	}

	int getCharacterId() { return charId; }
	void setCharacterId(int i) { charId = i; }

	//Coin related
	bool buy(int w, int c) {
		if (colors[w] || coins<c)return false;
		coins -= c;
		colors[w] = true;
		return true;
	}
	bool purchase(int k) {
		if (coins >= getPriceAt(k)) {
			coins -= getPriceAt(k);
			perks[k]++;
			return true;
		}
		return false;
	}
	void consume(int k) {
		if (k >= 11)return;
		if(getType(k)==2)perks[k]=0;
		else perks[k]--;
	}
	void addCoins(int c) {
		if (c < 0)return;
		coins += c;
	}

	//Level related
	int getUnlock(int k) {
		switch (k) {
		case 1: return 20;
		case 2: return 28;
		case 3: return 40;
		case 4: return 52;
		case 5: return 68;
		case 6: return 80;
		case 7: return 100;
		case 8: return 120;
		case 9: return 200;
		case 10: return 300;
		case 11: return 400;
		default:
			return 0;
		}
	}
	bool hasUnlocked(int k) {
		if (k >= 12)return false;
		return levels[k];
	}
	void unlock(int k) {
		if (k >= 12)return;
		if (!hasWonLevel(k - 1))return;
		if (!hasUnlocked(k) && getUnlock(k) <= coins) {
			coins -= getUnlock(k);
			levels[k] = true;
		}
	}
	string getLevelText(int k) {
		if (k >= 12)return string("");
		ostringstream os;
		if (hasUnlocked(k))os << "Level " << (k+1);
		else os << getUnlock(k) << " coins";
		return os.str();
	}

	int getReward(int k) {
		switch (k) {
		case 0: return 12;
		case 1: return 16;
		case 2: return 28;
		case 3: return 36;
		case 4: return 40;
		case 5: return 48;
		case 6: return 88;
		case 7: return 94;
		case 8: return 120;
		case 9: return 240;
		case 10: return 275;
		case 11: return 600;
			default: return 0;
		}
	}
	
	void undoWin() { winScreen = false; }

	string getWinText(int k) {
		if (k >= 12)return string("");
		ostringstream os;
		int result = (int)(getReward(k) / (hasWonLevel(k)?2:1) * (getPerk(9) == 2 ? 1.5 : 1));
		os << " " << result << " coins!";
		if(!winScreen)coins += result;
		if (getPerk(9) == 2)consume(9);
		winScreen = true;
		return os.str();
	}

	//Skin related
	long long getColorAt(int k) {
		return k == 0 ? 0xFF000000 : k == 1 ? 0xFFA0A0A0 : k == 2 ? 0xFFFFFFFF : k == 3 ? 0xFFFF0000 : k == 4 ? 0xFF00FF00 : k == 5 ? 0xFF0000FF : k == 6 ? 0xFFFF00FF : k == 7 ? 0xFFFFFF00 : k == 8 ? 0xFF00FFFF
			: k == 9 ? 0xFFA000FF : k == 10 ? 0xFFFFA000 : 0xFFA0000FF;
	}
	int getPrice(int k) {
		return 25;
	}

	//Perk related

	//Perks and debuffs:
	//First three perks are level bound, the other three are 'boolean' bound (0=Not bought, 1=Bought but not selected, 2=Bought and selected) (I know that booleans only have 0-1, but this is close, only you have the bought or not bought bit)
	//Then the other three are 'amount' bound, meaning that you can use them any time in the game.
	//The three debuffs are 'boolean' bound, they give you debuffs but you can get a fair reward for playing with them.
	bool perkAt(int i) {
		return i<3 * 3;
	}
	string getAt(int i) {
		ostringstream ss;
		if ((i >= 3 && i<6) || i >= 9)ss << (perks[i] == 0 ? Shortcut::append(getPriceAt(i), " coins") : perks[i] == 1 ? "Not selected" : "Selected");
		else {
			if(getType(i)!=0 || perks[i]<10)ss << getPriceAt(i) << " coins";
			else ss << "Max level";
		}
		return ss.str();
	}
	string getNameAt(int i) {
		//Buffs
		if (i == 0)return Shortcut::combine(Shortcut::append(perks[i], "Speed (Level ", true), ")");
		else if (i == 1)return Shortcut::combine(Shortcut::append(perks[i], "Jump height (Level ", true), ")");
		else if (i == 2)return Shortcut::combine(Shortcut::append(perks[i], "Health (Level ", true), ")");
		else if (i == 3)return string("Magnet");
		else if (i == 4)return string("Helmet");
		else if (i == 5)return string("Life");
		//Unused; I wanted to make it, though I was short on time
		else if (i == 6)return Shortcut::combine(Shortcut::append(perks[i], "Grenade (", true), ")");
		else if (i == 7)return Shortcut::combine(Shortcut::append(perks[i], "Missle (", true), ")");
		else if (i == 8)return Shortcut::combine(Shortcut::append(perks[i], "Drill (", true), ")");
		//Debuffs
		else if (i == 9)return string("Slowness");
		else if (i == 10)return string("Weakness");
		else return string("Heavy weight");
	}
	int getPriceAt(int i) {
		if (i<3)return (int)((3 * pow(perks[i], 1.1 + i*0.1) + (10 * (i + 1))*perks[i] + 25)*(1+i/20.0))/5;
		else if (i == 3)return 30;
		else if (i == 4)return 80;
		else if (i == 5)return 100;
		else if (i == 6)return 60;
		else if (i == 7)return 180;
		else if (i == 8)return 28;
		else if (i == 9)return 10;
		else if (i == 10)return 20;
		else return 40;
	}
	int getType(int k) {
		return k < 3 ? 0 : (k >= 3 && k < 6) || k >= 9 ? 2 : 1;
	}
	void toggle(int k) {
		if (perks[k] == 0) {
			if (purchase(k))perks[k] = 2;
		}
		else {
			if (perks[k] == 1)perks[k] = 2;
			else perks[k] = 1;
		}
	}
	bool getProperty(int k) {
		if (k < 0 || k >= 12)return false;
		return properties[k];
	}
	void setProperty(int k, bool b) {
		if (k < 0 || k >= 12)return;
		properties[k] = b;
	}
	void read(char *file);
	void write(char *file);
};

//Saving and loading the files
void Resources::read(char *file) {
	ifstream f(file);

	int length = 68;

	f.seekg(0, ios::end);
	int end = (int)f.tellg();
	if (end != length) {
		f.close();
		cout << "Incorrect resource file! (" << end << " instead of "  << length << ")" << endl;
		return;
	}
	//Reading everything
	ObjectReader::readLong(&color, f, 0);
	ObjectReader::readLong(&coins, f, 4);
	ObjectReader::readLong(&charId, f, 8);
	for (int i = 0; i < 12; i++)ObjectReader::readLong(&perks[i], f, 12 + 4 * i); 
	bool **bools = new bool*[4];
	ObjectReader::readBools(bools, f, 5*12, 4, 12);
	colors = bools[0];
	levels = bools[1];
	wonLevels = bools[2];
	properties = bools[3];
	delete[] bools;
	//
	f.close();
}
void Resources::write(char *file) {
	ofstream f(file, ios::out | ios::binary | ios::trunc);
	//Exporting the color
	ObjectWriter::writeInt(f,(int)color);
	//Exporting the coins
	ObjectWriter::writeInt(f, coins);
	//Exporting the charId
	ObjectWriter::writeInt(f, charId);
	//Exporting the perks
	for (int i = 0; i < 12; i++)ObjectWriter::writeInt(f, perks[i]);
	//Exporting the booleans
	bool **bools = new bool*[4]{ colors, levels, wonLevels, properties };
	ObjectWriter::writeBools(f, bools, 4, 12);
	delete[] bools;
	f.close();
}

#endif