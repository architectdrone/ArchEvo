#pragma once
#include <string>
using namespace std;
class ArchEvoGenUtil
{
public:
	//General utility functions for use around the entire codebase.
	static int true_mod(int n, int m);
	static string split_string(string to_split, string delimiter, int position);
};