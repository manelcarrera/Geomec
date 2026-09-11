/*************************************************************************

        cSetRescueDataContainer.cpp

 Keeps a list of pointers to some RescueDataContainer.

        Rod Hanks               Feb 2006

****************************************************************************/
#include "RescueModel.h"
#include "cSetRescueDataContainer.h"
#include "RescueDataContainer.h"

#define DATA_CONTAINER_FILE_VERSION 1

cSetRescueDataContainer::cSetRescueDataContainer(RescueModel *modelIn, RESCUEINT64 identifierIn)
{
  fileVersion = 0;
  model = modelIn;
  RescueContext *context = model->Context();
  identifier = identifierIn;
  if (identifier > context->uniqueIDCounter) context->uniqueIDCounter = identifier + 1;
  allocated = 10;
  count = 0;
  objects = (RescueDataContainer **) malloc(sizeof(RescueDataContainer *) * (size_t) allocated);

  RCHString fileName;
  if (model->oldPathName != 0)
  {
    fileName << model->oldPathName;
  }
  else
  {
    fileName << model->currentPathName;
  }
  fileName << "." << Identifier();
  FILE *archiveFile;
  if (context->binaryFlag)
  {
    archiveFile = (FILE *) fopen(fileName.String(), "rb");
    if (archiveFile != 0)
    {
      getc(archiveFile);
      fseek(archiveFile, 26, SEEK_CUR);
      myfscanf(context, archiveFile, &fileVersion);
      UnArchive(archiveFile);
      fclose(archiveFile);
    }
  }
  else
  {
    RESCUECHAR myString[255];

    archiveFile = (FILE *) fopen(fileName.String(), "rt");
    if (archiveFile != 0)
    {
      myfgets(context, myString, 255, archiveFile);
      RESCUEINT64 fileFormatVersion;
      sscanf(myString, "Rescue Data Container File Version %lld\n", &fileFormatVersion);
      myfscanf(context, archiveFile, &fileVersion);
      UnArchive(archiveFile);
      fclose(archiveFile);
    }
  }
  Relink(model);
}

cSetRescueDataContainer::cSetRescueDataContainer(RescueModel *modelIn)
{
  fileVersion = 0;
  model = modelIn;
  allocated = 10;
  count = 0;
  objects = (RescueDataContainer **) malloc(sizeof(RescueDataContainer *) * (size_t) allocated);
  identifier = (model->Context()->uniqueIDCounter)++;
}

void cSetRescueDataContainer::Archive()
{
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  FILE *archiveFile;
  model->MakeBackupFile(fileName.String());
  if (model->currentBinary)
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
    model->Context()->binaryFlag = model->currentBinary;
    if (model->Context()->binaryFlag)
    {
      putc((RESCUEUCHAR) DATA_CONTAINER_FILE_VERSION, archiveFile);
      fwrite("Rescue Data Container File", sizeof(RESCUECHAR), 26, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Data Container File Version %d\n", DATA_CONTAINER_FILE_VERSION);
    }
#ifdef TESTING
    myfprintf(model->Context(), archiveFile, fileVersion);
#else
    myfprintf(model->Context(), archiveFile, ++fileVersion);
#endif
    Archive(archiveFile);
    myfprintf(model->Context(), archiveFile, "abracadabra jump jump");
    if (ferror(archiveFile) != 0)
    {
      RCHString message;
      message << "Error writing to Rescue Data Container File:";
      message<< fileName.String();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
    }
    fclose(archiveFile);
  }
}

cSetRescueDataContainer::~cSetRescueDataContainer()
{
  RESCUEINT64 loop;

  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  free(objects);
}

RESCUEBOOL cSetRescueDataContainer::FileTruncated()
{
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  RESCUEBOOL myReturn = model->Context()->FileTruncatedPrimitive(fileName.String());
  return myReturn;
}

void cSetRescueDataContainer::Archive(FILE *archiveFile)
{
  myfprintf(model->Context(), archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Archive(archiveFile);
  }
}

void cSetRescueDataContainer::Relink(RescueObject *parent)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->Relink(parent);
  }
}

RESCUEBOOL cSetRescueDataContainer::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FileTruncated();
  if (myReturn == FALSE)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < count && myReturn == FALSE; loop++)
    {
      myReturn = objects[loop]->AnyFileTruncated();
    }
  }
  return myReturn;
}

void cSetRescueDataContainer::UnArchive(FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(model->Context(), archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    RescueDataContainer *newObject = new RescueDataContainer(model, archiveFile);
    (*this) += newObject;
  }
}

void cSetRescueDataContainer::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete objects[loop];
  }
  count = 0;
}

void cSetRescueDataContainer::operator+=(RescueDataContainer *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    objects = (RescueDataContainer **) realloc(objects, sizeof(RescueDataContainer *) * (size_t) allocated);
  }
  objects[count++] = newObject;
}

RESCUEBOOL cSetRescueDataContainer::operator-=(RescueDataContainer *existingObject)
{
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ndx = 0;

  while (ndx < count && found == FALSE)
  {
    if (existingObject == objects[ndx])
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    RESCUEINT64 loop;

    objects[ndx]->Dispose();
    count--;
    for (loop = ndx; loop < count; loop++)
    {
      objects[loop] = objects[loop + 1];
    }
  }
  return found;
}

RescueDataContainer *cSetRescueDataContainer::ObjectIdentifiedBy(RESCUEINT64 identifier)
{
  RESCUEINT64 ndx = 0;
  RESCUEBOOL found = FALSE;

  while (ndx < count && found == FALSE)
  {
    if (objects[ndx]->IsIdentifiedBy(identifier))
    {
      found = TRUE;
    }
    else
    {
      ndx++;
    }
  }
  if (found)
  {
    return objects[ndx];
  }
  else
  {
    return 0;
  }
}

RESCUEBOOL cSetRescueDataContainer::operator-=(RESCUEINT64 ndx)
{
  if (ndx >= 0 && ndx < count)
  {
    RESCUEINT64 loop;

    objects[ndx]->Dispose();
    count--;
    for (loop = ndx; loop < count; loop++)
    {
      objects[loop] = objects[loop + 1];
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

RescueDataContainer *cSetRescueDataContainer::NthObject(RESCUEINT64 ordinal)
{
  if (ordinal < 0 || ordinal >= count)
  {
    return 0;
  }
  else
  {
    return objects[ordinal];
  }
}

RESCUEINT64 cSetRescueDataContainer::Count64(void)
{
  return count;
}

RESCUEINT32 cSetRescueDataContainer::Count(void)
{
  return (RESCUEINT32) count;
}

void cSetRescueDataContainer::FindUniquePropertyNames(cSetString *container)
{
  int loop;
  for (loop = 0; loop < count; loop++)
  {
    objects[loop]->FindUniquePropertyNames(container);
  }

}

RESCUEINT32 cSetRescueDataContainer::Count(RESCUEBOOL throwIfTrue)
{
  if (count > 2147483647)
  {
    if (throwIfTrue)
    {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  }
  else
  {
    return (RESCUEINT32) count;
  }
}




