#ifndef FILEREADER_H
#define FILEREADER_H
#include <iostream>
#include <vector>
#include <iomanip>
#include "Startup.h"
#include "HuffmanTree.h"

typedef std::vector<struct Node> FrequencyMap;

class FileCompressor {
	public:

	FileCompressor(std::string fileName){
		original = fopen(fileName.c_str(), "rb");
		compressed = fopen((fileName.substr(0, fileName.find('.')) + ".huff" ).c_str() , "wb");
	}

	~FileCompressor() {
		fclose(original);
		fclose(compressed);
	}

	FrequencyMap GetCharacterFrequencies() {
		FrequencyMap hashmap(256);

		fseek(original, 0, SEEK_END);
		int length = ftell(original);
		fseek(original, 0, SEEK_SET);

		unsigned int bufferSize = Startup::Instance().fileBufferDisabled() ? length : Startup::Instance().fileBufferSize(); 

		std::vector<unsigned char> buffer(bufferSize);
		for (int j = 0; j < length; j+= bufferSize){
			fread(buffer.data(), buffer.size(), 1, original);
			for (int i = 0; i < buffer.size(); i++) {
				if (j +i < length){
					unsigned char letter = buffer[i];
					hashmap[letter].character = letter;
					hashmap[letter].frequency++;
				}
			}	
		}
		
		return std::move(hashmap);
	}

	void saveCompressed(HuffmanTree htree) {

		fseek(original, 0, SEEK_END);
		int length = ftell(original);
		fseek(original, 0, SEEK_SET);
		fseek(compressed, 0, SEEK_SET);

		std::vector<HuffmanCode> compressionMap = htree.convertToHashmap();

		/*Prepare compressed file by writing fileheader with frequency information*/
		std::vector<unsigned char> fileHeader = htree.generateFileHeader();
		fwrite(fileHeader.data(), fileHeader.size(), 1, compressed);

		unsigned int shiftcount = 0;
		unsigned char shiftregister = 0;

		unsigned int bufferSize = Startup::Instance().fileBufferDisabled() ? length : Startup::Instance().fileBufferSize(); 

		std::vector<unsigned char> buffer(bufferSize);
		for (int j = 0; j < length; j+= bufferSize){
			fread(buffer.data(), buffer.size(), 1, original);
			for (int i = 0; i < buffer.size(); i++) {
				if (j +i < length){

					unsigned char letter = buffer[i];
					HuffmanCode code = compressionMap[letter];

					shiftregister = (shiftregister << code.shift) | code.path;  
					shiftcount += code.shift;
					buffer[i] = shiftregister;
					if (shiftcount >= 7){
						shiftcount = 0;
						shiftregister = 0;
					}
				}
				fwrite(buffer.data(), sizeof(unsigned char), length, compressed);
			}	
		}

		fclose(compressed);
	}

	private:
		FILE* original;
		FILE* compressed;
};

#endif