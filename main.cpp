#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <functional>
#include <bitset>

#include "HuffmanTree.h"


/*Converts from huffman code to binary*/
void print(const HuffmanCode& huff) {
	std::cout << std::bitset<8 * sizeof(unsigned int)>(huff.path).to_string().substr(8 * sizeof(unsigned int) - huff.digits);
}

FrequencyMap GetFileCharacterFrequencies() {
	FILE* file;
	file = fopen("test.exe", "rb");
	FrequencyMap hashmap(256);

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = new unsigned char[length];
	fread(buffer, length, 1, file);
	fclose(file);

	for (int i = 0; i < length; i++) {
		unsigned char letter = buffer[i];
		hashmap[letter].character = letter;
		hashmap[letter].frequency++;
	}

	delete[] buffer;

	return hashmap;
}

void printCompressed(std::vector<HuffmanCode> hash) {
	FILE* file;
	file = fopen("text.exe", "rb");

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = new unsigned char[length];
	fread(buffer, length, 1, file);
	fclose(file);

	for (int i = 0; i < length; i++) {
		unsigned char letter = buffer[i];
		print(hash[letter]);
	}

	delete[] buffer;
}

int main() {
	FrequencyMap map = GetFileCharacterFrequencies();
	HuffmanTree tree = HuffmanTree(map);
	std::vector<HuffmanCode> encryptionMap = tree.convertToHashmap();
	printCompressed(encryptionMap);
}