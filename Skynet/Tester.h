#pragma once

namespace Testing {
	public ref class Tester abstract {
	public:
		void assertTrue(bool predicate);
		virtual void runTests();
	};
}