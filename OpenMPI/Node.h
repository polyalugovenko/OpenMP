#pragma once
#include <iostream>
#include <fstream>
#include <cstddef>
using namespace std;
class Node
{
public:
	int weight;
	char symb;
	Node* left, * right;

	Node();
	Node(Node* l, Node* r);
	bool operator()(const Node* l, const Node* r) const;
};
