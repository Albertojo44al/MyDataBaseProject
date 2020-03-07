#pragma once
#include "Struct.h"



class dataBlocks {
	int quantity = 0;
	metaData md;
	BlocksData block;


public:
	dataBlocks(metaData);
	void save(string);
	BlocksData readBlock(int);
	void cleanBlock(int);
};

