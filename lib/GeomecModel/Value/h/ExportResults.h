#ifndef _ExportResults_h_
#define _ExportResults_h_

class CExportResultData;
class IResultExporter;

#include "ExportFormat.h"

class CExportResults {
public:
  CExportResults(const CExportResultData &exportResultData, CModelBase &model);

  bool onExport(const QString &sFileName, bool bExcel, bool bOld, bool export2LAS = false);

  static bool isInputResult(const IResult *result);

private:
  CExportResults(const CExportResults &rhs);
  CExportResults &operator=(const CExportResults &rhs);

  void FillResultVector(IExportFormat::TDataVec &vcRet, const CAnalysisType &antype, CQuantity::UNIT unit,
                        const COpenGLNode &node, bool fillInput = false);
  void FillMatResultVector(IExportFormat::TDataVec &vcRet, CQuantity::UNIT unit, const COpenGLNode &node);
  std::vector<const IResultExporter *> GetResultExporters();

private:
  const CExportResultData &m_exportResultData;
  CModelBase &m_model;
};

#endif // _ExportResults_h_
