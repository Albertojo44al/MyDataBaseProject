#pragma warning(disable : 4996)
#include "metaData.h"
#include <string>
#include <ctime>
#include "stdio.h"

bool metaDataFunctions::fileExists(string _name) {
	string name = string(_name) + ".ADB";
	FILE* f;
	f = fopen(name.c_str(), "r");
	if (f == NULL)
		return false;
	else {
		fclose(f);
		return true;
	}
}

void metaDataFunctions::writeMetaData(const char* _name, int _blockSize, const char* _storageUnits, int _dbSize) {
	string name = string(_name) + ".ADB";
	ofstream metaDataWrite(name, ios::app | ios::binary);
	metaData md;
	if (!metaDataWrite) {
		cout << "error escribir metadata";
	}
	strcpy(md.name, name.c_str());
	long unsigned int fileSize = 0;

	if (strcmp(_storageUnits, "MB") == 0) {

		fileSize = _dbSize * (1024 * 1024);
	}
	if (strcmp(_storageUnits, "GB") == 0) {
		fileSize = _dbSize * ((1024 * 1024) * 1024);
	}

	md.size = fileSize;
	md.NumberBlocks = fileSize / _blockSize;
	md.bitmapSize = md.NumberBlocks / 8;
	md.blockSize = _blockSize - 4;
	strcpy(md.date, Date().c_str());
	metaDataWrite.write(reinterpret_cast<char*>(&md), sizeof(metaData));
	metaDataWrite.close();
}

metaData metaDataFunctions::readMetaData(string _nombre) {
	string name = string(_nombre) + ".ADB";
	ifstream metaDataRead(name, ios::in | ios::binary);

	if (metaDataRead) {
		metaData md;
		metaDataRead.seekg(0, ios::beg);
		metaDataRead.read(reinterpret_cast<char*>(&md), sizeof(metaData));
		metaDataRead.close();
		return md;
	}
}

string metaDataFunctions::Date() {
	time_t now = time(0);
	tm* time = localtime(&now);
	string fecha;
	fecha = to_string(time->tm_mday) + to_string(time->tm_mon + 1) + to_string(1900 + time->tm_year);
	return fecha;
}

bool metaDataFunctions::modifyMetaData(metaData _md) {
	FILE* file;
	file = fopen(_md.name, "r+b");
	if (file == 0) {
		return false;
	}
	metaData md;
	fseek(file, 0, SEEK_CUR);
	fwrite(&_md, sizeof(metaData), 1, file);
	fclose(file);
}