#include <cassert>
#include <qstring.h>

#include "GlobalMessage.h"
#include "StatusMessage.h"

void statusMessage(const QString &message) { outstream() << std::endl << message.toStdString() << std::endl; }
