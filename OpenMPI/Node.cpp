#include "Node.h"
Node::Node()
{
	left = NULL; right = NULL;
}
Node::Node(Node* l, Node* r)
{
	left = l;
	right = r;
	weight = l->weight + r->weight;
}

bool Node::operator()(const Node* l, const Node* r) const
{
	return l->weight < r->weight;
}
