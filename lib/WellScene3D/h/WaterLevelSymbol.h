// WellLevelDrawSpec.h: interface for the CWellLevelDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLLEVELDRAWSPEC_H__F0A051E9_DFA2_471B_A05D_B1D8264B8793__INCLUDED_)
#define AFX_WELLLEVELDRAWSPEC_H__F0A051E9_DFA2_471B_A05D_B1D8264B8793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellDrawSpecBase.h"

#include "WellSymbolBase.h"
#include <qstring.h>

class CWaterLevelSymbol : public CWellDrawSpecBase {
  friend class CWellSceneInterMed;

public:
  virtual void CreateScene();

private:
  CWaterLevelSymbol(CWellSceneInterMed &WellSceneInterMed);
};

#endif // !defined(AFX_WELLLEVELDRAWSPEC_H__F0A051E9_DFA2_471B_A05D_B1D8264B8793__INCLUDED_)
