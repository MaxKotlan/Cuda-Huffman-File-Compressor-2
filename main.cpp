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

void printNode(Node* n){
	if (n != nullptr)
		std::cout << n << "{ char: " << n->character << ", freq: " << n->frequency << ", lchild: " << n->lchild << " rchild: " << n->rchild << " }"; 
}

bool isLeaf(Node* node){
    return node->lchild == nullptr || node->rchild == nullptr;
}

struct HuffmanCode{
    char path;
    uint_least8_t shift; 
};

typedef std::vector<Node> FrequencyMap;

FrequencyMap GetFileCharacterFrequencies() {
	std::ifstream fin("hello.txt", std::ifstream::binary);
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

	/*Build a Huffman Tree*/
	while (map.size() > 1) {
		Node* parent = new Node();

		Node lchild = map.front();
		std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

		Node rchild = map.front();
		std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

        parent->frequency += lchild.frequency + rchild.frequency;
		parent->lchild = new Node(lchild);
		printNode(parent->lchild);

		parent->rchild = new Node(rchild);
		printNode(parent->rchild);
		std::cout << std::endl;


		map.push_back(*parent); std::push_heap(map.begin(), map.end(), Node());
	}
	printNode(&map.front());

	return &map.front();
}

HuffmanCode* convertTreeToHashmap(Node* root){
    HuffmanCode hashmap[256];
    std::function<void(Node*, std::string)> preorder = [&](Node* root, std::string str) {
        if (root != nullptr) {  
            
            if (root->lchild == nullptr || root->rchild == nullptr)
                std::cout << " : " << root->character << " - " << root->frequency << " " << str << std::endl;

            preorder(root->lchild, str + "0");
            preorder(root->rchild, str + "1");
        }
    };
    preorder(root, "");
    return {};
}

void print(Node* root){
    if (root != nullptr){
        print(root->lchild);
        if (root->lchild == nullptr || root->rchild == nullptr)
            std::cout << root->character << std::endl;
        print(root->rchild);
    }
}

int main() {
	FrequencyMap map = GetFileCharacterFrequencies();
	Node* root = buildHuffmanTree(map);
    print(root);
    convertTreeToHashmap(root);
}