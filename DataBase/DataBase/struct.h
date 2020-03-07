#pragma once
#pragma warning(disable : 4996)
#include <fstream>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
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
	char name[29];
	char dataType[8];
	int firstDataBlock;
	int countData;
	bool primaryKey;
};

struct Tables {
	char name[30];
	int columnsNumber;
	columns columns[10];
};


struct bitMap {
	char* bitmap;
};


struct BlocksData {
	char* blocks;
	int next;
	int actualPosition;
};






