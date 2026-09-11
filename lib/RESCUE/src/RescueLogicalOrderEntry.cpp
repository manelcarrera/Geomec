/********************************************************************

  RescueLogicalOrderEntry.cpp

  Base class for all objects that can be in a RescueLogicalOrder

  Rod Hanks,  June 2002

*********************************************************************/
#include "RescueModel.h"
#include "RescueLogicalOrderEntry.h"

RESCUEBOOL RescueLogicalOrderEntry::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueLogicalOrderEntry)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}


