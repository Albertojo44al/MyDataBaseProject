#include "Struct.h"
#include "bitMap.h"


bitMapFunctions::bitMapFunctions(metaData _md) {
	md = _md;
	map.bitmap = new char[md.bitmapSize];
}

bitMapFunctions::bitMapFunctions() {

}

void bitMapFunctions::initBitMap() {
	for (int i = 0; i < md.bitmapSize; i++)
	{
		map.bitmap[i] = 0;
	}
}

int bitMapFunctions::freePosition() {
	const int SHIFT = 8 * sizeof(char) - 1;
	char MASK = 1 << SHIFT;

	for (int i = 0; i < md.bitmapSize / 8; i++) {
		char value;
		value = map.bitmap[i];
		for (int c = 1; c <= SHIFT + 1; c++)
		{
			int bit = (value & MASK) ? 1 : 0;
			if (bit == 0)
				return (i * 8) + (c - 1);
			else {
				MASK = 1 << (SHIFT - c);
			}

		}
		MASK = 1 << SHIFT;
	}
	return -1;
}

int bitMapFunctions::nextPosition() {
	const int SHIFT = 8 * sizeof(char) - 1;
	char MASK = 1 << SHIFT;
	int contador = 0;
	for (int i = 0; i < md.bitmapSize / 8; i++) {
		char value;
		value = map.bitmap[i];
		for (int c = 1; c <= SHIFT + 1; c++)
		{
			int bit = (value & MASK) ? 1 : 0;
			if (bit == 0)
				contador++;
			if (contador == 2)
				return (i * 8) + (c - 1);
			else {
				MASK = 1 << (SHIFT - c);
			}

		}
		MASK = 1 << SHIFT;
	}
	return -1;
}


void bitMapFunctions::setOn(int nBlock) {
	int positionByte = nBlock / 8;
	int iniPosition = (nBlock / 8) * 8;

	for (int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if (i == nBlock)
		{
			map.bitmap[positionByte] |= 1 << x;
			break;
		}
	}
}

void bitMapFunctions::setOff(int nBlock) {
	int positionByte = nBlock / 8;
	int iniPosition = (nBlock / 8) * 8;

	for (int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if (i == nBlock)
		{
			map.bitmap[positionByte] &= ~(1 << x);
			break;
		}
	}
}

void bitMapFunctions::save(const char* _name) {
	string name = string(_name) + ".ADB";
	ofstream f(name, ios::app | ios::out | ios::binary);
	if (!f) {
		cout << "nell bitmap";
	}
	else {
		f.seekp(sizeof(metaData));
		f.write(reinterpret_cast<const char*>(map.bitmap), md.bitmapSize);
		f.close();
	}
}

void bitMapFunctions::modify(const char* _archivo) {
	FILE* archivo;
	archivo = fopen(_archivo, "r+b");
	if (archivo == 0) {
		cout << "nell";
	}
	else {
		fseek(archivo, sizeof(metaData), SEEK_CUR);
		fwrite(map.bitmap, md.bitmapSize, 1, archivo);
		fclose(archivo);
	}
}

void bitMapFunctions::read(const char* _name) {

	ifstream f(_name, ios::binary);

	f.seekg(sizeof(metaData), ios::beg);
	f.read(reinterpret_cast<char*>(map.bitmap), md.bitmapSize);
	f.close();
}
