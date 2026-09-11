// Line.h: interface for the CLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_H__199936C1_B558_4952_BB4F_0E014BFE89BF__INCLUDED_)
#define AFX_LINE_H__199936C1_B558_4952_BB4F_0E014BFE89BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point.h"
#include "ILine.h"
#include "Node.h"

#include "GeometryExports.h"

namespace geo {

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The CLine object represents an implementation of a line in 3D space 
// Status      : Finished (100%)
// Remarks     : See ILine
// Last review : 27-03-2002 

// simple 1st order line
class GEOMETRY_EXPORT  CLine : public ILine  
{
  CNode m_first, m_second;
public:

  //The default constructor of CLine constructs an 
  //undefined line for container purposes. Initializing is 
  //done by assignment. 
  CLine();

  //copy-constuctor with interface of line
  CLine(const ILine &rhs);
  CLine(const CLine &rhs);

  //This contructor initializes the line with 2 points
  //under the condition v1 != v2.
  //##ModelId=3B4C71CF00EE
  CLine(const IPoint &first, const IPoint &second);

  //This contructor initializes the line with 1 point
  //and a direction vector ( v1=v, v2=v+dir)
  //##ModelId=3B4C71CF010D
  CLine(const IPoint &p, const IVector &dir);
  

  CLine& operator=(const ILine& rhs);
  CLine& operator=(const CLine& rhs);

  virtual size_t Order() const;

  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint& point);
};

}




#endif // !defined(AFX_LINE_H__199936C1_B558_4952_BB4F_0E014BFE89BF__INCLUDED_)
