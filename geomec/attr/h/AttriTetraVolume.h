#ifndef _ATTRITETRAVOLUME_H_
#define _ATTRITETRAVOLUME_H_

#include "AttriFormationElementSet.h"
#include "TetraFormation.h"
#include "IListObject.h"
#include "ListCtrlBase.h"

class CAttriTetraVolume : public CAttriFormationElementSet<CTetraFormationVolume>
{
private:
  class CSurfaceListObject : public IListObject
  {
  private:
  QString m_sName;
  int m_nIconId;

  public:
  CSurfaceListObject(CListCtrl &ctrl, QString sName, int nIconId);
  ~CSurfaceListObject();
  virtual QString Text() const;
  virtual unsigned int Icon() const;
  };

public:
  CAttriTetraVolume(CTetraFormationVolume& volume, CWnd* pParent = 0);

  virtual BOOL OnInitDialog();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
  void OnUpdateLb();

private:
  CListCtrlBase m_lbSurfaces;
};

#endif // _ATTRITETRAVOLUME_H_
