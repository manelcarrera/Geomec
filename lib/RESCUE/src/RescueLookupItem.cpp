/********************************************************************

  RescueLookupItem.cpp

  An item in a table of lookups for properties. 

  Rod Hanks,  May 1997

*********************************************************************/
#include "RescueLookupItem.h"

RESCUEBOOL RescueLookupItem::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueLookupItem)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}





