#include <cassert>
#include <qstring.h>

#include "StatusMessage.h"
#include "GlobalMessage.h"

void statusMessage(const QString& message)
{
  outstream() << std::endl << message.toStdString() << std::endl;
}
