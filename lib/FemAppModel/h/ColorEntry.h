// ColorEntry.h: interface for the CColorEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORENTRY_H__45926DD0_4A11_442B_88B4_A717396591FB__INCLUDED_)
#define AFX_COLORENTRY_H__45926DD0_4A11_442B_88B4_A717396591FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "colornode.h"
#include <set>

class CColorEntry : public CGraphEntryTemp<CColorNode> {
  typedef QRgb TColor;
  typedef std::set<TColor> TColorSet;
  TColorSet m_stColor;
  void InitColor(TColor color);

public:
  CColorEntry(CFemAppModel &model);
  virtual ~CColorEntry();
  COLORREF GetColor() const;

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitColorEntry(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitColorEntry(*this); }
};

#endif // !defined(AFX_COLORENTRY_H__45926DD0_4A11_442B_88B4_A717396591FB__INCLUDED_)
