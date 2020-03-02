#pragma once
#pragma warning(disable : 4996)
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;



struct metaData {
	char name[30];
	long unsigned int size;
	char date[8];
	unsigned int bitmapSize;
	unsigned int NumberBlocks;
	unsigned int blockSize;
};


struct columns {
	char name[30];
	char dataType[6];
	int firstDataBlock;
	bool primaryKey;
};

struct Tables {
	char name[30];
	int columnsNumber;
	columns columns[11];
};


struct bitMap {
	char* bitmap;
};


struct BlocksData {
	char* blocks;
	int next;
	int actualPosition;
};






