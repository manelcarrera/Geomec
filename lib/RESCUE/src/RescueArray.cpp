/********************************************************************

  RescueArray.cpp

  A dynamically sized array handler for N dimensional grids of
  floating points for RESCUE's data model. The related grid gives
  the dimensionality.

  Rod Hanks,  May 1996

*********************************************************************/
#include <string.h>
#include "RescueArray.h"
#include "RescueModel.h"
#include "RescueArrayFragment.h"
#include "cSetRescueArrayFragment.h"
#include "RescueBuffer.h"

#define PROPERTY_FILE_VERSION 2
// File version for separate property files.
// 1 - Initial version.
// 2 - Compression.

void RescueArray::Relink(RescueObject *parent)
{
  parentModel = (RescueModel *) parent;
  if (parentModel->propertyActionImmediate && IsLoaded() == FALSE)
  {
    Load();
  }
}

void RescueArray::LoadFragment(RescueArrayFragment *toLoad)
{
  if (parentModel->ExistingModel())
  {
    parentModel->Context()->readFileVersion = parentModel->currentFileVersion; 
    parentModel->Context()->readFileMainSoftwareVersion = parentModel->readFileMainSoftwareVersion;
    parentModel->Context()->readFileSubSoftwareVersion = parentModel->readFileSubSoftwareVersion;
    ReadData(parentModel->currentPathName, FALSE, toLoad);
  }
}

RESCUEBOOL RescueArray::Load()
{
  DropFragments();
  RESCUEBOOL myReturn = FALSE;
  if (IsLoaded())
  {
    DropMemory();
  }
  if (parentModel->ExistingModel())
  {
    parentModel->Context()->readFileVersion = parentModel->currentFileVersion; 
    parentModel->Context()->readFileMainSoftwareVersion = parentModel->readFileMainSoftwareVersion;
    parentModel->Context()->readFileSubSoftwareVersion = parentModel->readFileSubSoftwareVersion;
    myReturn = ReadData(parentModel->currentPathName);
  }
  if (myReturn == TRUE)
  {
    hasChanged = FALSE;
  }
  return myReturn;
}

RESCUEBOOL RescueArray::HasFragments()
{
  RESCUEBOOL myReturn = FALSE;
  if (fragments != 0)
  {
    if (fragments->Count64() > 0)
    {
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueArray::Unload()
{
  RESCUEBOOL myReturn = FALSE;
  if (hasChanged == FALSE)
  {
    DropMemory();
    DropFragments();
    myReturn = TRUE;
  }
  else if ((IsLoaded() || HasFragments()) && parentModel->ExistingModel())
  {
    if (WriteData())
    {
      DropMemory();
      DropFragments();
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueArray::Version(RESCUEBOOL reload)
{
  if (reload)
  {
    RESCUEINT32 myReturn = arrayVersion;
    RESCUEINT32 savedArrayVersion = arrayVersion;
    if (ReadData(parentModel->currentPathName, TRUE))
    {
      myReturn = arrayVersion;
      arrayVersion = savedArrayVersion;
    }
    return myReturn;
  }
  else
  {
    return arrayVersion;
  }
}

RESCUEBOOL RescueArray::ReadData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly, RescueArrayFragment *toLoad)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  RCHString fileName;
  fileName << basePathName << "." << Identifier();
  FILE *archiveFile = (FILE *) fopen(fileName.String(), "rb");
  if (archiveFile != 0)
  {
    int fileVersion = getc(archiveFile);
    if (fileVersion == 'R')
    {
      parentModel->Context()->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 20, archiveFile);
      if (strncmp(buffer, "escue Property File", 19) == 0)
      {
        modelFile = TRUE;
      }
    }
    else
    {
      parentModel->Context()->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 20, archiveFile);
      if (strncmp(buffer, "Rescue Property File", 20) == 0)
      {
        modelFile = TRUE;
      }
    }
    fclose(archiveFile);
    if (modelFile)
    {
      RESCUEINT64 fileVersion = 0;
      if (parentModel->Context()->binaryFlag)
      {
        archiveFile = (FILE *) fopen(fileName.String(), "rb");
        fileVersion = getc(archiveFile);
        fseek(archiveFile, 20, SEEK_CUR);
        if (fileVersion >= 2)
        {
          myfscanf(parentModel->Context(), archiveFile, &arrayVersion);
        }
        if (versionOnly == FALSE)
        {
          if (toLoad == 0)
          {
            UnArchiveData(archiveFile, fileVersion);
          }
          else
          {
            toLoad->UnArchiveData(archiveFile, fileVersion);
          }
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
      else
      {
        RESCUECHAR myString[255];

        archiveFile = (FILE *) fopen(fileName.String(), "rt");
        myfgets(parentModel->Context(), myString, 255, archiveFile);
        sscanf(myString, "Rescue Property File Version %lld\n", &fileVersion);
        if (fileVersion >= 2)
        {
          myfscanf(parentModel->Context(), archiveFile, &arrayVersion);
        }
        if (versionOnly == FALSE)
        {
          if (toLoad == 0)
          {
            UnArchiveData(archiveFile, fileVersion);
          }
          else
          {
            toLoad->UnArchiveData(archiveFile, fileVersion);
          }
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
    }
    else
    {
      RCHString message;
      message << "Not a Rescue Property File: " << fileName.String();
      parentModel->Context()->SetError(message.NonNullString());
    }
  }
  else
  {
    RCHString message;
    message << "Unable to open " << fileName.String();
    parentModel->Context()->SetError(message.NonNullString());
  }
  return myReturn;
}

RESCUEBOOL RescueArray::FileTruncated()
{
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  return parentModel->Context()->FileTruncatedPrimitive(fileName.String());
}

RESCUEBOOL RescueArray::WriteData()
{
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  RCHString fragmentFileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  FILE *archiveFile;
  FILE *fragmentFile = 0;
  parentModel->MakeBackupFile(fileName.String());
  int fileVersion = PROPERTY_FILE_VERSION;

  if (HasFragments())
  {
    fragmentFileName = fileName;
    fragmentFile = fopen(fragmentFileName.String(), "r");
    if (fragmentFile == 0)
    {
      fragmentFileName << ".bak";
    }
/*
  If the file does not currently exist, we can use the backup file.
  Probably just means this is the first time we have written the
  property in this transaction.
*/
    else
    {
      if (ferror(fragmentFile) != 0)
      {
        RCHString message;
        message << "Error writing fragment file:";
        message << (RESCUEINT64) ferror(fragmentFile);
        parentModel->Context()->SetError(message.NonNullString());
      }
      fclose(fragmentFile);
      fragmentFileName << ".tmp";
      remove(fragmentFileName.String());
      rename(fileName.String(), fragmentFileName.String());
    }
/*
  If the file does exist, rename it to ".tmp".  We remove any existing
  file to keep nonsignifigant files in the directory from messing up
  our rename.
*/
    parentModel->Context()->binaryFlag = parentModel->currentBinary;
    if (parentModel->Context()->binaryFlag)
    {
      fragmentFile = (FILE *) fopen(fragmentFileName.String(), "rb");
      if (fragmentFile != 0)
      {
        fileVersion = getc(fragmentFile);
        fseek(fragmentFile, 20, SEEK_CUR);
        if (fileVersion >= 2)
        {
          myfscanf(parentModel->Context(), fragmentFile, &arrayVersion);
        }
      }
    }
    else
    {
      RESCUECHAR myString[255];

      fragmentFile = (FILE *) fopen(fragmentFileName.String(), "rt");
      if (fragmentFile != 0)
      {
        myfgets(parentModel->Context(), myString, 255, fragmentFile);
        sscanf(myString, "Rescue Property File Version %d\n", &fileVersion);
        if (fileVersion >= 2)
        {
          myfscanf(parentModel->Context(), fragmentFile, &arrayVersion);
        }
      }
    }
/*
  If folding in fragments, prepare a file channel we can read to get
  the data not in the fragments.
*/
  }

  if (parentModel->currentBinary)
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
    archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
    RescueContext *context = parentModel->Context(true);
    context->binaryFlag = parentModel->currentBinary;
    if (context->binaryFlag)
    {
      putc((RESCUEUCHAR) PROPERTY_FILE_VERSION, archiveFile);
      fwrite("Rescue Property File", sizeof(RESCUECHAR), 20, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Property File Version %d\n", PROPERTY_FILE_VERSION);
    }
#ifdef TESTING
    myfprintf(context, archiveFile, arrayVersion);
#else
    myfprintf(context, archiveFile, ++arrayVersion);
#endif
    ArchiveData(archiveFile, fragmentFile, fileVersion);
    myfprintf(context, archiveFile, "abracadabra jump jump");
    if (ferror(archiveFile) != 0)
    {
      RCHString message;
      message << "Error writing Rescue Property File:";
      message << (RESCUEINT64) ferror(archiveFile);
      parentModel->Context()->SetError(message.NonNullString());
    }
    fclose(archiveFile);
    if (fragmentFile != 0)
    {
      if (ferror(fragmentFile) != 0)
      {
        RCHString message;
        message << "Error writing Rescue Property Fragment File:";
        message << (RESCUEINT64) ferror(fragmentFile);
        context->SetError(message.NonNullString());
      }
      fclose(fragmentFile);
      if (fragmentFileName.EndsWith(".tmp"))
      {
        remove(fragmentFileName.String());
      }
    }
    myReturn = TRUE;
  }
  return myReturn;
}

RescueArray::RescueArray(const RESCUECHAR *propertyNameIn, 
    const RESCUECHAR *propertyTypeIn,
    const RESCUECHAR *unitOfMeasureIn,
                        RescueGrid *existingGrid, 
                        RescueModel *parentModelIn,
                        RESCUEBOOL cellCenteredIn)
                        :RescueObject(parentModelIn->Context())
                        ,propertyName(0)
                        ,propertyType(0)
                        ,unitOfMeasure(0)
                        ,grid(existingGrid)
                        ,cellCentered(cellCenteredIn)
                        ,hasChanged(TRUE)
                        ,dirtyMinMax(TRUE)
                        ,parentModel(parentModelIn)
                        ,arrayVersion(0)
                        ,fragments(0)
{
  propertyName = new RCHString(propertyNameIn);
  propertyType = new RCHString(propertyTypeIn);
  unitOfMeasure = new RCHString(unitOfMeasureIn);
}

RescueArray::~RescueArray()
{
  if (propertyName != 0)
  {
    delete propertyName;
  }
  if (propertyType != 0)
  {
    delete propertyType;
  }
  if (unitOfMeasure != 0)
  {
    delete unitOfMeasure;
  }
  if (fragments != 0)
  {
    delete fragments;
  }
}

void RescueArray::RescueDeleteFile()
{
  if (parentModel != 0)
  {
    if (parentModel->currentPathName != 0)
    {
      if (parentModel->currentPathName[0] != 0)
      {
        RCHString fileName;
        fileName << parentModel->currentPathName << "." << Identifier();
        parentModel->ScheduleDeleteFile(fileName.String());
      }
    }
  }
}

RescueArray::RescueArray(RescueContext *context,
                         FILE *archiveFile)
                            :RescueObject(context)
                            ,grid(0)
                            ,hasChanged(FALSE)
                            ,parentModel(0)
                            ,arrayVersion(-1)
                            ,fragments(0)
{
  RESCUECHAR myString[255];

  isA = R_RescueArray;
  myfgets(context, myString, 255, archiveFile);
  propertyName = new RCHString(myString);
  myfgets(context, myString, 255, archiveFile);
  propertyType = new RCHString(myString);
  myfgets(context, myString, 255, archiveFile);
  unitOfMeasure = new RCHString(myString);
  if (context->readFileVersion >= 5)
  {
    myfscanf(context, archiveFile, &cellCentered);
  }
  else
  {
    cellCentered = FALSE;
  }
  if (context->readFileVersion >= 12)
  {
    ReadId(context, archiveFile);
  }
  hasChanged = false;
}

void RescueArray::CalculateMinMax()
{
  if (IsLoaded())
  {
    int fileVersion = PROPERTY_FILE_VERSION;
    FILE *fragmentFile = 0;
    if (HasFragments())
    {
      RCHString fragmentFileName;
      fragmentFileName << parentModel->currentPathName << "." << Identifier();
      fragmentFile = fopen(fragmentFileName.String(), "r");
      if (fragmentFile == 0)
      {
        fragmentFileName << ".bak";
      }
      else
      {
        if (ferror(fragmentFile) != 0)
        {
          RCHString message;
          message << "Error writing Rescue Property Fragment File:";
          message << (RESCUEINT64) ferror(fragmentFile);
          parentModel->Context()->SetError(message.NonNullString());
        }
        fclose(fragmentFile);
      }
/*
  If the file does not currently exist, we can use the backup file.
  Probably just means this is the first time we have written the
  property in this transaction.
*/
      parentModel->Context()->binaryFlag = parentModel->currentBinary;
      if (parentModel->Context()->binaryFlag)
      {
        fragmentFile = (FILE *) fopen(fragmentFileName.String(), "rb");
        if (fragmentFile != 0)
        {
          fileVersion = getc(fragmentFile);
          fseek(fragmentFile, 20, SEEK_CUR);
          if (fileVersion >= 2)
          {
            myfscanf(parentModel->Context(), fragmentFile, &arrayVersion);
          }
        }
      }
      else
      {
        RESCUECHAR myString[255];

        fragmentFile = (FILE *) fopen(fragmentFileName.String(), "rt");
        if (fragmentFile != 0)
        {
          myfgets(parentModel->Context(), myString, 255, fragmentFile);
          sscanf(myString, "Rescue Property File Version %d\n", &fileVersion);
          if (fileVersion >= 2)
          {
            myfscanf(parentModel->Context(), fragmentFile, &arrayVersion);
          }
        }
      }
/*
  If folding in fragments, prepare a file channel we can read to get
  the data not in the fragments.
*/
    }
    CalculateMinMaxData(fragmentFile, fileVersion);
    if (fragmentFile != 0)
    {
      if (ferror(fragmentFile) != 0)
      {
        RCHString message;
        message << "Error writing Rescue Property Fragment File:";
        message << (RESCUEINT64) ferror(fragmentFile);
        parentModel->Context()->SetError(message.NonNullString());
      }
      fclose(fragmentFile);
    }
    dirtyMinMax = FALSE;
  }
}

void RescueArray::Archive(FILE *archiveFile)
{
  myfprintf(parentModel->Context(), archiveFile, propertyName->String());
  myfprintf(parentModel->Context(), archiveFile, propertyType->String());
  myfprintf(parentModel->Context(), archiveFile, unitOfMeasure->String());
  myfprintf(parentModel->Context(), archiveFile, cellCentered);
  if (parentModel->Context()->fileVersion > 9)
  {
    myfprintf(parentModel->Context(), archiveFile, Identifier());
    if (parentModel->Context()->fileVersion >= 37)
    {
      myfprintf(parentModel->Context(), archiveFile, "dirtyMinMax");
      RescueBuffer buf(parentModel->Context(), sizeof(dirtyMinMax) + 5);
      buf << dirtyMinMax;
      buf.Archive(archiveFile);
      myfprintf(parentModel->Context(), archiveFile, "minMax");
      WriteMinMax(parentModel->Context(), archiveFile);
      myfprintf(parentModel->Context(), archiveFile, "EOD");
    }
    if (IsLoaded())
    {
      if (parentModel->Context()->delayedPropertyLoad == FALSE
      ||  hasChanged == TRUE
      ||  parentModel->propertyActionImmediate == TRUE)
      {
        WriteData();
      }
    }
    else if (parentModel->propertyActionImmediate)
    {
      if (ReadData(parentModel->oldPathName))
      {
        WriteData();
        DropMemory();
        DropFragments();
      }
    }
  }
}

void RescueArray::DropFragments()
{
  if (fragments != 0)
  {
    delete fragments;
    fragments = 0;
  }
}

void RescueArray::DemandFragments()
{
  if (fragments == 0)
  {
    fragments = new cSetRescueArrayFragment();
  }
}

RESCUEBOOL RescueArray::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueArray)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}





