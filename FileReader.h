#ifndef FILEREADER_H
#define FILEREADER_H
#include <iostream>
#include <vector>
#include "HuffmanTree.h"

typedef std::vector<struct Node> FrequencyMap;

class FileReader {
	public:

	FileReader(std::string fileName){
		file = fopen(fileName.c_str(), "rb");
	}

	~FileReader() {
		fclose(file);
	}

	FrequencyMap GetCharacterFrequencies() {
		FrequencyMap hashmap(256);

		fseek(file, 0, SEEK_END);
		int length = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char* buffer = new unsigned char[length];
		fread(buffer, length, 1, file);

		for (int i = 0; i < length; i++) {
			unsigned char letter = buffer[i];
			hashmap[letter].character = letter;
			hashmap[letter].frequency++;
		}	
		
		delete[] buffer;

		return std::move(hashmap);
	}

	void printCompressed(std::vector<HuffmanCode> hash) {

		fseek(file, 0, SEEK_END);
		int length = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned char* buffer = new unsigned char[length];
		fread(buffer, length, 1, file);

		int previous = 0;
		for (int i = 0; i < length; i++) {
			unsigned char letter = buffer[i];
			//print(hash[letter]);
		}

		delete[] buffer;
	}

	private:
		FILE* file;
};

#endif