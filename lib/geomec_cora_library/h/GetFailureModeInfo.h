#ifndef _cora_GetFailureModeInfo_h_
#define _cora_GetFailureModeInfo_h_

#include <fstream>
#include <vector>

#include <QString>

class CModelBase;

#include "IStressTensorGroup.h"
#include "FilterResults.h"
#include "FilterOutputProperty.h"
#include "FailureMode.h"

namespace cora
{

class CGetFailureModeInfo
{
  public:
  CGetFailureModeInfo(CModelBase* modelBase, bool annotated = false);

  std::vector <QString> getExportLabels() const;
  TFailureModes getFailureModes();

  std::ostream& operator () (std::ostream& os) const;

  private:
  CGetFailureModeInfo(const CGetFailureModeInfo& rhs);
  CGetFailureModeInfo& operator = (CGetFailureModeInfo rhs);

  static TFailureModes getFailureModes(std::vector <QString>& exportLabels,
      CModelBase* modelBase, bool annotated,
      const CFilterResults& filterResults,
      const CFilterOutputProperty& filterOutputProperty);
  static void getFailureModes(TFailureModes& failureModes,
      std::vector <QString>& exportLabels, CModelBase* modelBase,
      const IResult* result, bool annotated,
      const CFilterResults& filterResults,
      const CFilterOutputProperty& filterOutputProperty);

  std::vector <QString> m_exportLabels;
  CFilterResults m_filterResults;
  CFilterOutputProperty m_filterOutputProperty;
  TFailureModes m_failureModes;
  CModelBase* m_modelBase;
  bool m_annotated;
};

} // namespace cora

std::ostream& operator << (std::ostream& os, const cora::CGetFailureModeInfo& i);

#endif  // _cora_GetFailureModeInfo_h_
