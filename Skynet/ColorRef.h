#pragma once

/*
	this file exists SOLELY so that I can pass a pointer to a Color object.

	Proper understand of C++/CLI handles/points would make this unneccesary.

*/
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing;

ref class ColorRef {
public:
	ColorRef(Color inColor) { theColor = inColor; }
	~ColorRef() {}

	Color theColor;

};