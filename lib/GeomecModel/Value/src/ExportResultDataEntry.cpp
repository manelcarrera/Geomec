#include "ExportResultDataEntry.h"
#include "BaseEntryTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "ExportResultData.h"
#include "ExportResultDataXML.h"
#include "ModelBase.h"
#include "GlobalMessage.h"


CExportResultDataEntry::CExportResultDataEntry(CFemAppModel& model)
: CStorageNodeEntry <CExportResultData> (MD_BASE_EXPORT_MACROS,
  IDI_EXPORT_MACROS, IDS_TREE_EXPORT_MACROS, model)
{
  initReservoirMacro();
}


void CExportResultDataEntry::Import(const QString& sFileName)
{
  erd_xml::CExportResultDataXML::TExportResultDataVector macros;
  erd_xml::CExportResultDataXML importer(static_cast<CModelBase&>(Model()), macros);

  try
  {
  importer.LoadFile(sFileName);
  }
  catch (erd_xml::CException& e)
  {
  _m()->msg(QString("An error occurred while processing '%1': %2").arg(sFileName).arg(e.error()));
  }
}

void CExportResultDataEntry::Export(const QString& sFileName)
{
  erd_xml::CExportResultDataXML::TExportResultDataVector macros;

  CExportResultDataEntry::TEntryNodeSet stNodes = GraphEntryNodes();
  macros.reserve(stNodes.size());

  for (CExportResultDataEntry::TEntryNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  macros.push_back(static_cast<CExportResultData *>(*it));

  erd_xml::CExportResultDataXML exporter(static_cast<CModelBase&>(Model()), macros);

  try
  {
  exporter.SaveFile(sFileName);
  }
  catch (erd_xml::CException& e)
  {
  _m()->msg(QString("An error occurred while processing '%1': %2").arg(sFileName).arg(e.error()));
  }
}


CExportResultData *CExportResultDataEntry::retrieveReservoirMacro() const
{
  CExportResultDataEntry::TNodeSet stNodes = EntryNodes();

  for (CExportResultDataEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  if ((*it)->ReservoirMacro())
  {
      return *it;
  }
  }

  return 0;
}

void CExportResultDataEntry::initReservoirMacro()
{
  if (!retrieveReservoirMacro())
  {
  CExportResultData *exportResultData = new CExportResultData("Reservoirs for coupled analysis", Model());
  assert(exportResultData);
  exportResultData->m_settings.m_bReservoirMacro = true;
  }
}

void CExportResultDataEntry::LoadStream(CFemAppModel& model, CStorageNodeEntry<CExportResultData>::TSTREAM& stream, CStreamVersion& version, CStorageNodeEntry<CExportResultData>::TPROGRESS& progress)
{
  CExportResultData *reservoirs = retrieveReservoirMacro();

  CStorageNodeEntry<CExportResultData>::LoadStream(model, stream, version, progress);

  // reservoirs can be the default one from when we created the model; if we also load one, we delete the default
  if (reservoirs)
  {
  CExportResultDataEntry::TNodeSet stNodes = EntryNodes();

  for (CExportResultDataEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if (*it != reservoirs && (*it)->ReservoirMacro())
      {
    reservoirs->Destroy();
    break;
      }
  }
  }
}