// MaterialServer.cpp: implementation of the CMaterialServer class.
//
//////////////////////////////////////////////////////////////////////

#include "MaterialServer.h"
#include "DepletionStage.h"

CMaterialServer::CMaterialServer(TMaterialServerParent &parent, const CDepletionStage &stage) : TBase(parent, stage) {
  reParent(&parent);
  LinkTo(const_cast<CDepletionStage &>(stage));
}
