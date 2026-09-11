
#include "NewWellSection.h"
#include "NewWellPathBase.h"
#include "NewWellPoint.h"

INewWellSection::INewWellSection(const CNewWellPathBase &WellPath) : m_pNewWellPath(&WellPath) {}

INewWellSection::~INewWellSection() {}

bool INewWellSection::Contains(const CNewWellPoint &point, bool bIncludeEdge) const {
  if (point < Top())
    return false;

  if (point > Bottom())
    return false;

  if (point == Top())
    return bIncludeEdge;

  if (point == Bottom())
    return bIncludeEdge;

  return true;
}

bool INewWellSection::Contains(const double &tmd, bool bIncludeEdge) const {
  return Contains(CNewWellPoint(*m_pNewWellPath, tmd), bIncludeEdge);
}

CNewWellSection::CNewWellSection(CNewWellPathBase &wellPath, const double &topTMD, const double &bottomTMD)
    : INewWellSection(wellPath), m_Top(wellPath, topTMD), m_Bottom(wellPath, bottomTMD) {}

CNewWellSection::~CNewWellSection() {}

const CNewWellPoint &CNewWellSection::Top() const { return m_Top; }

const CNewWellPoint &CNewWellSection::Bottom() const { return m_Bottom; }

std::string CNewWellSection::GetDescription() const { return m_Description; }
