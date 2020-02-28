#pragma once
#include "Struct.h"

class bitMapFunctions {
private:
	bitMap map;
	metaData md;
public:
	bitMapFunctions();
	bitMapFunctions(metaData _md);
	void initBitMap();
	int freePosition();
	int nextPosition();
	void setOn(int nBlock);
	void setOff(int nBlock);
	void save(const char* archivo);
	void read(const char* archivo);
	void modify(const char* archivo);

};
