#pragma once
using System::Tuple;
using System::String;

namespace Intelligence {
	ref class Pathfinder;
	ref class Node
	{
	public:
		Node(int i, int j, int id, Pathfinder^ pathfinder); 

		void set_children(array<Node^ >^ children);
		virtual String^ ToString() override;
		String^ __repr__();

		bool visited;
		int i,j,id,x,y;
		array<Node^>^ children;
			
	private:
		Tuple<int, int>^ coordinates;
		Pathfinder^ pathfinder;
	};
}
