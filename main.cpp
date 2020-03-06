#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <string> 

/*Datastructure Which Holds a character, and an integer with the number of occurences in a file*/
struct CharacterFrequency {
	char character;
	unsigned int frequency;

	bool operator()(const CharacterFrequency& a, const CharacterFrequency& b) const {
		return a.frequency > b.frequency;
	}
};
/*HashmapForCharacterFrequency*/
typedef std::vector<CharacterFrequency> FrequencyMap;

FrequencyMap GetFileCharacterFrequencies() {
	std::ifstream fin("main.cpp", std::ifstream::binary);
	FrequencyMap hashmap(256);

	std::vector<char> buffer(1024);
	do {
		fin.read(buffer.data(), buffer.size());
		for (int i = 0; i < fin.gcount(); i++) {
			char letter = buffer[i];
			hashmap[letter].character = letter;
			hashmap[letter].frequency++;
		}
	} while (fin);
	return hashmap;
}

struct node {
	node* lchild = nullptr;
	node* rchild = nullptr;
	CharacterFrequency value;
};

/*builds tree and returns root node*/
node* buildHuffmanTree(FrequencyMap& map) {
	/*Turn Hashmap into a Heap*/
	std::make_heap(map.begin(), map.end(), CharacterFrequency());

	/*Remove All Characters with Zero Frequencies from the Heap*/
	while (map.front().frequency == 0) {
		std::pop_heap(map.begin(), map.end(), CharacterFrequency()); map.pop_back();
	}

	/*Build a Huffman Tree*/
	node* root = nullptr;
	while (map.size() > 1) {
		/*Allocate 3 nodes dynamically*/
		node* lchild = new node();
		node* rchild = new node();
		node* parent = new node();

		lchild->value = map.front();
		std::pop_heap(map.begin(), map.end(), CharacterFrequency()); map.pop_back();

		rchild->value = map.front();
		std::pop_heap(map.begin(), map.end(), CharacterFrequency()); map.pop_back();

		parent->value.frequency += lchild->value.frequency + rchild->value.frequency;

		parent->lchild = lchild;
		parent->rchild = rchild;

		root = parent;

		map.push_back(parent->value); std::push_heap(map.begin(), map.end(), CharacterFrequency());
	}
	return root;
}

int main() {
	FrequencyMap map = GetFileCharacterFrequencies();
	node* root = buildHuffmanTree(map);
	//std::cout << (int)(root == nullptr) << std::endl;
	while (root->rchild != nullptr){
	    std::cout << root->value.frequency << ", ";
	    root = root->rchild;
	}

/*
	for (auto it = map.begin(); it != map.end(); it++){
		std::cout << it->second << " ";
	}*/

}