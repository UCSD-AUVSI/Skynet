#include "Node.h"

using namespace Intelligence;

Node::Node(int i, int j, int id){
	this->visited = false;
	this->i = i;
	this->j = j;
	this->id = id;
	// this->coordinates = pathfinder->arrayToMeters(i,j); TODO: Fix
	this->x = this->coordinates->Item1;
	this->y = this->coordinates->Item2;
	this->children = gcnew array<Node^>(6);
}

void Node::set_children(array<Node^>^ children){
	this->children = children;
}