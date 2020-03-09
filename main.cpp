#include <iostream>
#include "FileReader.h"
#include "HuffmanTree.h"
#include "Startup.h"

int main(int argc, char** argv) {
	Startup::Init(argc, argv);
	std::cout << Startup::Instance().MaximumRam() << std::endl;

	FileReader fr("hello.txt");
	FrequencyMap map = fr.GetCharacterFrequencies();
	HuffmanTree tree = HuffmanTree(map);
	std::vector<HuffmanCode> compressionMap = tree.convertToHashmap();
	fr.printCompressed(compressionMap);
	tree.generateFileHeader();
	tree.printFrequencies();
}