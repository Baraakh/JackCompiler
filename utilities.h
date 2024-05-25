#pragma once
#include <iostream>
#include <string>
using namespace std;

bool isNumber(string str)
{
	if (!str.size()) return false;

	for (char c : str)
	{
		if (!isdigit(c)) return false;
	}

	return true;
}