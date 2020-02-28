#include "Tables.h"



TableFunctions::TableFunctions(metaData _md) {
	md = _md;
}

TableFunctions::TableFunctions() {

}

int TableFunctions::addTable(const char* _name) {
	metaDataFunctions metadataFunctions;
	bitMapFunctions bitMap(md);
	dataBlocks blocksFunctions(md);



	bitMap.read(md.name);
	int i = bitMap.freePosition();

	b = blocksFunctions.readBlock(i);
 
	/////////////////////////////////
	strcpy(t.name, _name);
	b.actualPosition += (sizeof(Tables) + 1);
	
	int n = sizeof(columns);

	if (md.firstTab == -1) {
		md.firstTab = i;
		metadataFunctions.modifyMetaData(md);
	}
	
	b.next = bitMap.nextPosition();
	writeOnDatabase(i,b);

	bitMap.setOn(i);
	bitMap.modify(md.name);
	return 1;
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
		fwrite(&t, sizeof(Tables), 1, file);
		fseek(file, sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8) + md.blockSize), SEEK_CUR);
		fwrite(&_block.next,sizeof(int), 1, file);
		fwrite(&_block.actualPosition, sizeof(int), 1, file);
		fclose(file);
		return true;
	}
}

void TableFunctions::selectTable(const char* _tableName){
	ifstream TableRead(md.name, ios::in | ios::binary);
	if (TableRead) {
		int index = 0,position;
		Tables table;
		
		while (index < md.NumberBlocks) {
			position = sizeof(metaData) + md.bitmapSize + (index * (md.blockSize + 8));
			TableRead.seekg(position, ios::beg);
			TableRead.read(reinterpret_cast<char*>(&table), sizeof(Tables));
			if (strcmp(table.name, _tableName) == 0) {
				cout <<"\n" <<table.name << "\n";
				TableRead.close();
				return;  
			}
			index++;
		}
		cout << "\nError 404 \nTable not found\n";
	}
}


