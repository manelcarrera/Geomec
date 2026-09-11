#include "StdAfx.h"

#include "SaveModelGUI.h"
#include "GeomecDoc.h"

CSaveModelGUI::CSaveModelGUI()
: ISaveModel()
{
}

CSaveModelGUI::~CSaveModelGUI()
{
}

bool CSaveModelGUI::saveModel(const QString& fileName, bool bSwitch)
{
  CGeomecDoc& doc = *GetGeomecDoc();

  return doc.SaveModel(fileName.toStdString().c_str(), 0, bSwitch);
}
