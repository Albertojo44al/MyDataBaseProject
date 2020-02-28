 
#pragma once
#include "Struct.h"

class metaDataFunctions {

public:
	void writeMetaData(const char*, int, const char*, int);
	metaData readMetaData(string);
	string Date();
	bool fileExists(string);
	bool modifyMetaData(metaData);
};
