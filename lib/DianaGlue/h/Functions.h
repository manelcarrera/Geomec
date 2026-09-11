#pragma once

#include <QString>

namespace CTempPath
{
	QString TEMP_CALCULATION;
}

QString GetGeomecTempPathExt( QString param ){ return ""; }
void SplitPathAndFileName( QString, QString, QString ){}
QString RemoveExtension( QString ){ return ""; }
