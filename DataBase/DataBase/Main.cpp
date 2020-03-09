#pragma warning(disable : 4996)
#include <conio.h>
#include <iostream>
#include "Tables.h"
#include "metaData.h"
#include "Blocks.h"
#include "bitMap.h"
#include <string>
using namespace std;
int main() {

	metaDataFunctions mdF;
	string command;
	string name = "";
	while (true) {

		if (name != "") {
			cout << "\nSQL " << name << "> ";
			
		}
		else {
			cout << "\nSQL>";
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

			cout << "100% \n\nDatabase " << name << ".ADB" << " created!\n\n";
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
				cout << "\nError\ndatabase not found\n";
				name = "";
			}

		}

		else if (strncmp(command.c_str(), "CREATE TABLE", 12) == 0 && name != "") {
			string tableName = "";
			for (int i = 13; i < command.size(); i++) {
				if (command[i] == ' ')
					break;
				tableName += command[i];
			}
			metaData md = mdF.readMetaData(name);
			TableFunctions tf(md);


			string column, columnsName = "";
			while (true) {
				getline(cin, column);
				columnsName += column;
				if (column[column.size() - 1] == ';') {
					int result = tf.addTable(tableName.c_str(), columnsName);
					if (result == 1)
						cout << "\nTable " << tableName << " created\n";
					else if (result == -1)
						cout << "\nTable already exist \n";
					else if (result == -2)
						cout << "\nJust one primary key \n";
					else
						cout << "\nSyntax error\n";
					break;
				}
			}
		}

		else if (strncmp(command.c_str(), "INSERT INTO", 11) == 0 && name!="") {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			int index=0;
			string tableName = "", values = "",columns ="";
			for (int i = 12; i < command.size(); i++) {
				if (command[i] == '(') {
					index = i+1;
					break;
				}
				tableName += command[i];
			}
			for (int i = index; i < command.size(); i++) {
				if (command[i] == ' ') {
					index = i + 1;
					break;
				}
				columns += command[i];
			}

			if (strncmp(command.substr(index,command.size()).c_str(), "VALUES", 6) == 0){
				
				for (int i = index+7; i < command.size(); i++) {
					if (command[i] == ';')
						break;
					values += command[i];
				}
				metaData md = mdF.readMetaData(name);
				TableFunctions tf(md);
				int result = tf.insertData(tableName.c_str(), columns, values);
				if (result == 1)
					cout << "\nAdd data succes";
				else if (result == -1)
					cout << "\nError\nTable not found\n";
				else if (result == -2)
					cout << "\nError\nColumn not found\n";
			}
			
		}

		else if (strncmp(command.c_str(), "UPDATE", 6)==0) {
		if (command[command.size() - 1] != ';') {
			cout << "\ninvalid process\n";
			continue;
		}
			int index = 0;
			string tableName = "", values = "",whereSentence = "";
			for (int i = 7; i < command.size(); i++) {
				if (command[i] == ' ') {
					index = i + 1;
					break;
				}
				tableName += command[i];
			}
			if (strncmp(command.substr(index, command.size()).c_str(), "SET", 3) == 0) {
				index += 4;
				for (int i = index; i < command.size(); i++) {
					if (command[i] == ' ') {
						index = i + 1;
						break;
					}
					values += command[i];
				}
			}
			if (strncmp(command.substr(index, command.size()).c_str(), "WHERE", 5) == 0) {
				for (int i = index+6; i < command.size(); i++) {
					if (command[i] == ';')
						break;
					whereSentence += command[i];
				}
				metaData md = mdF.readMetaData(name);
				TableFunctions tf(md);
				int result = tf.updateData(tableName.c_str(), values, whereSentence);
				if (result == 1)
					cout << "\nUpdate succes\n";
				else if (result == -1)
					cout << "\nError\nTable not found\n";
				else if(result ==-2)
					cout << "\nError\nColumn not found\n";
				else
					cout << "\nError\nColumn in where not found\n";
			}
			else
				cout << "\nError\nSyntax error\n";
		}
		else if (strncmp(command.c_str(), "DELETE FROM", 11) == 0 && name != "") {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			int index = 0;
			string tableName = "", values="";
			for (int i = 12; i < command.size(); i++) {
				if (command[i] == ' ' || command[i] == ';') {
					index = i + 1;
					break;
				}
				tableName += command[i];
			}
			if (strncmp(command.substr(index, command.size()).c_str(), "WHERE", 5) == 0) {
				for (int i = index + 6; i < command.size(); i++) {
					if (command[i] == ';')
						break;
					values += command[i];
				}
			}
			metaData md = mdF.readMetaData(name);
			TableFunctions tf(md);
			
			tf.dropTable(tableName.c_str(), false, values);
		}

		else if (strncmp(command.c_str(), "SELECT * FROM", 13) == 0 && name != "") {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			int index = 0;
			string tableName = "",value ="";
			for (int i = 14; i < command.size(); i++) {
				if (command[i] == ' ' || command[i] == ';') {
					index = i +1;
					break;
				}
				tableName += command[i];
			}
			metaData md = mdF.readMetaData(name);
			TableFunctions tf(md);
			if (strncmp(command.substr(index, command.size()).c_str(), "WHERE", 5) == 0) {
				for (int i = index + 6; i < command.size(); i++) {
					if (command[i] == ';')
						break;
					value += command[i];
				}
				tf.selectAllTable(tableName.c_str(), value);
				continue;
			}
			
			tf.selectAllTable(tableName.c_str(),"");

		}

		else if (strncmp(command.c_str(), "SELECT", 6) == 0 && name != "") {
			int index = 0;
			string columns = "",tableName = "",value ="";
			for (int i = 7; i < command.size(); i++) {
				if (command[i] == ' ') {
					index = i + 1;
					break;
				}
				columns += command[i];
			}

			if (strncmp(command.substr(index, command.size()).c_str(), "FROM", 4) == 0) {
				
				for (int i = index + 5; i < command.size(); i++) {
					if (command[i] == ' ' || command[i] == ';') {
						index = i + 1;
						break;
					}
					tableName += command[i];
				}
				TableFunctions t(mdF.readMetaData(name));

				if (strncmp(command.substr(index, command.size()).c_str(), "WHERE", 5) == 0) {
					for (int i = index + 6; i < command.size(); i++) {
						if (command[i] == ';')
							break;
						value += command[i];
					}
					t.selectColumns(columns, tableName.c_str(), value);
					continue;
				}
				t.selectColumns(columns, tableName.c_str(),"");
			}
			else {
				cout << "\nSyntax error\n";
			}
			
			
			
		}

		else if (strncmp(command.c_str(), "DROP TABLE", 10) == 0 && name != "") {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			string tableName = "";
			for (int i = 11; i < command.size(); i++) {
				if (command[i] == ' ' || command[i] == ';')
					break;
				tableName += command[i];
			}
			TableFunctions tf(mdF.readMetaData(name));
			tf.dropTable(tableName.c_str(),true,"");
		}

		else if (strncmp(command.c_str(), "DROP DATABASE", 13) == 0) {
			if (command[command.size() - 1] != ';') {
				cout << "\ninvalid process\n";
				continue;
			}
			for (int i = 14; i < command.size(); i++) {
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

		else
		cout << "\n Invalid process\n";

	}
}
