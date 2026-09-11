#include "DCasingDefines.h"

QString ConstructItem(const QString & Description, bool Value)
{
	QString strValue;

	if (Value)
		strValue = "TRUE";
	else
		strValue = "FALSE";

	return QString("%1").arg(Description,-LEFTALIGN) + QString("%1").arg(QString("%1").arg(strValue),VALUESIZE);
}

void TrimQString(QString & varString)
{
	int counter = 0;
	bool IsSpace = true;
	QString Result;
	
	while( counter < varString.length() && IsSpace)
	{
		QChar theChar = varString[counter];
	
		if (theChar == ' ')  
			counter++;
		else
			IsSpace = false;
	}

	if (!IsSpace)
		varString = varString.right(varString.length() - counter);

	IsSpace = true;
	counter = varString.length() - 1;

	while( counter >= 0 && IsSpace)
	{
		QChar theChar = varString[counter];
	
		if (theChar == ' ')  
			counter--;
		else
			IsSpace = false;
	}
	
	if (!IsSpace)
		varString = varString.left(counter+1);
}

QString EnsureBackslash(const QString& path)
{
	if(path.right(1) != "\\")
		return path + "\\";
	else
		return path;
}


QString EnsureNoBackslash(const QString& path)
{
	if(path.right(1) == "\\")
		return path.left(path.length()-1);
	else
		return path;
}

template <>
double lexical_cast<double, QString>(QString arg)
{
  return lexical_cast<double>(arg.toStdString().c_str());
}
