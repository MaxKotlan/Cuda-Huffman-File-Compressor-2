#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

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

void printNode(Node* n) {
	if (n != nullptr)
		std::cout << n << "{ char: " << n->character << ", freq: " << n->frequency << ", lchild: " << n->lchild << " rchild: " << n->rchild << " }";
}

bool isLeaf(Node* node) {
	return node->lchild == nullptr || node->rchild == nullptr;
}

class HuffmanTree {

	public:

		HuffmanTree(FrequencyMap& map) {
			/*Turn Hashmap into a Heap Using bottom up construction O(n)*/
			std::make_heap(map.begin(), map.end(), Node());

			/*Remove All Characters with Zero Frequencies from the Heap*/
			while (map.front().frequency == 0) {
				std::pop_heap(map.begin(), map.end(), Node()); map.pop_back();
			}

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
						std::cout << str << " - " << stoi(str, 0, 2) << " : " << root->character << " - " << root->frequency << std::endl;
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

	private:
		Node* root;
};

#endif