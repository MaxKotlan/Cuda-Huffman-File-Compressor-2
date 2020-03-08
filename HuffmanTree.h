#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include <bitset>
#include <string>
#include <algorithm>
#include <functional>

#define MAX_NODES 256

struct Node {
	unsigned char character;
	unsigned int frequency;
	Node* lchild = nullptr;
	Node* rchild = nullptr;
	bool operator()(const Node& a, const Node& b) const {
		return a.frequency > b.frequency;
	}
};

typedef std::vector<Node> FrequencyMap;

struct HuffmanCode {
	unsigned int path;
	unsigned int digits;
};

void print(const HuffmanCode& huff) {
	std::cout << std::bitset<8 * sizeof(unsigned int)>(huff.path).to_string().substr(8 * sizeof(unsigned int) - huff.digits);
}

void printNode(Node* n) {
	if (n != nullptr)
		std::cout << n << "{ char: " << n->character << ", freq: " << n->frequency << ", lchild: " << n->lchild << " rchild: " << n->rchild << " }";
}

bool isLeaf(Node* node) {
	return node->lchild == nullptr || node->rchild == nullptr;
}

class HuffmanTree {

public:

	HuffmanTree(FrequencyMap& map) : size(0){
		/*Turn Hashmap into a Heap Using bottom up construction O(n)*/
		std::make_heap(map.begin(), map.end(), Node());

		/*Remove All Characters with Zero Frequencies from the Heap*/
		int removedCount = 0;
		while (map.front().frequency == 0) {
			std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();
			removedCount++;
		}
		size = (MAX_NODES - removedCount);

		while (map.size() > 1) {
			Node* parent = new Node();

			Node lchild = map.front();
			std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

			Node rchild = map.front();
			std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();

			parent->frequency += lchild.frequency + rchild.frequency;
			parent->lchild = new Node(lchild);
			parent->rchild = new Node(rchild);

			map.push_back(*parent); std::push_heap(map.begin(), map.end(), Node());
			size++;
			if (map.size() == 1) root = parent;
		}
	};


	~HuffmanTree() {
		std::function<void(Node*)> deleteChildren = [&](Node* root) {
			if (root != nullptr) {
				deleteChildren(root->lchild);
				deleteChildren(root->rchild);
				delete root;
			}
		};
		deleteChildren(root);
	}

	std::vector<HuffmanCode> convertToHashmap() {
		std::vector<HuffmanCode> hashmap = std::vector<HuffmanCode>(256);
		std::function<void(Node*, std::string)> preorder = [&](Node* root, std::string str) {
			if (root != nullptr) {

				if (isLeaf(root)) {
					//std::cout << str << " - " << stoi(str, 0, 2) << " : " << root->character << " - " << root->frequency << std::endl;
					hashmap[root->character].path = stoi(str, 0, 2);
					hashmap[root->character].digits = str.length();
				}

				preorder(root->lchild, str + "0");
				preorder(root->rchild, str + "1");
			}
		};
		preorder(root, "");
		return std::move(hashmap);
	};


	/*Generates Header which contains all the nodes in the tree append to top of compressed file
	Stores bytes in little endian order, and stores nodes in preorder.
	*/
	std::vector<unsigned char> generateFileHeader() {
		unsigned int headerBytesNeeded = sizeof(unsigned int) + sizeof(unsigned short) + size * (sizeof(unsigned char) + sizeof(unsigned int));
		std::vector<unsigned char> header(headerBytesNeeded);
		/*Magic Bytes for Identifying File*/
		header[0] = 0x1B; header[1] = 0x0B; header[2] = 0x3E; header[3] = 0x70;
		/*Number of Nodes as a ushort*/
		for (int i = 0; i < sizeof(unsigned short); i++)
			header[sizeof(unsigned int)+i] = static_cast<unsigned char*>(static_cast<void*>(&size))[i];
		
		int nodeIndex = sizeof(unsigned short)+sizeof(unsigned int);
		std::function<void(Node*)> preorder = [&](Node* root) {
			if (root != nullptr) {

				/*Node character*/
				header[nodeIndex] = root->character;
				/*Node frequency*/
				for (int i = 0; i < sizeof(unsigned int); i++)
					header[nodeIndex+sizeof(unsigned char)+i] = static_cast<unsigned int*>(static_cast<void*>(&root->frequency))[i];
				nodeIndex+=sizeof(unsigned char)+sizeof(unsigned int);

				preorder(root->lchild);
				preorder(root->rchild);
			}
		};
		preorder(root);

		std::cout << "Compressed File Header: (" << header.size() << ")" << std::endl;
		for (auto it = header.begin(); it != header.end(); it++)
			std::cout << std::hex << (int)*it << " ";

		return std::move(header);
	}

private:
	Node* root;
	unsigned short size;
};

#endif