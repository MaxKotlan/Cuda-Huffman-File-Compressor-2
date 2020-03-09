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
		compressed = fopen((fileName.substr(0, fileName.find('.')) + ".kot" ).c_str() , "wb");
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
		for (unsigned int j = 0; j < length; j+= bufferSize){
			fread(buffer.data(), buffer.size(), 1, original);
			for (unsigned int i = 0; i < buffer.size() && (j +i) < length; i++) {

				unsigned char letter = buffer[i];
				hashmap[letter].character = letter;
				hashmap[letter].frequency++;

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

		std::vector<unsigned char>  inputBuffer(bufferSize);
		std::vector<unsigned char> outputBuffer(bufferSize);
		outputBuffer.resize(0);

		for (unsigned int j = 0; j < length; j+= bufferSize){
			fread(inputBuffer.data(), inputBuffer.size(), 1, original);
			unsigned int i = 0;
			for (i = 0; i < inputBuffer.size() && (j +i) < length; i++) {
				unsigned char letter = inputBuffer[i];
				HuffmanCode code = compressionMap[letter];

				if (code.shift + shiftregister < 7){
				shiftregister = shiftregister << code.shift;
				shiftregister = (shiftregister << code.shift) | code.path;  
				shiftcount += code.shift;
				}
				if (shiftcount >= 8){
					shiftcount = 0;
					shiftregister = 0;
				}
				outputBuffer.push_back(shiftregister);
				if (outputBuffer.size() == bufferSize || (j+i) < length){
					fwrite(outputBuffer.data(), outputBuffer.size(), 1, compressed);
					outputBuffer.resize(0);
				}
			}	
		}

		fclose(compressed);
	}

	private:

		void clearAndWriteBuffer(std::vector<unsigned char>& buffer, unsigned int length){
			fwrite(buffer.data(), length, 1, compressed);
		}

		FILE* original;
		FILE* compressed;
};

#endif