#include "CFunctions.h"

namespace CFunctions{
	string ReplaceString(string subject, const string& search,const string& replace) 
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			 subject.replace(pos, search.length(), replace);
			 pos += replace.length();
		}
		return subject;
	}

	vector<string> split_string(const std::string& str,const std::string& delimiter)
	{
		vector<string> strings;

		string::size_type pos = 0;
		string::size_type prev = 0;
		while ((pos = str.find(delimiter, prev)) != string::npos)
		{
			strings.push_back(str.substr(prev, pos - prev));
			prev = pos + 1;
		}

		// To get the last substring (or only, if delimiter is not found)
		strings.push_back(str.substr(prev));

		return strings;
	}
};