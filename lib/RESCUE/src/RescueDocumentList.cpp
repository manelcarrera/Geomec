/********************************************************************

  RescueDocumentList.cpp

  A document of documents attached to a RescueModel.

  Rod Hanks,  June, 2007

*********************************************************************/
#include "myHeaders.h"
#include "RescueContext.h"
#include "RescueModel.h"
#include "RescueDocumentList.h"
#include "RescueDocument.h"

#define DOCUMENT_FILE_VERSION 1

RescueDocumentList::~RescueDocumentList()
{
  if (documents != 0)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++)
    {
      delete documents[loop];
    }
    free(documents);
  }
}

RescueDocument *RescueDocumentList::NthRescueDocument(RESCUEINT64 zeroBasedIndex)
{
  RescueDocument *myReturn = 0;
  if (zeroBasedIndex >= 0 && zeroBasedIndex < count)
  {
    myReturn = documents[zeroBasedIndex];
  }
  return myReturn;
}

cSetString *RescueDocumentList::UniqueMetaKeys()
{
  cSetString *myReturn = new cSetString();
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    documents[loop]->UniqueMetaKeys(myReturn);
  }
  return myReturn;
}

RescueDocument *RescueDocumentList::NthDocumentWithKey(RESCUEINT64 zbn, const RESCUECHAR *keyToFind)
{
  RescueDocument *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++)
  {
    if (documents[loop]->ContainsMetaKey(keyToFind))
    {
      zbn--;
      if (zbn < 0)
      {
        myReturn = documents[loop];
      }
    }
  }
  return myReturn;
}

RESCUEBOOL RescueDocumentList::RemoveDocument(RescueDocument *toRemove)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    if (myReturn == TRUE)
    {
      documents[loop - 1] = documents[loop];
    }
    else if (documents[loop] == toRemove)
    {
      delete toRemove;
      myReturn = TRUE;
    }
  }
  if (myReturn == TRUE)
  {
    count--;
  }
  return myReturn;
}

RescueDocumentList::RescueDocumentList(RescueModel *modelIn):RescueHistoryObject(modelIn->Context())
{
  isA = R_RescueDocumentList;
  model = modelIn;
  count = 0;
  allocated = 0;
  documents = 0;
  fileVersion = 0;
}

RescueDocumentList::RescueDocumentList(RescueModel *modelIn, RESCUEINT64 identifierIn):RescueHistoryObject(modelIn->Context())
{
  isA = R_RescueDocumentList;
  model = modelIn;

  fileVersion = 0;
  model = modelIn;
  RescueContext *context = model->Context();
  uniqueID = identifierIn;
  if (uniqueID > context->uniqueIDCounter) context->uniqueIDCounter = uniqueID + 1;
  allocated = 10;
  count = 0;
  documents = (RescueDocument **) malloc(sizeof(RescueDocument *) * (size_t) allocated);

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
      fseek(archiveFile, 20, SEEK_CUR);
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
      sscanf(myString, "Rescue Document File Version %lld\n", &fileFormatVersion);
      myfscanf(context, archiveFile, &fileVersion);
      UnArchive(archiveFile);
      fclose(archiveFile);
    }
  }
}

void RescueDocumentList::UnArchive(FILE *archiveFile)
{
  RESCUEINT64 newCount;

  EmptySelf();

  myfscanf(model->Context(), archiveFile, &newCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < newCount; loop++)
  {
    new RescueDocument(this, model, archiveFile);
  }
}

void RescueDocumentList::Add(RescueDocument *newObject)
{
  if (allocated == count)
  {
    allocated += 10;
    documents = (RescueDocument **) realloc(documents, sizeof(RescueDocument *) * (size_t) allocated);
  }
  documents[count++] = newObject;
}

void RescueDocumentList::Archive()
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
      putc((RESCUEUCHAR) DOCUMENT_FILE_VERSION, archiveFile);
      fwrite("Rescue Document File", sizeof(RESCUECHAR), 20, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Document File Version %d\n", DOCUMENT_FILE_VERSION);
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
      message << "Error writing to Rescue Document File:";
      message<< fileName.String();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
    }
    fclose(archiveFile);
  }
}

void RescueDocumentList::Archive(FILE *archiveFile)
{
  myfprintf(model->Context(), archiveFile, count);
  RESCUEINT64 loop;
  for (loop = 0; loop < count; loop++)
  {
    documents[loop]->Archive(archiveFile);
  }
}

RESCUEBOOL RescueDocumentList::FileTruncated()
{
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  RESCUEBOOL myReturn = model->Context()->FileTruncatedPrimitive(fileName.String());
  return myReturn;
}

RESCUEBOOL RescueDocumentList::AnyFileTruncated()
{
  RESCUEBOOL myReturn = FileTruncated();
  if (myReturn == FALSE)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < count && myReturn == FALSE; loop++)
    {
      myReturn = documents[loop]->AnyFileTruncated();
    }
  }
  return myReturn;
}

void RescueDocumentList::EmptySelf(void)
{
  RESCUEINT64 loop;
 
  for (loop = 0; loop < count; loop++)
  {
    delete documents[loop];
  }
  count = 0;
}



