#include "FaultFractureList.h"
#include "HorizonBase.h"
#include "FemAppModel.h"
#include "BaseEntryTypes.h"

CFaultFractureList::CFaultFractureList(const CFemAppModel* femAppModel, CDoubleQuantity::UNIT unit)
: m_faultFractureList(retrieveFaultFractureList(femAppModel, unit))
{
}

bool CFaultFractureList::hasFaultFractures(const CFemAppModel* femAppModel)
{
  THorizonBaseEntry::TEntryNodeSet entryNodeSet =
    femAppModel->GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();

  for (THorizonBaseEntry::TEntryNodeSet::const_iterator entryNode =
    entryNodeSet.begin(); entryNode != entryNodeSet.end(); ++entryNode)
  {
    CHorizonBase* horizonBase = static_cast <CHorizonBase*> (*entryNode);

    if (horizonBase->Slip())
    {
      return true;
    }
  }
  
  return false;
}

size_t CFaultFractureList::size() const
{
  return m_faultFractureList.size();
}

CFaultFracture& CFaultFractureList::operator [] (size_t index)
{
  return m_faultFractureList[index];
}

const CFaultFracture& CFaultFractureList::operator [] (size_t index) const
{
  return m_faultFractureList[index];
}

// private

// static

std::vector <CFaultFracture> CFaultFractureList::retrieveFaultFractureList(const CFemAppModel* femAppModel, CDoubleQuantity::UNIT unit)
{
  std::vector <CFaultFracture> faultFractureList;

  THorizonBaseEntry::TEntryNodeSet entryNodeSet =
    femAppModel->GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();

  for (THorizonBaseEntry::TEntryNodeSet::const_iterator entryNode =
    entryNodeSet.begin(); entryNode != entryNodeSet.end(); ++entryNode)
  {
    CHorizonBase* horizonBase = static_cast <CHorizonBase*> (*entryNode);

    if (horizonBase->Slip())
    {
      faultFractureList.push_back(CFaultFracture(horizonBase, unit));
    }
  }
  
  return faultFractureList;
}
