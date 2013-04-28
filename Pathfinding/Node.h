#pragma once

namespace Intelligence {
	ref class Node
	{
	public:
		Node(int i, int j, int id); 

		void set_children(array<Node^ >^ children);

		bool visited;
		int i,j,id,x,y;
		array<Node^>^ children;
			
	private:
		System::Tuple<int, int>^ coordinates;
	};
}
