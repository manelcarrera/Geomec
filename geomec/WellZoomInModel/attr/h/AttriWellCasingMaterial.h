#ifndef _ATTRIWELLCASINGMATERIAL_H_
#define _ATTRIWELLCASINGMATERIAL_H_

#include "ISubListObject.h"

class CWellCasingSteelMaterial;

#include "AttributeTempl.h"
#include "ListCtrlBase.h"

class CAttriWellCasingMaterialDlg : public CAttributesTemplate<CWellCasingSteelMaterial>
{
  typedef CAttributesTemplate<CWellCasingSteelMaterial> TBase;

  class CStressStrainListObject;

public:
  CAttriWellCasingMaterialDlg(CWellCasingSteelMaterial& mat, CWnd* pParent = 0);
  bool CanSetStrain(double dStrain) const;
  void Sort();
  void SetWarning(const CString& sWarning);
  void AddNew();
  void DeleteItem(std::pair<double, double>& prDelete, CStressStrainListObject* pListObject);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

private:
  // TODO AppendContextMenu
  // CStressStrainListObject is not derived from CGraphNode, hence
  // the function AppendContextMenu does not ask for a delegate. However one
  // could verify that this function is never used!

  class CStressStrainListObject : public IListObject
  {
  public:
  typedef std::pair<double, double> TStressStrainPair;

  CStressStrainListObject(CListCtrl& ctrl, TStressStrainPair& prStressStrain, CAttriWellCasingMaterialDlg& dlg, CDoubleQuantity::UNIT unit);
  virtual unsigned int Icon() const;
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
  virtual BOOL EditText(const QString& strText);
  virtual void OnDoubleClick(int nItemIndex);
  double Stress() const;
  double Strain() const;
  BOOL EditStrainText(const CString& strText);
  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  private:
  void DeleteItem();

  private:
  TStressStrainPair& m_prStressStrain;
  CAttriWellCasingMaterialDlg& m_dlg;
  CDoubleQuantity::UNIT m_unit;
  };

  class CStressStrainListSubObject : public ISubListObject
  {
  public:
  CStressStrainListSubObject(CStressStrainListObject& parent);
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
  virtual BOOL EditText(const QString& strText);
  virtual void OnDoubleClick(const CPoint& point);
  };

  class CNewStressStrainListObject : public IListObject
  {
  public:
  CNewStressStrainListObject(CListCtrl& ctrl, CAttriWellCasingMaterialDlg& dlg);
  virtual QString Text() const;
  virtual void OnDoubleClick();
  virtual unsigned int Icon() const;

  private:
  CAttriWellCasingMaterialDlg& m_dlg;

  private:
  class CDummySubListObject : public ISubListObject
  {
  public:
      CDummySubListObject(CNewStressStrainListObject& parent);
      virtual QString Text() const;
  };
  };

private:
  static int CALLBACK ListCtrlCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
  CListCtrlBase m_lbStressStrain;
  CNewStressStrainListObject* m_pNewListObject;
};

#endif // _ATTRIWELLCASINGMATERIAL_H_
