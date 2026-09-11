#ifndef __STRINGUTILS_H_
#define __STRINGUTILS_H_

#include <string>
#include <vector>

using namespace std;



class StringUtils
{

public:

	static int SplitString(const string& input, const string& delimiter, vector<string>& results);
	static int SplitString(const CString& input, const CString& delimiter, vector<CString>& results);

};

#endif
