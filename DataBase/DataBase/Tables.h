#pragma once
#include "Struct.h"
#include "bitMap.h"
#include "MetaData.h"
#include "Blocks.h"

class TableFunctions {
	Tables t;
	metaData md;
	BlocksData b;
public:
	TableFunctions();
	TableFunctions(metaData);
	int addTable(const char*, string);
	bool writeOnDatabase(int, BlocksData);
	void selectTable(const char*);
	void dropTable(const char*);
	int searchTable(const char*);
	void writeColumns(string);
	void splitColumns(string,const char*);

};

