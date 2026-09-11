/********************************************************************

  RescueTruncation.cpp

  A Truncation on a Pillar grid.  We don't need the overhead
  so this is not a RescueObject.

  Either truncation of base or top is stored here:

             #     *        #
           A |   ./         |
             |  ./ B        |
             | ./           *
             |./          A | \. C
             *              |  \.
             |              |   \.
             |              |    \.
             #              #      *

  Pillar A: Is the truncating pillar: Does not know anything.
  Pillar B: Is the BASE truncated pillar and has a pointer to
      the Truncation. i,j gives the node of the pillar A.
  Pillar C: Is the TOP truncated pillar and has a pointer to the
      Truncation.  i,j gives the node of pillar A.

  The self_fault_id and self_limit are used for self truncating
  pillars only.  Then the same pillar is truncating and truncated,
  and is found where the truncated pillar branches from the truncating
  pillar.

  self_fault_id: The id of the fault which is truncating
  self_limit:    The z value where the truncation begins.

  Rod Hanks,  Oct, 2003

*********************************************************************/
#include "RescueTruncation.h"
#include "RescueModel.h"
#include <limits.h>
#include <float.h>
#include "RescueBuffer.h"
#include <string.h>

RescueTruncation::RescueTruncation()
{
  i = INT_MAX;
  j = INT_MAX;
  self_fault_id = USHRT_MAX;
  self_limit = FLT_MAX;
}

RESCUEBOOL RescueTruncation::isOk(RCHString &message) const
{
  RESCUEBOOL myReturn = TRUE;
  RESCUEBOOL is_i_def = (i != INT_MAX);
  RESCUEBOOL is_j_def = (j != INT_MAX);

  // Either (i,j) is fully defined for a truncating-truncated pillar pair,
  // or (i,j) is left fully undefined for a self-truncating fault.
  if (is_i_def != is_j_def)
  {
  message = "either I and J node index is not defined for truncation";
  myReturn = FALSE;
  }
  else
  {
  RESCUEBOOL self_fault_id_def = (self_fault_id != USHRT_MAX);
  RESCUEBOOL self_limit_def = (self_limit != FLT_MAX);

  if (self_fault_id_def != self_limit_def)
  {
      message = "Self fault Id must be set if self limit is set or opposite";
      myReturn = FALSE;
  }
  }
  return myReturn;
}

RESCUEBOOL RescueTruncation::isSelfTruncating() const
{
  RESCUEBOOL self_fault_id_def = (self_fault_id != USHRT_MAX);
  RESCUEBOOL self_limit_def    = (self_limit    != FLT_MAX);
  RESCUEBOOL is_i_undef        = (i             == INT_MAX);
  RESCUEBOOL is_j_undef        = (j             == INT_MAX);

  return (self_fault_id_def
       && self_limit_def
       && is_i_undef
       && is_j_undef);
}

void RescueTruncation::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, i);
  myfprintf(context, archiveFile, j);
  myfprintf(context, archiveFile, self_fault_id);
  myfprintf(context, archiveFile, self_limit);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RescueTruncation::RescueTruncation(RescueContext *context, FILE *archiveFile)
{
  myfscanf(context, archiveFile, &i);
  myfscanf(context, archiveFile, &j);
  myfscanf(context, archiveFile, &self_fault_id);
  myfscanf(context, archiveFile, &self_limit);
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueTruncation::SwapIJAxis(bool swapI, RESCUEINT64 iLowBound, RESCUEINT64 iCount, 
                                  bool swapJ, RESCUEINT64 jLowBound, RESCUEINT64 jCount)
{
  if (swapI)
  {
  i = ((iLowBound + iCount) - 1) + (i - iLowBound);
  }
  if (swapJ)
  {
  j = ((jLowBound + jCount) - 1) + (j - jLowBound);
  }
}


