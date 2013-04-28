#pragma once
using System::String;
using namespace System::Collections::Generic;

namespace Intelligence{ 
	ref class Node;
	ref class AdjacencyList
	{
	public:
		AdjacencyList(){};
		AdjacencyList(List<Node^>^ nodes);
		void push(Node^ node);
		Node^ node_at(int i, int j);
		String^ __repr__();
		virtual String^ ToString() override;
		List<Node^>^ arr;
	};
}

