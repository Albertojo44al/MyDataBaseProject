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
	if (!writeColumns(_columns))
		return -2;

	strcpy(t.name, _name);

	bitMap.read(md.name);
	int index = bitMap.freePosition();
	b = blocksFunctions.readBlock(index);

	b.next = -1;
	writeTableOnDatabase(index);

	bitMap.setOn(index);
	bitMap.modify(md.name);
	return 1;
}

bool TableFunctions::writeColumns(string _columns) {
	int i = 0,spaces =0,primaryKey=0;
	while (i < 10) {
		string column = "";
		
		for (int j = 0; j < _columns.size(); j++) {
			
			if (_columns[j] == ' ' && spaces == 0) {
				strcpy(t.columns[i].name, column.c_str());
				t.columns[i].primaryKey = false;
				t.columns[i].countData = 0;
				column = "";
				spaces++;
			}
			else if (_columns[j] == 'P' && spaces==1 && _columns[j+1] == ' '|| _columns[j] == ' ' && spaces == 1) {
					t.columns[i].primaryKey = true;
					primaryKey++;
					if (primaryKey == 3)
						return false;
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
	return true;
}

void TableFunctions::selectColumns(string _columns,const char* _tableName,string _whereSentence) {
	int index = searchTable(_tableName),position, charLength = 0, space = 0;
	int dataPosition = where(index, _whereSentence);
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
						cout << t.columns[i].name << "\n";
						for (int i = 0; i < 15; i++) {
							cout << char(205);
						}
						cout << "\n\t";
						dataBlocks blocksFunctions(md);
						b = blocksFunctions.readBlock(t.columns[i].firstDataBlock);
						for (int j = 0; j < t.columns[i].countData; j++) {
							charLength = atoi(t.columns[i].dataType);
							if (charLength > md.blockSize)
								charLength = md.blockSize;

							if (space >= md.blockSize) {

								while (b.next != -1) {
									selectData(b.next, 0, t.columns[i].dataType);
									b = blocksFunctions.readBlock(b.next);
									if ((charLength + charLength) < md.blockSize)
										space = 0;
									break;
								}
							}
							else {
								if (dataPosition == -1) {
									selectData(t.columns[i].firstDataBlock, j, t.columns[i].dataType);
								}
								else {
									selectData(t.columns[i].firstDataBlock, dataPosition, t.columns[i].dataType);
									break;
								}
								if (strcmp(t.columns[i].dataType, "INT") == 0)
									space += sizeof(int);
								else if (strcmp(t.columns[i].dataType, "DOUBLE") == 0)
									space += sizeof(double);
								else
									space += charLength;
							}
						}
						cout << "\n\n";
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

bool TableFunctions::writeTableOnDatabase(int _index) {
	FILE* file;
	file = fopen(md.name, "r+b");
	if (file == 0) {
		return false;
	}
	else {
		int Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));

		fseek(file, Blockposition, SEEK_CUR);
		fwrite(&t.name, 30, 1, file);
		fwrite(&t.columnsNumber, sizeof(int), 1, file);
		for (int i = 0; i < t.columnsNumber; i++) {
			fwrite(&t.columns[i], sizeof(columns), 1, file);
		}
		Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + md.blockSize;
		fseek(file, Blockposition , SEEK_SET);
		fwrite(&b.next, sizeof(int), 1, file);
		fwrite(&b.actualPosition, sizeof(int), 1, file);
		fclose(file);
		return true;
	}
}

void TableFunctions::selectAllTable(const char* _tableName,string _whereSentence) {
	int index = searchTable(_tableName), position, charLength=0, space =0;
	int dataPosition = where(index, _whereSentence);
	if (index != -1) {
		ifstream TableRead(md.name, ios::in | ios::binary);
		if (TableRead) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&t.name), 30);
			TableRead.read(reinterpret_cast<char*>(&t.columnsNumber), sizeof(int));
			cout << "\n" << t.name << "\n\n";
			for (int i = 0; i < t.columnsNumber; i++) {
				TableRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
				if (t.columns[i].primaryKey)
					cout << "(PK) ";
				cout << t.columns[i].name << "\n\n";
				for (int i = 0; i < 15; i++) {
					cout << char(205);
				}
				cout << "\n\t";
				dataBlocks blocksFunctions(md);
				b = blocksFunctions.readBlock(t.columns[i].firstDataBlock);
				for (int j = 0; j < t.columns[i].countData; j++) {
					charLength = atoi(t.columns[i].dataType);
					if (charLength > md.blockSize)
						charLength = md.blockSize; 

					if (space >= md.blockSize) {
						
						while (b.next != -1) {
							selectData(b.next, 0, t.columns[i].dataType);
							b = blocksFunctions.readBlock(b.next);
							if((charLength + charLength) < md.blockSize)
								space = 0;
							break;
						}
					}
					else {
						if (dataPosition == -1) {
							selectData(t.columns[i].firstDataBlock, j, t.columns[i].dataType);
						}
						else {
							selectData(t.columns[i].firstDataBlock, dataPosition, t.columns[i].dataType);
							break;
						}
							
						if(strcmp(t.columns[i].dataType,"INT")==0)
							space += sizeof(int);
						else if (strcmp(t.columns[i].dataType, "DOUBLE") == 0)
							space += sizeof(double);
						else
							space += charLength;
					}
				}
				cout << "\n\n";
			}
			cout << "\n\n";
			TableRead.close();
			return;
		}
		cout << "\nError 404 \nTable not found\n";
	}
}

void TableFunctions::selectData(int _index,int _number,const char* _dataType) {
	ifstream DataRead(md.name, ios::in | ios::binary);
	if (DataRead) {
		int position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));
		DataRead.seekg(position, ios::beg);
		if (strcmp(_dataType, "INT") == 0) {
			position += (_number * sizeof(int));
			DataRead.seekg(position, ios::beg);
			int data;
			DataRead.read(reinterpret_cast<char*>(&data), sizeof(int));
			if(data!=-1)
			cout << data <<char(186);
		}
		else if(strcmp(_dataType, "DOUBLE")==0){
			position += (_number * sizeof(double));
			DataRead.seekg(position, ios::beg);
			double data;
			DataRead.read(reinterpret_cast<char*>(&data), sizeof(double));
			if (data != -1)
			cout << data << char(186);
		}
		else {
			int length = atoi(_dataType);
			if (length > md.blockSize)
				length = md.blockSize;
			position += (_number * length);
			DataRead.seekg(position, ios::beg);
			char* data = new char[length];
			DataRead.read(reinterpret_cast<char*>(data), length);
			if (data[0] != -1)
			cout << data << char(186);
		}
		DataRead.close();
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

void TableFunctions::dropTable(const char* _tableName,bool _table, string _whereSentence) {
	
	dataBlocks db(md);
	bitMapFunctions map(md);
	int position, index;
	b.blocks = new char[md.blockSize];
	index = searchTable(_tableName);
	int IndexData = where(index,_whereSentence);
	if (index == -1)
		cout << "\nError \nTable not found\n";
	else {
		ifstream TableRead(md.name, ios::in | ios::binary);
		if (TableRead) {
			
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position+30, ios::cur);
			TableRead.read(reinterpret_cast<char*>(&t.columnsNumber), sizeof(int));
			for (int i = 0; i < t.columnsNumber; i++) {
				TableRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
				if (IndexData != -1) {
					deleteDataWhere(t.columns[i].firstDataBlock, IndexData, t.columns[i].dataType);
				}
				else if (IndexData == -1 && _whereSentence != "") {
					cout << "\nColumn doesn't exist\n";
					return;
				}
				else {
					if (t.columns[i].firstDataBlock != -1) {

						deleteData(t.columns[i].firstDataBlock);
					}
					if (!_table) {
						t.columns[i].firstDataBlock = -1;
						t.columns[i].countData = 0;
						modifyColumns(index, i);
					}
				}
			}	
			TableRead.close();
			if (_table) {
				deleteData(index);
				cout << "\nTable " << _tableName << " droped\n";
			}
			else
				cout << "\nDelete complete\n";
		}
	}
}

bool TableFunctions::readColumns(int _index) {
	int position;
	
	if (_index != -1) {
		ifstream TableRead(md.name, ios::in | ios::binary);
		position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));
		TableRead.seekg(position, ios::beg);
		TableRead.read(reinterpret_cast<char*>(&t.name), 30);
		TableRead.read(reinterpret_cast<char*>(&t.columnsNumber), 4);
		for (int i = 0; i < t.columnsNumber; i++) {
			TableRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
		}
		return true;
	}
	return false;
}

int TableFunctions::insertData(const char* _tablename, string _columns, string _values) {
	int index = searchTable(_tablename);
	dataBlocks blocksFunctions(md);
	BlocksData block;
	if (readColumns(index)) {
		bitMapFunctions map(md);
		int indexValues = 0,columnPosition=0;
		string columName = "",valueName="";
		for (int i = 0; i < _columns.size(); i++) {
			if (_columns[i] == ',' || _columns[i] == ')') {
				for (int j = indexValues; j < _values.size(); j++) {
					if (_values[j] == ',' || _values[j] == ')') {
						if (strcmp(columName.c_str(), t.columns[columnPosition].name) == 0) {
							if (t.columns[columnPosition].firstDataBlock != -1) {
								b = blocksFunctions.readBlock(t.columns[columnPosition].firstDataBlock);
									if (md.blockSize <= (b.actualPosition + castInt(t.columns[columnPosition].dataType)) || md.blockSize < (b.actualPosition+sizeof(double))){
										writeData(t.columns[columnPosition].firstDataBlock, valueName, t.columns[columnPosition].dataType,true);
									}
									else {
										writeData(t.columns[columnPosition].firstDataBlock, valueName, t.columns[columnPosition].dataType,false);
									}
							}
							else {
								
								map.read(md.name);
								int freePosition = map.freePosition();
								b = blocksFunctions.readBlock(freePosition);
								writeData(freePosition, valueName, t.columns[columnPosition].dataType,false);
								t.columns[columnPosition].firstDataBlock = freePosition;
								
								map.setOn(freePosition);
								map.modify(md.name);
							}
							t.columns[columnPosition].countData++;
							modifyColumns(index, columnPosition);
							columnPosition++;
							valueName = "";
							columName = "";
							indexValues = j + 1;
							break;
						}
						else
							return -2;//mal escroto column name
					}else
					valueName += _values[j];
				}
			}else
			columName += _columns[i];
		}
	}
	else {
		return -1; // mal escroto el table name
	}
	return 1;
}

bool TableFunctions::writeData(int _index, string _value, const char* _dataType,bool _otherBlock) {
	FILE* file;
	file = fopen(md.name, "r+b");
	if (file == 0) {
		return false;
	}
	else {
		int Blockposition, dataInt;
		double dataDouble;
		if (_otherBlock) {
			dataBlocks blocksFunctions(md);
			while (b.next != -1) {
				_index = b.next;
				b = blocksFunctions.readBlock(b.next);
			}
			bitMapFunctions map(md);
			map.read(md.name);
			int freePosition = map.freePosition();
			b.next = freePosition;
			Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + md.blockSize;
			fseek(file, Blockposition, SEEK_SET);
			fwrite(&b.next, sizeof(int), 1, file);
			fwrite(&b.actualPosition, sizeof(int), 1, file);
			map.setOn(freePosition);
			map.modify(md.name);
			_index = freePosition;
			b = blocksFunctions.readBlock(_index);
		}
		
		Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));
		fseek(file, Blockposition+b.actualPosition, SEEK_SET);

		if (strcmp(_dataType, "INT") == 0) {
			dataInt = castInt(_value);
			fwrite(&dataInt, sizeof(int), 1, file);
			b.actualPosition += sizeof(int);
		}
		else if (strcmp(_dataType, "DOUBLE") == 0) {
			dataDouble = castDouble(_value);
			fwrite(&dataDouble, sizeof(double), 1, file);
			b.actualPosition += sizeof(double);
		}
		else {
			int sizeOfChar = castInt(_dataType);
			if (sizeOfChar > md.blockSize)
				sizeOfChar = md.blockSize;
			char* value = new char[sizeOfChar];
			strcpy(value, _value.c_str());
			fwrite(value, sizeOfChar, 1, file);
			b.actualPosition += sizeOfChar;
		}
		Blockposition = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + md.blockSize;
		fseek(file, Blockposition, SEEK_SET);
		fwrite(&b.next, sizeof(int), 1, file);
		fwrite(&b.actualPosition, sizeof(int), 1, file);
		fclose(file);
		return true;
	}

}

void TableFunctions::modifyColumns(int _indexBlock,int _indexColumn) {
	FILE* file;
	file = fopen(md.name, "r+b");
	if (file == 0) {
		cout << "\nError\n";
		return;
	}
	else {
		int Blockposition = sizeof(metaData) + md.bitmapSize + (_indexBlock * (md.blockSize + 8))+34 +(_indexColumn*sizeof(columns));
		fseek(file, Blockposition, SEEK_CUR);
		fwrite(&t.columns[_indexColumn], sizeof(columns), 1, file);
		fclose(file);
	}
}

int TableFunctions::castInt(string _value) {
	return atoi(_value.c_str());
}

double TableFunctions::castDouble(string _value) {
	return strtod(_value.c_str(),NULL);
}

void TableFunctions::deleteData(int _index) {
	dataBlocks db(md);
	bitMapFunctions map(md);
	int position;
	b.blocks = new char[md.blockSize];
		ifstream TableRead(md.name, ios::in | ios::binary);
		if (TableRead) {
			while (_index != -1) {
				position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));
				TableRead.seekg(position, ios::beg);
				TableRead.read(reinterpret_cast<char*>(b.blocks), md.blockSize);
				TableRead.read(reinterpret_cast<char*>(&b.next), 4);
				map.read(md.name);
				map.setOff(_index);
				db.cleanBlock(position);
				_index = b.next;
			}
			TableRead.close();
			map.modify(md.name);
		}
}

void TableFunctions::deleteDataWhere(int _index,int _indexData,const char* _dataType) {
	dataBlocks db(md);
	int size = 0;
	int position;
	b = db.readBlock(_index);
	FILE* file;
	file = fopen(md.name, "r+b");
	if (strcmp(_dataType, "INT") == 0) {
		size = sizeof(int);
		int data =-1;
		position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + (_indexData * size);
		fseek(file, position, SEEK_CUR);
		fwrite(&data, size, 1, file);
	}
	else if (strcmp(_dataType, "DOUBLE") == 0) {
		size = sizeof(double);
		double data = -1;
		position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + (_indexData * size);
		fseek(file, position, SEEK_CUR);
		fwrite(&data, size, 1, file);
	}
	else{
		size = castInt(_dataType);
		char * data = new char[size];
		data[0] = -1;
		position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8)) + (_indexData * size);
		fseek(file, position, SEEK_CUR);
		fwrite(data, size, 1, file);
		
	}
	
	
	
	fclose(file);
}

void TableFunctions::modifyData(int _positionBlock, int _positionData,const char* _dataType, string _value) {
	FILE* file;
	file = fopen(md.name, "r+b");
	int position;
	if (strcmp(_dataType, "INT") == 0) {
		position = _positionBlock + (_positionData * sizeof(int));
		fseek(file, position, SEEK_CUR);
		int data = castInt(_value);
		fwrite(&data, sizeof(int), 1, file);
	}
	else if (strcmp(_dataType, "DOUBLE") == 0) {
		position = _positionBlock + (_positionData * sizeof(double));
		fseek(file, position, SEEK_CUR);
		double data = castDouble(_value);
		fwrite(&data, sizeof(double), 1, file);
	}
	else {
		int sizeOfChar = castInt(_dataType);
		if (sizeOfChar > md.blockSize)
			sizeOfChar = md.blockSize;
		char* value = new char[sizeOfChar];
		position = _positionBlock + (_positionData * sizeOfChar);
		fseek(file, position, SEEK_CUR);
		strcpy(value, _value.c_str());
		fwrite(value, sizeOfChar, 1, file);
	}
	fclose(file);
}

int TableFunctions::updateData(const char* _tableName, string _values, string _where) {
	int index = searchTable(_tableName), position,size;
	string column = "", value = "";
	int split = 0;
	int row = where(index, _where);
	if (row == -1)
		return -3;
	for (int i = 0; i < _values.size(); i++) {
		if (_values[i] == '=')
			split = 1;
		else if (split == 0)
			column += _values[i];
		else
			value += _values[i];
	}
	
	if (index != -1) {
		ifstream DataRead(md.name, ios::in | ios::binary);
		if (DataRead) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			DataRead.seekg(position, ios::beg);
			DataRead.read(reinterpret_cast<char*>(&t.name), 30);
			DataRead.read(reinterpret_cast<char*>(&t.columnsNumber), sizeof(int));
			for (int i = 0; i < t.columnsNumber; i++) {
				DataRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
				if (strcmp(t.columns[i].name, column.c_str()) == 0){
					index = t.columns[i].firstDataBlock;
					position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
							modifyData(position, row, t.columns[i].dataType, value);
							return 1;
				}
			}
			return -2;//columna mal escrota
		}
	}
	else 
		return -1; //Tabla mal escrota
	
}

int TableFunctions::where(int _tableIndex,string _sentence) {
	dataBlocks db(md);
	string column = "", value = "";
	int split = 0, space= 0;
	for (int i = 0; i < _sentence.size(); i++) {
		if (_sentence[i] == '=')
			split = 1;
		else if (split == 0)
			column += _sentence[i];
		else
			value += _sentence[i];
	}
	if (value == "-1")
		return -1;
	if (_tableIndex != -1) {
		ifstream DataRead(md.name, ios::in | ios::binary);
		int position = sizeof(metaData) + md.bitmapSize + (_tableIndex * (md.blockSize + 8));
		DataRead.seekg(position, ios::beg);
		DataRead.read(reinterpret_cast<char*>(&t.name), 30);
		DataRead.read(reinterpret_cast<char*>(&t.columnsNumber), sizeof(int));
		for (int i = 0; i < t.columnsNumber; i++) {
			DataRead.read(reinterpret_cast<char*>(&t.columns[i]), sizeof(columns));
			if (strcmp(t.columns[i].name, column.c_str()) == 0) {
				_tableIndex = t.columns[i].firstDataBlock;
				b = db.readBlock(_tableIndex);
				position = sizeof(metaData) + md.bitmapSize + (_tableIndex * (md.blockSize + 8));
				DataRead.seekg(position, ios::beg);
				int size;
				if (strcmp(t.columns[i].dataType, "INT") == 0) {
					size = sizeof(int);
					int data;
					for (int j = 0; j < t.columns[i].countData; j++) {
						DataRead.read(reinterpret_cast<char*>(&data), size);
						if (data == castInt(value))
							return j;
					}
				}
				else if (strcmp(t.columns[i].dataType, "DOUBLE") == 0) {
					size = sizeof(double);
					double data;
					for (int j = 0; j < t.columns[i].countData; j++) {

						DataRead.read(reinterpret_cast<char*>(&data), size);
						if (data == castDouble(value))
							return j;
					}
					
				}
				else {
					int sizeOfChar = castInt(t.columns[i].dataType);
					if (sizeOfChar > md.blockSize)
						sizeOfChar = md.blockSize;
					char* data = new char[sizeOfChar];
					for (int j = 0; j < t.columns[i].countData; j++) {
						DataRead.read(reinterpret_cast<char*>(data), sizeOfChar);
						if (strcmp(data, value.c_str()) == 0)
							return j;
					}
				}
			}
		}

	}
	return -1;
}