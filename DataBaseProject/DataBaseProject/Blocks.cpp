#include "Blocks.h"
#include "Struct.h"

dataBlocks::dataBlocks(metaData _md) {
	md = _md;
	quantity = md.NumberBlocks;
	block.blocks = new char[md.blockSize];
	block.next = -1;
	block.actualPosition = 0;
}

void dataBlocks::save(string _name) {
	
	string name = _name + ".ADB";
	ofstream f(name, ios::app || ios::binary);
	if (!f) {
		return;
	}
	else {
		int percent = 0;
		int charge =0;
		f.seekp(0, ios::end);
		for (int i = 0; i < quantity; i++) {
			
			percent = ((i * 10) /quantity )*10;
			if(percent!=charge){
				cout << percent << "%\t\t";
				charge = percent;
			}
			f.write(reinterpret_cast<const char*>(block.blocks), md.blockSize);
			f.write(reinterpret_cast<const char*>(&block.next), sizeof(block.next));
			f.write(reinterpret_cast<const char*>(&block.actualPosition), sizeof(block.actualPosition));
			
		}
		f.close();
	}
}

BlocksData dataBlocks::readBlock(int _index) {
	ifstream blockRead(md.name, ios::in | ios::binary);

	if (blockRead) {
		int position = sizeof(metaData) + md.bitmapSize + (_index * (md.blockSize + 8));

		blockRead.seekg(position, ios::cur);
		blockRead.read(reinterpret_cast<char*>(block.blocks), md.blockSize);
		blockRead.read(reinterpret_cast<char*>(&block.next),4);
		blockRead.read(reinterpret_cast<char*>(&block.actualPosition), 4);
		blockRead.close();
		return block;
	}
	return block;
}