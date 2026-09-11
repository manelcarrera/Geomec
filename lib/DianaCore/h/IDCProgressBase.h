// IProgressBase.h: interface for the IProgressBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROGRESSBASE_H__D47F1390_321E_4B46_8514_5E5695CEDF2F__INCLUDED_)
#define AFX_IPROGRESSBASE_H__D47F1390_321E_4B46_8514_5E5695CEDF2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace dia {

class IProgressBase  
{
public:
  IProgressBase();
  virtual ~IProgressBase();

  virtual void Begin(int nSteps) = 0;
  virtual void End() = 0;
  virtual void Step(bool &bContinue) = 0;
  virtual void Text(const std::string &strText) = 0;
};

}

#endif // !defined(AFX_IPROGRESSBASE_H__D47F1390_321E_4B46_8514_5E5695CEDF2F__INCLUDED_)
