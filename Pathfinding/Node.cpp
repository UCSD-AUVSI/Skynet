#include "Node.h"
#include "Pathfinder.h"

using namespace Intelligence;

Node::Node(int i, int j, int id, Pathfinder^ pathfinder){
	this->visited = false;
	this->i = i;
	this->j = j;
	this->id = id;
	this->pathfinder = pathfinder;
	this->coordinates = pathfinder->arrayToMeters(i,j);
	this->x = this->coordinates->Item1;
	this->y = this->coordinates->Item2;
	this->children = gcnew array<Node^>(6);
}

void Node::set_children(array<Node^>^ children){
	this->children = children;
}

/*virtual*/ String^ Node::ToString() //override
{
	return __repr__();
}

String^ Node::__repr__()
{
	array<int>^ ids;
	/*while(Pathfinder::inNode(child, children))
	{
		if ( child != nullptr)
		{
			ids->Append(this.id);
		}
	}

    for(int i = 0; i< children->Length; i++)
	{
		*/
	return "ID: " + id + " I: " + i +" J: " + j + " X: " + coordinates->Item1 + " Y: " + coordinates->Item2 + " Children: " + ids;
}