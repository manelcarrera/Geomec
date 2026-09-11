#include "GetPointSetInfo.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"

namespace cora
{

CGetPointSetInfo::CGetPointSetInfo(const CModelBase* modelBase)
: CGetSetInfo()
, m_modelBase(modelBase)
{
}

std::ostream& CGetPointSetInfo::operator () (std::ostream& os) const
{
  listPointSets(os, "pointset2d:", IPointSet::DIM_2D);
  listPointSets(os, "pointset3d:", IPointSet::DIM_3D);

  return os;
}

// private

void CGetPointSetInfo::listPointSets(std::ostream& os, const QString& preFix,
  IPointSet::DIMENSION dimension) const
{
  const TPointSetEntry* pointSetEntry = dynamic_cast <const TPointSetEntry*> (
  m_modelBase->GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TSortedNodeSet
  pointSets = pointSetEntry->SortedEntryNodes();
  size_t pointSetsSize = 0;

  for (TPointSetEntry::TSortedNodeSet::const_iterator
  pointSet = pointSets.begin(); pointSet != pointSets.end(); ++pointSet)
  {
  if ((!isElementSet(**pointSet)) && ((*pointSet)->Dimension() == dimension))
  {
      ++pointSetsSize;
  }
  }

  os << pointSetsSize << std::endl;

  for (TPointSetEntry::TSortedNodeSet::const_iterator
  pointSet = pointSets.begin(); pointSet != pointSets.end(); ++pointSet)
  {
  if ((!isElementSet(**pointSet)) && ((*pointSet)->Dimension() == dimension))
  {
      os << preFix.toStdString().c_str() <<
    (*pointSet)->Name().toStdString().c_str() << std::endl;
  }
  }
}

} // namespace cora

// global

std::ostream& operator << (std::ostream& os, const cora::CGetPointSetInfo& i)
{
  return i(os);
}
