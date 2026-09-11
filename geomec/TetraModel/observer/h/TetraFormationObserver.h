// TetraFormationObserver.h: interface for the CTetraFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRAFORMATIONOBSERVER_H__69F7E3BC_4363_4e03_A895_E96E73E19221__INCLUDED_)
#define AFX_TETRAFORMATIONOBSERVER_H__69F7E3BC_4363_4e03_A895_E96E73E19221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DFormation.h"
#include "FormationVolume.h"

#include <set>

class CTetraFormation;
class CTetraModel;
class CTetraHorizonBase;

#include "OpenGLNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "TetraFormation_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CTetraFormation, CTetraFormation_Delegate, CDummyNode, CDummyObserver, FALSE,
                                     FIXED_ITEM>
    TTetraFormationObserver;

class CTetraFormationObserver : public TTetraFormationObserver {
public:
  CTetraFormationObserver(CTetraFormation &formation, CTreeNode &parent, const BOOL rename, const REMOVE_TYPE remove);

  virtual unsigned int StateIcon() const;
  virtual void ToggleState();

private:
  TChildren ElementSetObservers() const;
  BOOL AllSameState() const;
};

class CTetraFormationEntryObserver
    : public CStateBranch_Delegate<CTetraFormationEntry, CTetraFormationEntry_Delegate, CTetraFormation,
                                   CTetraFormationObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM> {
protected:
  virtual CString FormationElementSetTitle() const;
  virtual unsigned int FormationElementSetIcon() const;

public:
  CTetraFormationEntryObserver(CTetraFormationEntry &entry, CTreeCtrl &ctrl, HTREEITEM hParent = TVI_ROOT,
                               HTREEITEM hInsertAfter = TVI_LAST);
  virtual CTreeNode *InsertChild(CTetraFormation &t);
};

#endif // !defined(AFX_TETRAFORMATIONOBSERVER_H__69F7E3BC_4363_4e03_A895_E96E73E19221__INCLUDED_)
