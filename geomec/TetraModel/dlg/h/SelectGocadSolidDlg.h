// SelectGocadSolidDlg.h: interface for the CSelectGocadSolidDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTGOCADSOLIDDLG_H__54D0B8CC_B230_47D5_9072_4F01CDF58434__INCLUDED_)
#define AFX_SELECTGOCADSOLIDDLG_H__54D0B8CC_B230_47D5_9072_4F01CDF58434__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListCtrlBase;

#include "ISubListObject.h"

class CGocadSolidListCtrl;

#ifdef SKUA_NEW
namespace gm_skua {
struct SKUAParseData;
}
#else
#include "GocadData.h"
#include "GocadImport.h"
#endif

class CSelectGocadSolidDlg : public CDialog {
public:
#ifdef SKUA_NEW
  CSelectGocadSolidDlg(const std::vector<const gm_skua::SKUAParseData *> &solids, CWnd *pParent = 0);
#else
  CSelectGocadSolidDlg(const std::vector<QSharedPointer<CGocadData::CTSolid>> &solids, CWnd *pParent = 0);
#endif
  virtual ~CSelectGocadSolidDlg();

  int SelectedSize() const;
  int SelectedEntry(int nIndex) const;

  void SelectionChanged();
  void OnDoubleClick(int nIndex);

protected:
  void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();

private:
  class CGocadSolidListObject : public IListObject {
  public:
    CGocadSolidListObject(CListCtrlBase &ctrl, const CString &sName, int nVolumes, int nTetras);
    virtual QString Text() const;
    virtual BOOL CanEditText() const;
    virtual BOOL EditText(const QString &strText);
    virtual unsigned int Icon() const;
    virtual void OnDoubleClick();

  private:
    CString m_sName;
  };

  class CIntSubListObject : public ISubListObject {
  public:
    CIntSubListObject(CGocadSolidListObject &parent, int nColumn, int nValue);
    virtual QString Text() const;
    virtual BOOL CanEditText() const;
    virtual BOOL EditText(const QString &strText);

  private:
    int m_nValue;
  };

  CListCtrlBase *m_plcList;
#ifdef SKUA_NEW
  const std::vector<const gm_skua::SKUAParseData *> &m_solids;
#else
  const std::vector<QSharedPointer<CGocadData::CTSolid>> &m_solids;
#endif
  std::vector<int> m_vcSelected;
};

#endif // !defined(AFX_SELECTGOCADSOLIDDLG_H__54D0B8CC_B230_47D5_9072_4F01CDF58434__INCLUDED_)
