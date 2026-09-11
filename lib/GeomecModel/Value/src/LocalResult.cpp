#include "LocalResult.h"
#include "ivaluecomposite.h"



bool CResultComponentLess::operator()(const TResultComponent &c1, const TResultComponent &c2) const
{
  if (c1.first->Less(*c2.first))
    return true;
  if (c2.first->Less(*c1.first))
    return false;

  assert(c1.first == c2.first);

  return c1.second < c2.second;
}

