// MaterialServer.h: interface for the CMaterialServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALSERVER_H__6B3C776C_2124_4ABB_98C3_2557F6238AC0__INCLUDED_)
#define AFX_MATERIALSERVER_H__6B3C776C_2124_4ABB_98C3_2557F6238AC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IRockMaterialServerTempl.h"
#include "MaterialServerParent.h"

class CMaterialServer;
typedef CMaterialServerParent<CColorNode, CMaterialServer> TMaterialServerParent;

class CMaterialServer : public IRockMaterialServerTempl<TMaterialServerParent> {
  typedef IRockMaterialServerTempl<TMaterialServerParent> TBase;

public:
  CMaterialServer(TMaterialServerParent &parent, const CDepletionStage &stage);

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialServer);
};

#endif // !defined(AFX_MATERIALSERVER_H__6B3C776C_2124_4ABB_98C3_2557F6238AC0__INCLUDED_)
