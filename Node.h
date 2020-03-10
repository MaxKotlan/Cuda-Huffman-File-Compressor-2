#ifndef NODE_H
#define NODE_H

struct Node {
	unsigned char character = 0;
	unsigned int frequency = 0;
	Node* lchild = nullptr;
	Node* rchild = nullptr;
	bool operator()(const Node& a, const Node& b) const {
		return a.frequency > b.frequency;
	}
};

#endif