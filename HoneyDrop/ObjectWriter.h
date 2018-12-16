#ifndef OBJECTWRITER_H_
#define OBJECTWRITER_H_

using namespace std;

#include <iostream>
#include <fstream>

class ObjectWriter {
public:
	static void writeInt(ofstream &f, int l) {
		f << (unsigned char)((l & 0xFF000000) >> 24);
		f << (unsigned char)((l & 0xFF0000) >> 16);
		f << (unsigned char)((l & 0xFF00) >> 8);
		f << (unsigned char)((l & 0xFF));
	}
	static void writeBools(ofstream &f, bool **bools, int w, int h) {
		double length = (w*h) / 32.0;
		if ((int)length != length)length = (int)length + 1;
		int *a = new int[(int)length];
		for (int i = 0; i < length; i++)a[i] = 0;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++){
				int k = i*h + j;
				int index = k / 32;
				int where = k % 32;
				if(bools[i][j])a[index] |= 1 << (31-where);
			}
		for (int i = 0; i < length; i++)writeInt(f, a[i]);
	}
};

#endif