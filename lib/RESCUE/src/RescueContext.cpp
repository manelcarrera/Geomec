/********************************************************************

  RescueContext.cpp

  Context object for RescueModel.  A separate context object is
  required because of CleanFromUnarchive, which has to operate
  without a RescueModel pointer.

  Rod Hanks,  February 2002

*********************************************************************/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <process.h>
#define getpid _getpid
#define S_IFDIR _S_IFDIR
#else
#include <unistd.h>
#endif
#include <ctype.h>
#include "RescueModel.h"
#ifdef _WIN32
static RESCUECHAR fileSeparator[] = {"\\"};
#define RESCUE_STAT _stat
#else
static RESCUECHAR fileSeparator[] = {"/"};
#define RESCUE_STAT stat
#endif
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#if !defined(WIN32) && !defined(WIN64)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)

RescueContext::RescueContext()
{
  unarchivingModel = 0;
  fileVersion = 0;
  readFileVersion = 0;
  uniqueIDCounter = 1;
  progressReportObject = 0;

  archiveFile = 0;
  delayedPropertyLoad = TRUE;
  delayedGeometryLoad = TRUE;
  vetFileNames = TRUE;
  backupEnabled = TRUE;

  wellboreProperties = 0;      // These objects are used only during
  timeStepGroups = 0;          // relinking.
  properties = 0;
  modelPropertyGroups = 0;
  macroVolumes = 0;
  grids = 0;
  geometries = 0;
  geobodyVolumes = 0;
  geobodyParts = 0;
  blockUnitSides = 0;
  propertyGroups = 0;
  lastError = 0;

  RESCUEINT64 magicNumber = 16909060; // 4 + 256 * 3 + 256^2*2 + 256^3
  char *nP = (char *) &magicNumber;
  littleEndian =  (*nP == 4);

  uintSize = sizeof(RESCUEUINT64);
  intSize = sizeof(RESCUEINT64);
  if (littleEndian)
  {
    uintSwap = TRUE;
    intSwap = TRUE;
  }
  else
  {
    uintSwap = FALSE;
    intSwap = FALSE;
  }
  write32 = TRUE;
}

RescueContext::~RescueContext()
{
  if (lastError != 0)
  {
    delete lastError;
  }
}

void RescueContext::CleanFromUnarchive()
{
  if (unarchivingModel != 0)
  {
    delete unarchivingModel;
  }
  unarchivingModel = 0;
  if (archiveFile != 0)
  {
    fclose(archiveFile);
    archiveFile = 0;
  }
}

void RescueContext::SetError(const char *newError)
{
  if (lastError == 0)
  {
    lastError = new RCHString(newError);
  }
  else
  {
    lastError->AddTo("\n");
    lastError->AddTo(newError);
  }
}

const char *RescueContext::LastError()
{
  const char *myReturn = 0;
  if (lastError != 0)
  {
    myReturn = lastError->String();
  }
  return myReturn;
}

void RescueContext::ClearError()
{
  if (lastError != 0)
  {
    delete lastError;
    lastError = 0;
  }
}

void RescueContext::RescueProgress(const RESCUECHAR *progressDescription)
{
  if (progressReportObject != 0)
  {
    progressReportObject->ReportProgress(progressDescription);
  }
}

RescueProgressReporter *RescueContext::RegisterProgressReporter(RescueProgressReporter *newProgressObject)
{
  RescueProgressReporter *myReturn = progressReportObject;
  progressReportObject = newProgressObject;
  return myReturn;
}

RESCUEINT32 RescueContext::LibraryVersion()
{
  return FILE_VERSION;
}

RESCUEINT32 RescueContext::LibrarySubVersion()
{
  return FILE_SUBVERSION;
}

void RescueContext::VetFileName(RCHString *fileName, RESCUEBOOL creating, RESCUEBOOL desireBinary)
{
  if (vetFileNames)
  {
    struct RESCUE_STAT buf;
    RESCUEBOOL existsAndIsDir = FALSE;
    RESCUEBOOL exists = FALSE;
    RESCUEINT64 result = RESCUE_STAT(fileName->String(), &buf);
    if (result == 0)
    {
      if ((buf.st_mode & S_IFDIR) != 0)
      {
        existsAndIsDir = TRUE;
      }
      else
      {
        exists = TRUE;
      }
    }
    RESCUEBOOL handled = FALSE;
    if (existsAndIsDir == FALSE)
    {
      RESCUEBOOL pleaseContinue = TRUE;
      while (pleaseContinue)
      {
        RESCUECHAR *ext = strrchr(fileName->String(), '.');
        RESCUECHAR *sep = strrchr(fileName->String(), fileSeparator[0]);
        if (ext != 0 && (sep == 0 || sep < ext))
        {
          if (strcmp(ext, ".wireframe") == 0
          ||  strcmp(ext, ".lock") == 0)
          {
            *ext = 0;
          }
          else
          {
            RESCUECHAR *pos = ext + 1;
            RESCUEBOOL isNumber = TRUE;
            while (*pos != 0 && isNumber == TRUE)
            {
              if (isdigit(*pos++) == FALSE)
              {
                isNumber = FALSE;
              }
            }
            if (isNumber == TRUE)
            {
              *ext = 0;
            }
            else
            {
              pleaseContinue = FALSE;
              handled = TRUE;
            }
          }
        }
        else
        {
          pleaseContinue = FALSE;
        }
      }
    }
    if (handled == FALSE)
    {
      RESCUECHAR *sep = strrchr(fileName->String(), fileSeparator[0]);
      if (sep == 0)
      {
        sep = fileName->String();
      }
      else
      {
        sep++;
      }
      sep = _strdup(sep);
      (*fileName) << fileSeparator << sep;
      free(sep);
      if (creating)
      {
        if (desireBinary)
        {
          (*fileName) << ".bin";
        }
        else
        {
          (*fileName) << ".txt";
        }
      }
      else if (exists == FALSE)
      {
        (*fileName) << ".bin";
        result = RESCUE_STAT(fileName->String(), &buf);
        if (result != 0)
        {
          RESCUECHAR *ext = strrchr(fileName->String(), '.');
          *ext = 0;
          (*fileName) << ".txt";
        }
      }
    }
  }
}


RESCUEINT32 RescueContext::FileVersion(RESCUECHAR *pathName)
{
  RCHString vettedFileName = pathName;
  VetFileName(&vettedFileName, FALSE, FALSE);
  RESCUEINT32 fileVersion;
  RESCUEBOOL modelFile = FALSE;
  FILE *archiveFile = (FILE *) fopen(vettedFileName.String(), "rb");
  if (archiveFile != 0)
  {
    RESCUECHAR buffer[50];
    fileVersion = getc(archiveFile);
    if (fileVersion == 'F')
    {
      binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 12, archiveFile);
      if (strncmp(buffer, "ile Version ", 12) == 0)
      {
        RESCUEINT64 aByte = getc(archiveFile);
        RESCUECHAR *pos = buffer;
        while (aByte >= '0' && aByte <= '9')
        {
          *pos++ = (RESCUECHAR) aByte;
          aByte = getc(archiveFile);
        }
        *pos = 0;
        fileVersion = atoi(buffer);
        if (fileVersion >= 4 && fileVersion < 11)
        {
          modelFile = TRUE;
        }
      }
    }
    else if (fileVersion == 'R')
    {
      binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 25, archiveFile);
      if (strncmp(buffer, "escue Model File Version ", 25) == 0)
      {
        RESCUEINT64 aByte = getc(archiveFile);
        RESCUECHAR *pos = buffer;
        while (aByte >= '0' && aByte <= '9')
        {
          *pos++ = (RESCUECHAR) aByte;
          aByte = getc(archiveFile);
        }
        *pos = 0;
        fileVersion = atoi(buffer);
        if (fileVersion >= 12)
        {
          modelFile = TRUE;
        }
      }
    }
    else
    {
      binaryFlag = TRUE;
      if (fileVersion >= 4 && fileVersion < 12)
      {
        modelFile = TRUE;
      }
      else if (fileVersion >= 12)
      {
        fread(buffer, sizeof(RESCUECHAR), 17, archiveFile);
        if (strncmp(buffer, "Rescue Model File", 17) == 0)
        {
          modelFile = TRUE;
        }
      }
    }
/*
  Figure out on our own if the file is binary or not.
*/
    fclose(archiveFile);
  }
  if (modelFile)
  {
    return fileVersion;
  }
  else
  {
    return -1;
  }
}

RESCUEBOOL RescueContext::TypeMarker(RESCUECHAR *pathName, RESCUEUCHAR *outBuf, RESCUEINT64 bufLen)
{
  RESCUEBOOL myReturn = FALSE;
  RCHString vettedFileName = pathName;
  VetFileName(&vettedFileName, FALSE, FALSE);
  RESCUECHAR buffer[91];

  FILE *archiveFile = (FILE *) fopen(vettedFileName.String(), "rb");
  if (archiveFile != 0)
  {
    fseek(archiveFile, -90L, SEEK_END);
    fread(buffer, sizeof(RESCUECHAR), 90, archiveFile);
    buffer[90] = 0;
    RESCUECHAR *pos = buffer;
    while ((pos < buffer + 60) && (strncmp(pos, "written by code version ", 24) != 0))
    {
      pos++;
    }
    RESCUECHAR *spot = strstr(pos, "written by code version ");
    if (spot != 0)
    {
      int versionNo;
      sscanf(spot, "written by code version %d", &versionNo);
      if (versionNo >= 37)
      {
        myReturn = TRUE;
        RESCUEUCHAR *xfer = (RESCUEUCHAR *) (pos - 22);
        if (outBuf != 0)
        {
          if (bufLen > 20)
          {
            bufLen = 20;
          }
          while (bufLen > 0)
          {
            *outBuf++ = *xfer++;
            bufLen--;
          }
        }
      }
    }
    fclose(archiveFile);
  }
  return myReturn;  
}

void RescueContext::SoftwareVersion(RESCUECHAR *pathName, int &fileVersion, int &subVersion)
{
  fileVersion = -1;
  subVersion = 0;
  RCHString vettedFileName = pathName;
  VetFileName(&vettedFileName, FALSE, FALSE);
  RESCUECHAR buffer[61];

  FILE *archiveFile = (FILE *) fopen(vettedFileName.String(), "rb");
  if (archiveFile != 0)
  {
    fseek(archiveFile, -60L, SEEK_END);
    fread(buffer, sizeof(RESCUECHAR), 60, archiveFile);
    buffer[60] = 0;
    RESCUECHAR *pos = buffer;
    while ((pos < buffer + 30) && (strncmp(pos, "written by code version ", 24) != 0))
    {
      pos++;
    }
    RESCUECHAR *spot = strstr(pos, "written by code version ");
    if (spot != 0)
    {
      RESCUEINT64 returns = sscanf(spot, "written by code version %d.%d", &fileVersion, &subVersion);
      if (returns == 1)
      {
        subVersion = 0;
      }
    }
    fclose(archiveFile);
  }
}

RESCUEINT32 RescueContext::SoftwareVersion(RESCUECHAR *pathName)
{
  RESCUEINT32 myReturn = -1;
  RCHString vettedFileName = pathName;
  VetFileName(&vettedFileName, FALSE, FALSE);
  RESCUECHAR buffer[61];

  FILE *archiveFile = (FILE *) fopen(vettedFileName.String(), "rb");
  if (archiveFile != 0)
  {
    fseek(archiveFile, -60L, SEEK_END);
    fread(buffer, sizeof(RESCUECHAR), 60, archiveFile);
    buffer[60] = 0;
    RESCUECHAR *pos = buffer;
    while ((pos < buffer + 30) && (strncmp(pos, "written by code version ", 24) != 0))
    {
      pos++;
    }
    RESCUECHAR *spot = strstr(pos, "written by code version ");
    if (spot != 0)
    {
      sscanf(spot, "written by code version %d", &myReturn);
    }
    fclose(archiveFile);
  }
  return myReturn;
}

RESCUEBOOL RescueContext::FileTruncated(RESCUECHAR *pathName)
{
  RCHString vettedFileName = pathName;
  VetFileName(&vettedFileName, FALSE, FALSE);
  return FileTruncatedPrimitive(vettedFileName.String());
}

RESCUEUINT64 RescueContext::FileSizePrimitive(RESCUECHAR *fileName)
{
  RESCUEUINT64 myReturn = 0;
  struct RESCUE_STAT buf;
  RESCUEINT64 result = RESCUE_STAT(fileName, &buf);
  if (result == 0)
  {
    myReturn = buf.st_size;
  }
  return myReturn;
}

RESCUEBOOL RescueContext::FileTruncatedPrimitive(RESCUECHAR *fileName)
{
  RESCUEBOOL myReturn = TRUE;
  RESCUECHAR buffer[31];

  FILE *archiveFile = (FILE *) fopen(fileName, "rb");
  if (archiveFile != 0)
  {
    fseek(archiveFile, -30L, SEEK_END);
    fread(buffer, sizeof(RESCUECHAR), 30, archiveFile);
    RESCUEINT64 loop;
    for (loop = 0; loop < 9 && myReturn == TRUE; loop++)
    {
      if (strncmp(&buffer[loop], "abracadabra jump jump", 21) == 0)
      {
        myReturn = FALSE;
      }
    }
    fclose(archiveFile);
  }
  return myReturn;
}

RESCUECHAR *RescueContext::GetLock(RESCUECHAR *rescueFileName, RESCUEBOOL needWrite, RESCUEBOOL forceLock)
{
  RCHString message;
  RCHString conflictingFiles;
  RCHString fileName(rescueFileName);
  RCHString lockDirName;
  VetFileName(&fileName, FALSE, FALSE);

  RESCUECHAR *sep = strrchr(fileName.String(), fileSeparator[0]);
  RCHString *lockFileName = new RCHString();
  if (sep == 0)
  {
    lockDirName = "lock";
    (*lockFileName) << lockDirName << fileSeparator << fileName;
  }
  else
  {
    *sep++ = 0;
    lockDirName << fileName.String() << fileSeparator << "lock";
    (*lockFileName) << lockDirName << fileSeparator << sep;
  }
/*
  Put the lock file in it's own directory.
*/
  struct RESCUE_STAT buf;
  RESCUEBOOL existsAndIsDir = FALSE;
  RESCUEINT64 result = RESCUE_STAT(lockDirName.String(), &buf);
  if (result == 0)
  {
    if ((buf.st_mode & S_IFDIR) != 0)
    {
      existsAndIsDir = TRUE;
    }
  }
  if (existsAndIsDir == FALSE)
  {
#ifdef _WIN32
    _mkdir(lockDirName.String());
#else
    mkdir(lockDirName.String(), 0777);
#endif
  }
/*
  Make sure the directory exists.
*/
  (*lockFileName) << ".lock";
  RESCUEINT64 lockNameLen = (*lockFileName).length64();
  RESCUEINT64 myPid = getpid();
  RESCUEBOOL pleaseContinue = TRUE;
#ifdef _WIN32
  RESCUECHAR pathSeparator = '\\';
  RESCUECHAR *lookName = (RESCUECHAR *) malloc((size_t) (lockNameLen + 10));
  strcpy(lookName, (*lockFileName).String());
  strcat(lookName, "*");
  RESCUECHAR *dirName = _strdup((*lockFileName).String());
  RESCUECHAR *end = strrchr(dirName, pathSeparator);
  if (end != 0)
  {
    *end = 0;
  }
  else
  {
    dirName[0] = 0;
  }
  WIN32_FIND_DATAA file_data;
  HANDLE search = FindFirstFileA(lookName, &file_data);
  if (search == INVALID_HANDLE_VALUE)
  {
    pleaseContinue = FALSE;
  }
#else
  RESCUECHAR pathSeparator = '/';
  RESCUECHAR *dirName = _strdup((*lockFileName).String());
  RESCUECHAR *end = strrchr(dirName, pathSeparator);
  if (end != 0)
  {
    *end = 0;
  }
  else
  {
    free(dirName);
    dirName = _strdup(".");
  }
  DIR *current_dir;
  struct dirent *this_entry;
  current_dir = opendir(dirName);
  if (strcmp(dirName, ".") == 0)
  {
    dirName[0] = 0;
  }
  this_entry = readdir(current_dir);
  if (this_entry == 0)
  {
    pleaseContinue = FALSE;
  }
#endif
  while (pleaseContinue == TRUE)
  {
    RCHString thisName = dirName;
    if (thisName.length64() > 0)
    {
      thisName << pathSeparator;
    }
#ifdef _WIN32
    thisName << file_data.cFileName;
#else
    thisName << this_entry->d_name;
#endif
    if (strncmp(thisName.String(), (*lockFileName).String(), (int) lockNameLen) == 0)
    {
      if (needWrite
      ||  thisName.length64() == lockNameLen)
      {
        RESCUEBOOL messageHandled = FALSE;
        conflictingFiles << thisName << "\n";
        FILE *file = fopen(thisName.String(), "r");
        if (file != 0)
        {
          RESCUECHAR line[512];
          RESCUECHAR *fileLine = fgets(line, 512, file);
          if (fileLine != 0)
          {
            message << fileLine;
            messageHandled = TRUE;
          }
          fclose(file);
        }
        if (messageHandled == FALSE)
        {
          message << thisName << "\n";
        }
      }
    }
#ifdef _WIN32
    pleaseContinue = FindNextFileA(search, &file_data);
#else
    this_entry = readdir(current_dir);
    if (this_entry == 0)
    {
      pleaseContinue = FALSE;
    }
#endif
  }
  free(dirName);
#ifdef _WIN32
  free(lookName);
  if (search != INVALID_HANDLE_VALUE)
  {
    FindClose(search);
  }
#else
  closedir(current_dir);
#endif
  if (forceLock == TRUE && conflictingFiles.length64() != 0)
  {
    RESCUEINT64 removeStatus = 0;
    conflictingFiles.tokenize("\n");
    RCHString fileName;
    while ((conflictingFiles >> fileName) == TRUE)
    {
      if (remove(fileName.String()) == -1)
      {
        removeStatus = -1;
      }
    }
    if (removeStatus == 0)
    {
      conflictingFiles = "";
    }
  }
/*
  Remove the offending files.
*/
  if (conflictingFiles.length64() == 0)
  {
    if (needWrite == FALSE)
    {
      (*lockFileName) << "." << myPid;
    }
    FILE *lockFile = fopen((*lockFileName).String(), "w");
    if (lockFile != 0)       
    {
#ifdef _WIN32
      time_t timeNow;
      time(&timeNow);
      fprintf(lockFile, "%lld\t%s\n", myPid, ctime(&timeNow));
#else
      RESCUECHAR timeBuf[255];
      time_t timeNow;
      time(&timeNow);
#ifdef __SUNPRO_CC
      fprintf(lockFile, "%d\t%s\n", myPid, ctime_r(&timeNow, timeBuf, 255));
#else
      fprintf(lockFile, "%lld\t%s\n", myPid, ctime_r(&timeNow, timeBuf));
#endif
#endif
      fclose(lockFile);
      message = "";
    }
    else
    {
      message = "fopen(";
      message << (*lockFileName);
      message << ", \"w\") failed\n";
    }
  }
  if (message.length64() == 0)
  {
    locksAcquired += lockFileName;
    return 0;
  }
  else
  {
    delete lockFileName;
    return _strdup(message.String());
  }
}

RCHString *RescueContext::LockFileForRescueFile(RESCUECHAR *rescueFileName)
{
  RCHString vettedFileName = rescueFileName;
  VetFileName(&vettedFileName, FALSE, FALSE);

  RESCUECHAR *sep = strrchr(vettedFileName.String(), fileSeparator[0]);
  RCHString lockFileName;
  if (sep == 0)
  {
    lockFileName << "lock" << fileSeparator << vettedFileName;
  }
  else
  {
    *sep++ = 0;
    lockFileName << vettedFileName << fileSeparator << "lock" << fileSeparator << sep;
  }

  RESCUEINT64 nameLen = lockFileName.length64();
  RESCUEINT64 howMany = locksAcquired.Count64();
  RCHString *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
    RCHString *thisLock = locksAcquired.NthObject(loop);
    if (strncmp(thisLock->String(), lockFileName.String(), (int) nameLen) == 0)
    {
      myReturn = thisLock;
    }
  }
  return myReturn;
}

void RescueContext::ReleaseLock(RESCUECHAR *rescueFileName)
{
  RCHString *lockFileName = LockFileForRescueFile(rescueFileName);
  if (lockFileName != 0)
  {
    FILE *existingLock = fopen(lockFileName->String(), "r");
    if (existingLock != 0)
    {
      RESCUECHAR *message = (RESCUECHAR *) malloc((size_t) 255);
      fgets(message, 255, existingLock);
      fclose(existingLock);

      int lockPid;
      if (sscanf(message, "%d", &lockPid) == 1)
      {
        if (lockPid == getpid())
        {
          remove(lockFileName->String());
        }
      }
      free(message);
    }
    locksAcquired -= lockFileName;
    delete lockFileName;
  }
}

RESCUEBOOL RescueContext::CheckLock(RESCUECHAR *rescueFileName)
{
  RESCUEBOOL myReturn = FALSE;
  RCHString *lockFileName = LockFileForRescueFile(rescueFileName);
  if (lockFileName != 0)
  {
    FILE *existingLock = fopen(lockFileName->String(), "r");
    if (existingLock != 0)
    {
      RESCUECHAR *message = (RESCUECHAR *) malloc((size_t) 255);
      fgets(message, 255, existingLock);
      fclose(existingLock);

      int lockPid;
      if (sscanf(message, "%d", &lockPid) == 1)
      {
        if (lockPid == getpid())
        {
          myReturn = TRUE;
        }
      }
      free( message );
    }
    if (myReturn == FALSE)
    {
      locksAcquired -= lockFileName;
    }
  }
  return myReturn;
}

void RescueContext::CurrentISOTime(RCHString &buffer)
{
  time_t ltime;

  time( &ltime );
#ifdef _WIN32
  struct tm *tStruct = gmtime(&ltime);
#else
  struct tm tStructBuf;
  struct tm *tStruct;
  tStruct = gmtime_r(&ltime, &tStructBuf);
#endif
  buffer = "YYYY-MM-DD HH:MM:SS Z   ";
  sprintf(buffer.String(), "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d Z",
              (tStruct->tm_year + 1900), (tStruct->tm_mon + 1), tStruct->tm_mday,
              tStruct->tm_hour, tStruct->tm_min, tStruct->tm_sec);
}

void RescueContext::SwapArray(RESCUECHAR *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUECHAR temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}


void RescueContext::SwapArray(RESCUEUCHAR *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUEUCHAR temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}


void RescueContext::SwapArray(RESCUEDOUBLE *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUEDOUBLE temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}


void RescueContext::SwapArray(RESCUEFLOAT *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUEFLOAT temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}


void RescueContext::SwapArray(RESCUEINT32 *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUEINT32 temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}

void RescueContext::SwapArray(RESCUESHORT *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUESHORT temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}

void RescueContext::SwapArray(RESCUEUSHORT *array, RESCUEINT64 arrayLength)
{
  RESCUEINT64 fNdx = 0;
  RESCUEINT64 eNdx = arrayLength - 1;
  while (fNdx < eNdx)
  {
    RESCUEUSHORT temp = array[fNdx];
    array[fNdx] = array[eNdx];
    array[eNdx] = temp;
    fNdx++;
    eNdx--;
  }
}

void RescueContext::SwapAxes(RESCUEFLOAT *array, bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEFLOAT temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx)
    {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++)
      {
        RESCUEFLOAT temp = array[(ifNdx * iNodes) + iLoop];
        array[(ifNdx * iNodes) + iLoop] = array[(efNdx * iNodes) + iLoop];
        array[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueContext::SwapAxes(RESCUEINT32 *array, bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEINT32 temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx)
    {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++)
      {
        RESCUEINT32 temp = array[(ifNdx * iNodes) + iLoop];
        array[(ifNdx * iNodes) + iLoop] = array[(efNdx * iNodes) + iLoop];
        array[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueContext::SwapAxes(RESCUEUSHORT *array, bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEUSHORT temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx)
    {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++)
      {
        RESCUEUSHORT temp = array[(ifNdx * iNodes) + iLoop];
        array[(ifNdx * iNodes) + iLoop] = array[(efNdx * iNodes) + iLoop];
        array[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueContext::SwapAxes(RESCUEDOUBLE *array, bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEDOUBLE temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx)
    {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++)
      {
        RESCUEDOUBLE temp = array[(ifNdx * iNodes) + iLoop];
        array[(ifNdx * iNodes) + iLoop] = array[(efNdx * iNodes) + iLoop];
        array[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueContext::SwapAxes(RESCUEUCHAR *array, bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEUCHAR temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 ifNdx = 0;
    RESCUEINT64 efNdx = jNodes - 1;
    while (ifNdx < efNdx)
    {
      RESCUEINT64 iLoop;
      for (iLoop = 0; iLoop < iNodes; iLoop++)
      {
        RESCUEUCHAR temp = array[(ifNdx * iNodes) + iLoop];
        array[(ifNdx * iNodes) + iLoop] = array[(efNdx * iNodes) + iLoop];
        array[(efNdx * iNodes) + iLoop] = temp;
      }
      ifNdx++;
      efNdx--;
    }
  }
}

void RescueContext::SwapAxes(RESCUEFLOAT *array, bool swapI, RESCUEINT64 iNodes, 
                                                 bool swapJ, RESCUEINT64 jNodes,
                                                 bool swapK, RESCUEINT64 kNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEFLOAT temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++)
    {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEFLOAT temp = array[offset + (ifNdx * iNodes) + iLoop];
          array[offset + (ifNdx * iNodes) + iLoop] = array[offset + (efNdx * iNodes) + iLoop];
          array[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK)
  {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx)
    {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++)
      {
        RESCUEFLOAT temp = array[(kfNdx * iNodes * jNodes) + ijLoop];
        array[(kfNdx * iNodes * jNodes) + ijLoop] = array[(keNdx * iNodes * jNodes) + ijLoop];
        array[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
}


void RescueContext::SwapAxes(RESCUEINT32 *array, bool swapI, RESCUEINT64 iNodes, 
                                                 bool swapJ, RESCUEINT64 jNodes,
                                                 bool swapK, RESCUEINT64 kNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEINT32 temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++)
    {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEINT32 temp = array[offset + (ifNdx * iNodes) + iLoop];
          array[offset + (ifNdx * iNodes) + iLoop] = array[offset + (efNdx * iNodes) + iLoop];
          array[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK)
  {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx)
    {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++)
      {
        RESCUEINT32 temp = array[(kfNdx * iNodes * jNodes) + ijLoop];
        array[(kfNdx * iNodes * jNodes) + ijLoop] = array[(keNdx * iNodes * jNodes) + ijLoop];
        array[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
}


void RescueContext::SwapAxes(RESCUEUSHORT *array, bool swapI, RESCUEINT64 iNodes, 
                                                 bool swapJ, RESCUEINT64 jNodes,
                                                 bool swapK, RESCUEINT64 kNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEUSHORT temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++)
    {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEUSHORT temp = array[offset + (ifNdx * iNodes) + iLoop];
          array[offset + (ifNdx * iNodes) + iLoop] = array[offset + (efNdx * iNodes) + iLoop];
          array[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK)
  {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx)
    {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++)
      {
        RESCUEUSHORT temp = array[(kfNdx * iNodes * jNodes) + ijLoop];
        array[(kfNdx * iNodes * jNodes) + ijLoop] = array[(keNdx * iNodes * jNodes) + ijLoop];
        array[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
}


void RescueContext::SwapAxes(RESCUEDOUBLE *array, bool swapI, RESCUEINT64 iNodes, 
                                                 bool swapJ, RESCUEINT64 jNodes,
                                                 bool swapK, RESCUEINT64 kNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEDOUBLE temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++)
    {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEDOUBLE temp = array[offset + (ifNdx * iNodes) + iLoop];
          array[offset + (ifNdx * iNodes) + iLoop] = array[offset + (efNdx * iNodes) + iLoop];
          array[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK)
  {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx)
    {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++)
      {
        RESCUEDOUBLE temp = array[(kfNdx * iNodes * jNodes) + ijLoop];
        array[(kfNdx * iNodes * jNodes) + ijLoop] = array[(keNdx * iNodes * jNodes) + ijLoop];
        array[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
}


void RescueContext::SwapAxes(RESCUEUCHAR *array, bool swapI, RESCUEINT64 iNodes, 
                                                 bool swapJ, RESCUEINT64 jNodes,
                                                 bool swapK, RESCUEINT64 kNodes)
{
  if (swapI)
  {
    RESCUEINT64 jLoop;
    for (jLoop = 0; jLoop < jNodes * kNodes; jLoop++)
    {
      RESCUEINT64 fNdx = (jLoop * iNodes);
      RESCUEINT64 eNdx = fNdx + (iNodes - 1);
      while (fNdx < eNdx)
      {
        RESCUEUCHAR temp = array[fNdx];
        array[fNdx] = array[eNdx];
        array[eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
  if (swapJ)
  {
    RESCUEINT64 kLoop;
    for (kLoop = 0; kLoop < kNodes; kLoop++)
    {
      RESCUEINT64 offset = kLoop * iNodes * jNodes;
      RESCUEINT64 ifNdx = 0;
      RESCUEINT64 efNdx = (jNodes - 1);
      while (ifNdx < efNdx)
      {
        RESCUEINT64 iLoop;
        for (iLoop = 0; iLoop < iNodes; iLoop++)
        {
          RESCUEUCHAR temp = array[offset + (ifNdx * iNodes) + iLoop];
          array[offset + (ifNdx * iNodes) + iLoop] = array[offset + (efNdx * iNodes) + iLoop];
          array[offset + (efNdx * iNodes) + iLoop] = temp;
        }
        ifNdx++;
        efNdx--;
      }
    }
  }
  if (swapK)
  {
    RESCUEINT64 kfNdx = 0;
    RESCUEINT64 keNdx = kNodes - 1;
    while (kfNdx < keNdx)
    {
      RESCUEINT64 ijLoop;
      for (ijLoop = 0; ijLoop < iNodes * jNodes; ijLoop++)
      {
        RESCUEUCHAR temp = array[(kfNdx * iNodes * jNodes) + ijLoop];
        array[(kfNdx * iNodes * jNodes) + ijLoop] = array[(keNdx * iNodes * jNodes) + ijLoop];
        array[(keNdx * iNodes * jNodes) + ijLoop] = temp;
      }
      kfNdx++;
      keNdx--;
    }
  }
}


RESCUEBOOL RescueContext::SamePoint(RESCUEFLOAT x1, RESCUEFLOAT y1, RESCUEFLOAT z1,
                                    RESCUEFLOAT x2, RESCUEFLOAT y2, RESCUEFLOAT z2)
{
  RESCUEFLOAT r = 0.000001F;
  RESCUEBOOL myReturn = TRUE;
  if (fabs(x1 - x2) > r)
  {
    myReturn = FALSE;
  }
  else if (fabs(y1 - y2) > r)
  {
    myReturn = FALSE;
  }
  else if (fabs(z1 - z2) > r)
  {
    myReturn = FALSE;
  }
  return myReturn;
/*
  No more than a point of departure for discussion.
  1) The r value is purely arbitrary and not related to UOMs.
  2) Not so much of an "Open Ball" as it is an "Open Cube",
     but this way I don't have to do sqrt's.
*/
}

RESCUEINT64* RescueContext::Allocate64For32(RESCUEINT32 *inputArray, RESCUEINT32 cellToCellCount, RESCUEBOOL freeInput)
{
  RESCUEINT64 *myReturn = new RESCUEINT64[cellToCellCount];
  RESCUEINT32 loop;
  for (loop = 0; loop < cellToCellCount; loop++)
  {
    myReturn[loop] = inputArray[loop];
  }
  if (freeInput)
  {
    delete [] inputArray;
  }
  return myReturn;
}

RESCUEINT32* RescueContext::Allocate32For64(RESCUEINT64 *inputArray, RESCUEINT32 cellToCellCount, RESCUEBOOL freeInput, RESCUEBOOL /*throwIfTooBig*/)
{
  RESCUEINT32 *myReturn = 0;
  if (inputArray != 0 && cellToCellCount > 0)
  {
    myReturn = new RESCUEINT32[cellToCellCount];
    RESCUEINT32 loop;
    for (loop = 0; loop < cellToCellCount; loop++)
    {
      RESCUEINT64 output = inputArray[loop];
      if (output > 2147483647 || output < -2147483647)
      {
        throw "Model is too large to be read in 32 bit mode.";
      }
      myReturn[loop] = (RESCUEINT32) output;
    }
    if (freeInput)
    {
      delete [] inputArray;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueContext::Return32For64(RESCUEINT64 output, RESCUEBOOL throwIfTooBig)
{
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}




