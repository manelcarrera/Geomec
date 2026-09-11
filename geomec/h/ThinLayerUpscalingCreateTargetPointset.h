#ifndef _THINLAYERUPSCALINGCREATETARGETPOINTSET_H_
#define _THINLAYERUPSCALINGCREATETARGETPOINTSET_H_

#include "ListCtrlBase.h"

class CDefineRegular2DGridDlg : public CDialog {
public:
  CDefineRegular2DGridDlg(const CModelBase &model, CWnd *pParent = NULL); // standard constructor
  virtual ~CDefineRegular2DGridDlg();

  geo::CPoint Corner1() const;
  geo::CPoint Corner2() const;
  int NumPointsNorthing() const;
  int NumPointsEasting() const;

  // Dialog Data
  enum { IDD = IDD_DEFINEREGULAR2DGRID_DLG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

private:
  const CModelBase &m_model;
  CLengthQuantity m_Corner1Northing;
  CLengthQuantity m_Corner1Easting;
  CLengthQuantity m_Corner2Northing;
  CLengthQuantity m_Corner2Easting;
  int m_nPointsNorthing;
  int m_nPointsEasting;
};

class ISelectObjectDlg : public CDialog {
public:
  ISelectObjectDlg(const CModelBase &model, CWnd *pParent = NULL);

  const CModelBase &Model() const;

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual CString ObjectTitle() const = 0;
  virtual void CreateListObjects(CListCtrl &ctrl) = 0;

private:
  const CModelBase &m_model;
  CListCtrlBase m_lbObjects;
};

// CSelect2DPointsetDlg dialog

class CPointSet;
class CSelect2DPointsetDlg : public ISelectObjectDlg {
public:
  CSelect2DPointsetDlg(const CModelBase &model, CWnd *pParent = NULL); // standard constructor

  const CPointSet *SelectedPointset() const;

  // set the selection, may be NULL to deselect all
  void SelectPointset(const CPointSet *pPointset);

protected:
  virtual CString ObjectTitle() const;
  virtual void CreateListObjects(CListCtrl &ctrl);

private:
  class CPointsetListObject : public IListObject {
  public:
    CPointsetListObject(const CPointSet &pointset, CSelect2DPointsetDlg &dlg, CListCtrl &ctrl);
    virtual unsigned int Icon() const;
    virtual QString Text() const;
    virtual void SelectionStateChanged(bool bSelected);
    virtual void OnDoubleClick();

  private:
    const CPointSet &m_pointset;
    CSelect2DPointsetDlg &m_dlg;
  };

private:
  const CPointSet *m_pSelectedPointset;
};

class CSurfaceBase;
class CSelectSurfaceDlg : public ISelectObjectDlg {
public:
  CSelectSurfaceDlg(const CModelBase &model, CWnd *pParent = NULL);

  const CSurfaceBase *SelectedSurface() const;

  // set the selection, may be NULL to deselect all
  void SelectSurface(const CSurfaceBase *pSurface);

protected:
  virtual CString ObjectTitle() const;
  virtual void CreateListObjects(CListCtrl &ctrl);

private:
  class CSurfaceListObject : public IListObject {
  public:
    CSurfaceListObject(const CSurfaceBase &surface, CSelectSurfaceDlg &dlg, CListCtrl &ctrl);
    virtual unsigned int Icon() const;
    virtual QString Text() const;
    virtual void SelectionStateChanged(bool bSelected);
    virtual void OnDoubleClick();

  private:
    const CSurfaceBase &m_surface;
    CSelectSurfaceDlg &m_dlg;
  };

private:
  const CSurfaceBase *m_pSelectedSurface;
};

#endif // _THINLAYERUPSCALINGCREATETARGETPOINTSET_H_
