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
		original = fopen(fileName.c_str(), "rb");
		compressed = fopen((fileName.substr(0, fileName.find('.')) + ".huff" ).c_str() , "w");
	}

	~FileReader() {
		fclose(original);
		fclose(compressed);
	}

	FrequencyMap GetCharacterFrequencies() {
		FrequencyMap hashmap(256);

		fseek(original, 0, SEEK_END);
		int length = ftell(original);
		fseek(original, 0, SEEK_SET);

		unsigned char* buffer = new unsigned char[length];
		fread(buffer, length, 1, original);

		for (int i = 0; i < length; i++) {
			unsigned char letter = buffer[i];
			hashmap[letter].character = letter;
			hashmap[letter].frequency++;
		}	
		
		delete[] buffer;

		return std::move(hashmap);
	}

	void saveCompressed(HuffmanTree htree) {

		fseek(original, 0, SEEK_END);
		int length = ftell(original);
		fseek(original, 0, SEEK_SET);
		fseek(compressed, 0, SEEK_SET);


		unsigned char* buffer = new unsigned char[length];
		fread(buffer, length, 1, original);

		unsigned int shiftcount = 0;
		unsigned char shiftregister = 0;

		std::vector<HuffmanCode> compressionMap = htree.convertToHashmap();
		std::vector<unsigned char> fileHeader = htree.generateFileHeader();

		if (!compressed){
			std::cout << "Compressed file not opened!" << std::endl;
		}

		/*write file header*/
		fwrite(fileHeader.data(), sizeof(unsigned char), fileHeader.size(), compressed);
		for (int i = 0; i < length; i++) {
			unsigned char letter = buffer[i];
			HuffmanCode code = compressionMap[letter];

			bool canFitInRegister = (shiftcount+code.shift) < 8;
			if (canFitInRegister){				
				shiftregister = (shiftregister << code.shift) | code.path;
				shiftcount += code.shift;
				buffer[i] = static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0];
				//std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0] << " ";
				//std::cout << std::bitset<8>(shiftregister) << std::endl;
			} else {
				do {
					unsigned int spaceToFill = 8 - shiftcount;
					unsigned char bitstofit = (unsigned char)(code.path >> (unsigned int)(sizeof(code.path) - spaceToFill));
					code.path = code.path << spaceToFill;
					//unsigned int codepart = (code.path >> (code.shift - spaceToFill)) << (code.shift - spaceToFill);
					//code.shift = (code.shift << (8 - code.shift)) >> (code.shift - spaceToFill);
					//shiftregister +=  ((code.path << (code.shift - (8 - shiftcount)) >> (code.shift - (8 - shiftcount))));
					shiftregister = (shiftregister << spaceToFill) | bitstofit;
					buffer[i] = static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0];
					//std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char*>(static_cast<void*>(&shiftregister))[0] << " ";
					//std::cout << std::bitset<8>(shiftregister) << std::endl;
					shiftcount = 0;
					shiftregister = 0;
				} while (shiftcount + code.shift >= 8);
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
		}

		fwrite(buffer, sizeof(unsigned char), length, compressed);
		
		delete[] buffer;
		fclose(compressed);
	}

	private:
		FILE* original;
		FILE* compressed;
};

#endif