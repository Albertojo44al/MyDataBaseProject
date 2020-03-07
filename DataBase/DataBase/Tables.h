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
	bool writeTableOnDatabase(int);
	void selectAllTable(const char*);
	void dropTable(const char*);
	int searchTable(const char*);
	bool writeColumns(string);
	void selectColumns(string,const char*);
	bool readColumns(int);
	int insertData(const char*, string,string);
	int castInt(string);
	double castDouble(string);
	void modifyColumns(int,int);
	bool writeData(int,string,const char*);
	void selectData(int,int,const char*);
	void deleteData(const char*, const char*);
};

