
#include <QRegExp>

#include "StringUtils.h"

int findOneOf(const QString& target, const QString& characterSet)
{
  QString newCharacterSet;

  for (QString::const_iterator begin = characterSet.begin(), end = characterSet.end();
  begin != end;)
  {
  if (*begin == '|')
  {
      newCharacterSet += "\\|";
  }
  else
  {
      newCharacterSet += *begin;
  }

  ++begin;

  if (begin != end)
  {
      newCharacterSet += '|';
  }
  }

  if (newCharacterSet.isEmpty())
  {
  return -1;
  }

  QRegExp regularExpression(newCharacterSet);

  return target.indexOf(regularExpression);
}

QString spanExcluding(const QString& target, const QString& characterSet)
{
  int length = findOneOf(target, characterSet);

  if (length == -1)
  {
  length = target.length();
  }

  return target.left(length);
}

QString trimRight(const QString& target)
{
  int length = target.length();

  while ((length > 0) && (target[length - 1].isSpace()))
  {
  --length;
  }

  return target.left(length);
}
