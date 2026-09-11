/********************************************************************

  RescueDataContainer.cpp

  A container for properties on semi-arbitrary objects in a Rescue
  Model.

  Rod Hanks,  Feb, 2006

*********************************************************************/
#include "RescueModel.h"
#include "cSetRescueDataContainer.h"
#include "RescueDataContainer.h"
#include "cNameValuePair.h"

RescueDataContainer::RescueDataContainer(RescueModel *modelIn, RescueGrid *customGrid)
                                              :RescueHistoryObject(modelIn->Context())
                                              ,model(modelIn)
{
  propertyGrid = customGrid;
  namedData = new cNameValuePair();
  properties = new cSetRescueProperty();
  propertyGroups = new cSetRescuePropertyGroup();

}

void RescueDataContainer::Dispose()
{
  properties->Dispose();
  delete this;
}

RescueDataContainer::~RescueDataContainer()
{
  delete propertyGrid;
  delete namedData;
  delete properties;
  delete propertyGroups;
}

RescueDataContainer::RescueDataContainer(RescueModel *modelIn, FILE *archiveFile)
                                              :RescueHistoryObject(modelIn->Context())
                                              ,model(modelIn)
{
  isA = R_RescueDataContainer;
  propertyGrid = new RescueGrid(model->Context(), archiveFile);
  namedData = new cNameValuePair(model->Context(), archiveFile);
  properties = new cSetRescueProperty();
  properties->UnArchive(model->Context(), archiveFile);
  propertyGroups = new cSetRescuePropertyGroup();
  propertyGroups->UnArchive(model->Context(), archiveFile);
}

void RescueDataContainer::Archive(FILE *archiveFile)
{
  propertyGrid->Archive(model->Context(), archiveFile);
  namedData->Archive(model->Context(), archiveFile);
  properties->Archive(model->Context(), archiveFile);
  propertyGroups->Archive(model->Context(), archiveFile);
}

void RescueDataContainer::Relink(RescueObject *parent)
{
  model = (RescueModel *) parent;
  propertyGrid->Relink(model);
  properties->Relink(this);
  propertyGroups->Relink(this);
}

RESCUEBOOL RescueDataContainer::AnyFileTruncated()
{
  return properties->AnyFileTruncated();
}

void RescueDataContainer::FindUniquePropertyNames(cSetString *container)
{
  properties->FindUniquePropertyNames(container);
}



