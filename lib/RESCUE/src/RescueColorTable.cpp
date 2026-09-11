/********************************************************************

  RescueColorTable.cpp

  A table which specifies how color relates to property values.

  Rod Hanks,  May, 1999

*********************************************************************/
#include "myHeaders.h"
#include "RescueModel.h"
#include "RescueColorTable.h"
#include "RCHString.h"
#include "RescueColor.h"
#include "RescueBuffer.h"
#include <string.h>
#include <malloc.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define COLOR_TABLE_VERSION 1

RescueColorTable::RescueColorTable(RescueModel *parentModelIn, 
                                   RESCUEINT64 probableRows, 
                                   Role roleIn)
                                   :RescueObject(parentModelIn->Context())
                                   ,parentModel(parentModelIn)
{
  isA = R_RescueColorTable;
  rowCount = 0;
  role = roleIn;
  allocated = MAX(probableRows, 10);
  lowColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  highColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  haveLowHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  lowHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  haveHighHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  highHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  hasChanged = FALSE;
  isLoaded = TRUE;
  colorVersion = 0;
}

RescueColorTable::RescueColorTable(RescueColorTable &other):RescueObject(other.parentModel->Context())
{
  isA = R_RescueColorTable;
  if (other.IsLoaded() == FALSE)
  {
    other.Load();
  }
  role = other.role;
  rowCount = other.rowCount;
  allocated = other.allocated;
  lowColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  highColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  haveLowHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  lowHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  haveHighHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  highHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  RESCUEINT64 loop;
  for (loop = 0; loop < rowCount; loop++)
  {
    lowColor[loop] = other.lowColor[loop];
    highColor[loop] = other.highColor[loop];
    haveLowHint[loop] = other.haveLowHint[loop];
    lowHint[loop] = other.lowHint[loop];
    haveHighHint[loop] = other.haveHighHint[loop];
    highHint[loop] = other.highHint[loop];
  }
  hasChanged = TRUE;
  isLoaded = TRUE;
  colorVersion = 0;
}

RescueColorTable::~RescueColorTable()
{
  DropMemory();
}

void RescueColorTable::DropMemory()
{
  RESCUEINT64 loop;
  if (lowColor != 0)
  {
    for (loop = 0; loop < rowCount; loop++)
    {
      if (lowColor[loop] != 0)
      {
        delete lowColor[loop];
      }
    }
    free(lowColor);
    lowColor = 0;
  }
  if (highColor != 0)
  {
    for (loop = 0; loop < rowCount; loop++)
    {
      if (highColor[loop] != 0)
      {
        delete highColor[loop];
      }
    }
    free(highColor);
    highColor = 0;
  }
  if (haveLowHint != 0)
  {
    free(haveLowHint);
    haveLowHint = 0;
  }
  if (lowHint != 0)
  {
    free(lowHint);
    lowHint = 0;
  }
  if (haveHighHint != 0)
  {
    free(haveHighHint);
    haveHighHint = 0;
  }
  if (highHint != 0)
  {
    free(highHint);
    highHint = 0;
  }
  isLoaded = FALSE;
}

RESCUEBOOL RescueColorTable::Load()
{
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
    isLoaded = TRUE;
  }
  return myReturn;
}

RESCUEBOOL RescueColorTable::Unload()
{
  RESCUEBOOL myReturn = FALSE;
  if (hasChanged == FALSE)
  {
    DropMemory();
    myReturn = TRUE;
    isLoaded = FALSE;
  }
  else if (IsLoaded() && parentModel->ExistingModel())
  {
    if (WriteData())
    {
      DropMemory();
      myReturn = TRUE;
      isLoaded = FALSE;
    }
  }
  return myReturn;
}

void RescueColorTable::ArchiveData(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, rowCount);
  RESCUEINT64 loop;
  for (loop = 0; loop < rowCount; loop++)
  {
    if (lowColor[loop] == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      lowColor[loop]->Archive(context, archiveFile);
    }
    if (highColor[loop] == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      highColor[loop]->Archive(context, archiveFile);
    }
    myfprintf(context, archiveFile, haveLowHint[loop]);
    myfprintf(context, archiveFile, lowHint[loop]);
    myfprintf(context, archiveFile, haveHighHint[loop]);
    myfprintf(context, archiveFile, highHint[loop]);
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
  myfprintf(context, archiveFile, "abracadabra jump jump");
}

void RescueColorTable::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueContext *context = parentModel->Context();
  myfscanf(context, archiveFile, &rowCount);
  allocated = rowCount;
  lowColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  highColor = (RescueColor **) malloc(sizeof(RescueColor *) * (size_t) allocated);
  haveLowHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  lowHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  haveHighHint = (RESCUEBOOL *) malloc(sizeof(RESCUEBOOL) * (size_t) allocated);
  highHint = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT *) * (size_t) allocated);
  RESCUEINT64 loop;
  for (loop = 0; loop < rowCount; loop++)
  {
    RESCUEINT64 flag;
    myfscanf(context, archiveFile, &flag);
    if (flag == 0)
    {
      lowColor[loop] = 0;
    }
    else
    {
      lowColor[loop] = new RescueColor(context, archiveFile);
    }
    myfscanf(context, archiveFile, &flag);
    if (flag == 0)
    {
      highColor[loop] = 0;
    }
    else
    {
      highColor[loop] = new RescueColor(context, archiveFile);
    }
    myfscanf(context, archiveFile, &haveLowHint[loop]);
    myfscanf(context, archiveFile, &lowHint[loop]);
    myfscanf(context, archiveFile, &haveHighHint[loop]);
    myfscanf(context, archiveFile, &highHint[loop]);
  }
  isLoaded = TRUE;
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

RESCUEBOOL RescueColorTable::ReadData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly)
{
  RescueContext *context = parentModel->Context();
  parentModel->LoadContext();
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  RCHString fileName;
  fileName << basePathName << "." << Identifier();
  FILE *archiveFile = (FILE *) fopen(fileName.String(), "rb");
  if (archiveFile != 0)
  {
    RESCUEINT64 fileVersion = getc(archiveFile);
    if (fileVersion == 'R')
    {
      context->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 23, archiveFile);
      if (strncmp(buffer, "escue Color Table File", 22) == 0)
      {
        modelFile = TRUE;
      }
    }
    else
    {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 23, archiveFile);
      if (strncmp(buffer, "Rescue Color Table File", 23) == 0)
      {
        modelFile = TRUE;
      }
    }
    fclose(archiveFile);
    if (modelFile)
    {
      int fileVersion = 0;
      if (context->binaryFlag)
      {
        archiveFile = (FILE *) fopen(fileName.String(), "rb");
        fileVersion = getc(archiveFile);
        fseek(archiveFile, 23, SEEK_CUR);
        myfscanf(context, archiveFile, &colorVersion);
        if (versionOnly == FALSE)
        {
          UnArchiveData(archiveFile, fileVersion);
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
      else
      {
        RESCUECHAR myString[255];

        archiveFile = (FILE *) fopen(fileName.String(), "rt");
        myfgets(context, myString, 255, archiveFile);
        sscanf(myString, "Rescue Color Table File Version %d\n", &fileVersion);
        myfscanf(context, archiveFile, &colorVersion);
        if (versionOnly == FALSE)
        {
          UnArchiveData(archiveFile, fileVersion);
        }
        fclose(archiveFile);
        myReturn = TRUE;
      }
    }
    else
    {
      RCHString message;
      message << "Not a Rescue Color Table File:" << fileName.String();
      context->SetError(message.NonNullString());
    }
  }
  return myReturn;
}

RESCUEBOOL RescueColorTable::WriteData()
{
  RescueContext *context = parentModel->Context(true);
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  FILE *archiveFile;
  parentModel->MakeBackupFile(fileName.String());
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
    context->binaryFlag = parentModel->currentBinary;
    if (context->binaryFlag)
    {
      putc((RESCUEUCHAR) COLOR_TABLE_VERSION, archiveFile);
      fwrite("Rescue Color Table File", sizeof(RESCUECHAR), 23, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue Color Table File Version %d\n", COLOR_TABLE_VERSION);
    }
#ifdef TESTING
    myfprintf(context, archiveFile, colorVersion);
#else
    myfprintf(context, archiveFile, ++colorVersion);
#endif
    ArchiveData(archiveFile);
    if (ferror(archiveFile) != 0)
    {
      RCHString message;
      message << "Error writing color table:";
      message << fileName.String();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
      context->SetError(message.NonNullString());
    }
    fclose(archiveFile);
    myReturn = TRUE;
  }
  return myReturn;
}

RESCUEINT32 RescueColorTable::Version(RESCUEBOOL reload)
{
  if (reload)
  {
    RESCUEINT32 myReturn = colorVersion;
    RESCUEINT32 savedColorVersion = colorVersion;
    if (ReadData(parentModel->currentPathName, TRUE))
    {
      myReturn = colorVersion;
      colorVersion = savedColorVersion;
    }
    return myReturn;
  }
  else
  {
    return colorVersion;
  }
}

void RescueColorTable::RescueDeleteFile()
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
                                      
void RescueColorTable::Archive(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, "; Color Table");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (RESCUEINT64) role);
  if (context->FileVersion() > 9)
  {
    if (IsLoaded())
    {
      if (context->delayedPropertyLoad == FALSE
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
      }
    }
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueColorTable::RescueColorTable(RescueContext *context, FILE *archiveFile):RescueObject(context)
{
  isA = R_RescueColorTable;
  isLoaded = FALSE;
  hasChanged = FALSE;
  colorVersion = -1;
  ReadId(context, archiveFile);
  RESCUEINT64 roleFlag;
  myfscanf(context, archiveFile, &roleFlag);
  role = (Role) roleFlag;
  rowCount = 0;
  lowColor = 0;
  highColor = 0;
  haveLowHint = 0;
  lowHint = 0;
  haveHighHint = 0;
  highHint = 0;
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

void RescueColorTable::Relink(RescueObject *parentModelIn)
{
  parentModel = (RescueModel *) parentModelIn;
}

RESCUEBOOL RescueColorTable::FileTruncated()
{
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  return parentModel->Context()->FileTruncatedPrimitive(fileName.String());
}

void RescueColorTable::ResetColorTable()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < rowCount; loop++)
  {
    if (lowColor[loop] != 0)
    {
      delete lowColor[loop];
    }
    if (lowColor[loop] != 0)
    {
      delete highColor[loop];
    }
  }
  rowCount = 0;
}

void RescueColorTable::GetColorRow(RESCUEINT64 zeroBasedOrdinal, 
                                   RescueColor **lowColorBuf, RescueColor **highColorBuf,
                                   RESCUEBOOL &haveLowHintBuf, RESCUEFLOAT &lowHintBuf,
                                   RESCUEBOOL &haveHighHintBuf, RESCUEFLOAT &highHintBuf)
{
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < rowCount)
  {
    *lowColorBuf = lowColor[zeroBasedOrdinal];
    *highColorBuf = highColor[zeroBasedOrdinal];
    haveLowHintBuf = haveLowHint[zeroBasedOrdinal];
    lowHintBuf = lowHint[zeroBasedOrdinal];
    haveHighHintBuf = haveHighHint[zeroBasedOrdinal];
    highHintBuf = highHint[zeroBasedOrdinal];
  }
}

void RescueColorTable::InsureCapacity()
{
  if (rowCount >= allocated)
  {
    allocated += MAX(allocated / 2, 5);
    lowColor = (RescueColor **) realloc(lowColor, sizeof(RescueColor *) * (size_t) allocated);
    highColor = (RescueColor **) realloc(highColor, sizeof(RescueColor *) * (size_t) allocated);
    haveLowHint = (RESCUEBOOL *) realloc(haveLowHint, sizeof(RESCUEBOOL) * (size_t) allocated);
    lowHint = (RESCUEFLOAT *) realloc(lowHint, sizeof(RESCUEFLOAT *) * (size_t) allocated);
    haveHighHint = (RESCUEBOOL *) realloc(haveHighHint, sizeof(RESCUEBOOL) * (size_t) allocated);
    highHint = (RESCUEFLOAT *) realloc(highHint, sizeof(RESCUEFLOAT *) * (size_t) allocated);
  }
}

void RescueColorTable::AddColorRow(RESCUEFLOAT lowValue, RESCUEFLOAT highValue, RescueColor *color)
{
  InsureCapacity();
  lowColor[rowCount] = new RescueColor(*color);
  highColor[rowCount] = 0;
  haveLowHint[rowCount] = TRUE;
  lowHint[rowCount] = lowValue;
  haveHighHint[rowCount] = TRUE;
  highHint[rowCount] = highValue;
  rowCount++;
}

void RescueColorTable::AddColorRow(RESCUEFLOAT lowValue, RESCUEFLOAT highValue, 
                                   RescueColor *colorLow, RescueColor *colorHigh)
{
  InsureCapacity();
  lowColor[rowCount] = new RescueColor(*colorLow);
  highColor[rowCount] = new RescueColor(*colorHigh);
  haveLowHint[rowCount] = TRUE;
  lowHint[rowCount] = lowValue;
  haveHighHint[rowCount] = TRUE;
  highHint[rowCount] = highValue;
  rowCount++;
}

void RescueColorTable::AddColorRow(RESCUEBOOL lowToInfinite, RESCUEFLOAT lowValue, 
                                   RESCUEBOOL highToInfinite, RESCUEFLOAT highValue, 
                                   RescueColor *color)
{
  InsureCapacity();
  lowColor[rowCount] = new RescueColor(*color);
  highColor[rowCount] = 0;
  haveLowHint[rowCount] = (lowToInfinite == TRUE) ? FALSE : TRUE;
  lowHint[rowCount] = lowValue;
  haveHighHint[rowCount] = (highToInfinite == TRUE) ? FALSE : TRUE;
  highHint[rowCount] = highValue;
  rowCount++;
}

void RescueColorTable::AddColorRow(RESCUEBOOL lowToInfinite, RESCUEFLOAT lowValue, 
                                   RESCUEBOOL highToInfinite, RESCUEFLOAT highValue, 
                                   RescueColor *colorLow, RescueColor *colorHigh)
{
  InsureCapacity();
  lowColor[rowCount] = new RescueColor(*colorLow);
  highColor[rowCount] = new RescueColor(*colorHigh);
  haveLowHint[rowCount] = (lowToInfinite == TRUE) ? FALSE : TRUE;
  lowHint[rowCount] = lowValue;
  haveHighHint[rowCount] = (highToInfinite == TRUE) ? FALSE : TRUE;
  highHint[rowCount] = highValue;
  rowCount++;
}

RESCUEINT32 RescueColorTable::ColorRowCount(RESCUEBOOL throwIfTrue)
{
  RESCUEINT64 output = ColorRowCount64();
  if (output > 2147483647)
  {
    if (throwIfTrue)
    {
      throw "Model is too large to be accessed in 32 bit mode.";
    }
    return 0;
  }
  else
  {
    return (RESCUEINT32) output;
  }
}




