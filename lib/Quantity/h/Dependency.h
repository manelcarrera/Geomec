// Dependency.h: interface for the CDependency class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPENDENCY_H__6109E8BF_6902_4A74_A83B_F44303ACE7D7__INCLUDED_)
#define AFX_DEPENDENCY_H__6109E8BF_6902_4A74_A83B_F44303ACE7D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"
#include <vector>

typedef std::vector<const CDoubleQuantity *> CDependencyVec;

// ##ModelId=3C0DDDA9038B
class CDependency {
private:
protected:
  // ##ModelId=3CAAA65600DF
  CDependencyVec m_qVec;
  // ##ModelId=3C0DDDAA0129
  CDependencyVec GetRemainings(const CDoubleQuantity *pQuantity) const;

public:
  // ##ModelId=3C0DDDAA012C
  CDependency();
  // ##ModelId=3C0DDDAA012D
  virtual ~CDependency();
  // ##ModelId=3C0DDDAA012F
  bool IsMember(const CDoubleQuantity *pQuantity) const;
  // ##ModelId=3C0DDDAA013B
  bool RemainingsUndefined(const CDoubleQuantity *pQuantity) const;

  // ##ModelId=3C60D5640000
  virtual bool Check() const;

  // ##ModelId=3C0DDDAA013E
  virtual std::string GetDependencyString() const = 0;

  // ##ModelId=3C0DDDAA0140
  virtual bool CheckQuantity(const CDoubleQuantity *pQuantity) const = 0;
};

#endif // !defined(AFX_DEPENDENCY_H__6109E8BF_6902_4A74_A83B_F44303ACE7D7__INCLUDED_)
