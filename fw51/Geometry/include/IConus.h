// IConus.h: interface for the IConus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONUS_H__CAAC506E_5E05_4E13_AE25_14A5502D34C9__INCLUDED_)
#define AFX_ICONUS_H__CAAC506E_5E05_4E13_AE25_14A5502D34C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IObject.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {
class IPoint;
class IVector;
//*******************************************************************
//              
//  FILE:       IConus.h
//  AUTHOR:     htg
//  PROJECT:    Geometry
//  COMPONENT:  CConus
//  DATE:       08.10.2003
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  COMMENTS:   The ratio is the ratio of the upper radius and lower radius
//				if the ratio is 1 the it is a cilinder		
//              
//*******************************************************************
class GEOMETRY_EXPORT  IConus : public IObject  
{
public:
  virtual ~IConus();
  
  virtual const IPoint&  BasePoint() const =0;
  virtual const IVector& Direction() const =0;
  virtual const double& Radius() const =0;
  virtual const double& Height() const=0;
  virtual const double& Ratio() const=0;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleConus(*this); }

protected:
  IConus();

};

}

#endif // !defined(AFX_ICONUS_H__CAAC506E_5E05_4E13_AE25_14A5502D34C9__INCLUDED_)
