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
	int i = 0,spaces =0;
	while (i < 11) {
		string column = "";
		
		for (int j = 0; j < _columns.size(); j++) {
			
			if (_columns[j] == ' ' && spaces == 0) {
				strcpy(t.columns[i].name, column.c_str());
				t.columns[i].primaryKey = false;
				column = "";
				spaces++;
			}
			else if (_columns[j] == 'P' && spaces==1 && _columns[j+1] == ' '|| _columns[j] == ' ' && spaces == 1) {
					t.columns[i].primaryKey = true;
			}
			else if (_columns[j] == ',' || _columns[j] == ';') {
				strcpy(t.columns[i].dataType, column.c_str());
				column = "";
				t.columns[i].firstDataBlock = -1;
				
				i++;
				spaces = 0;
				if (_columns[j] == ';')
					break;
				continue;
			}
			else {
				column += toupper(_columns[j]);
			}
		}
		break;
	}
	t.columnsNumber = i;
}

void TableFunctions::splitColumns(string _columns,const char* _tableName) {
	int index = searchTable(_tableName),position;
	if (index != -1) {
		char* prueba;
		prueba = new char[_columns.size() + 1];
		strcpy(prueba, _columns.c_str());
		ifstream TableRead(md.name, ios::in | ios::binary);
		if (TableRead) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8)+30);
			TableRead.seekg(position, ios::cur);
			TableRead.read(reinterpret_cast<char*>(&t.columnsNumber), 4);
			char delimeter[] = ",";
			char* splitColumns = strtok(prueba, delimeter);
			int i = 0;
			cout << "\n" << _tableName << "\n\n";
			while (splitColumns != NULL) {
				while (i<t.columnsNumber)
				{
					TableRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
					if (strcmp(t.columns[i].name, splitColumns) == 0) {
						if (t.columns[i].primaryKey)
							cout << "(PK) ";
						cout << "\t" << t.columns[i].name << "\t" << char(186);
						break;
					}
					i++;
				}	
				splitColumns = strtok(NULL, delimeter);
				TableRead.seekg((position + 4), ios::beg);
				i = 0;
			}
			cout << "\n\n";
			TableRead.close();
			return;
		}
	}
	else {
		cout << "\nError 404 \nTable not found\n";
	}

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

		while (index < md.NumberBlocks) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&t.name), 30);
			if (strcmp(t.name, _tableName) == 0) {
				TableRead.read(reinterpret_cast<char*>(&t.columnsNumber), sizeof(int));
				cout << "\n" << t.name << "\n\n";
				for (int i = 0; i < t.columnsNumber; i++) {
					TableRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
					if (t.columns[i].primaryKey)
						cout << "(PK) ";
					cout << "\t" << t.columns[i].name << "\t" << char(186);
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
		while (index < md.NumberBlocks) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&t.name), 30);
			if (strcmp(t.name, _tableName) == 0) {
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


