// ColorScaleNode.h: interface for the CColorScaleNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCALENODE_H__BC907C2C_50A3_452D_B153_E54562C2ADB4__INCLUDED_)
#define AFX_COLORSCALENODE_H__BC907C2C_50A3_452D_B153_E54562C2ADB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColorScaleEntry;
class CColorScaleNode : public CStorageNode {
  friend class CColorScaleEntry;

protected:
  COLORREF LoadColor(TSTREAM &stream);
  void SaveColor(TSTREAM &stream, COLORREF color);

public:
  CColorScaleNode(CGraphModel &model);
  CColorScaleNode(const UINT uName, CGraphModel &model);
  CColorScaleNode(const CString &strName, CGraphModel &model);
  CColorScaleNode(const CColorScaleNode &rhs);

  virtual COLORREF Color(const double &dVal, const IValueComponent::TMinMax &MinMax) const = 0;

  bool operator==(const CColorScaleNode &rhs) const;
  CColorScaleNode &operator=(const CColorScaleNode &rhs);

  virtual const IValueComponent::TMinMax &MinMaxValue() const = 0;
  virtual void SetMinMaxValue(const IValueComponent::TMinMax &min_max) = 0;

  // save and load
};

#endif // !defined(AFX_COLORSCALENODE_H__BC907C2C_50A3_452D_B153_E54562C2ADB4__INCLUDED_)
