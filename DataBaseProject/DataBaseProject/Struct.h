#pragma once
#pragma warning(disable : 4996)
#include <fstream>
#include <iostream>
#include <cmath>
#define Size 300
using namespace std;



struct metaData {
	char name[30];
	long unsigned int size;
	char date[8];
	unsigned int bitmapSize;
	unsigned int NumberBlocks;
	unsigned int blockSize;
	int firstTab;
};


struct columns {
	char name[30];
	char dataType[6];
	unsigned int firstBlock;
};

struct Tables {
	char name[30];
	
};


struct bitMap {
	char * bitmap;
};


struct BlocksData {
	char* blocks;
	int next;
	int actualPosition;
};

struct BlocksTable {
	char block[1024];
	int next;
	int endBlock;
};

struct BlocksColumn{
	char block[1024];
	int next;

};




