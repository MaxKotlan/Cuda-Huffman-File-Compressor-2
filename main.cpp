#include <iostream>
#include "FileCompressor.h"
#include "HuffmanTree.h"
#include "Startup.h"

int main(int argc, char** argv) {
	Startup::Init(argc, argv);

	if (Startup::Instance().fileOperation() == Compressing){
		FileCompressor fr(Startup::Instance().fileName());
		FrequencyMap map = fr.GetCharacterFrequencies();
		HuffmanTree tree = HuffmanTree(map);
		tree.printFrequencies();
		fr.saveCompressed(tree);
	}
	if (Startup::Instance().fileOperation() == Decompressing){
		FileCompressor fr(Startup::Instance().fileName());
		FrequencyMap map = fr.GetCharacterFrequencies();
		HuffmanTree tree = HuffmanTree(map);
		fr.saveCompressed(tree);
	}
}