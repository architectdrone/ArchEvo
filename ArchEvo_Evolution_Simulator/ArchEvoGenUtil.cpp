#include "ArchEvoGenUtil.h"
#include <string>
using namespace std;
int ArchEvoGenUtil::true_mod(int n, int m)
{
	return (m + (n % m)) % m;
}

string ArchEvoGenUtil::split_string(string to_split, string delimiter, int position)
{
	//Adapted from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c 
	auto start = 0U;
	auto end = to_split.find(delimiter);
	int cur_pos = 0;
	while (end != std::string::npos)
	{
		if (cur_pos == position)
		{
			return to_split.substr(start, end - start);
		}
		start = end + delimiter.length();
		end = to_split.find(delimiter, start);
		cur_pos++;
	}
	if (cur_pos == position)
	{
		return to_split.substr(start, end - start);
	}
	return "";
}