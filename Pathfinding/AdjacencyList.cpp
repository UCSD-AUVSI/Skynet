#include <String>
#include "AdjacencyList.h"
#include "Node.h"

using namespace Intelligence;


AdjacencyList::AdjacencyList(List<Node^>^ nodes)
{
	arr = nodes;
}

void AdjacencyList::push(Node^ node)
{
	arr->Add(node);
}

Node^ AdjacencyList::node_at(int i, int j)
{
	for(int k = 0; k < arr->Count; k++)
	{
		Node^ node = arr[k];
		if(node->i == i && node->j == j)
			return node;
	}	
	return nullptr;
}

String^ AdjacencyList::__repr__()
{
	String^ str;
	for(int i = 0; i < arr->Count; i++)
	{
		str += arr[i];
	}
	return "Array: " + str;
}

String^ AdjacencyList::ToString()
{
	return __repr__();
}

