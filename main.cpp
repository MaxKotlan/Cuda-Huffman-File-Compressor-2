#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <functional>

struct Node {
	char character;
	unsigned int frequency;
    Node* lchild = nullptr;
    Node* rchild = nullptr;

	bool operator()(const Node& a, const Node& b) const {
		return a.frequency > b.frequency;
	}
};

struct HuffmanCode{
    char path;
    uint_least8_t shift; 
};

typedef std::vector<Node> FrequencyMap;

FrequencyMap GetFileCharacterFrequencies() {
	std::ifstream fin("lotr.txt", std::ifstream::binary);
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

/*builds tree and returns root node*/
Node* buildHuffmanTree(FrequencyMap& map) {
	/*Turn Hashmap into a Heap*/
	std::make_heap(map.begin(), map.end(), Node());

	/*Remove All Characters with Zero Frequencies from the Heap*/
	while (map.front().frequency == 0) {
		std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();
	}

    Node* root = nullptr;
	/*Build a Huffman Tree*/
	while (map.size() > 1) {
		/*Allocate 3 nodes dynamically*/
		Node* lchild = new Node();
		Node* rchild = new Node();
		Node* parent = new Node();

        root = parent;

		*lchild = map.front();
		std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

		*rchild = map.front();
		std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

		parent->frequency += lchild->frequency + rchild->frequency;

		parent->lchild = lchild;
		parent->rchild = rchild;

		root = parent;

		map.push_back(*parent); std::push_heap(map.begin(), map.end(), Node());
	}
	return root;
}

HuffmanCode* convertTreeToHashmap(Node* root){
    HuffmanCode hashmap[256];
    std::function<void(Node*)> preorder = [&](Node* root) {
        if (root != nullptr) {  
            std::cout << "0"; 
            preorder(root->lchild);
            preorder(root->rchild);
            std::cout << "1";

            if (root->lchild == nullptr && root->rchild == nullptr)
                std::cout << " : " << root->character << std::endl;
        }
    };
    preorder(root);
    return {};
}

int main() {
	FrequencyMap map = GetFileCharacterFrequencies();
	Node* root = buildHuffmanTree(map);
    convertTreeToHashmap(root);
}