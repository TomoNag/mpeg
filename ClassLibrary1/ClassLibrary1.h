// ClassLibrary1.h
#include <string>
#include <iostream>
#include <fstream>

#pragma once

using namespace System;
using namespace std;

static int test(String^ input, String^ output);

namespace OpenCVdll {

	

	public ref class Class1
	{
	public:
		// TODO: Add your methods for this class here.
		static int testX(String^ input);
		static int video(String^ input, String^ output);
	};
}
