#ifndef _StringUtils_h_
#define _StringUtils_h_

#include <qstring.h>

int findOneOf(const QString& target, const QString& characterSet);
QString spanExcluding(const QString& target, const QString& characterSet);
QString trimRight(const QString& target);

#endif  // _StringUtils_h_
