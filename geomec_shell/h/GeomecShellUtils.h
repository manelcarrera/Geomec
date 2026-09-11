#ifndef _UTILS_H_
#define _GEOMEC_SHELL_UTILS_H_


#include "InterfaceContext.h"


class QString;


namespace gm_shell
{


const char* ProgName(const char* argv0);
void MakeAbsolutePath(QString& strPath);
void copyFile(const QString& destination, const QString& source);
void moveFile(const QString& destination, const QString& source);


}



#endif