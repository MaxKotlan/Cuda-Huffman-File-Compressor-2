#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
#include <bitset>
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>

#define MAX_NODES 256

#include "Node.h"

typedef std::vector<Node> FrequencyMap;

struct HuffmanCode {
	unsigned int path;
	unsigned int shift;
};

void print(const HuffmanCode& huff) {
	std::cout << std::bitset<8 * sizeof(unsigned int)>(huff.path).to_string().substr(8 * sizeof(unsigned int) - huff.shift);
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

		/*Keep track of the number of nodes and leaf nodes */
		size = (MAX_NODES - removedCount);
		leaf_size = size;

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
		//deleteChildren(root);
	}

	std::vector<HuffmanCode> convertToHashmap() {
		std::vector<HuffmanCode> hashmap = std::vector<HuffmanCode>(256);
		std::function<void(Node*, std::string)> preorder = [&](Node* root, std::string str) {
			if (root != nullptr) {

				if (isLeaf(root)) {
					if (str.size() >= (sizeof(HuffmanCode::path) * 8)) {
						std::cout << "ERROR: Implementation requires tree to have a Max Depth of " << sizeof(HuffmanCode::path) * 8 << "! This branch has a depth of " << str.size() << std::endl;
						exit(-1);
					}
					hashmap[root->character].path = stoi(str, 0, 2);
					hashmap[root->character].shift = str.size();
				}

				preorder(root->lchild, str + "0");
				preorder(root->rchild, str + "1");
			}
		};
		preorder(root, "");
		return std::move(hashmap);
	};


	void print(){
		std::function<void(Node*, std::string)> preorder = [&](Node* root, std::string str) {
		if (root != nullptr) {

				if (isLeaf(root)) {
					std::cout << "character: " << root->character << " code: " <<  str << std::endl;
				}

				preorder(root->lchild, str + "0");
				preorder(root->rchild, str + "1");
			}
		};
		preorder(root, "");
	}

	/*Generates Header which contains the leaf nodes in the tree append to top of compressed file
	Stores bytes in little endian order, and stores nodes in preorder. Using the leafnodes, we can regenerate the tree
	when it's loaded in again
	*/
	std::vector<unsigned char> generateFileHeader() {
		unsigned int headerBytesNeeded = sizeof(unsigned int) + sizeof(unsigned short) + leaf_size * (sizeof(unsigned char) + sizeof(unsigned int));
		std::vector<unsigned char> header(headerBytesNeeded);
		/*Magic Bytes for Identifying File*/
		header[0] = 0x1B; header[1] = 0x0B; header[2] = 0x3E; header[3] = 0x70;
		/*Number of Nodes as a ushort*/
		std::copy(static_cast<const unsigned char*>(static_cast<const void*>(&leaf_size)),
			static_cast<const unsigned char*>(static_cast<const void*>(&leaf_size)) + sizeof(leaf_size),
			&header[4]);
		
		int nodeIndex = sizeof(unsigned short)+sizeof(unsigned int);
		std::function<void(Node*)> preorder = [&](Node* root) {
			if (root != nullptr) {

				if (isLeaf(root)){
					/*Node character*/
					header[nodeIndex] = root->character;

					std::copy(static_cast<const unsigned char*>(static_cast<const void*>(&root->frequency)),
						static_cast<const unsigned char*>(static_cast<const void*>(&root->frequency)) + sizeof (root->frequency),
						&header[nodeIndex+1]);

					nodeIndex+=sizeof(unsigned char)+sizeof(unsigned int);
				}

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

	void printFrequencies(){
		std::function<void(Node*)> inorder = [&](Node* root) {
			if (root != nullptr) {
				if (isLeaf(root)){
					std::cout << std::dec <<  root->character << " - " << root->frequency << std::endl;
				}
				inorder(root->lchild);
				inorder(root->rchild);
			}
		};
		inorder(root);
	}

private:
	Node* root;
	unsigned short size;
	unsigned short leaf_size;
};

#endif