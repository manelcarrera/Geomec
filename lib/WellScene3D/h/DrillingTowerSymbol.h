// DrillingTowerSymbol.h: interface for the CDrillingTowerSymbol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRILLINGTOWERSYMBOL_H__D9631AA1_0129_42B6_8936_D655E77B2851__INCLUDED_)
#define AFX_DRILLINGTOWERSYMBOL_H__D9631AA1_0129_42B6_8936_D655E77B2851__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellDrawSpecBase.h"

class CDrillingTowerSymbol : public CWellDrawSpecBase {
  friend class CWellSceneInterMed;

public:
  virtual void CreateScene();

private:
  CDrillingTowerSymbol(CWellSceneInterMed &WellSceneInterMed);
};

#endif // !defined(AFX_DRILLINGTOWERSYMBOL_H__D9631AA1_0129_42B6_8936_D655E77B2851__INCLUDED_)
