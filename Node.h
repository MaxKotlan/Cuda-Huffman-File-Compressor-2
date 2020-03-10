#ifndef NODE_H
#define NODE_H

struct Node {
	unsigned char character;
	unsigned int frequency;
	Node* lchild = nullptr;
	Node* rchild = nullptr;
	bool operator()(const Node& a, const Node& b) const {
		return a.frequency > b.frequency;
	}
};

#endif