#include "GetElementSetInfo.h"
#include "BaseEntryTypes.h"
#include "IPointSet.h"
#include "ModelBase.h"

namespace cora {

CGetElementSetInfo::CGetElementSetInfo(const CModelBase *modelBase) : CGetSetInfo(), m_modelBase(modelBase) {}

std::ostream &CGetElementSetInfo::operator()(std::ostream &os) const {
  const TPointSetEntry *pointSetEntry = dynamic_cast<const TPointSetEntry *>(m_modelBase->GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TSortedNodeSet elementSets = pointSetEntry->SortedEntryNodes();
  size_t elementSetsSize = 0;

  for (TPointSetEntry::TSortedNodeSet::const_iterator elementSet = elementSets.begin(); elementSet != elementSets.end();
       ++elementSet) {
    if (isElementSet(**elementSet)) {
      ++elementSetsSize;
    }
  }

  os << elementSetsSize << std::endl;

  for (TPointSetEntry::TSortedNodeSet::const_iterator elementSet = elementSets.begin(); elementSet != elementSets.end();
       ++elementSet) {
    if (isElementSet(**elementSet)) {
      os << "elementset:" << (*elementSet)->Name().toStdString().c_str() << std::endl;
    }
  }

  return os;
}

} // namespace cora

// global

std::ostream &operator<<(std::ostream &os, const cora::CGetElementSetInfo &i) { return i(os); }
