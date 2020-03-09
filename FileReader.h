#ifndef FILEREADER_H
#define FILEREADER_H
#include <iostream>
#include <vector>
#include <iomanip>
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

		unsigned int shiftcount = 0;
		unsigned char shiftregister = 0;

		for (int i = 0; i < length; i++) {
			unsigned char letter = buffer[i];
			HuffmanCode code = hash[letter];

			if (shiftcount+code.shift < 8){				
				shiftregister = shiftregister << code.shift;
				shiftregister += code.path;
				shiftcount += code.shift;
				std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0] << " ";
				std::cout << std::bitset<8>(shiftregister) << std::endl;
			} else {
				while (shiftcount+code.shift >= 8){
					unsigned int spaceNeeded = 8 - shiftcount;
					unsigned int codepart = code.path >> (8 - code.shift);
					code.shift = (code.shift << (8 - code.shift)) >> (8 - code.shift);
					//shiftregister +=  ((code.path << (code.shift - (8 - shiftcount)) >> (code.shift - (8 - shiftcount))));
					shiftregister += codepart;
					std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0] << " ";
					std::cout << std::bitset<8>(shiftregister) << std::endl;
					shiftcount = 0;
					shiftregister = 0;
				}
			}
			//shiftregister = shiftregister ;
			//shiftregister +=  code.path;
			//std::cout << std::bitset<32>(shiftregister) << std::endl;


			//int bitsToShift = (hash[letter].shift+shiftCount)%
			//shiftcount += code.shift;
			//shiftregister = (shiftregister << code.shift) + hash[letter].path;
			//while (shiftcount >= 8){
			//	std::cout << std::bitset<8>(shiftregister) << std::endl; 
			//	std::cout << std::hex << (int)static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[3] << " ";
			//	shiftregister = shiftregister << 8;
			//	shiftcount -=8;
			//}
			//std::cout << std::hex << shiftedCodes << " ";
			//print(hash[letter]); std::cout << " ";
		} std::cout << std::endl << std::endl;

		delete[] buffer;
	}

	private:
		FILE* file;
};

#endif