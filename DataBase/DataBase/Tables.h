#pragma once
#include "Struct.h"
#include "bitMap.h"
#include "MetaData.h"
#include "Blocks.h"
#include <windows.h>

class TableFunctions {
	Tables t;
	metaData md;
	BlocksData b;
public:
	TableFunctions();
	TableFunctions(metaData);
	int addTable(const char*, string);
	bool writeTableOnDatabase(int);
	void selectAllTable(const char*,string);
	void dropTable(const char*,bool,string);
	int searchTable(const char*);
	bool writeColumns(string);
	void selectColumns(string,const char*,string);
	bool readColumns(int);
	int insertData(const char*, string,string);
	int castInt(string);
	double castDouble(string);
	void modifyColumns(int,int);
	bool writeData(int,string,const char*,bool);
	void selectData(int,int,const char*);
	void deleteData(int);
	void deleteDataWhere(int,int,const char*);
	int where(int,string);
	int updateData(const char*, string, string);
	void modifyData(int,int,const char*,string);
	void gotoxy(int,int);
};

