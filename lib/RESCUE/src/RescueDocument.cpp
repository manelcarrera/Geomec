/********************************************************************

  RescueDocument.cpp

  A document attached to a RescueModel.

  Rod Hanks,  June, 2007

*********************************************************************/
#include "myHeaders.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "RescueContext.h"
#include "RescueModel.h"
#include "RescueDocumentList.h"
#include "RescueDocument.h"
#include "RescueBuffer.h"
#ifdef _WIN32
#define RESCUE_STAT _stat
#else
#define RESCUE_STAT stat
#endif

RescueDocument::RescueDocument(RescueModel *modelIn):RescueHistoryObject(modelIn->Context())
                          ,model(modelIn)
                          ,documentName(0)
                          ,metaKeywords(0)
                          ,documentLength(0)
{
  isA = R_RescueDocument;
  RescueDocumentList *list = model->Documents();
  list->Add(this);
}

RescueDocument::~RescueDocument()
{
  if (documentName != 0)
  {
  delete documentName;
  }
  if (metaKeywords != 0)
  {
  delete metaKeywords;
  }
}

bool RescueDocument::ImportDocument(const RESCUECHAR *pathName)
{
  bool myReturn = false;
  documentLength = 0;
  FILE *importF = fopen(pathName, "rb");
  if (importF != 0)
  {
  RCHString fileName;
  if (model->oldPathName != 0)
  {
      fileName << model->oldPathName;
  }
  else if (model->currentPathName != 0)
  {
      fileName << model->currentPathName;
  }
  else
  {
      fclose(importF);
      throw "Cannot import documents until the model is archived.";
  }
  fileName << "." << Identifier();
  FILE *file = fopen(fileName.String(), "wb");
  if (file != 0)
  {
      int next = fgetc(importF);
      while (ferror(file) == 0 && ferror(importF) == 0 && feof(importF) == 0)
      {
    fputc(next, file);
    documentLength++;
    next = fgetc(importF);
      }
      myfprintf(model->Context(), file, "abracadabra jump jump");
      if (ferror(file) == 0 && ferror(importF) == 0)
      {
    myReturn = true;
      }
      fclose(file);
  }
  fclose(importF);
  }
  return myReturn;
}

void RescueDocument::SetDocumentName(const RESCUECHAR *documentNameIn)
{
  if (documentNameIn != 0)
  {
  if (documentName == 0)
  {
      documentName = new RCHString(documentNameIn);
  }
  else
  {
      (*documentName) = documentNameIn;
  }
  }
  else if (documentName != 0)
  {
  (*documentName) = "";
  }
}

void RescueDocument::ClearMetaKeywords()
{
  if (metaKeywords != 0)
  {
  delete metaKeywords;
  metaKeywords = 0;
  }
}

void RescueDocument::AddMetaKeyword(const RESCUECHAR *keywordToAdd)
{
  if (metaKeywords == 0)
  {
  metaKeywords = new cSetString();
  }
  metaKeywords->AddIfUnique(keywordToAdd);
}

RCHString *RescueDocument::DocumentName()
{
  return documentName;
}

cSetString *RescueDocument::MetaKeywords()
{
  if (metaKeywords == 0)
  {
  metaKeywords = new cSetString();
  }
  return metaKeywords;
}

RESCUEBOOL RescueDocument::ContainsMetaKey(const RESCUECHAR *keyToFind)
{
  RESCUEBOOL myReturn = FALSE;
  if (metaKeywords != 0)
  {
  RCHString lookup(keyToFind);
  myReturn = metaKeywords->Contains(&lookup);
  }
  return myReturn;
}

RESCUEINT64 RescueDocument::DocumentLength64()
{
  return documentLength;
}

bool RescueDocument::ExportAs(const RESCUECHAR *pathName)
{
  bool myReturn = false;
  FILE *exportF = fopen(pathName, "wb");
  if (exportF != 0)
  {
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
  FILE *file = fopen(fileName.String(), "rb");
  if (file != 0)
  {
      RESCUEINT64 byteCount = documentLength;
      int next = fgetc(file);
      while (ferror(file) == 0 && ferror(exportF) == 0 && feof(file) == 0 && byteCount > 0)
      {
    fputc(next, exportF);
    byteCount--;
    next = fgetc(file);
      }
      if (ferror(file) == 0 && ferror(exportF) == 0 && feof(file) == 0)
      {
    myReturn = true;
      }
      fclose(file);
  }
  fclose(exportF);
  }
  return myReturn;
}

RescueDocument::RescueDocument(RescueDocumentList *list, RescueModel *modelIn, FILE *archiveFile)
                          :RescueHistoryObject(modelIn->Context())
                          ,model(modelIn)
                          ,documentName(0)
                          ,metaKeywords(0)
                          ,documentLength(0)
{
  isA = R_RescueDocument;
  list->Add(this);

  ReadId(model->Context(), archiveFile);
  myfscanf(model->Context(), archiveFile, &documentLength);
  RESCUEINT64 flag;
  myfscanf(model->Context(), archiveFile, &flag);
  if (flag == 1)
  {
  documentName = new RCHString(model->Context(), archiveFile);
  }
  myfscanf(model->Context(), archiveFile, &flag);
  if (flag > 0)
  {
  metaKeywords = new cSetString();
  RESCUEINT64 loop;
  for (loop = 0; loop < flag; loop++)
  {
      RCHString *keyword = new RCHString(model->Context(), archiveFile);
      metaKeywords->AddIfUnique(keyword->String());
      delete keyword;
  }
  }
  {
  RESCUECHAR myString[255];

  myfgets(model->Context(), myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      RescueBuffer buf(model->Context(), archiveFile);
      myfgets(model->Context(), myString, 255, archiveFile);
  }
  }
}

void RescueDocument::Archive(FILE *archiveFile)
{
  myfprintf(model->Context(), archiveFile, Identifier());
  myfprintf(model->Context(), archiveFile, documentLength);
  if (documentName != 0)
  {
  myfprintf(model->Context(), archiveFile, (RESCUEINT64) 1);
  documentName->Archive(model->Context(), archiveFile);
  }
  else
  {
  myfprintf(model->Context(), archiveFile, (RESCUEINT64) 0);
  }
  if (metaKeywords != 0)
  {
  RESCUEINT64 howMany = metaKeywords->Count64();
  myfprintf(model->Context(), archiveFile, howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RCHString *object = metaKeywords->NthObject(loop);
      object->Archive(model->Context(), archiveFile);
  }
  }
  else
  {
  myfprintf(model->Context(), archiveFile, (RESCUEINT64) 0);
  }
  myfprintf(model->Context(), archiveFile, "EOD");
}

void RescueDocument::UniqueMetaKeys(cSetString *toFill)
{
  if (metaKeywords != 0)
  {
  RESCUEINT64 howMany = metaKeywords->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RCHString *object = metaKeywords->NthObject(loop);
      toFill->AddIfUnique(object->String());
  }
  }
}

RESCUEBOOL RescueDocument::AnyFileTruncated()
{
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
  return model->Context()->FileTruncated(fileName.String());
}




