
#include "stdafx.h" // comment if not using precompiled headers in MVC++
#include "StringUtils.h"

int StringUtils::SplitString(const string& input, const string& delimiter, vector<string>& results)
{
	int iPos = 0;
	int newPos = -1;
	int sizeS2 = delimiter.size();
	int isize = input.size();

	vector<int> positions;

	newPos = input.find (delimiter, 0);

	if( newPos < 0 ) { return 0; }

	int numFound = 0;

	while( newPos > iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = input.find (delimiter, iPos+sizeS2+1);
	}

	for( size_t i=0; i <= positions.size(); i++ )
	{
		string s;
		if( i == 0 ) { s = input.substr( i, positions[i] ); }
		int offset = (i == 0) ? isize : positions[i-1] + sizeS2;
		if( offset < isize )
		{
			if( i == positions.size() )
			{
				s = input.substr(offset);
			}
			else if( i > 0 )
			{
				s = input.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
			}
		}
		if( s.size() > 0 )
		{
			if(s != delimiter)
			{
				results.push_back(s);
			}
		}
	}
	return numFound;
}

int StringUtils::SplitString(const CString& input, const CString& delimiter, vector<CString>& results)
{
	CString s;
	std::vector<string> vec;
	std::string str_in = input;
	std::string str_delimit=delimiter;
	int ret=StringUtils::SplitString(str_in,str_delimit,vec);
	for(int i=0;i<vec.size();i++)
	{
		s = CString(vec[i].c_str());
		s.TrimLeft();
		results.push_back(s);
	}
	return ret;
}