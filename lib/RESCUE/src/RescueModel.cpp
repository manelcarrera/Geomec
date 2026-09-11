/********************************************************************

  RescueModel.cpp

  The main object for RESCUE's data model.

  Rod Hanks,  May 1996       

*********************************************************************/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include "RescueCoordinateSystem.h"
#include "cNameValuePair.h"
#include "RescueModelPropertyGroup.h"
#include "RescueIdTree.h"
#include "RescueColorTableList.h"
#include "RescueGeobody.h"
#include "RescueFaultIntersection.h"
#include "RescueReservoir.h"
#include "RescueWireframe.h"
#include "RescueGeobodyPart.h"
#include "RescueLogicalOrder.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include "RescueOrientationLedger.h"
#include "RescueHistory.h"
#include "RCHXMLTokenizer.h"
#include "RescueWellboreProperty.h"
#include "RescueArrayFloat.h"
#include "RescueVertex.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#if !defined(WIN32) && !defined(WIN64)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)

void RescueModel::DeletePropertyGroup(RescuePropertyGroup *existingPropertyGroup)
{
  RESCUEINT64 ndx = 0;
  RescueModelPropertyGroup *group = propertyGroup->NthObject(ndx++);
  while (group != 0)
  {
  group->RemoveGroup(existingPropertyGroup);
  group = propertyGroup->NthObject(ndx++);
  }
}

RescueWireframeCallBack *RescueModel::RegisterWireframeCallBack(RescueWireframeCallBack *newCallBackObject)
{
  RescueWireframeCallBack *myReturn = wireframeCallBackObject;
  wireframeCallBackObject = newCallBackObject;
  return myReturn;
}

void RescueModel::CommonInitialization(RescueContext *contextIn, const RESCUECHAR *nameIn)
{
  context = contextIn;
  context->uniqueIDCounter = 1;

  isA = R_RescueModel;
  modelMoniker = new RCHString(nameIn);
  units = new cSetRescueUnit();
  blocks = new cSetRescueBlock();
  horizons = new cSetRescueHorizon();
  wellbores = new cSetRescueWellbore();
  sections = new cSetRescueSection();
  referenceSurfaces = new cSetRescueReferenceSurface();
  lookups = new cSetRescueLookup();
  nameValuePairs = new cNameValuePair();
  propertyGroup = new cSetRescueModelPropertyGroup();
  history = new cSetRescueHistory();
  geoBodies = new cSetRescueGeobody();
  eventDescr = new cSetRescueEventDescr();
  faultIntersections = new cSetRescueFaultIntersection();
  reservoirs = new cSetRescueReservoir();
  wireframes = new cSetRescueWireframe();
  gridGeometries = new cSetRescueGeometry();
  localGroups = new cSetRescuePropertyGroup();

  currentPathName = 0;
  oldPathName = 0;
  currentBinary = FALSE;
  currentFileVersion = FILE_VERSION;
  desiredFileVersion = FILE_VERSION;

  wireframeLoaded = TRUE;
  wireframeHasChanged = TRUE;
  propertyActionImmediate = FALSE;
  geometryActionImmediate = FALSE;

  mainFileVersion = 0;
  wireframeVersion = 0;

  registeredObjects = new RescueTree();
  if (coordinateSystem != 0)
  {
  coordinateSystem->RegisterWith(this);
  }
  uintSize = sizeof(RESCUEUINT64);
  if (context->LittleEndian())
  {
  uintSwap = TRUE;
  }
  else
  {
  uintSwap = FALSE;
  }
  intSize = sizeof(RESCUEINT64);
  if (context->LittleEndian())
  {
  intSwap = TRUE;
  }
  else
  {
  intSwap = FALSE;
  }
  write32 = TRUE;
}

void RescueModel::RegisterObject(RescueObject *toRegister)
{
  registeredObjects->Add(toRegister);
}

void RescueModel::UnRegisterObject(RescueObject *toUnregister)
{
  if (registeredObjects != 0)
  {
  registeredObjects->Delete(toUnregister);
  }
}

RescueModel::RescueModel(RescueContext *contextIn, RESCUECHAR *modelNameIn)
          :RescueHistoryObject(contextIn)
          ,modelMoniker(0)
          ,coordinateSystem(0)
          ,units(0)
          ,blocks(0)
          ,horizons(0)
          ,wellbores(0)
          ,sections(0)
          ,referenceSurfaces(0)
          ,lookups(0)
          ,nameValuePairs(0)
          ,logicalOrder(0)
          ,wireframeCallBackObject(0)
          ,rotation((RESCUEFLOAT) 0)
          ,properties(0)
          ,propertyContainerId(0)
          ,documents(0)
          ,documentListId(0)
{
  CommonInitialization(contextIn, modelNameIn);
  colorTableList = new RescueColorTableList(context);
  wireframeLoaded = TRUE;
}


RescueModel::RescueModel(RESCUECHAR *modelNameIn,
                         RescueCoordinateSystem *coordinateSystemIn)
                   :RescueHistoryObject(coordinateSystemIn->Context())
          ,modelMoniker(0)
          ,coordinateSystem(coordinateSystemIn)
          ,units(0)
          ,blocks(0)
          ,horizons(0)
          ,wellbores(0)
          ,sections(0)
          ,referenceSurfaces(0)
          ,lookups(0)
          ,nameValuePairs(0)
          ,logicalOrder(0)
          ,wireframeCallBackObject(0)
          ,rotation((RESCUEFLOAT) 0)
          ,properties(0)
          ,propertyContainerId(0)
          ,documents(0)
          ,documentListId(0)
{
  CommonInitialization(coordinateSystemIn->Context(), modelNameIn);
  colorTableList = new RescueColorTableList(context);
  wireframeLoaded = TRUE;
}

RescueModel::~RescueModel()
{
  if (gridGeometries != 0)
  {
  delete gridGeometries;
  }
  if (blocks != 0)
  {
  blocks->PrepareModelDelete();
  }
  if (modelMoniker != 0)
  {
  delete modelMoniker;
  }
  if (units != 0)
  {
  delete units;
  }
  if (blocks != 0)
  {
  delete blocks;
  }
  if (wellbores != 0)
  {
  delete wellbores;
  }
  if (horizons != 0)
  {
  delete horizons;
  }
  if (sections != 0)
  {
  delete sections;
  }
  if (referenceSurfaces != 0)
  {
  delete referenceSurfaces;
  }
  if (lookups != 0)
  {
  delete lookups;
  }
  if (nameValuePairs != 0)
  {
  delete nameValuePairs;
  }
  if (currentPathName != 0)
  {
  free(currentPathName);
  }
  if (oldPathName != 0)
  {
  free(oldPathName);
  }
  if (propertyGroup != 0)
  {
  delete propertyGroup;
  }
  if (registeredObjects != 0)
  {
  RescueObject *object = (RescueObject *) registeredObjects->NthObject(0);
  while (object != 0)
  {
      delete object;
      object = (RescueObject *) registeredObjects->NthObject(0);
  }
  delete registeredObjects;
  }
  if (history != 0)
  {
  delete history;
  }
  if (colorTableList != 0)
  {
  delete colorTableList;
  }
  if (geoBodies != 0)
  {
  delete geoBodies;
  }
  if (eventDescr != 0)
  {
  delete eventDescr;
  }
  if (faultIntersections != 0)
  {
  delete faultIntersections;
  }
  if (reservoirs != 0)
  {
  delete reservoirs;
  }
  if (localGroups != 0)
  {
  delete localGroups;
  }
  if (wireframes != 0)
  {
  delete wireframes;
  }
  if (logicalOrder != 0)
  {
  delete logicalOrder;
  }
  if (properties != 0)
  {
  delete properties;
  }
}

RescueCoordinateSystem *RescueModel::CoordinateSystemIdentifiedBy(RESCUEINT64 identifier)
{
  RescueCoordinateSystem *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueObject *object = (RescueObject *) registeredObjects->NthObject(ordinal++);
  while (myReturn == 0 && object != 0)
  {
  if (object->Identifier() == identifier)
  {
      myReturn = (RescueCoordinateSystem *) object;
  }
  else
  {
      object = (RescueObject *) registeredObjects->NthObject(ordinal++);
  }
  }
  return myReturn;
}

RescueBlockUnit *RescueModel::BlockUnitIdentifiedBy(RESCUEINT64 identifier)
{
   RescueBlockUnit *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < blocks->Count64() && myReturn == 0; loop++)
   {
     RescueBlock *block = blocks->NthObject(loop);
     if (block)
     {
       myReturn = block->BlockUnitIdentifiedBy(identifier);
     }
   }

   return myReturn;
}

RescueTriangulatedSurface *RescueModel::TriangulatedSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
  RescueTriangulatedSurface *myReturn = HorizonTriangulatedSurfaceIdentifiedBy(identifier);
  if (myReturn == 0)
  {
  myReturn = SectionTriangulatedSurfaceIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueIJSurface *RescueModel::IJSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
  RescueIJSurface *myReturn = HorizonIJSurfaceIdentifiedBy(identifier);
  if (myReturn == 0)
  {
  myReturn = SectionIJSurfaceIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueSurface *RescueModel::SurfaceIdentifiedBy(RESCUEINT64 identifier)
{
  RescueSurface *myReturn = HorizonSurfaceIdentifiedBy(identifier);
  if (myReturn == 0)
  {
  myReturn = SectionIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueGeobodyVolume *RescueModel::GeobodyVolumeIdentifiedBy(RESCUEINT64 identifier)
{
  RescueGeobodyVolume *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < geoBodies->Count64() && myReturn == 0; loop++)
  {
  RescueGeobody *body = geoBodies->NthObject(loop);
  myReturn = body->GeobodyVolumeIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueMacroVolume *RescueModel::MacroVolumeIdentifiedBy(RESCUEINT64 identifier)
{
  RescueMacroVolume *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < blocks->Count64() && myReturn == 0; loop++)
  {
  RescueBlock *block = blocks->NthObject(loop);
  myReturn = block->MacroVolumeIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueBlockUnitSide *RescueModel::BlockUnitSideIdentifiedBy(RESCUEINT64 identifier)
{
  RescueBlockUnitSide *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < blocks->Count64() && myReturn == 0; loop++)
  {
  RescueBlock *block = blocks->NthObject(loop);
  myReturn = block->BlockUnitSideIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueGeobodySurface *RescueModel::GeobodySurfaceIdentifiedBy(RESCUEINT64 identifier)
{
  RescueGeobodySurface *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < geoBodies->Count64() && myReturn == 0; loop++)
  {
  RescueGeobody *body = geoBodies->NthObject(loop);
  myReturn = body->GeobodySurfaceIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueBlockUnitHorizonSurface *RescueModel::HorizonSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
   RescueBlockUnitHorizonSurface *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < horizons->Count64() && myReturn == 0; loop++)
   {
     RescueHorizon *horizon = horizons->NthObject(loop);
     myReturn = horizon->HorizonSurfaceIdentifiedBy(identifier);
   }

   return myReturn;
}

RescueTriangulatedSurface *RescueModel::HorizonTriangulatedSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
   RescueTriangulatedSurface *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < horizons->Count64() && myReturn == 0; loop++)
   {
     RescueHorizon *horizon = horizons->NthObject(loop);
     myReturn = horizon->SurfaceTriangulatedIdentifiedBy(identifier);
   }

   return myReturn;
}

RescueTriangulatedSurface *RescueModel::SectionTriangulatedSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
   RescueTriangulatedSurface *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < sections->Count64() && myReturn == 0; loop++)
   {
     RescueSection *section = sections->NthObject(loop);
     myReturn = section->SurfaceTriangulatedIdentifiedBy(identifier);
   }

   return myReturn;
}

RescueIJSurface *RescueModel::HorizonIJSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
   RescueIJSurface *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < horizons->Count64() && myReturn == 0; loop++)
   {
     RescueHorizon *horizon = horizons->NthObject(loop);
     myReturn = horizon->SurfaceIJIdentifiedBy(identifier);
   }

   return myReturn;
}

RescueIJSurface *RescueModel::SectionIJSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
   RescueIJSurface *myReturn = 0;
   RESCUEINT64 loop;
   for (loop = 0; loop < sections->Count64() && myReturn == 0; loop++)
   {
     RescueSection *section = sections->NthObject(loop);
     myReturn = section->SurfaceIJIdentifiedBy(identifier);
   }

   return myReturn;
}

RescueReferenceSurface *RescueModel::ReferenceSurfaceIdentifiedBy(RESCUEINT64 identifier)
{
  RescueReferenceSurface *myReturn = referenceSurfaces->ObjectIdentifiedBy(identifier);
  if (myReturn == 0)
  {
  myReturn = HorizonSurfaceIdentifiedBy(identifier);
  }
  return myReturn;
}

void RescueModel::MakeBackupFile(RESCUECHAR *fileName)
{
  if (context->backupEnabled == TRUE)
  {
  RCHString backupName = fileName;
  backupName << ".bak";
  FILE *testFile1 = fopen(backupName.String(), "r");
  if (testFile1 == 0)
  {
      if (rename(fileName, backupName.String()) != 0)
      {
    filesCreated += new RCHString(fileName);
      }
/*
  If no current bak file, rename existing to bak.  If
  that fails, assume it is because existing file in
  fact does not exist.  In that case, remember we
  created it.
*/
  }
  else
  {
      fclose(testFile1);
  }
/*
  Can't count on rename to fail if backup file exists.
  Some platforms replace silently.  If we are able to
  open the bak file it means this is a subsequent time we
  have written to the file during this transaction.  Just
  ignore the whole thing.
*/
  }
}

void RescueModel::ScheduleDeleteFile(RESCUECHAR *fileName)
{
  RCHString *filePackage = new RCHString(fileName);
  filesDeleted += filePackage;
}

RESCUEBOOL RescueModel::ArchiveModel()
{
  if (ExistingModel())
  {
  propertyActionImmediate = FALSE;
  geometryActionImmediate = FALSE;
  context->fileVersion = FILE_VERSION;
  desiredFileVersion = FILE_VERSION;
  context->readFileVersion = currentFileVersion;
  RESCUEBOOL needRewrite = (currentFileVersion != context->fileVersion);
  if (needRewrite)
  {
      propertyActionImmediate = TRUE;
      geometryActionImmediate = TRUE;
      oldPathName = currentPathName;
  }
/*
  Because subsidiary files may also change format when we change
  versions, we need to re-write all files when changing versions.
*/
  RESCUEBOOL myReturn = ArchiveModelPrimitive(currentPathName, currentBinary);
  if (myReturn)
  {
      currentFileVersion = desiredFileVersion;
  }
  if (myReturn)
  {
      if (context->backupEnabled == FALSE)
      {
    RESCUEINT64 howMany = filesDeleted.Count64();
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++)
    {
          RCHString *thisFile = filesDeleted.NthObject(loop);
          remove(thisFile->String());
    }
    filesDeleted.EmptySelf();
      }
  }
  if (needRewrite)
  {
      oldPathName = 0;
  }
  return myReturn;
  }
  else
  {
  context->SetError("Use ArchiveModel method with a file name.");
  return FALSE;
  }
}

RESCUEBOOL RescueModel::ArchiveModel(RESCUECHAR *pathName, RESCUEBOOL binary, RESCUEINT32 fileVersionIn, RESCUEBOOL write32in)
{
  if (fileVersionIn != 9
  &&  fileVersionIn < 19)
  {
  return FALSE;
  }
  else
  {
  RCHString vettedFileName = pathName;
  context->VetFileName(&vettedFileName, TRUE, binary);
  context->fileVersion = fileVersionIn;
  desiredFileVersion = fileVersionIn;
  context->readFileVersion = currentFileVersion;
  currentBinary = binary;
  propertyActionImmediate = FALSE;
  geometryActionImmediate = FALSE;
  if (currentPathName != 0)
  {
      if (strcmp(vettedFileName.String(), currentPathName) != 0
      ||  context->fileVersion != currentFileVersion)
      {
    propertyActionImmediate = TRUE;
    geometryActionImmediate = TRUE;
    if (oldPathName != 0)
    {
          free(oldPathName);
    }
    oldPathName = currentPathName;
    currentPathName = _strdup(vettedFileName.String());
    write32 = write32in;
    context->write32 = write32;
    if (write32)
    {
          Set32();
    }
    else
    {
          Set64();
    }
      }
/*
  Note we are not paying attention to the user's write32 argument if they already have
  a file and they are not changing the name.  They could end up with half a file in
  one condition and half in another.
*/
  }
  else
  {
      oldPathName = 0;
      currentPathName = _strdup(vettedFileName.String());
      currentBinary = binary;
      write32 = write32in;
      context->write32 = write32;
      if (write32)
      {
    Set32();
      }
      else
      {
    Set64();
      }
  }
  RESCUEBOOL myReturn = ArchiveModelPrimitive(currentPathName, currentBinary);
  if (myReturn)
  {
      currentFileVersion = desiredFileVersion;
  }
  return myReturn;
  }
}

RESCUEBOOL RescueModel::ArchiveModelPrimitive(RESCUECHAR *pathName, RESCUEBOOL binary)
{
  RESCUEBOOL myReturn = FALSE;
  FILE *archiveFile;
  
  MakeBackupFile(pathName);
  if (binary)
  {
  archiveFile = (FILE *) fopen(pathName, "wb");
  }
  else
  {
  archiveFile = (FILE *) fopen(pathName, "wt");
  }
  context->binaryFlag = binary;
  if (archiveFile != NULL)
  {
  Archive(archiveFile);
  if (ferror(archiveFile) == 0)
  {
      myReturn = TRUE;
  }
  else
  {
      RCHString message;
      message << "Error writing to file:";
      message << pathName;
      message << "( ";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
      context->SetError(message.NonNullString());
  }
  WriteEOFMarks(archiveFile);
  }
  else
  {
  RCHString message(100);
  message << "Unable to open " << pathName;
  context->SetError(message.NonNullString());
  }
  return myReturn;
}

void RescueModel::WriteEOFMarks(FILE *archiveFile)
{

  RESCUEUINT64 typeMarker = 16909060; // 4 + 256 * 3 + 256^2*2 + 256^3
  myfprintf(context, archiveFile, typeMarker);
  RESCUEINT64 markerSize = 10 - (context->Write32() ? sizeof(RESCUEUINT32) : sizeof(RESCUEUINT64));
  while (markerSize != 0)
  {
  myfprintf(context, archiveFile, (RESCUEUCHAR) 255);
  markerSize--;
  }
  RESCUEINT64 typeMarker2 = 16909060; // 4 + 256 * 3 + 256^2*2 + 256^3
  myfprintf(context, archiveFile, typeMarker2);
  markerSize = 10 - (context->Write32() ? sizeof(RESCUEINT32) : sizeof(RESCUEINT64));
  while (markerSize != 0)
  {
  myfprintf(context, archiveFile, (RESCUEUCHAR) 255);
  markerSize--;
  }

  RESCUECHAR buffer[60];
  sprintf(buffer, "written by code version %d.%d ", FILE_VERSION, FILE_SUBVERSION);
  myfprintf(context, archiveFile, buffer);
  myfprintf(context, archiveFile, "abracadabra jump jump");

  if (ferror(archiveFile) != 0)
  {
  RCHString message;
  message << "Error writing to main file: (";
  message << (RESCUEINT64) ferror(archiveFile);
  message << ")";
  context->SetError(message.NonNullString());
  }

  fclose(archiveFile);
}

RescueLogicalOrder *RescueModel::LogicalOrder()
{
  if (logicalOrder == 0)
  {
  logicalOrder = new RescueLogicalOrder(context);
  }
  return logicalOrder;
}

RescueEventDescr *RescueModel::MakeUnconformity(RESCUECHAR *name)
{
  RescueEventDescr *myReturn = new RescueEventDescr(Context(), name, 
                          RescueEventDescr::UNCONFORMITY);
  (*eventDescr) += myReturn;
  return myReturn;
}

RescueEventDescr *RescueModel::MakeDiapir(RESCUECHAR *name )
{
  RescueEventDescr *myReturn = new RescueEventDescr(Context(), name, 
                          RescueEventDescr::UNCONFORMITY);
  (*eventDescr) += myReturn;
  return myReturn;
}

RESCUEBOOL RescueModel::DropRescueGeobody(RescueGeobody *bodyToDrop)
{
  bodyToDrop->Dispose();
  return ((*geoBodies) -= bodyToDrop);
}

RESCUEBOOL RescueModel::DropRescueHorizon(RescueHorizon *horizonToDrop)
{
  horizonToDrop->Dispose();
  return ((*horizons) -= horizonToDrop);
}

RESCUEBOOL RescueModel::DropRescueEventDescr(RescueEventDescr *eventDescrToDrop)
{
  return ((*eventDescr) -= eventDescrToDrop);
}

RESCUEBOOL RescueModel::DropRescueFaultIntersection(RescueFaultIntersection *intersectionToDrop)
{
  return ((*faultIntersections) -= intersectionToDrop);
}

RESCUEBOOL RescueModel::DropRescueReservoir(RescueReservoir *reservoirToDrop)
{
  return ((*reservoirs) -= reservoirToDrop);
}

RESCUEBOOL RescueModel::DropRescueWireframe(RescueWireframe *wireframeToDrop)
{
  return ((*wireframes) -= wireframeToDrop);
}

RESCUEBOOL RescueModel::DropRescueReferenceSurface(RescueReferenceSurface *surfaceToDrop)
{
  surfaceToDrop->Dispose();
  return ((*referenceSurfaces) -= surfaceToDrop);
}

RESCUEBOOL RescueModel::DropRescueSection(RescueSection *sectionToDrop)
{
  sectionToDrop->Dispose();
  return ((*sections) -= sectionToDrop);
}

RESCUEBOOL RescueModel::DropRescueWellbore(RescueWellbore *wellboreToDrop)
{
  wellboreToDrop->Dispose();
  return ((*wellbores) -= wellboreToDrop);
}

RescueTrimVertex *RescueModel::TrimVertexLocatedBy(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  RescueTrimVertex *myReturn = 0;
  RESCUEINT64 howMany = wireframes->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
  RescueWireframe *wireframe = wireframes->NthObject(loop);
  myReturn = wireframe->TrimVertexLocatedBy(x, y, z);
  }
  return myReturn;
}

RescuePolyLine *RescueModel::PolyLineBetween(RescueTrimVertex *end1, RescueTrimVertex *end2)
{
  RescuePolyLine *myReturn = 0;
  RESCUEINT64 howMany = wireframes->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
  RescueWireframe *wireframe = wireframes->NthObject(loop);
  myReturn = wireframe->PolyLineBetween(end1, end2);
  }
  return myReturn;
}

void RescueModel::Archive(FILE *archiveFile)
{
  RESCUEBOOL localBinary = context->binaryFlag;
  RESCUEBOOL loadedNow = IsWireframeLoaded();
  if (context->fileVersion == 9)
  {
  if (loadedNow == FALSE)
  {
      ReadWireframeData(oldPathName);
  }
  RESCUEINT64 howMany = wireframes->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      if (wireframes->NthObject(loop)->IsWireframeLoaded() == FALSE)
      {
    wireframes->NthObject(loop)->ReadWireframeData(oldPathName);
      }
  }
  }
  context->binaryFlag = localBinary;
  if (context->binaryFlag)
  {
  if (context->fileVersion == 9)
  {
      putc((RESCUEUCHAR) context->fileVersion, archiveFile);
  }
  else
  {
      putc((RESCUEUCHAR) context->fileVersion, archiveFile);
      fwrite("Rescue Model File", sizeof(RESCUECHAR), 17, archiveFile);
  }
  }
  else
  {
  if (context->fileVersion == 9)
  {
      fprintf(archiveFile, "File Version %d\n", context->fileVersion);
  }
  else
  {
      fprintf(archiveFile, "Rescue Model File Version %d\n", context->fileVersion);
  }
  }
  if (context->fileVersion > 9)
  {
#ifdef TESTING
  myfprintf(context, archiveFile, mainFileVersion);
#else
  myfprintf(context, archiveFile, ++mainFileVersion);
#endif
  }
  myfprintf(context, archiveFile, modelMoniker->String());
  context->RescueProgress("Coordinate System");
  coordinateSystem->Archive(archiveFile);
  units->Archive(context, archiveFile);
  blocks->Archive(context, archiveFile);
  horizons->Archive(context, archiveFile);
  wellbores->Archive(context, archiveFile);
  sections->Archive(context, archiveFile);
  if (context->fileVersion == 9)
  {
  wireframes->ArchiveCompatiblePolyLines(context, archiveFile);
  wireframes->ArchiveCompatibleTrimVertexes(context, archiveFile);
  if (loadedNow == FALSE)
  {
      DropWireframeMemory();
  }
  }
  referenceSurfaces->Archive(context, archiveFile);
  context->RescueProgress("Lookup Tables");
  lookups->Archive(context, archiveFile);
  context->RescueProgress("Name/Value Pairs");
  nameValuePairs->Archive(context, archiveFile);

  if (context->fileVersion > 9)
  {
  propertyGroup->Archive(context, archiveFile);
  if (IsWireframeLoaded())
  {
      if (context->delayedGeometryLoad == FALSE
      ||  WireframeHasChanged() == TRUE
      ||  geometryActionImmediate == TRUE)
      {
    if (context->fileVersion >= 28)
    {
          wireframes->UpdateWireframeData();
    }
      }
      WriteWireframeData();
  }
  else if (geometryActionImmediate)
  {
      if (ReadWireframeData(oldPathName))
      {
    if (context->fileVersion >= 28)
    {
          wireframes->CopyWireframeData(oldPathName);
          wireframes->DropWireframeMemory();
    }
    WriteWireframeData();
    DropWireframeMemory();
      }
  }
  if (context->fileVersion >= 20)
  {
      history->Archive(context, archiveFile);
      colorTableList->Archive(context, archiveFile);
      geoBodies->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 24)
  {
      eventDescr->Archive(this, context, archiveFile);
      faultIntersections->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 25)
  {
      myfprintf(context, archiveFile, rotation);
  }
  if (context->fileVersion >= 28)
  {
      gridGeometries->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 29)
  {
      localGroups->Archive(context, archiveFile);
      reservoirs->Archive(context, archiveFile);
  }
  if (context->fileVersion >= 30)
  {
      LogicalOrder()->Archive(context, archiveFile);
  }
  }
  if (context->FileVersion() >= 37)
  {
  if (properties == 0 
  &&  propertyContainerId != 0 
  &&  this->propertyActionImmediate == TRUE)
  {
      properties = new cSetRescueDataContainer(this, propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << properties->Identifier();
      buf1.Archive(archiveFile);

      properties->Archive(); // Goes into it's own file.
  }
  else if (properties == 0 && propertyContainerId != 0)
  {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << propertyContainerId;
      buf1.Archive(archiveFile);
  }
  
  if (documents == 0 
  &&  documentListId != 0 
  &&  this->propertyActionImmediate == TRUE)
  {
      documents = new RescueDocumentList(this, documentListId);  // Will Read.
  }
  if (documents != 0)
  {
      myfprintf(context, archiveFile, "documents");
      RescueBuffer buf1(context, 10);
      buf1 << documents->Identifier();
      buf1.Archive(archiveFile);

      documents->Archive(); // Goes into it's own file.
  }
  else if (documents == 0 && documentListId != 0)
  {
      myfprintf(context, archiveFile, "documents");
      RescueBuffer buf1(context, 10);
      buf1 << documentListId;
      buf1.Archive(archiveFile);
  }

  myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueModel::AnyFileTruncated()
{
  RESCUEBOOL myReturn = context->FileTruncated(currentPathName);
  if (myReturn == FALSE)
  {
  if (context->readFileVersion > 12)
  {
      myReturn = WireframeFileTruncated();
      if (myReturn == FALSE)
      {
    myReturn = geoBodies->AnyFileTruncated();
    if (myReturn == FALSE)
    {
          myReturn = blocks->AnyFileTruncated();
          if (myReturn == FALSE)
          {
      myReturn = wellbores->AnyFileTruncated();
      if (myReturn == FALSE)
      {
              myReturn = horizons->AnyFileTruncated();
              if (myReturn == FALSE)
              {
        myReturn = referenceSurfaces->AnyFileTruncated();
        if (myReturn == FALSE)
        {
                  myReturn = sections->AnyFileTruncated();
                  if (myReturn == FALSE)
                  {
          myReturn = wireframes->AnyFileTruncated();
          if (myReturn == FALSE)
          {
                      myReturn = gridGeometries->AnyFileTruncated();
          }
                  }
        }
              }
      }
          }
    }
      }
  }
  }
  if (myReturn == FALSE)
  {
  if (properties == 0 && propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(this, propertyContainerId);  // Will Read.
  }
  if (properties != 0)
  {
      myReturn = properties->AnyFileTruncated();
  }
  }
  if (myReturn == FALSE)
  {
  if (documents == 0 && documentListId != 0)
  {
      documents = new RescueDocumentList(this, documentListId);  // Will Read.
  }
  if (documents != 0)
  {
      myReturn = documents->AnyFileTruncated();
  }
  }
  return myReturn;
}

RescueModel *RescueModel::UnarchiveModel(RescueContext *context, RESCUECHAR *pathName)
{
  context->uniqueIDCounter = 1;

  context->unarchivingModel = 0;
  RCHString vettedFileName = pathName;
  context->VetFileName(&vettedFileName, FALSE, FALSE);
  RESCUEINT64 version = context->FileVersion(vettedFileName.String());
/*
  This also loads the context->binaryFlag.
*/
  if (version >= 4 && version <= FILE_VERSION)
  {
  context->SoftwareVersion(vettedFileName.String(), context->readFileMainSoftwareVersion, context->readFileSubSoftwareVersion);
  if (context->binaryFlag)
  {
      RESCUECHAR buffer[91];

      context->archiveFile = (FILE *) fopen(vettedFileName.String(), "rb");

      fseek(context->archiveFile, -90L, SEEK_END);
      fread(buffer, sizeof(RESCUECHAR), 90, context->archiveFile);
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
/*
  I'm assuming here that %d is always the correct format spec for "int".
  Having no 64-bit machine, it is more than I can prove.  Passing
  RESCUEINT64 is bad if RESCUEINT64 is 64 bit and it is a 32 bit machine.
*/
    sscanf(spot, "written by code version %d", &versionNo);
    if (versionNo >= 37)
    {
          RESCUEUCHAR *xfer = (RESCUEUCHAR *) (pos - 22);
          if (*xfer == 1)
          {
      context->uintSize = 4;  //4 Normal
      if (context->LittleEndian())
      {
              context->uintSwap = TRUE;
      }
      else
      {
              context->uintSwap = FALSE;
      }
          }
          else if (*xfer == 4)
          {
      if (*(xfer + 4) == 0)
      {
              context->uintSize = 8; // 8 Swapped
              if (context->LittleEndian())
              {
        context->uintSwap = FALSE;
              }
              else
              {
        context->uintSwap = TRUE;
              }
      }
      else
      {
              context->uintSize = 4; // 4 Swapped
              if (context->LittleEndian())
              {
        context->uintSwap = FALSE;
              }
              else
              {
        context->uintSwap = TRUE;
              }
      }
          }
          else if (*xfer == 0)
          {
      context->uintSize = 8; // 8 Normal
      if (context->LittleEndian())
      {
              context->uintSwap = TRUE;
      }
      else
      {
              context->uintSwap = FALSE;
      }
          }
          xfer += 10;
          if (*xfer == 1)
          {
      context->intSize = 4;  //4 Normal
      if (context->LittleEndian())
      {
              context->intSwap = TRUE;
      }
      else
      {
              context->intSwap = FALSE;
      }
          }
          else if (*xfer == 4)
          {
      if (*(xfer + 4) == 0)
      {
              context->intSize = 8; // 8 Swapped
              if (context->LittleEndian())
              {
        context->intSwap = FALSE;
              }
              else
              {
        context->intSwap = TRUE;
              }
      }
      else
      {
              context->intSize = 4; // 4 Swapped
              if (context->LittleEndian())
              {
        context->intSwap = FALSE;
              }
              else
              {
        context->intSwap = TRUE;
              }
      }
          }
          else if (*xfer == 0)
          {
      context->intSize = 8; // 8 Normal
      if (context->LittleEndian())
      {
              context->intSwap = TRUE;
      }
      else
      {
              context->intSwap = FALSE;
      }
          }
          else if (*xfer == 255)
          {
      context->intSize = context->uintSize;
      context->intSwap = context->uintSwap;
          }
/*
  The file is v37.1 or earlier.
*/
    }
    else
    {
          context->intSize = 4;
          context->uintSize = 4;
#ifdef WIN32
          context->intSwap = TRUE;
          context->uintSwap = TRUE;
#else
          context->intSwap = FALSE;
          context->intSwap = FALSE;
#endif
    }
      }
      fseek(context->archiveFile, 0L, SEEK_SET);
  }
  else
  {
      context->archiveFile = (FILE *) fopen(vettedFileName.String(), "rt");
  }
  if (context->archiveFile != NULL)
  {
      context->wellboreProperties = new RescueIdTree();     
      context->timeStepGroups = new RescueIdTree();        
      context->properties = new RescueIdTree();
      context->modelPropertyGroups = new RescueIdTree();
      context->macroVolumes = new RescueIdTree();
      context->grids = new RescueIdTree();
      context->geometries = new RescueIdTree();
      context->geobodyVolumes = new RescueIdTree();
      context->geobodyParts = new RescueIdTree();
      context->blockUnitSides = new RescueIdTree();
      context->propertyGroups = new RescueIdTree();
 /*
  Use of static variables saves us some pointer passing, but means an
  application cannot unarchive two models at once.
*/
      context->unarchivingModel = new RescueModel(context, context->archiveFile);
      fclose(context->archiveFile);
      context->archiveFile = 0;

      context->unarchivingModel->currentPathName = _strdup(vettedFileName.String());
      context->unarchivingModel->currentBinary = context->binaryFlag;
      context->unarchivingModel->readFileMainSoftwareVersion = context->readFileMainSoftwareVersion;
      context->unarchivingModel->readFileSubSoftwareVersion = context->readFileSubSoftwareVersion;
      context->unarchivingModel->SaveContext();
/*
  Save this information for use in subsequent reads and writes.
*/
      context->unarchivingModel->RelinkNoArgs();
/*
  Turn all our id's back into pointers.
*/
      delete context->wellboreProperties;     
      delete context->timeStepGroups;        
      delete context->properties;
      delete context->modelPropertyGroups;
      delete context->macroVolumes;
      delete context->grids;
      delete context->geometries;
      delete context->geobodyVolumes;
      delete context->geobodyParts;
      delete context->blockUnitSides;
      delete context->propertyGroups;

      context->wellboreProperties = 0;     
      context->timeStepGroups = 0;        
      context->properties = 0;
      context->modelPropertyGroups = 0;
      context->macroVolumes = 0;
      context->grids = 0;
      context->geometries = 0;
      context->geobodyVolumes = 0;
      context->geobodyParts = 0;
      context->blockUnitSides = 0;
      context->propertyGroups = 0;
  }
  else
  {
      context->SetError("Failed to open main model file.");
  }
  }
  else
  {
  context->SetError("Model version number is out of range.");
  }
  return context->unarchivingModel;
}

RescueContext *RescueModel::Context(bool setContext)
{
  if (setContext)
  {
  context->write32 = write32;
  }
  return context;
}

void RescueModel::Set32()
{
  uintSize = sizeof(RESCUEUINT32);
  if (context->LittleEndian())
  {
  uintSwap = TRUE;
  }
  else
  {
  uintSwap = FALSE;
  }
  intSize = sizeof(RESCUEINT32);
  if (context->LittleEndian())
  {
  intSwap = TRUE;
  }
  else
  {
  intSwap = FALSE;
  }
  LoadContext();
}

void RescueModel::Set64()
{
  uintSize = sizeof(RESCUEUINT64);
  if (context->LittleEndian())
  {
  uintSwap = TRUE;
  }
  else
  {
  uintSwap = FALSE;
  }
  intSize = sizeof(RESCUEINT64);
  if (context->LittleEndian())
  {
  intSwap = TRUE;
  }
  else
  {
  intSwap = FALSE;
  }
  LoadContext();
}

void RescueModel::LoadContext()
{
  context->uintSize = uintSize;
  context->uintSwap = uintSwap;
  context->intSize = intSize;
  context->intSwap = intSwap;
}

void RescueModel::SaveContext()
{
  uintSize = context->uintSize;
  uintSwap = context->uintSwap;
  intSize = context->intSize;
  intSwap = context->intSwap;
}

RescueModel::RescueModel(RescueContext *contextIn, FILE *archiveFile)
                  :RescueHistoryObject(contextIn)
                  ,modelMoniker(0)
                  ,coordinateSystem(0)
                  ,units(0)
                  ,blocks(0)
                  ,horizons(0)
                  ,wellbores(0)
                  ,sections(0)
                  ,referenceSurfaces(0)
                  ,lookups(0)
                  ,logicalOrder(0)
                  ,colorTableList(0)
                  ,wireframeCallBackObject(0)
                  ,rotation((RESCUEFLOAT) 0)
                  ,properties(0)
                  ,propertyContainerId(0)
                  ,documents(0)
                  ,documentListId(0)
{
  CommonInitialization(contextIn, "Unnamed");
  UnArchive(archiveFile);
}


RESCUEINT32 RescueModel::Version(RESCUEBOOL reload)
{
  RESCUEINT32 myReturn = mainFileVersion;
  if (reload)
  {
  if (context->binaryFlag)
  {
      FILE *archiveFile = (FILE *) fopen(currentPathName, "rb");
      if (archiveFile != 0)
      {
    context->fileVersion = getc(archiveFile);
    if (context->fileVersion >= 12)
    {
          fseek(archiveFile, 17, SEEK_CUR);
    }
    if (context->fileVersion >= 4 && context->fileVersion <= FILE_VERSION)
    {
          if (context->fileVersion >= 14)
          {
      myfscanf(context, archiveFile, &myReturn);
          }
    }
    fclose(archiveFile);
      }
  }
  else
  {
      FILE *archiveFile = (FILE *) fopen(currentPathName, "rt");
      if (archiveFile != 0)
      {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    if (strncmp(myString, "Rescue", 6) == 0)
    {
          sscanf(myString, "Rescue Model File Version %d\n", &context->fileVersion);
    }
    else
    {
          sscanf(myString, "File Version %d\n", &context->fileVersion);
    }
    if (context->fileVersion >= 4 && context->fileVersion <= FILE_VERSION)
    {
          if (context->fileVersion >= 14)
          {
      myfscanf(context, archiveFile, &myReturn);
          }
    }
    fclose(archiveFile);
      }
  }
  }
  return myReturn;
}

void RescueModel::UnArchive(FILE *archiveFile)
{
  RESCUECHAR myString[255];

  context->unarchivingModel = this;
  wireframeLoaded = FALSE;
  wireframeHasChanged = FALSE;
  if (context->binaryFlag)
  {
  context->readFileVersion = getc(archiveFile);
  if (context->readFileVersion >= 12)
  {
      fseek(archiveFile, 17, SEEK_CUR);
  }
  }
  else
  {
  myfgets(context, myString, 255, archiveFile);
  if (strncmp(myString, "Rescue", 6) == 0)
  {
      sscanf(myString, "Rescue Model File Version %d\n", &context->readFileVersion);
  }
  else
  {
      sscanf(myString, "File Version %d\n", &context->readFileVersion);
  }
  }
  currentFileVersion = context->readFileVersion;
  context->fileVersion = context->readFileVersion;
  desiredFileVersion = context->readFileVersion;
  if (context->readFileVersion >= 4 && context->readFileVersion <= FILE_VERSION)
  {
  if (context->readFileVersion >= 14)
  {
      myfscanf(context, archiveFile, &mainFileVersion);
  }
  myfgets(context, myString, 255, archiveFile);
  (*modelMoniker) = myString;
  context->RescueProgress("Coordinate System");
  coordinateSystem = new RescueCoordinateSystem(context, archiveFile);
  coordinateSystem->RegisterWith(this);
  units->UnArchive(context, archiveFile);
  blocks->UnArchive(context, archiveFile);
  horizons->UnArchive(context, archiveFile);
  wellbores->UnArchive(context, archiveFile);
  sections->UnArchive(context, archiveFile);
  if (context->readFileVersion < 14)
  {
      context->RescueProgress("Poly Lines");
      wireframes->UnArchiveCompatiblePolyLines(context, archiveFile);
      context->RescueProgress("Trim Vertexes");
      wireframes->UnArchiveCompatibleTrimVertexes(context, archiveFile);

      wireframeLoaded = TRUE;
      wireframeHasChanged = FALSE;
  }
  if (context->readFileVersion >= 5)
  {
      referenceSurfaces->UnArchive(context, archiveFile);
      context->RescueProgress("Lookup Tables");
      lookups->UnArchive(context, archiveFile);
  }

  if (context->readFileVersion >= 8)
  {
      context->RescueProgress("Name/Value Pairs");
      delete nameValuePairs;
      nameValuePairs = new cNameValuePair(context, archiveFile);
  }

  if (context->readFileVersion >= 11)
  {
      context->RescueProgress("Property Model Groups");
      propertyGroup->UnArchive(context, archiveFile);
  }
  if (context->readFileVersion >= 20)
  {
      context->RescueProgress("Histories");
      history->UnArchive(context, archiveFile);
      context->RescueProgress("Color Tables");
      colorTableList = new RescueColorTableList(context, archiveFile);
      context->RescueProgress("GeoBodies");
      geoBodies->UnArchive(context, archiveFile);
  }
  if (context->readFileVersion >= 24)
  {
      context->RescueProgress("Event Descriptions");
      eventDescr->UnArchive(context, archiveFile);
      context->RescueProgress("Fault Intersections");
      faultIntersections->UnArchive(context, archiveFile);
  }
  if (context->readFileVersion >= 25)
  {
      myfscanf(context, archiveFile, &rotation);
  }
  if (context->readFileVersion >= 28)
  {
      context->RescueProgress("Global Geometries");
      gridGeometries->UnArchive(context, archiveFile);
  }
  if (context->readFileVersion >= 29)
  {
      context->RescueProgress("Global Geometry Property Groups");
      localGroups->UnArchive(context, archiveFile);
      reservoirs->UnArchive(context, archiveFile);
  }
  if (context->readFileVersion >= 30)
  {
      logicalOrder = new RescueLogicalOrder(this, archiveFile);
  }
  else
  {
      logicalOrder = new RescueLogicalOrder(this->Context());
      logicalOrder->BuildFrom(this);
  }
  }
  if (colorTableList == 0)
  {
  colorTableList = new RescueColorTableList(context);
  }
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      if (strcmp(myString, "properties") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    buf >> propertyContainerId;
      }
      else if (strcmp(myString, "documents") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    buf >> documentListId;
      }
      else
      {
    RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueModel::RelinkNoArgs()
{
  if (context->delayedPropertyLoad == FALSE)
  {
  propertyActionImmediate = TRUE;
  }
  else
  {
  propertyActionImmediate = FALSE;
  }
  if (context->delayedGeometryLoad == FALSE)
  {
  geometryActionImmediate = TRUE;
  }
  else
  {
  geometryActionImmediate = FALSE;
  }
  context->RescueProgress("Relinking units");
  units->Relink(this);
  context->RescueProgress("Relinking blocks");
  blocks->Relink(this);
  context->RescueProgress("Relinking horizons");
  horizons->Relink(this);
  context->RescueProgress("Relinking sections");
  sections->Relink(this);
  context->RescueProgress("Relinking wellbores");
  wellbores->Relink(this);
  context->RescueProgress("Relinking reference surfaces");
  referenceSurfaces->Relink(this);
  context->RescueProgress("Relinking lookup tables");
  lookups->Relink(this);
  context->RescueProgress("Relinking property model groups");
  propertyGroup->Relink(this);

  context->RescueProgress("Relinking fault intersections");
  faultIntersections->Relink(this);
/*
  Fault intersections must be relinked before the wireframe can be loaded.
*/
  if (geometryActionImmediate && IsWireframeLoaded() == FALSE)
  {
  LoadWireframe();
  }
  else if (IsWireframeLoaded())
  {
  wireframes->Relink(this);
  }
/*
  When we read an old (version 12 or newer) file we load the wireframe
  immediately.  In this case, we must relink it here.
*/
  context->RescueProgress("Relinking histories");
  history->Relink(this);
  context->RescueProgress("Relinking color tables");
  colorTableList->Relink(this);
  context->RescueProgress("Relinking geobodies");
  geoBodies->Relink(this);
  context->RescueProgress("Relinking event descriptions");
  eventDescr->Relink(this);
  context->RescueProgress("Relinking global geometries");
  gridGeometries->Relink(this);
  context->RescueProgress("Relinking global geometry property groups");
  localGroups->Relink(this);
} 

RESCUEINT32 RescueModel::WireframeVersion(RESCUEBOOL reload)
{
  if (reload)
  {
  RESCUEINT32 myReturn = wireframeVersion;
  RESCUEINT32 savedArrayVersion = wireframeVersion;
  if (ReadWireframeData(currentPathName, TRUE))
  {
      myReturn = wireframeVersion;
      wireframeVersion = savedArrayVersion;
  }
  return myReturn;
  }
  else
  {
  return wireframeVersion;
  }
}

RESCUEBOOL RescueModel::LoadWireframe()
{
  RESCUEBOOL myReturn = FALSE;
  if (IsWireframeLoaded())
  {
  DropWireframeMemory();
  }
  if (ExistingModel())
  {
  context->readFileVersion = currentFileVersion;
  context->readFileMainSoftwareVersion = readFileMainSoftwareVersion;
  context->readFileSubSoftwareVersion = readFileSubSoftwareVersion;
  myReturn = ReadWireframeData(currentPathName);
  }
  if (myReturn == TRUE)
  {
  wireframeHasChanged = FALSE;
  }
  return myReturn;
}

RESCUEBOOL RescueModel::UnloadWireframe()
{
  wireframes->UnloadWireframe();
/*
  Unload all our wireframe children at the same time.
*/
  RESCUEBOOL myReturn = FALSE;
  if (WireframeHasChanged() == FALSE)
  {
  DropWireframeMemory();
  myReturn = TRUE;
  }
  else if (IsWireframeLoaded() && ExistingModel())
  {
  context->fileVersion = desiredFileVersion;
  if (WriteWireframeData())
  {
      DropWireframeMemory();
      myReturn = TRUE;
  }
  }
  return myReturn;
}

RESCUEBOOL RescueModel::ReadWireframeData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  LoadContext();
  RCHString fileName;
  fileName << basePathName << ".wireframe";
  FILE *archiveFile = (FILE *) fopen(fileName.String(), "rb");
  if (archiveFile != 0)
  {
  RESCUEINT64 readFileVersion = getc(archiveFile);
  if (readFileVersion == 'R')
  {
      context->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 21, archiveFile);
      if (strncmp(buffer, "escue Wireframe File", 20) == 0)
      {
    modelFile = TRUE;
      }
  }
  else
  {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 21, archiveFile);
      if (strncmp(buffer, "Rescue Wireframe File", 21) == 0)
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
    readFileVersion = fileVersion;
    fseek(archiveFile, 21, SEEK_CUR);
    if (versionOnly)
    {
          myfscanf(context, archiveFile, &wireframeVersion);
    }
    else
    {
          UnArchiveWireframeData(archiveFile);
    }
    fclose(archiveFile);
      }
      else
      {
    RESCUECHAR myString[255];

    archiveFile = (FILE *) fopen(fileName.String(), "rt");
    myfgets(context, myString, 255, archiveFile);
    sscanf(myString, "Rescue Wireframe File Version %d\n", &fileVersion);
    readFileVersion = fileVersion;
    if (versionOnly)
    {
          myfscanf(context, archiveFile, &wireframeVersion);
    }
    else
    {
          UnArchiveWireframeData(archiveFile);
    }
    fclose(archiveFile);
      }
      myReturn = TRUE;
      if (versionOnly == FALSE)
      {
    wireframeLoaded = TRUE;
    wireframeHasChanged = FALSE;
      }
  }
  else
  {
      RCHString message;
      message << "Not a Rescue Wireframe File:" << fileName.NonNullString();
      context->SetError(message.NonNullString());
  }
  }
  return myReturn;
}

RESCUEBOOL RescueModel::WireframeHasChanged()
{
  RESCUEBOOL myReturn = wireframeHasChanged;
  if (wireframes != 0)
  {
  RESCUEINT64 howMany = wireframes->Count64();
  RESCUEINT64 ndx = 0;
  while (myReturn == FALSE && ndx < howMany)
  {
      RescueWireframe *wireframe = wireframes->NthObject(ndx++);
      myReturn = wireframe->WireframeHasChanged();
  }
  }
  return myReturn;
}

RESCUEBOOL RescueModel::WireframeFileTruncated()
{
  RCHString fileName;
  fileName << currentPathName << ".wireframe";
  return context->FileTruncatedPrimitive(fileName.String());
}

RESCUEBOOL RescueModel::WriteWireframeData()
{
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << currentPathName << ".wireframe";
  MakeBackupFile(fileName.String());
  FILE *archiveFile;
  if (currentBinary)
  {
  archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
  archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
  Context(true);
  context->fileVersion = desiredFileVersion;
  context->binaryFlag = currentBinary;
  if (context->binaryFlag)
  {
      putc((RESCUEUCHAR) context->fileVersion, archiveFile);
      fwrite("Rescue Wireframe File", sizeof(RESCUECHAR), 21, archiveFile);
  }
  else
  {
      fprintf(archiveFile, "Rescue Wireframe File Version %d\n", context->fileVersion);
  }
  ArchiveWireframeData(archiveFile);
  myfprintf(context, archiveFile, "abracadabra jump jump");
  if (ferror(archiveFile) != 0)
  {
      RCHString message;
      message << "Error writing to file:";
      message << fileName.NonNullString();
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

void RescueModel::UnArchiveWireframeData(FILE *archiveFile)
{
  myfscanf(context, archiveFile, &wireframeVersion);
  if (context->readFileVersion >= 28)
  {
  context->RescueProgress("Wireframe files");
  wireframes->UnArchive(context, archiveFile);
  }
  context->RescueProgress("Blocks wireframe");
  blocks->UnArchiveWireframeData(this, archiveFile);
  context->RescueProgress("Horizons wireframe");
  horizons->UnArchiveWireframeData(this, archiveFile);
  context->RescueProgress("Sections wireframe");
  sections->UnArchiveWireframeData(this, archiveFile);
  if (context->readFileVersion < 28)
  {
  context->RescueProgress("Poly Lines");
  wireframes->UnArchiveCompatiblePolyLines(context, archiveFile);
  context->RescueProgress("Trim Vertexes");
  wireframes->UnArchiveCompatibleTrimVertexes(context, archiveFile);
  }
  context->RescueProgress("Reference surfaces wire frame");
  referenceSurfaces->UnArchiveWireframeData(this, archiveFile);
  if (context->readFileVersion >= 20)
  {
  geoBodies->UnArchiveWireframeData(this, archiveFile);
  }
  if (context->readFileVersion >= 24)
  {
  RescueFaultIntersection::UnArchiveWireframeData(this, archiveFile);
  }
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

  RelinkWireframeData();
}

void RescueModel::RelinkWireframeData()
{
  wireframeLoaded = TRUE;
/*
  We need this to be set before we begin trying to rebuild pre v28 files.
*/
  wireframes->Relink(this);
  blocks->RelinkWireframeData(this);
  horizons->RelinkWireframeData(this);
  sections->RelinkWireframeData(this);
  referenceSurfaces->RelinkWireframeData(this);
  geoBodies->RelinkWireframeData(this);
  faultIntersections->RelinkWireframeData(this);
}

void RescueModel::ArchiveWireframeData(FILE *archiveFile)
{
#ifdef TESTING
  myfprintf(context, archiveFile, wireframeVersion);
#else
  myfprintf(context, archiveFile, ++wireframeVersion);
#endif
  if (context->fileVersion >= 28)
  {
  wireframes->Archive(context, archiveFile);
  }
  else
  {
  RESCUEBOOL localBinary = context->binaryFlag;
  RESCUEINT64 howMany = wireframes->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RescueWireframe *wireframe = wireframes->NthObject(loop);
      if (wireframe->IsWireframeLoaded() == FALSE)
      {
    wireframes->NthObject(loop)->ReadWireframeData(oldPathName);
      }
  }
  context->binaryFlag = localBinary;
  }
  blocks->ArchiveWireframeData(archiveFile);
  horizons->ArchiveWireframeData(archiveFile);
  sections->ArchiveWireframeData(archiveFile);
  if (context->fileVersion < 28)
  {
  context->RescueProgress("Poly Lines");
  wireframes->ArchiveCompatiblePolyLines(context, archiveFile);
  context->RescueProgress("Trim Vertexes");
  wireframes->ArchiveCompatibleTrimVertexes(context, archiveFile);
  }
  referenceSurfaces->ArchiveWireframeData(archiveFile);
  if (context->fileVersion >= 20)
  {
  geoBodies->ArchiveWireframeData(archiveFile);
  }
  if (context->fileVersion >= 24)
  {
  faultIntersections->ArchiveWireframeData(context, archiveFile);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueModel::DropWireframeMemory()
{
  if (blocks != 0)
  {
  blocks->DropWireframeMemory();
  }
  if (horizons != 0)
  {
  horizons->DropWireframeMemory();
  }
  if (sections != 0)
  {
  sections->DropWireframeMemory();
  }
  if (wireframes != 0)
  {
  delete wireframes;
  wireframes = new cSetRescueWireframe();
  }
  if (referenceSurfaces != 0)
  {
  referenceSurfaces->DropWireframeMemory();
  }
  if (geoBodies != 0)
  {
  geoBodies->DropWireframeMemory();
  }
  wireframeLoaded = FALSE;
  wireframeHasChanged = FALSE;
}

RESCUEBOOL RescueModel::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueModel)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

cSetString *RescueModel::ListBakFiles()
{
  cSetString *bakFilesToDelete = new cSetString();
  RESCUEBOOL pleaseContinue = TRUE;
#ifdef _WIN32
  RESCUECHAR pathSeparator = '\\';
  RESCUECHAR *lookName = (RESCUECHAR *) malloc((size_t) (strlen(currentPathName) + 10));
  strcpy(lookName, currentPathName);
  strcat(lookName, "*.bak");
  RESCUECHAR *dirName = _strdup(lookName);
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
  RESCUECHAR *dirName = _strdup(currentPathName);
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
  if (thisName.EndsWith(".bak") && thisName.StartsWith(currentPathName))
  {
      RCHString *toDelete = new RCHString(thisName.String());
      (*bakFilesToDelete) += toDelete;
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
  return bakFilesToDelete;
}

void RescueModel::Rollback()
{
  cSetString *bakFilesToDelete = ListBakFiles();
  RESCUEINT64 ordinal = 0;
  RCHString *fileName = bakFilesToDelete->NthObject(ordinal++);
  while (fileName != 0)
  {
  RCHString oldFileName = fileName->String();
  RESCUECHAR *pos = strstr(oldFileName.String(), ".bak");
  if (pos != 0)
  {
      *pos = 0;
      remove(oldFileName.String());
      rename(fileName->String(), oldFileName.String());
  }
  fileName = bakFilesToDelete->NthObject(ordinal++);
  }
  delete bakFilesToDelete;
/*
  Return any files we made bak files out of to their original state.
*/
  filesDeleted.EmptySelf();
/*
  Any files we would have deleted will now be kept.
*/
  RESCUEINT64 howMany = filesCreated.Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  RCHString *thisFile = filesCreated.NthObject(loop);
  remove(thisFile->String());
  }
  filesCreated.EmptySelf();
/*
  Files we created are now deleted.
*/
}

void RescueModel::Commit()
{
  cSetString *bakFilesToDelete = ListBakFiles();
  RESCUEINT64 ordinal = 0;
  RCHString *fileName = bakFilesToDelete->NthObject(ordinal++);
  while (fileName != 0)
  {
  remove(fileName->String());
  fileName = bakFilesToDelete->NthObject(ordinal++);
  }
  delete bakFilesToDelete;
/*
  Get rid of bak files.
*/
  RESCUEINT64 howMany = filesDeleted.Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
  RCHString *thisFile = filesDeleted.NthObject(loop);
  remove(thisFile->String());
  }
  filesDeleted.EmptySelf();
/*
  Delete any files that we don't need anymore.
*/
  filesCreated.EmptySelf();
/*
  Files that we created will now be kept.
*/
}

RescueGeometry *RescueModel::GeometryIdentifiedBy(RESCUEINT64 identifier)
{
  RescueGeometry *myReturn = blocks->GeometryIdentifiedBy(identifier);
  if (myReturn == 0)
  {
  myReturn = geoBodies->GeometryIdentifiedBy(identifier);
  }
  if (myReturn == 0)
  {
  myReturn = gridGeometries->ObjectIdentifiedBy(identifier);
  }
  return myReturn;
}

void RescueModel::DropGeometry(RescueGeometry *toDrop)
{
  toDrop->RescueDeleteFile();
  (*gridGeometries) -= toDrop;
}

RescueProperty *RescueModel::PropertyIdentifiedBy(RESCUEINT64 id)
{
  RescueProperty *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueGeometry *geometry = GridGeometry(ordinal++);
  while (geometry != 0 && myReturn == 0)
  {
  myReturn = geometry->PropertyIdentifiedBy(id);
  geometry = GridGeometry(ordinal++);
  }
  return myReturn;
}

RescueProperty *RescueModel::GlobalPropertyIdentifiedBy(RESCUEINT64 id)
{
  RescueProperty *myReturn = PropertyIdentifiedBy(id);
  if (myReturn == 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < blocks->Count64() && myReturn == 0; loop++)
  {
      RescueBlock *block = blocks->NthObject(loop);
      if (block != 0)
      {
    myReturn = block->PropertyIdentifiedBy(id);
      }
  }
  }
  if (myReturn == 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < geoBodies->Count64() && myReturn == 0; loop++)
  {
      RescueGeobody *gb = geoBodies->NthObject(loop);
      if (gb)
      {
    myReturn = gb->PropertyIdentifiedBy(id);
      }
  }
  }
  if (myReturn == 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < horizons->Count64() && myReturn == 0; loop++)
  {
      RescueHorizon *horizon = horizons->NthObject(loop);
      if (horizon)
      {
    myReturn = horizon->PropertyIdentifiedBy(id);
      }
  }
  }
  if (myReturn == 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < sections->Count64() && myReturn == 0; loop++)
  {
      RescueSection *section = sections->NthObject(loop);
      if (section)
      {
    myReturn = section->PropertyIdentifiedBy(id);
      }
  }
  }
  if (myReturn == 0)
  {
  RESCUEINT64 loop;
  for (loop = 0; loop < referenceSurfaces->Count64() && myReturn == 0; loop++)
  {
      RescueReferenceSurface *surface = referenceSurfaces->NthObject(loop);
      if (surface)
      {
    myReturn = surface->PropertyIdentifiedBy(id);
      }
  }
  }
  return myReturn;
}

RescueDocumentList *RescueModel::Documents()
{
  if (documents == 0)
  {
  if (documentListId != 0)
  {
      documents = new RescueDocumentList(this, documentListId);  // Will Read.
  }
  else
  {
      documents = new RescueDocumentList(this);
  }
  }
  return documents;
}

cSetRescueDataContainer *RescueModel::DataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(this, propertyContainerId);  // Will Read.
  }
  }
  return properties;
}

cSetRescueDataContainer *RescueModel::DemandDataContainers()
{
  if (properties == 0)
  {
  if (propertyContainerId != 0)
  {
      properties = new cSetRescueDataContainer(this, propertyContainerId);  // Will Read.
  }
  else
  {
      properties = new cSetRescueDataContainer(this);
  }
  }
  return properties;
}

void RescueModel::SetIJKPreference(RescueCoordinateSystem::Orientation preferredOrientation)
{
  const RESCUECHAR *prefName[] = {"LUF", "LUB", "LDF", "LDB", "RUF", "RUB", "RDF", "RDB"};
  RESCUEINT32 prefNdx = (RESCUEINT32) preferredOrientation;
  nameValuePairs->SetNameValuePair("Preferred User Orientation", prefName[prefNdx]);
}

RESCUECHAR *RescueModel::IJKPreference()
{
  return nameValuePairs->GetNameValuePair("Preferred User Orientation");
}

RescueCoordinateSystem::Orientation RescueModel::IJKPreferenceEnum()
{
  RESCUECHAR *name = nameValuePairs->GetNameValuePair("Preferred User Orientation");
  if (name == 0)
  {
  throw "User IJK Preference not given.";
  }
  else
  {
  RescueCoordinateSystem::Orientation orientation = RescueCoordinateSystem::LDF;
  if (strcmp(name, "LDB") == 0)
  {
      orientation = RescueCoordinateSystem::LDB;
  }
  else if (strcmp(name, "LUF") == 0)
  {
      orientation = RescueCoordinateSystem::LUF;
  }
  else if (strcmp(name, "LUB") == 0)
  {
      orientation = RescueCoordinateSystem::LUB;
  }
  else if (strcmp(name, "RUB") == 0)
  {
      orientation = RescueCoordinateSystem::RUB;
  }
  else if (strcmp(name, "RUF") == 0)
  {
      orientation = RescueCoordinateSystem::RUF;
  }
  else if (strcmp(name, "RDF") == 0)
  {
      orientation = RescueCoordinateSystem::RDF;
  }
  else if (strcmp(name, "RDB") == 0)
  {
      orientation = RescueCoordinateSystem::RDB;
  }
  return orientation;
  }
}

RescueHistory *RescueModel::GetWriterHistory()
{
  RescueHistory *myReturn = 0;
  RESCUEINT64 ordinal = 0;
  RescueHistory *candidate = history->NthObject(ordinal++);
  while (candidate != 0 && myReturn == 0)
  {
  if (strncmp(candidate->ChangeDescription()->NonNullString(), "Rescue Creator Record:", 22) == 0)
  {
      myReturn = candidate;
  }
  else
  {
      candidate = history->NthObject(ordinal++);
  }
  }
  return myReturn;
}

void RescueModel::SetWriter(const RESCUECHAR *vendorName, const RESCUECHAR *vendorVersion, 
              const RESCUECHAR *modelDate,  const RESCUECHAR *userName)
{
  RCHString readable(100);
  readable << "Rescue Creator Record:Vendor="
           << vendorName
           << " Version="
           << vendorVersion
           << " Date="
           << modelDate
           << " User="
           << userName;
  RCHString stamp(200);
  stamp << "<stamp>";
  stamp << "<vendor>";
  stamp << vendorName;
  stamp << "</vendor>";
  stamp << "<version>";
  stamp << vendorVersion;
  stamp << "</version>";
  stamp << "<date>";
  stamp << modelDate;
  stamp << "</date>";
  stamp << "<user>";
  stamp << userName;
  stamp << "</user>";
  stamp << "</stamp>";
  RescueHistory *existingStamp = GetWriterHistory();
  if (existingStamp == 0)
  {
  new RescueHistory(this, readable.String(), stamp.String());
  }
  else
  {
  existingStamp->SetChangeDescription(readable.String());
  existingStamp->SetParsableDescription(stamp.String());
  }
}

RCHString *RescueModel::VendorSegment(const RESCUECHAR *beginKey, const RESCUECHAR *endKey)
{
  RCHString *myReturn = 0;
  RescueHistory *stamp = GetWriterHistory();
  if (stamp != 0)
  {
  RESCUECHAR *pos = stamp->ParsableDescription()->NonNullString();
  RESCUECHAR *beginPos = strstr(pos, beginKey);
  RESCUECHAR *endPos = strstr(pos, endKey);
  if (beginPos != 0 && endPos != 0)
  {
      myReturn = new RCHString((int) (endPos - beginPos) + 5);
      beginPos += strlen(beginKey);
      while (beginPos != endPos)
      {
    myReturn->AddTo(*beginPos++);
      }
  }
  }
  return myReturn;
}

RCHString *RescueModel::VendorName()
{
  return VendorSegment("<vendor>", "</vendor>");
}

RCHString *RescueModel::VendorVersion()
{
  return VendorSegment("<version>", "</version>");
}

RCHString *RescueModel::ModelDate()
{
  return VendorSegment("<date>", "</date>");
}

RCHString *RescueModel::UserName()
{
  return VendorSegment("<user>", "</user>");
}

int RescueModel::SetOrientation(RescueContext *context, 
                char *modelFileName, 
                RescueCoordinateSystem::Orientation orientation)
{
  int myReturn = 0;
  RescueModel *myModel = UnarchiveModel(context, modelFileName);
  if (myModel == 0)
  {
  myReturn = 1;
  }
  else
  {
  myReturn = myModel->SetOrientationPrimitive(orientation);
  delete myModel;
  }
  return myReturn;
}

int RescueModel::SetOrientationPrimitive(RescueCoordinateSystem::Orientation orientation)
{
  int myReturn = 3;
  RescueOrientationLedger *ledger = new RescueOrientationLedger();
  RESCUEINT64 ordinal = 0;
  RescueGeometry *geometry = gridGeometries->NthObject(ordinal++);
  while (geometry != 0)
  {
  geometry->SetOrientation(ledger, orientation);
  geometry = gridGeometries->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueBlock *block = blocks->NthObject(ordinal++);
  while (block != 0)
  {
  block->SetOrientation(ledger, orientation);
  block = blocks->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueGeobody *geobody = geoBodies->NthObject(ordinal++);
  while (geobody != 0)
  {
  geobody->SetOrientation(ledger, orientation);
  geobody = geoBodies->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueWellbore *wellbore = wellbores->NthObject(ordinal++);
  while (wellbore != 0)
  {
  wellbore->SetOrientation(ledger, orientation);
  wellbore = wellbores->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueHorizon *horizon = horizons->NthObject(ordinal++);
  while (horizon != 0)
  {
  horizon->SetOrientation(ledger, orientation);
  horizon = horizons->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueSection *section = sections->NthObject(ordinal++);
  while (section != 0)
  {
  section->SetOrientation(ledger, orientation);
  section = sections->NthObject(ordinal++);
  }
  ordinal = 0;
  RescueReferenceSurface *referenceSurface = referenceSurfaces->NthObject(ordinal++);
  while (referenceSurface != 0)
  {
  referenceSurface->SetOrientation(ledger, orientation);
  referenceSurface = referenceSurfaces->NthObject(ordinal++);
  }
  LoadWireframe();
  ordinal = 0;
  RescueWireframe *wireframe = wireframes->NthObject(ordinal++);
  while (wireframe != 0)
  {
  wireframe->SetOrientation(ledger);
  wireframe = wireframes->NthObject(ordinal++);
  }
  MarkWireframeChanged();
  UnloadWireframe();

  ArchiveModel();
  Commit();
  delete ledger;
  return myReturn;
}

cSetString *RescueModel::UniquePropertyNames()    // Returns a set of unique property names found in
{                                                 // the model.  Geometries, surfaces, and
                                                  // Data containers may be loaded and
                                                  // unloaded by this process.
                                                  // Delete the object returned.
  cSetString *myReturn = new cSetString();
  blocks->FindUniquePropertyNames(myReturn);
  geoBodies->FindUniquePropertyNames(myReturn);
  gridGeometries->FindUniquePropertyNames(myReturn);
  horizons->FindUniquePropertyNames(myReturn);
  sections->FindUniquePropertyNames(myReturn);
  referenceSurfaces->FindUniquePropertyNames(myReturn);
  return myReturn;
}

bool RescueModel::ReadWITSML(const RESCUECHAR *pathName, RescueProgressReporter *loggingObject)
{
  bool myReturn = false;
  FILE *file = fopen(pathName, "r");
  if (file != 0)
  {
  RCHXMLTokenizer *tokens = new RCHXMLTokenizer(file);
  RESCUECHAR *token = tokens->NextToken();
  while (token != 0)
  {
      if (tokens->TokenIsTag("trajectory"))
      {
    RCHString wellboreName;
    RCHString depthUOM;
    size_t dCount = 0;
    size_t dAlloc = 100;
    RESCUEFLOAT *mds = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * dAlloc);
    RESCUEFLOAT *tvds = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * dAlloc);
    RESCUEFLOAT *eastings = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * dAlloc);
    RESCUEFLOAT *northings = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * dAlloc);

    tokens->ConsumeTag();
    token = tokens->NextToken();
    RESCUEBOOL pleaseContinue = TRUE;
    while (token != 0 && pleaseContinue == TRUE)
    {
          if (tokens->TokenIsTag("/trajectory"))
          {
      tokens->ConsumeTag();
      pleaseContinue = FALSE;
          }
          else
          {
      if (tokens->TokenIsTag("nameWellbore"))
      {
              tokens->ConsumeTag();
              wellboreName = tokens->NextToken();
      }
      else if (tokens->TokenIsTag("trajectoryStation"))
      {
              if (dCount == dAlloc)
              {
        dAlloc *= 2;
        mds = (RESCUEFLOAT *) realloc(mds, sizeof(RESCUEFLOAT) * dAlloc);
        tvds = (RESCUEFLOAT *) realloc(tvds, sizeof(RESCUEFLOAT) * dAlloc);
        eastings = (RESCUEFLOAT *) realloc(eastings, sizeof(RESCUEFLOAT) * dAlloc);
        northings = (RESCUEFLOAT *) realloc(northings, sizeof(RESCUEFLOAT) * dAlloc);
              }
      }
      else if (tokens->TokenIsTag("/trajectoryStation"))
      {
              dCount++;
      }
      else if (tokens->TokenIsTag("md"))
      {
              if (depthUOM.length64() == 0)
              {
        while (tokens->InTag())
        {
                  token = tokens->NextToken();
                  if (strcmp(token, "uom"))
                  {
          token = tokens->NextToken();
          if (strcmp(token, "=") == 0)
          {
                      depthUOM = tokens->UnquotedToken();
          }
          tokens->ConsumeTag();
                  }
        }
              }
              else
              {
        tokens->ConsumeTag();
              }
              mds[dCount] = (float) atof(tokens->NextToken());
      }
      else if (tokens->TokenIsTag("tvd"))
      {
              tokens->ConsumeTag();
              tvds[dCount] = (float) atof(tokens->NextToken());
      }
      else if (tokens->TokenIsTag("northing"))
      {
              tokens->ConsumeTag();
              northings[dCount] = (float) atof(tokens->NextToken());
      }
      else if (tokens->TokenIsTag("easting"))
      {
              tokens->ConsumeTag();
              eastings[dCount] = (float) atof(tokens->NextToken());
      }
      token = tokens->NextToken();
          }
    }
    if (dCount > 0 && wellboreName.length64() > 0)
    {
          RESCUEINT64 ndx = 0;
          RescueWellbore *logWellbore = 0;
          RescueWellbore *wellbore = NthRescueWellbore(ndx++);
          while (wellbore != 0 && logWellbore == 0)
          {
      if (wellboreName == wellbore->WellboreName()->String())
      {
              logWellbore = wellbore;
      }
      else
      {
              wellbore = NthRescueWellbore(ndx++);
      }
          }
          if (logWellbore == 0)
          {
      logWellbore = new RescueWellbore(RescueCoordinateSystem::LUB,
                                                          this,
                                                          wellboreName.String(),
                                                          -999.25F,
                                                          0, dCount);
      if (loggingObject != 0)
      {
              RCHString message;
              message << "Adding RescueWellbore ";
              message << wellboreName.String();
              loggingObject->ReportProgress(message.NonNullString());
      }
          }
          else if (loggingObject != 0)
          {
      RCHString message;
      message << "Setting RescueWellbore geometry ";
      message << wellboreName.String();
      loggingObject->ReportProgress(message.NonNullString());
          }
          RescueQuadrupletArray *geometry = logWellbore->WellboreGeometry();
          geometry->SetMDValue(mds);
          geometry->SetZValue(tvds);
          geometry->SetXValue(eastings);
          geometry->SetYValue(northings);

          if (loggingObject != 0)
          {
      RCHString stats;
      stats << (RESCUEINT32) dCount << " points MD:" << mds[0] << " to " << mds[dCount - 1];
      loggingObject->ReportProgress(stats.String());
          }
    }
    free(mds);
    free(tvds);
    free(eastings);
    free(northings);
      }
      else if (tokens->TokenIsTag("wellLog"))
      {
    RescueWellbore *wellbore = 0;
    RescueWellboreSampling *sampling = 0;
    RCHString depthUOM;

    size_t pDepthNdx = 0;
    size_t dCount = 0;
    size_t dAlloc = 100;
    size_t pCount = 0;
    size_t pAlloc = 10;
    size_t bCount = 0;
    size_t bAlloc = 10;
    float **pValue = (RESCUEFLOAT **) malloc(sizeof(RESCUEFLOAT *) * pAlloc);
    RESCUECHAR **pCurveId = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * pAlloc);
    RESCUECHAR **pName = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * pAlloc);
    RESCUECHAR **pType = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * pAlloc);
    RESCUECHAR **pUom = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * pAlloc);
    RESCUEFLOAT *pNull = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * pAlloc);

    RESCUECHAR **bId = (RESCUECHAR **) malloc(sizeof(RESCUECHAR *) * bAlloc);
    size_t **pSpot = (size_t **) malloc(sizeof(size_t *) * bAlloc);

    size_t pLoop;

    tokens->ConsumeTag();
    token = tokens->NextToken();
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEBOOL skipLog = FALSE;
    while (token != 0 && pleaseContinue == TRUE)
    {
          if (tokens->TokenIsTag("/wellLog"))
          {
      tokens->ConsumeTag();
      pleaseContinue = FALSE;
          }
          else if (skipLog == FALSE)
          {
      if (tokens->TokenIsTag("nameWell"))
      {
              tokens->ConsumeTag();
              RCHString wellboreName = tokens->NextToken();
              RESCUEINT64 ndx = 0;
              RescueWellbore *candidate = NthRescueWellbore(ndx++);
              while (candidate != 0 && wellbore == 0)
              {
        if (wellboreName == candidate->WellboreName()->String())
        {
                  wellbore = candidate;
        }
        else
        {
                  candidate = NthRescueWellbore(ndx++);
        }
              }
              if (wellbore == 0)
              {
        skipLog = TRUE;
        if (loggingObject != 0)
        {
                  RCHString message;
                  message << "ERROR: wellLog skipped for " << wellboreName.String() << " RescueWellbore not found.";
                  loggingObject->ReportProgress(message.String());
        }
              }
              else if (loggingObject != 0)
              {
        RCHString message;
        message << "For RescueWellbore " << wellboreName.String();
        loggingObject->ReportProgress(message.String());
              }
      }
      else if (tokens->TokenIsTag("name"))
      {
              tokens->ConsumeTag();
              RCHString nameValue = tokens->NextToken();
              if (wellbore != 0 && sampling == 0)
              {
        RESCUECHAR *lastDash = strrchr(nameValue.NonNullString(), '-');
        if (lastDash != 0)
        {
                  RESCUEINT64 id = atoi(++lastDash);
                  sampling = wellbore->WellboreSamplingIdentifiedBy(id);
        }
              }
      }
      else if (tokens->TokenIsTag("logCurveInfo"))
      {
              RCHString mnemonic;
              RCHString unit;
              RCHString nullValue;
              RCHString curveDescription;
              RCHString curveId;

              while (tokens->InTag())
              {
        token = tokens->NextToken();
        if (strcmp(token, "uid") == 0)
        {
                  token = tokens->NextToken();
                  if (*token == '=')
                  {
          curveId = tokens->UnquotedToken();
                  }
        }
              }

              RESCUEBOOL continueLoop = TRUE;
              while (tokens != 0 && continueLoop == TRUE)
              {
        if (tokens->TokenIsTag("/logCurveInfo"))
        {
                  continueLoop = FALSE;
        }
        else
        {
                  if (tokens->TokenIsTag("mnemonic"))
                  {
          mnemonic = tokens->NextToken();
                  }
                  else if (tokens->TokenIsTag("unit"))
                  {
          unit = tokens->NextToken();
                  }
                  else if (tokens->TokenIsTag("nullValue"))
                  {
          nullValue = tokens->NextToken();
                  }
                  else if (tokens->TokenIsTag("curveDescription"))
                  {
          curveDescription = tokens->NextToken();
                  }
                  else if (tokens->TokenIsTag("classWitsml"))
                  {
          token = tokens->NextToken();
          if (strcmp(token, "measured depth") == 0)
          {
                      pDepthNdx = pCount;
          }
                  }
                  tokens->NextToken();
        }
              }
              if (pCount == pAlloc)
              {
        pAlloc += 20;
        pValue = (RESCUEFLOAT **) realloc(pValue, sizeof(RESCUEFLOAT *) * pAlloc);
        pCurveId = (RESCUECHAR **) realloc(pCurveId, sizeof(RESCUECHAR *) * pAlloc);
        pName = (RESCUECHAR **) realloc(pName, sizeof(RESCUECHAR *) * pAlloc);
        pType = (RESCUECHAR **) realloc(pType, sizeof(RESCUECHAR *) * pAlloc);
        pUom = (RESCUECHAR **) realloc(pUom, sizeof(RESCUECHAR *) * pAlloc);
        pNull = (RESCUEFLOAT *) realloc(pNull, sizeof(RESCUEFLOAT) * pAlloc);
              }
              pValue[pCount] = (RESCUEFLOAT *) malloc(sizeof(RESCUEFLOAT) * dAlloc);
              pCurveId[pCount] = _strdup(curveId.NonNullString());
              pName[pCount] = _strdup(mnemonic.NonNullString());
              pType[pCount] = _strdup(curveDescription.NonNullString());
              pUom[pCount] = _strdup(unit.NonNullString());
              pNull[pCount] = (RESCUEFLOAT) atof(nullValue.NonNullString());
              pCount++;
      }
      else if (tokens->TokenIsTag("blockInfo"))
      {
              if (bCount == bAlloc)
              {
        bAlloc += 10;
        bId = (RESCUECHAR **) realloc(bId, sizeof(RESCUECHAR *) * bAlloc);
        pSpot = (size_t **) realloc(pSpot, sizeof(size_t *) * bAlloc);
              }
              pSpot[bCount] = (size_t *) malloc(sizeof(size_t) * pCount);
              for (pLoop = 0; pLoop < pCount; pLoop++)
              {
        pSpot[bCount][pLoop] = pLoop;
              }
              while (tokens->InTag())
              {
        token = tokens->NextToken();
        if (strcmp(token, "uid") == 0)
        {
                  token = tokens->NextToken();
                  if (*token == '=')
                  {
          bId[bCount] = _strdup(tokens->UnquotedToken());
                  }
        }
              }

              token = tokens->NextToken();

              RESCUEBOOL continueLoop = TRUE;
              while (tokens != 0 && continueLoop == TRUE)
              {
        if (tokens->TokenIsTag("/blockInfo"))
        {
                  bCount++;
                  continueLoop = FALSE;
        }
        else if (tokens->TokenIsTag("blockCurveInfo"))
        {
                  RCHString curveId;
                  RCHString columnNo;
                  RESCUEBOOL continueLoop = TRUE;
                  while (tokens != 0 && continueLoop == TRUE)
                  {
          if (tokens->TokenIsTag("/blockCurveInfo"))
          {
                      continueLoop = FALSE;
          }
          else
          {
                      if (tokens->TokenIsTag("curveId"))
                      {
            tokens->ConsumeTag();
            curveId = tokens->NextToken();
                      }
                      else if (tokens->TokenIsTag("columnIndex"))
                      {
            tokens->ConsumeTag();
            columnNo = tokens->NextToken();
                      }
                      tokens->NextToken();
          }
                  }
                  if (curveId.length64() > 0 && columnNo.length64() > 0)
                  {
          size_t pNdx = 0;
          size_t cNdx = pCount + 1;
          while (pNdx < pCount && cNdx > pCount)
          {
                      if (curveId == pCurveId[pNdx])
                      {
            cNdx = pNdx;
                      }
                      else
                      {
            pNdx++;
                      }
          }
          if (cNdx < pCount)
          {
                      RESCUEINT32 cNo = atoi(columnNo.String()) - 1;
                      if (cNo != (RESCUEINT32) cNdx)
                      {
            size_t temp = pSpot[bCount][cNo];
            pSpot[bCount][cNo] = pSpot[bCount][cNdx];
            pSpot[bCount][cNdx] = temp;
                      }
          }
                  }
        }
        if (continueLoop)
        {
                  token = tokens->NextToken();
        }
              }
      }
      else if (tokens->TokenIsTag("logData"))
      {
              if (bCount == 0)
              {
        pSpot[bCount] = (size_t *) malloc(sizeof(size_t) * pCount);
        for (pLoop = 0; pLoop < pCount; pLoop++)
        {
                  pSpot[bCount][pLoop] = pLoop;
        }
        bCount++;
              }

              RESCUEBOOL continueLoop = TRUE;
              while (tokens != 0 && continueLoop == TRUE)
              {
        if (tokens->TokenIsTag("/logData"))
        {
                  continueLoop = FALSE;
        }
        else
        {
                  if (tokens->TokenIsTag("data"))
                  {
          size_t bNdx = 0;
          while (tokens->InTag())
          {
                      token = tokens->NextToken();
                      if (strcmp(token, "id") == 0)
                      {
            token = tokens->NextToken();
            if (*token == '=')
            {
                          token = tokens->UnquotedToken();
                          for (pLoop = 0; pLoop < bCount; pLoop++)
                          {
              if (bId[pLoop] != 0)
              {
                              if (strcmp(bId[pLoop], token) == 0)
                              {
                bNdx = pLoop;
                              }
              }
                          }
            }
                      }
          }
          tokens->ConsumeTag();
          token = tokens->NextToken();

          if (dCount == dAlloc)
          {
                      dAlloc *= 2;
                      for (pLoop = 0; pLoop < pCount; pLoop++)
                      {
            pValue[pSpot[bNdx][pLoop]] = (RESCUEFLOAT *) realloc(pValue[pSpot[bNdx][pLoop]], sizeof(RESCUEFLOAT *) * dAlloc);
                      }
          }
          RESCUECHAR *commaPos = strchr(token, ',');
          for (pLoop = 0; pLoop < pCount; pLoop++)
          {
                      if (commaPos != 0)
                      {
            *commaPos++ = 0;
            sscanf(token, "%f", &pValue[pSpot[bNdx][pLoop]][dCount]);
            token = commaPos;
            commaPos = strchr(token, ',');
                      }
                      else
                      {
            sscanf(token, "%f", &pValue[pSpot[bNdx][pLoop]][dCount]);
                      }
          }
          dCount++;
                  }
                  token = tokens->NextToken();
        }
              }
      }
          }
          if (pleaseContinue == TRUE)
          {
      token = tokens->NextToken();
          }
    }
    if (wellbore != 0 && skipLog == FALSE && dCount > 0)
    {
          if (sampling == 0)
          {
      sampling = new RescueWellboreSampling(wellbore, dCount, pValue[pDepthNdx]);
      if (loggingObject != 0)
      {
              loggingObject->ReportProgress("Adding new RescueWellboreSampling.");
      }
          }
          else
          {
      if (dCount == (size_t) sampling->Count64())
      {
              sampling->SetValues(pValue[pDepthNdx]);
              if (loggingObject != 0)
              {
        RCHString message;
        message << "Updating existing RescueWellboreSampling " << sampling->Identifier();
        loggingObject->ReportProgress(message.NonNullString());
              }
      }
      else
      {
              if (loggingObject != 0)
              {
        RCHString message;
        message << "Replacing existing RescueWellboreSampling " << sampling->Identifier();
        loggingObject->ReportProgress("Replacing new RescueWellboreSampling.");
              }
              wellbore->DropRescueWellboreSampling(sampling);
              sampling = new RescueWellboreSampling(wellbore, dCount, pValue[pDepthNdx]);
      }
          }
          pValue[pDepthNdx] = 0;
          for (pLoop = 0; pLoop < pCount; pLoop++)
          {
      if (pLoop != pDepthNdx)
      {
              RescueWellboreProperty *incumbent = sampling->PropertyDescribedBy(pName[pLoop], pType[pLoop], pUom[pLoop]);
              if (incumbent != 0)
              {
        RescueArray *data = incumbent->Data();
        if (data->IsA() != R_RescueArrayFloat)
        {
                  if (loggingObject != 0)
                  {
          RCHString message;
          message << "Did not update property " << pName[pLoop] << "," << pType[pLoop] << "," << pUom[pLoop] << " incumbent was not RescueArrayFloat";
          loggingObject->ReportProgress(message.String());
                  }
        }
        else
        {
                  RescueArrayFloat *fData = (RescueArrayFloat *) data;
                  data->Load();
                  fData->AcceptValue(pNull[pLoop], pValue[pLoop]);
                  data->MarkChanged();
                  fData->Unload();
                  pValue[pLoop] = 0;
                  if (loggingObject != 0)
                  {
          RCHString message;
          message << "Updated existing property " << pName[pLoop] << "," << pType[pLoop] << "," << pUom[pLoop];
          loggingObject->ReportProgress(message.String());
                  }
        }
              }
              else
              {
  new RescueWellboreProperty(sampling, 
    pName[pLoop], 
    pType[pLoop], 
    pUom[pLoop], 
    pNull[pLoop], 
    pValue[pLoop]);
        pValue[pLoop] = 0;

        if (loggingObject != 0)
        {
                  RCHString message;
                  message << "Adding property " << pName[pLoop] << "," << pType[pLoop] << "," << pUom[pLoop];
                  loggingObject->ReportProgress(message.String());
        }
              }
      }
          }
    }
    for (pLoop = 0; pLoop < pCount; pLoop++)
    {
          if (pCurveId[pLoop] != 0)
          {
      free(pCurveId[pLoop]);
          }
          if (pValue[pLoop] != 0)
          {
      free(pValue[pLoop]);
          }
          if (pName[pLoop] != 0)
          {
      free(pName[pLoop]);
          }
          if (pType[pLoop] != 0)
          {
      free(pType[pLoop]);
          }
          if (pUom[pLoop] != 0)
          {
      free(pUom[pLoop]);
          }
    }
    for (pLoop = 0; pLoop < bCount; pLoop++)
    {
          if (bId[pLoop] != 0)
          {
      free(bId[pLoop]);
          }
          free(pSpot[pLoop]);
    }
    free(pValue);
    free(pCurveId);
    free(pType);
    free(pName);
    free(pUom);
    free(pNull);
    free(bId);
    free(pSpot);
      }
      token = tokens->NextToken();
  }
  delete tokens;
  myReturn = true;
  }
  else if (loggingObject != 0)
  {
  RCHString errorMessage;
  errorMessage << "ERROR: Unable to open ";
  errorMessage << pathName;
  errorMessage << " for reading.";
  loggingObject->ReportProgress(errorMessage.String());
  }
  return myReturn;
}

bool RescueModel::WriteWITSML(const RESCUECHAR *pathName)
{
  bool myReturn = false;
  FILE *file = fopen(pathName, "w");
  if (file != 0)
  {
  fprintf(file, "%s\n", "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>");
  fprintf(file, "%s\n", "<WITSMLComposite xmlns=\"http://www.witsml.org/schemas/131\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.witsml.org/schemas/131 http://www.witsml.org/schemas/131/obj_trajectory.xsd\" version=\"1.3.1.0\">");
  fprintf(file, "%s\n", "  <allObjects>");
  fprintf(file, "%s\n", "    <wellDatum>");
  fprintf(file, "%s\n", "      <name>modelDatum</name>");
  RescueVertex *mVertex = coordinateSystem->Vertex();
  if (mVertex != 0)
  {
      RescueCoordinateSystemAxis *axis = mVertex->CoordinateSystem()->Axis(2);
      if (axis != 0)
      {
    fprintf(file, "      <elevation uom=\"%s\" datum=\"%s\">%f</elevation>\n", axis->UnitOfMeasure()->NonNullString(), mVertex->VertexName()->NonNullString(), mVertex->Z());
      }
  }
  fprintf(file, "%s\n", "    </wellDatum>");
  fprintf(file, "%s\n", "    <trajectorySet>");
  fprintf(file, "      <name>%s Trajectories</name>\n", modelMoniker->NonNullString());
  fprintf(file, "%s\n", "      <trajectorys>");

  RCHString *uom = CoordinateSystem()->Axis(2)->UnitOfMeasure();
  RCHString *csName = CoordinateSystem()->Name();

  RESCUEINT64 ordinal = 0;
  RescueWellbore *wellbore = wellbores->NthObject(ordinal++);
  while (wellbore != 0)
  {
      RescueQuadrupletArray *wellboreGeom = wellbore->WellboreGeometry();
      wellboreGeom->WriteWITSML(file, wellbore, uom, csName);

      wellbore = wellbores->NthObject(ordinal++);
  }

  fprintf(file, "%s\n", "      </trajectorys>");
  fprintf(file, "%s\n", "    </trajectorySet>");
  fprintf(file, "%s\n", "    <wellLogSet>");
  fprintf(file, "      <name>%s Well Logs</name>\n", modelMoniker->NonNullString());
  fprintf(file, "%s\n", "      <wellLogs>");

  ordinal = 0;
  wellbore = wellbores->NthObject(ordinal++);
  while (wellbore != 0)
  {
      RESCUEINT64 subOrdinal = 0;
      RescueWellboreSampling *sampling = wellbore->NthRescueWellboreSampling(subOrdinal++);
      while (sampling != 0)
      {
    sampling->WriteWITSML(file, wellbore, uom);
    sampling = wellbore->NthRescueWellboreSampling(subOrdinal++);
      }

      wellbore = wellbores->NthObject(ordinal++);
  }

  fprintf(file, "%s\n", "      </wellLogs>");
  fprintf(file, "%s\n", "    </wellLogSet>");
  fprintf(file, "%s\n", "  </allObjects>");
  fprintf(file, "%s\n", "</WITSMLComposite>");

  myReturn = ferror(file) == 0;
  fclose(file);
  }
  return myReturn;
}

