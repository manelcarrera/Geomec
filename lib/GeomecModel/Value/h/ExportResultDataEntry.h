#ifndef _ExportResultDataEntry_h_
#define _ExportResultDataEntry_h_

class CExportResultData;

#include "StorageNode.h"
#include "GeomecModelVisitor.h"

class CExportResultDataEntry : public CStorageNodeEntry <CExportResultData>
{
public:
  CExportResultDataEntry(CFemAppModel& model);

  void Import(const QString& sFileName);
  void Export(const QString& sFileName);

  CExportResultData *retrieveReservoirMacro() const;
  void initReservoirMacro();

  virtual void LoadStream(CFemAppModel& model, CStorageNodeEntry<CExportResultData>::TSTREAM& stream, CStreamVersion& version, CStorageNodeEntry<CExportResultData>::TPROGRESS& progress);

private:
  CExportResultDataEntry(const CExportResultDataEntry& rhs);
  CExportResultDataEntry& operator = (const CExportResultDataEntry& rhs);

  ACCEPT_GEOMECMODELVISITORS(VisitExportResultDataEntry);
};

#endif  // _ExportResultDataEntry_h_
