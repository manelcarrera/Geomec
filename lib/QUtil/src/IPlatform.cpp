
#include <cassert>

#include "IPlatform.h"

IPlatform::~IPlatform()
{
}

IPlatform* IPlatform::instance()
{
  assert(m_singleton != 0);

  return m_singleton;
}

// protected

IPlatform::IPlatform()
{
}

IPlatform* IPlatform::m_singleton = 0;
