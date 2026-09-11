#include "stdafx.h"
#include "geomec.h"

#include "AttriWellCasingMaterial.h"

#include "WellCasingSteelMaterial.h"
#include "FieldFactor.h"
#include "GlobalMessage.h"
#include "ContextMenuInvoker.h"
#include "resourceIDI.h"

static const int MAX_NUM_SS_ITEMS = 10;

CAttriWellCasingMaterialDlg::CAttriWellCasingMaterialDlg(CWellCasingSteelMaterial& mat, CWnd* pParent)
: TBase(IDD_ATTRI_WELLCASINGSTEEL, mat, pParent),
  m_pNewListObject(0)
{
}

bool CAttriWellCasingMaterialDlg::CanSetStrain(double dStrain) const
{
  CWellCasingSteelMaterial::CStressStrain::const_iterator it;
  for(it = Copy().StressStrain().begin(); it != Copy().StressStrain().end(); ++it)
  {
  if(it->second == dStrain)
      return false;
  }

  return true;
}

void CAttriWellCasingMaterialDlg::Sort()
{
  Copy().StressStrain().sort();
  m_lbStressStrain.SortItems(ListCtrlCompareFunc, 0);
}

void CAttriWellCasingMaterialDlg::SetWarning(const CString& sWarning)
{
  GetDlgItem(IDC_LBL_WARNING)->SetWindowText(sWarning);
}

void CAttriWellCasingMaterialDlg::AddNew()
{
  CWellCasingSteelMaterial::CStressStrain& ss = Copy().StressStrain();
  if(ss.empty())
  ss.push_back(std::make_pair(10000., 0.0001));
  else if(ss.size() == 1)
  {
  CWellCasingSteelMaterial::CStressStrain::TStressStrainPair pr = *ss.begin(); // copy
  pr.first += 1000;
  pr.second += 0.0001;
  ss.push_back(pr);
  }
  else
  {
  CWellCasingSteelMaterial::CStressStrain::reverse_iterator rit = ss.rbegin();
  CWellCasingSteelMaterial::CStressStrain::TStressStrainPair prLast = *rit;
  CWellCasingSteelMaterial::CStressStrain::TStressStrainPair prNext = *(++rit);
  prLast.first += (prLast.first - prNext.first);
  prLast.second += (prLast.second - prNext.second);
  ss.push_back(prLast);
  }

  new CStressStrainListObject(m_lbStressStrain, *ss.rbegin(), *this, UnitNode().Unit());
  Sort();

  if(ss.size() == MAX_NUM_SS_ITEMS) // max
  {
  delete m_pNewListObject;
  m_pNewListObject = 0;
  }
}

void CAttriWellCasingMaterialDlg::DeleteItem(std::pair<double, double>& prDelete, CStressStrainListObject* pListObject)
{
  CWellCasingSteelMaterial::CStressStrain::iterator it;
  for(it = Copy().StressStrain().begin(); it != Copy().StressStrain().end(); ++it)
  {
  if(&*it == &prDelete)
  {
      Copy().StressStrain().erase(it);
      delete pListObject;
      break;
  }
  }

  if(Copy().StressStrain().size() < MAX_NUM_SS_ITEMS && m_pNewListObject == 0)
  m_pNewListObject = new CNewStressStrainListObject(m_lbStressStrain, *this);
}

void CAttriWellCasingMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
  CString strName;
  double dYoung;
  CString strYoungUnit;
  double dPoisson;
  double dDensity;
  CString strDensityUnit;
  double dThermX;
  CString strThermXUnit;
  double dThermConduc;
  CString strThermConducUnit;
  double dThermCapaci;
  CString strThermCapaciUnit;
  int nPerforated;

  CThermalExpansionQuantity qnThermX(0, 0, 1e-3, true, true);

  if(!pDX->m_bSaveAndValidate)
  {
  strName = Copy().Name().toStdString().c_str();
  dPoisson = Copy().PoissonsRatio();
  dThermX = Copy().ThermalExpansion();
  strThermXUnit = "C";
  nPerforated = (Copy().Perforated() ? 1 : 0);

  switch(UnitNode().Unit())
  {
  case CDoubleQuantity::SI_UNIT:
      dYoung = Copy().YoungsModulus();
      strYoungUnit = "MPa";
      dDensity = Copy().Density();
      strDensityUnit = "kg/m3";
      dThermConduc = Copy().ThermalConductivity();
      strThermConducUnit = "W/m/K";
      dThermCapaci = Copy().ThermalCapacity();
      strThermCapaciUnit = "J/kg/K";
      break;
  case CDoubleQuantity::FIELD_UNIT:
      dYoung = Copy().YoungsModulus() * FF_FACTOR_STRESS;
      strYoungUnit = "psi";
      dDensity = Copy().Density() * FF_FACTOR_DENSITY;
      strDensityUnit = "lb/gal";
      dThermConduc = Copy().ThermalConductivity() * FF_FACTOR_THERM_CONDUCT;
      strThermConducUnit = "W/ft/K";
      dThermCapaci = Copy().ThermalCapacity() * FF_FACTOR_THERM_CAPACI;
      strThermCapaciUnit = "J/lb/K";
      break;
  default:
      assert(FALSE);
  }
  }

  DDX_Control(pDX, IDC_LST_STRESS_STRAIN, m_lbStressStrain);
  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ED_YOUNG, dYoung);
  DDX_Text(pDX, IDC_UN_YOUNG, strYoungUnit);
  DDX_Text(pDX, IDC_ED_POISSON, dPoisson);
  DDX_Text(pDX, IDC_ED_DENSITY, dDensity);
  DDX_Text(pDX, IDC_UN_DENSITY, strDensityUnit);
  DDX_Text(pDX, IDC_ED_THERMX, dThermX);
  DDX_Text(pDX, IDC_UN_THERMX, strThermXUnit);
  DDX_Text(pDX, IDC_ED_THERMCONDUC, dThermConduc);
  DDX_Text(pDX, IDC_UN_THERMCONDUC, strThermConducUnit);
  DDX_Text(pDX, IDC_ED_THERMCAPACI, dThermCapaci);
  DDX_Text(pDX, IDC_UN_THERMCAPACI, strThermCapaciUnit);
  DDX_Check(pDX, IDC_CH_PERFORATED, nPerforated);

  if(pDX->m_bSaveAndValidate)
  {
  if(strName.IsEmpty())
  {
      _m()->msg("Please enter a valid name");
      pDX->PrepareEditCtrl(IDC_ED_NAME);
      pDX->Fail();
  }

  if(UnitNode().Unit() == CDoubleQuantity::FIELD_UNIT)
  {
      dYoung /= FF_FACTOR_STRESS;
      dDensity /= FF_FACTOR_DENSITY;
      dThermConduc /= FF_FACTOR_THERM_CONDUCT;
      dThermCapaci /= FF_FACTOR_THERM_CAPACI;
  }

  if(dYoung < 0)
  {
      _m()->msg("Young's modulus must be greater than or equal to 0");
      pDX->PrepareEditCtrl(IDC_ED_YOUNG);
      pDX->Fail();
  }

  if(dPoisson < 0 || dPoisson >= 0.5)
  {
      _m()->msg("Poisson's ratio must greater than or equal to 0 and less than 0.5");
      pDX->PrepareEditCtrl(IDC_ED_POISSON);
      pDX->Fail();
  }

  if(dDensity < 0)
  {
      _m()->msg("Density must be greater than or equal to 0");
      pDX->PrepareEditCtrl(IDC_ED_DENSITY);
      pDX->Fail();
  }

  if(dThermX < 0 || dThermX > 1e-3)
  {
      _m()->msg("Thermal Expansion Coefficient must be at least 0 and at most 1e-3");
      pDX->PrepareEditCtrl(IDC_ED_THERMX);
      pDX->Fail();
  }

  if(dThermConduc < 0)
  {
      _m()->msg("Thermal Conductivity must be at least 0");
      pDX->PrepareEditCtrl(IDC_ED_THERMCONDUC);
      pDX->Fail();
  }

  if(dThermCapaci < 0)
  {
      _m()->msg("Thermal Capacity must be at least 0");
      pDX->PrepareEditCtrl(IDC_ED_THERMCAPACI);
      pDX->Fail();
  }

  Copy().Name((LPCSTR) strName);
  Copy().SetYoungsModulus(dYoung);
  Copy().SetPoissonsRatio(dPoisson);
  Copy().SetDensity(dDensity);
  Copy().SetThermalExpansion(dThermX);
  Copy().SetThermalConductivity(dThermConduc);
  Copy().SetThermalCapacity(dThermCapaci);
  Copy().SetPerforated(nPerforated != 0);
  }
}

BOOL CAttriWellCasingMaterialDlg::OnInitDialog()
{
  TBase::OnInitDialog();

  CRect rect;
  m_lbStressStrain.GetWindowRect(&rect);
  CString sUnit = (UnitNode().Unit() == CDoubleQuantity::SI_UNIT ? "MPa" : "psi");
  CString sStress;
  sStress.Format("Stress [%s]", sUnit);
  m_lbStressStrain.InsertColumn(0, sStress, LVCFMT_LEFT, (rect.Width() - 4) * 0.5, 0);
  m_lbStressStrain.InsertColumn(1, "Plastic Strain [-]", LVCFMT_LEFT, (rect.Width() - 4) * 0.5, 1);

  CWellCasingSteelMaterial::CStressStrain::iterator it;
  for(it = Copy().StressStrain().begin(); it != Copy().StressStrain().end(); ++it)
  new CStressStrainListObject(m_lbStressStrain, *it, *this, UnitNode().Unit());

  if(Copy().StressStrain().size() < MAX_NUM_SS_ITEMS)
  m_pNewListObject = new CNewStressStrainListObject(m_lbStressStrain, *this);

  return TRUE;
}

BEGIN_MESSAGE_MAP(CAttriWellCasingMaterialDlg, CDialog)
END_MESSAGE_MAP()

int CALLBACK CAttriWellCasingMaterialDlg::ListCtrlCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  const IListObject* pListObject1 = (const IListObject*)lParam1;
  const IListObject* pListObject2 = (const IListObject*)lParam2;

  const CAttriWellCasingMaterialDlg::CNewStressStrainListObject* pNewObject1 = dynamic_cast<const CAttriWellCasingMaterialDlg::CNewStressStrainListObject*>(pListObject1);
  const CAttriWellCasingMaterialDlg::CNewStressStrainListObject* pNewObject2 = dynamic_cast<const CAttriWellCasingMaterialDlg::CNewStressStrainListObject*>(pListObject2);

  if(pNewObject1 && pNewObject2)
  return 0;
  else if(pNewObject1)
  return 1;
  else if(pNewObject2)
  return -1;

  const CAttriWellCasingMaterialDlg::CStressStrainListObject* pObject1 = (const CAttriWellCasingMaterialDlg::CStressStrainListObject*)lParam1;
  const CAttriWellCasingMaterialDlg::CStressStrainListObject* pObject2 = (const CAttriWellCasingMaterialDlg::CStressStrainListObject*)lParam2;

  if(pObject1->Strain() < pObject2->Strain())
  return -1;
  if(pObject1->Strain() > pObject2->Strain())
  return 1;
  return 0;
}

/////

CAttriWellCasingMaterialDlg::CStressStrainListObject::CStressStrainListObject(CListCtrl& ctrl, TStressStrainPair& prStressStrain, CAttriWellCasingMaterialDlg& dlg, CDoubleQuantity::UNIT unit)
: IListObject(ctrl),
  m_prStressStrain(prStressStrain),
  m_dlg(dlg),
  m_unit(unit)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CStressStrainListSubObject(*this);
}

unsigned int CAttriWellCasingMaterialDlg::CStressStrainListObject::Icon() const
{
  return IDI_SIGMA_EPSILON;
}

QString CAttriWellCasingMaterialDlg::CStressStrainListObject::Text() const
{
  QString s;
  double dFactor = (m_unit == CDoubleQuantity::SI_UNIT ? 1 : FF_FACTOR_STRESS);
  s = QString("%1").arg(m_prStressStrain.first * dFactor);
  return s;
}

BOOL CAttriWellCasingMaterialDlg::CStressStrainListObject::CanEditText() const
{
  return TRUE;
}

BOOL CAttriWellCasingMaterialDlg::CStressStrainListObject::EditText(const QString& strText)
{
  QString qs(strText);
  bool b;
  double d = qs.toDouble(&b);
  if(!b)
  {
  m_dlg.SetWarning("Invalid value");
  return FALSE;
  }

  if(d < 0)
  {
  m_dlg.SetWarning("Stresses must be greater than 0");
  return FALSE;
  }

  double dFactor = (m_unit == CDoubleQuantity::SI_UNIT ? 1 : FF_FACTOR_STRESS);
  m_prStressStrain.first = d / dFactor;
  m_dlg.SetWarning("");
  return TRUE;
}

void CAttriWellCasingMaterialDlg::CStressStrainListObject::OnDoubleClick(int nItemIndex)
{
  if(!nItemIndex)
  Ctrl().EditLabel(Index());
}

double CAttriWellCasingMaterialDlg::CStressStrainListObject::Stress() const
{
  return m_prStressStrain.first;
}

double CAttriWellCasingMaterialDlg::CStressStrainListObject::Strain() const
{
  return m_prStressStrain.second;
}

BOOL CAttriWellCasingMaterialDlg::CStressStrainListObject::EditStrainText(const CString& strText)
{
  QString qs(strText);
  bool b;
  double dStrain = qs.toDouble(&b);
  if(!b)
  {
  m_dlg.SetWarning("Invalid value");
  return FALSE;
  }

  if(dStrain < 0)
  {
  m_dlg.SetWarning("Strains must be greater than 0");
  return FALSE;
  }

  if(!m_dlg.CanSetStrain(dStrain))
  {
  m_dlg.SetWarning("Strain values must be unique");
  return false;
  }

  m_prStressStrain.second = dStrain;
  m_dlg.Sort();
  m_dlg.SetWarning("");
  return true;
}

/* NOT OBSOLETE */ void CAttriWellCasingMaterialDlg::CStressStrainListObject::AppendContextMenu(CContextMenuInvoker& invoker)
{
  if(Index() > 0)
  invoker.AddCommand("Delete", *new CSingleCommandTemplate<CStressStrainListObject>(*this, &CStressStrainListObject::DeleteItem));
}

void CAttriWellCasingMaterialDlg::CStressStrainListObject::DeleteItem()
{
  m_dlg.DeleteItem(m_prStressStrain, this);
}


/////

CAttriWellCasingMaterialDlg::CStressStrainListSubObject::CStressStrainListSubObject(CStressStrainListObject& parent)
: ISubListObject(parent, 1)
{
}

QString CAttriWellCasingMaterialDlg::CStressStrainListSubObject::Text() const
{
  const CStressStrainListObject& parent = static_cast<const CStressStrainListObject&>(ListObject());
  QString s;
  s = QString("%1").arg(parent.Strain());
  return s;
}

BOOL CAttriWellCasingMaterialDlg::CStressStrainListSubObject::CanEditText() const
{
  return (ListObject().Index() > 0);
}

BOOL CAttriWellCasingMaterialDlg::CStressStrainListSubObject::EditText(const QString& strText)
{
  CStressStrainListObject& parent = static_cast<CStressStrainListObject&>(ListObject());
  return parent.EditStrainText(strText.toStdString().c_str());
}

void CAttriWellCasingMaterialDlg::CStressStrainListSubObject::OnDoubleClick(const CPoint& point)
{
  if(CanEditText())
  {
  CListCtrlBase& ctrl = static_cast<CListCtrlBase&>(Ctrl());
  ctrl.EditSubLabel(std::make_pair(ListObject().Index(), Index()));
  }
}


/////

CAttriWellCasingMaterialDlg::CNewStressStrainListObject::CNewStressStrainListObject(CListCtrl& ctrl, CAttriWellCasingMaterialDlg& dlg)
: IListObject(ctrl),
  m_dlg(dlg)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CDummySubListObject(*this);
}

QString CAttriWellCasingMaterialDlg::CNewStressStrainListObject::Text() const
{
  return "<add new>";
}

void CAttriWellCasingMaterialDlg::CNewStressStrainListObject::OnDoubleClick()
{
  m_dlg.AddNew();
}

unsigned int CAttriWellCasingMaterialDlg::CNewStressStrainListObject::Icon() const
{
  return IDI_NEW_ITEM;
}


/////

CAttriWellCasingMaterialDlg::CNewStressStrainListObject::CDummySubListObject::CDummySubListObject(CNewStressStrainListObject& parent)
: ISubListObject(parent, 1)
{
}

QString CAttriWellCasingMaterialDlg::CNewStressStrainListObject::CDummySubListObject::Text() const
{
  return QString();
}
