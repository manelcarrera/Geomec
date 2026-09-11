// IColorScaleNode.h: interface for the IColorScaleNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOLORSCALENODE_H__7ED5861E_584D_4617_A353_2C9BBA94A472__INCLUDED_)
#define AFX_ICOLORSCALENODE_H__7ED5861E_584D_4617_A353_2C9BBA94A472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qrgb.h>

#include "StorageNode.h"
#include "ivaluecomponent.h"

class IColorScaleNode : public CStorageNode 
{
public:
	// Typedefs for values supplied by the value component
	typedef geo::CValue TValue;
	typedef IValueDomainScalar::TValueVec TValueVec;
	typedef IValueDomainScalar::TMinMax TMinMax;
	typedef QRgb TColor;

	IColorScaleNode(CFemAppModel &model);
	IColorScaleNode(const unsigned int uName, CFemAppModel &model);
	IColorScaleNode(const QString& strName, CFemAppModel &model);
	IColorScaleNode(const IColorScaleNode &rhs);

	virtual TColor Color(const TValue& value, const TMinMax &MinMax) const = 0;

	bool operator==(const IColorScaleNode& rhs) const;
	IColorScaleNode& operator=(const IColorScaleNode& rhs);

	void SetIndex(int nIndex);

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitIColorScaleNode(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitIColorScaleNode(*this); }

protected:
	TColor LoadColor(TSTREAM& stream);
	void SaveColor(TSTREAM& stream, TColor color);
};

#endif // !defined(AFX_ICOLORSCALENODE_H__7ED5861E_584D_4617_A353_2C9BBA94A472__INCLUDED_)
