// IBox.h: interface for the IBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBOX_H__FE86F108_9EED_40BC_A2BA_0D7A22AD9BAF__INCLUDED_)
#define AFX_IBOX_H__FE86F108_9EED_40BC_A2BA_0D7A22AD9BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IHexahedron.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The IBox object represents an rectangular box in 3D space 
// Status      : Finished (100%)
// Remarks     : See IBody
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  IBox  : public IHexahedron
{
public:
  IBox();
  IBox(const IBox& box);
  virtual ~IBox();

  virtual double Width() const = 0;
  virtual double Depth() const = 0;
  virtual double Height() const = 0;

  virtual double Size() const;
  virtual bool Visit(IVisitor &visitor) { return visitor.HandleBox(*this); }
};
}
#endif // !defined(AFX_IBOX_H__FE86F108_9EED_40BC_A2BA_0D7A22AD9BAF__INCLUDED_)
