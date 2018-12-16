#ifndef OBJECTREADER_H_
#define OBJECTREADER_H_

using namespace std;

#include <iostream>
#include <fstream>

class ObjectReader {
public:
	//Three readLongs; because a long and an int is actually the same; and a long long, because I made a mistake and unfortunately casted everything to a long long (I don't have a lot of time now, so I couldn't fix it)
	static void readLong(long long *l, ifstream &f, int index) {
		char *memblock = new char[4];
		f.seekg(index, ios::beg);
		f.read(memblock, 4);
		unsigned char *mem = reinterpret_cast<unsigned char*>(memblock);
		*l = mem[0] << 24 | mem[1] << 16 | mem[2] << 8 | mem[3];
		delete[] mem;
	}
	static void readLong(long *l, ifstream &f, int index) {
		char *memblock = new char[4];
		f.seekg(index, ios::beg);
		f.read(memblock, 4);
		unsigned char *mem = reinterpret_cast<unsigned char*>(memblock);
		*l = mem[0] << 24 | mem[1] << 16 | mem[2] << 8 | mem[3];
		delete[] mem;
	}
	static void readLong(int *l, ifstream &f, int index) {
		char *memblock = new char[4];
		f.seekg(index, ios::beg);
		f.read(memblock, 4);
		unsigned char *mem = reinterpret_cast<unsigned char*>(memblock);
		*l = mem[0] << 24 | mem[1] << 16 | mem[2] << 8 | mem[3];
		delete[] mem;
	}
	//Reading booleans
	static void readBools(bool **bl, ifstream &f, int index, int w, int h) {
		for (int i = 0; i < w; i++) {
			bl[i] = new bool[h];
			for (int j = 0; j < h; j++)bl[i][j] = false;
		}
		double length = (w*h) / 32.0;
		if ((int)length != length)length = (int)length + 1;
		int *a = new int[(int)length];
		for (int i = 0; i < length; i++)readLong(&a[i], f, index + 4 * i);
		for(int i=0;i<w;i++)
			for (int j = 0; j < h; j++) {
				int k = i*h + j;
				int index = k / 32;
				int where = k % 32;
				bl[i][j] = (a[index] & (1<<(31-where))) != 0;
			}
	}
};

#endif