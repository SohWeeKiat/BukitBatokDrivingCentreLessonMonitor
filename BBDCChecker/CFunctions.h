#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using namespace std;

namespace CFunctions{
	string ReplaceString(string subject, const string& search,const string& replace);

	vector<string> split_string(const std::string& str,const std::string& delimiter);
};