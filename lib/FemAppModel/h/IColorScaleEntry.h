// IColorScaleEntry.h: interface for the IColorScaleEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOLORSCALEENTRY_H__91EF8352_FF3A_4108_90A0_A8B99118FD6C__INCLUDED_)
#define AFX_ICOLORSCALEENTRY_H__91EF8352_FF3A_4108_90A0_A8B99118FD6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IColorScaleNode.h"

class IColorScaleEntry : public CGraphEntryTemp<IColorScaleNode>
{
public:
	// Constructing
	IColorScaleEntry(CFemAppModel& model);
	virtual ~IColorScaleEntry();

	// Interface of color scale entry
	virtual const IColorScaleNode& CurrentColorScale() const = 0;
	virtual IColorScaleNode& CurrentColorScale() = 0;

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitIColorScaleEntry(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitIColorScaleEntry(*this); }
};

#endif // !defined(AFX_ICOLORSCALEENTRY_H__91EF8352_FF3A_4108_90A0_A8B99118FD6C__INCLUDED_)
