#include <iostream>
#include "FileReader.h"
#include "HuffmanTree.h"

int main() {
	FileReader fr("Test.exe");
	FrequencyMap map = fr.GetCharacterFrequencies();
	HuffmanTree tree = HuffmanTree(map);
	std::vector<HuffmanCode> encryptionMap = tree.convertToHashmap();
	fr.printCompressed(encryptionMap);
}