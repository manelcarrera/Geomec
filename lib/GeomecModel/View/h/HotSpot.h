// HotSpot.h: interface for the CHotSpot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOTSPOT_H__28EC9B47_B43F_4645_B7AE_240B5771717E__INCLUDED_)
#define AFX_HOTSPOT_H__28EC9B47_B43F_4645_B7AE_240B5771717E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IColorScaleNode.h"
#include "GeomecModelVisitor.h"

class CHotSpot : public IColorScaleNode
{
  std::pair<double, double> m_MinMaxValue;
  TColor m_nMaxColor;
  TColor m_nMinColor;
  bool m_bCenterIsTransparent;
public:
  // Construction destruction ...
  CHotSpot(unsigned int uName, CFemAppModel &model, TColor min_color, TColor max_color);
  CHotSpot(CFemAppModel &model);
  
  CHotSpot(const CHotSpot &rhs);
  ~CHotSpot();
  
  virtual TColor Color(const TValue& value, const TMinMax& min_max) const;
  bool CenterIsTransparent() const {return m_bCenterIsTransparent;}
  void CenterIsTransparent(const bool bNewVal) {m_bCenterIsTransparent = bNewVal;}
  void SetMinMaxValue(const double& dMin, const double& dMax);
  void SetMaxMinColors(const TColor &nMaxColor, const TColor &nMinColor);
  const std::pair<double, double>& MinMaxValue() const;
  TColor MaxColor() const {return m_nMaxColor;}
  TColor MinColor() const {return m_nMinColor;}
  void MinColor(const TColor color) {m_nMinColor = color;}
  void MaxColor(const TColor color) {m_nMaxColor = color;}

  bool operator == (const CHotSpot &rhs) const;
  CHotSpot& operator = (const CHotSpot &rhs);
  
  // Load and Save..
  virtual bool Empty() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHotSpot);
};

#endif // !defined(AFX_HOTSPOT_H__28EC9B47_B43F_4645_B7AE_240B5771717E__INCLUDED_)
