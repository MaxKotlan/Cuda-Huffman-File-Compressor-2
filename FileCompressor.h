#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <assert.h>
#include <vector>
#include <iomanip>
#include <time.h> 
#include "Startup.h"
#include "HuffmanTree.h"
#include "kernels/kernels.h"

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
		clock_t t = clock();
		for (unsigned int j = 0; j < length; j+= bufferSize){
			fread(buffer.data(), buffer.size(), 1, original);
			if (Startup::Instance().UseCuda()){
				CudaGetCharacterFrequencies(hashmap, buffer, j, length);
			} else {
				for (unsigned int i = 0; i < buffer.size() && (j +i) < length; i++) {

					unsigned char letter = buffer[i];
					hashmap[letter].character = letter;
					hashmap[letter].frequency++;

				}	
			}
		}
  		t = clock() - t;
		std::cout << "Operation Took " << ((float)t)/CLOCKS_PER_SEC << " seconds" << std::endl;
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

		/*
		The idea of this algorithm is to encode the file in linear time (with respect to the input file's bytecount)
		You use a hashmap to retrive two integers, (the path encoded as an integer, but treated like a shift register) 
		and a bitshift count to know how many bits are being used.
		Using these two integers, it should be possible to continuously shift them into a globally shared register. 
		When the shared register fills up to 8 bits, it is saved to an output buffer (which is then saved to a file) as a byte and the 
		shared register is reset.
		
		My implementation almost works. Something is off with it, but in some cases it gets realy close.
		*/
		for (unsigned int j = 0; j < length; j+= bufferSize){
			fread(inputBuffer.data(), inputBuffer.size(), 1, original);
			unsigned int i = 0;
			for (i = 0; i < inputBuffer.size() && (j +i) < length; i++) {
				unsigned char letter = inputBuffer[i];
				HuffmanCode code = compressionMap[letter];
				//code.shift;
				if (((code.shift) + shiftcount) <= 8){
					shiftregister = ((shiftregister << code.shift) | code.path);  
					shiftcount += code.shift;
					
					if (shiftcount == 8) {
						AddToWriteBufferAndResetRegister(outputBuffer, shiftregister, bufferSize);
						shiftcount = 0;
						shiftregister = 0;
					}

				} else {
					do {
						unsigned int remainingBitsToFill = sizeof(shiftregister)*8 - shiftcount;
						unsigned int difference = code.shift - remainingBitsToFill;
						unsigned char subpath = (code.path >> difference);
						//unsigned char subpath = (code.path >> 1);
						shiftregister = shiftregister << remainingBitsToFill;
						code.shift = code.shift - difference;
						shiftregister = shiftregister | subpath;
						code.path = ( code.path >> remainingBitsToFill);
						//code.path = ((code.path << (32 - (code.shift - remainingBitsToFill))) >> remainingBitsToFill >> (32- (code.shift - remainingBitsToFill)));

						//assert(code.shift == 8);
						//if (shiftcount == 8) {
							AddToWriteBufferAndResetRegister(outputBuffer, shiftregister, bufferSize);
							shiftcount = 0;
							shiftregister = 0;
						//}
						//shiftcount = code.shift;


					} while (0 >= 8);
				}
				
				/*
				if (shiftcount >= 7){
					outputBuffer.push_back(shiftregister);
					shiftcount = 0;
					shiftregister = 0;
				}
				for (int i = 0; i < outputBuffer.size(); i++){
					std::cout << std::hex << (int)outputBuffer[i] << " ";
				}*/

				/*If at the end of the datablock, write remaining bytes to file*/
				if ((j+i) == length-1){
					fwrite(outputBuffer.data(), outputBuffer.size(), 1, compressed);
					outputBuffer.resize(0);
				}

			}	
		}

		fclose(compressed);
	}

	private:

		void AddToWriteBufferAndResetRegister(std::vector<unsigned char>& outputBuffer, unsigned char shiftRegister,  unsigned int bufferSize){
			//std::cout << std::bitset<8>(shiftRegister) << " [" << std::hex << (int)shiftRegister << "] ";
			outputBuffer.push_back(shiftRegister);
			if (outputBuffer.size() == bufferSize){
				fwrite(outputBuffer.data(), outputBuffer.size(), 1, compressed);
				outputBuffer.resize(0);
			}
		}

		FILE* original;
		FILE* compressed;
};

#endif