#include "Tables.h"

TableFunctions::TableFunctions(metaData _md) {
	md = _md;
}

TableFunctions::TableFunctions() {

}

int TableFunctions::addTable(const char* _name, string _columns) {
	metaDataFunctions metadataFunctions;
	bitMapFunctions bitMap(md);
	dataBlocks blocksFunctions(md);

	if (searchTable(_name) != -1)
		return-1;


	/////////////////////////////////
	strcpy(t.name, _name);
	writeColumns(_columns);

	bitMap.read(md.name);
	int index = bitMap.freePosition();
	b = blocksFunctions.readBlock(index);

	b.next = bitMap.nextPosition();
	writeOnDatabase(index, b);

	bitMap.setOn(index);
	bitMap.modify(md.name);
	return 1;
}

void TableFunctions::writeColumns(string _columns) {
	int i = 0;
	while (i < 11) {
		string column = "";

		for (int j = 0; j < _columns.size(); j++) {
			if (_columns[j] == ' ') {
				strcpy(t.columns[i].name, column.c_str());
				column = "";
			}
			else if (_columns[j] == ',' || _columns[j] == ';') {
				strcpy(t.columns[i].dataType, column.c_str());
				column = "";
				t.columns[i].firstDataBlock = -1;
				i++;
				if (_columns[j] == ';')
					break;
				continue;
			}
			else {
				column += _columns[j];
			}
		}
		break;
	}
	t.columnsNumber = i;
}

bool TableFunctions::writeOnDatabase(int _index, BlocksData _block) {
	FILE* file;
	file = fopen(md.name, "r+b");
	if (file == 0) {
		return false;
	}
	else {
		int positionInBlock = _index * md.blockSize;
		int Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));

		fseek(file, Blockposition, SEEK_CUR);
		fwrite(&t.name, 30, 1, file);
		fwrite(&t.columnsNumber, sizeof(int), 1, file);
		for (int i = 0; i < t.columnsNumber; i++) {
			fwrite(&t.columns[i], sizeof(columns), 1, file);
		}
		fseek(file, sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8) + md.blockSize), SEEK_CUR);
		fwrite(&_block.next, sizeof(int), 1, file);
		fwrite(&_block.actualPosition, sizeof(int), 1, file);
		fclose(file);
		return true;
	}
}

void TableFunctions::selectTable(const char* _tableName) {
	ifstream TableRead(md.name, ios::in | ios::binary);
	if (TableRead) {
		int index = 0, position;
		Tables table;

		while (index < md.NumberBlocks) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&table.name), 30);
			if (strcmp(table.name, _tableName) == 0) {
				TableRead.read(reinterpret_cast<char*>(&table.columnsNumber), sizeof(int));
				cout << "\n" << table.name << "\n\n";
				for (int i = 0; i < table.columnsNumber; i++) {
					TableRead.read(reinterpret_cast<char*>(&table.columns[i]), sizeof(columns));
					cout << "\t" << table.columns[i].name << "\t" << char(186);
				}
				cout << "\n\n";
				TableRead.close();
				return;
			}
			index++;
		}
		cout << "\nError 404 \nTable not found\n";
	}
}

int TableFunctions::searchTable(const char* _tableName) {
	ifstream TableRead(md.name, ios::in | ios::binary);
	if (TableRead) {
		int index = 0, position;
		Tables table;
		while (index < md.NumberBlocks) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&table.name), 30);
			if (strcmp(table.name, _tableName) == 0) {
				return index;
			}
			index++;
		}
		return -1;
	}
}

void TableFunctions::dropTable(const char* _tableName) {
	dataBlocks db(md);
	bitMapFunctions map(md);
	int position, index;
	b.blocks = new char[md.blockSize];
	index = searchTable(_tableName);
	if (index == -1)
		cout << "\nError \nTable not found\n";
	else {
		ifstream TableRead(md.name, ios::in | ios::binary);
		if (TableRead) {
			while (index != -1) {
				position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
				TableRead.seekg(position, ios::cur);
				TableRead.read(reinterpret_cast<char*>(b.blocks), md.blockSize);
				TableRead.read(reinterpret_cast<char*>(&b.next), 4);
				map.setOff(index);
				db.cleanBlock(position);
				index = b.next;
			}
			TableRead.close();
			map.modify(md.name);
			cout << "\nTable " << _tableName << " droped\n";
		}
	}
}


