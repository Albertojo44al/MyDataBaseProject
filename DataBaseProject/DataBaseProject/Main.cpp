#pragma warning(disable : 4996)
#include <conio.h>
#include <iostream>
#include "Tables.h"
#include "metaData.h"
#include "Blocks.h"
#include "bitMap.h"
#include "string"
using namespace std;
int main() {

	metaDataFunctions mdF;
	string command;
	string name = "";
	while (true) {
		

		if (name != "") {
			cout << "SQL " << name << "> ";
		}
		else {
			cout << "SQL>";
		}
		getline(cin, command);
		for (int i = 0; i < command.size(); i++) {
			command[i] = toupper(command[i]);
		}
		
		if (strncmp(command.c_str(), "CREATE DATABASE", 15) == 0) { // create database 

			//name of file
			for (int i = 16; i < command.size(); i++) {
				if (command[i] == ' ')
					break;
				name += command[i];
			}

			//File Exist
			if (mdF.fileExists(name)) {
				cout << "\nError \nDatabase already exists\n\n";
				name = "";
				continue;
			} 

			//block size
			string tamanioBloques = "";
			int spaces = 0;
			for (unsigned int j = command.size() - 4; j != command.size(); j++) {
				tamanioBloques += command[j];
				if (command[j] == ' ')
					spaces++;
				if (spaces == 2)
					break;
			}
			int blockSize = atoi(tamanioBloques.c_str());

			//block size valid
			if (blockSize < 512 || blockSize > 2048) { 
				cout << "\nError \nBlock size not valid\n\n";
				name = "";
				continue;
			}
			string databaseSize = "";
			cout << "Size: ";
			getline(cin, databaseSize);

			for (int i = 0; i < databaseSize.size(); i++) {
				databaseSize[i] = toupper(databaseSize[i]);
			}

			//storage Units
			string storageUnits = "";
			for (unsigned int j = databaseSize.size() - 2; j != databaseSize.size(); j++) {
				storageUnits += databaseSize[j];
			}

			//Size of file
			string size = "";
			for (unsigned int j = 0; j != databaseSize.size() - 2; j++) {
				size += databaseSize[j];
				if (databaseSize[j] == ' ')
					break;
			}
			int dbSize = atoi(size.c_str());

			//write metadata
			mdF.writeMetaData(name.c_str(), blockSize, storageUnits.c_str(), dbSize);

			//write bitmap
			bitMapFunctions b(mdF.readMetaData(name));
			b.initBitMap();
			b.save(name.c_str());

			//write blocks
			dataBlocks blocks(mdF.readMetaData(name));
			blocks.save(name);

			cout << "100% \n\nDatabase "<< name<<".ADB" << " created!\n\n";
			name = "";
		}

		else if (strncmp(command.c_str(), "CONNECT TO", 10) == 0) { //connect database
			for (int i = 11; i < command.size(); i++) {
				if (command[i] == ' ')
					break;
				name += command[i];
			}
			if (mdF.fileExists(name)) {
				continue;
			}
			else {
				name = "";
			}
		
		}

		else if (strncmp(command.c_str(), "CREATE TABLE", 12) == 0) {
			string tableName = "";
			for (int i = 13; i < command.size(); i++) {
				if (command[i] == ' ')
					break;
				tableName += command[i];
			}
			metaData md = mdF.readMetaData(name);
			TableFunctions tf(md);
			tf.addTable(tableName.c_str());

			string column, columnsName = "";
			while (true) {
				getline(cin, column);
				columnsName += "\n " + column;
				if (column[column.size() - 1] == ';') {
					cout << "simon\n";
					break;
				}
			}
			cout << columnsName;
		}

		else if (strncmp(command.c_str(), "SELECT * FROM", 13) == 0){
			string tableName = "";
			for (int i = 14; i < command.size(); i++) {
				if (command[i] == ' ')
					break;
				tableName += command[i];
		}
			metaData md = mdF.readMetaData(name);
			TableFunctions tf(md);
			tf.selectTable(tableName.c_str());

		}

		else if (strncmp(command.c_str(), "DROP", 4) == 0) {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			for (int i = 5; i < command.size(); i++) {
				if (command[i] == ' ' || command[i] == ';')
					break;
				name += command[i];
			}
			name += ".ADB";
			if (!remove(name.c_str()))
				cout << "\nfile " << name << " removed!\n";
			else
				cout << "\nfile not found!\n";
			name = "";
		}

		else if (strncmp(command.c_str(), "CLOSE", 6) == 0) {
			name = "";
		}

		else if (strncmp(command.c_str(), "EXIT", 5) == 0) {
			exit(1);
		}

	}
}
