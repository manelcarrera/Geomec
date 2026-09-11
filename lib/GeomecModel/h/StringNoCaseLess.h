#ifndef _StringNoCaseLess_h_
#define _StringNoCaseLess_h_

#include <qstring.h>

// Less operator for case insensitive input tag searching
class CStringNoCaseLess
{
public:
	bool operator()(const QString& s1, const QString& s2) const
	{ return s1.compare(s2, Qt::CaseInsensitive) < 0; }
};

#endif  // _StringNoCaseLess_h_
