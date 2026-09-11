/************************* R e s c u e O b j e c t *************************/
/*
 An abstract class for all of the objects in the application view,
 the main purpose being initialization, run-time typing of objects, 
 and tombstones.

  Rod Hanks   May, 1996

*/
#include "myHeaders.h"
void RescueObject::ReadId(RescueContext *context, FILE *archiveFile) 
{
  myfscanf(context, archiveFile, &uniqueID);
  if (uniqueID > context->uniqueIDCounter) context->uniqueIDCounter = uniqueID + 1;
}




