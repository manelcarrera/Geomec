#include "Platform.h"

#include <cassert>
#include <QDebug>

#include "GlobalMessage.h"

void CPlatform::init()
{
  assert(!m_singleton);
  m_singleton = new CPlatform;
}

long CPlatform::processMemorySize() const
{
  return 0l;
}

void CPlatform::trace(const QString& message) const
{
  // FIXME

  /*if(&outstream() != &std::cout)
  outstream() << message.toStdString() << std::endl;
  else
  qDebug() << message;*/

  _m()->msg( message );
}

CPlatform::CPlatform()
{
}
