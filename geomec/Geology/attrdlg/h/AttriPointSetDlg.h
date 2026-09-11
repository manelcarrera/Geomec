// AttriPointSetDlg.h: interface for the CAttriPointSetTemplate<T> class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIPOINTSETDLG_H__7DBF9677_72A9_4502_98A6_ECE16C7D4738__INCLUDED_)
#define AFX_ATTRIPOINTSETDLG_H__7DBF9677_72A9_4502_98A6_ECE16C7D4738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AttriPointSetUtil.h"
#include "AttributesTemplate.h"
#include "ElementSet.h"
#include "GeomecStringTable.h"
#include "GlobalMessage.h"
#include "ISettings.h"
#include "PointSet.h"
#include "RpnDlg.h"
#include "RpnValueSet.h"
#include "RpnValueSet_Delegate.h"
#include "ValueComponent.h"
#include "ValueTypeFactory.h"
#include "ValueTypes.h"

#define COMMAND_OFFSET 200

#define ID_MENU_YOUNGS_MODULUS -1
#define ID_MENU_POISSONS_RATIO -2
#define ID_MENU_THERMAL_PARAMETERS -3
#define ID_MENU_FRACTURE_PARAMETERS -4
#define ID_MENU_HARDENING_PARAMETERS -5
#define ID_MENU_RIGIDITY_PARAMETERS -6

template <class T> class CAttriPointSetTemplate : public CAttributesTemplate<T> {
public:
  typedef CTreeCtrlTemp<CAttriPointSetTemplate<T>> TTreeCtrl;
  typedef CListCtrlTemp<CAttriPointSetTemplate<T>> TListCtrl;

private:
  // { waij TFS 81622
  std::map<unsigned int, const CBitmap *> m_mpBitmap;
  CBrush *m_pBrush;
  void CreateBitmaps();
  void FreeBitmaps();
  const CBitmap *FindBitmap(unsigned int ValueTypeId);
  const CBitmap *CreateBitmap(unsigned int id);
  void WhiteBackgroundColor(CMenu &menu);
  // }

  typedef std::pair<unsigned int, unsigned int> TValuePair; // TypeId and TNameId
  typedef std::map<unsigned int, TValuePair> TCommandMap;
  TCommandMap m_mpCommand;
  TValuePair m_prValue;
  CValueTypeTreeObject *m_pCoordinate;
  TListCtrl *m_pListCtrl;
  TTreeCtrl *m_pTreeCtrl;

  void OnUpdateMinMax();
  CButton m_btInsertValueSet;
  CButton m_btBrowseQuantity;
  CButton m_btInsertQuantity;
  // Helpers
  CString PointToString(const geo::IPoint &pt) const;
  void AppendValueTypeToMenu(CMenu &menu, unsigned int uQuantityId, unsigned int uQuantityNameId,
                             const CBitmap *pBitmap = 0);
  void SetUnitRadioButton(unsigned int uRbSI, unsigned int uRbField, CQuantity::UNIT unit);

  void update_list_cols_width();

  CHeaderCtrl *GetList_HeaderCtrl() const { return (CHeaderCtrl *)(ListCtrl().GetDlgItem(0)); };
  int GetList_ColumnsCount() const { return GetList_HeaderCtrl()->GetItemCount(); };

public:
  // Construction
  CAttriPointSetTemplate(T &set, CWnd *pParent = 0); // standard constructor
  virtual ~CAttriPointSetTemplate();

  // Controls
  TTreeCtrl &TreeCtrl() const;
  TListCtrl &ListCtrl() const;

  BOOL Validate(CDataExchange *pDX);

  // Handlers
protected:
  virtual BOOL OnInitDialog();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
  void OnBrowseQuantity();
  void OnInsertQuantity();
  void OnInsertValueSet();
  void OnCoordinateFieldUnit();
  void OnCoordinateSiUnit();
  void OnValueFieldUnit();
  void OnValueSiUnit();
};

//
// Horizontal scroll (only for CListCtrl, not for CListBox!!)
// CListCtrl - get the length of the item text
// https://stackoverflow.com/questions/9405824/clistctrl-get-the-length-of-the-item-text
//
// ListCtrl().SetColumnWidth(0,LVSCW_AUTOSIZE_USEHEADER); // dont work
// can we assume always one column ?
//
template <class T> void CAttriPointSetTemplate<T>::update_list_cols_width() {
  int column = 0;
  // int num_rows = Copy().ValueSetSize();
  int num_rows = ListCtrl().GetItemCount();
  int num_cols = GetList_ColumnsCount();

  CSize sz;
  CString str;
  int dx = 0;
  CDC *pDC = ListCtrl().GetDC();
  for (int i = 0; i < num_rows; i++) {
    str = ListCtrl().GetItemText(i, column);
    sz = pDC->GetTextExtent(str); // get string size for each row
    if (sz.cx > dx)
      dx = sz.cx;
  }
  ListCtrl().SetColumnWidth(column, dx + 32); // add width for icon + some extra
}

template <class T>
CAttriPointSetTemplate<T>::CAttriPointSetTemplate(T &set, CWnd *pParent)
    : CAttributesTemplate<T>(IDD_ATTRI_POINTSET, set, pParent), m_pCoordinate(0), m_pBrush(new CBrush) {
  // Get value from registry
  int nValueType = ISettings::instance()->getProfileInt("PointSetDlg", "DefaultValueType", -1);
  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  if ((factory->NameIndex(nValueType) > 0) &&
      ((set.Dimension() == CPointSet::DIM_2D) || (nValueType != IDT_VALUETYPE_SURFACE)))
    m_prValue = std::make_pair(nValueType, factory->NameIndex(nValueType));
  else
    m_prValue = std::make_pair(IDT_VALUETYPE_PRESSURE, IDS_VALUENAME_PRESSURE);

  m_pTreeCtrl = new TTreeCtrl(*this);
  m_pListCtrl = new TListCtrl(*this);

  CreateBitmaps();

  // http://msdn.microsoft.com/en-us/library/1tycsy47%28v=vs.100%29.aspx
  m_pBrush->CreateSolidBrush(RGB(255, 255, 255));
}

template <class T> CAttriPointSetTemplate<T>::~CAttriPointSetTemplate() {
  FreeBitmaps();
  delete m_pTreeCtrl;
  delete m_pListCtrl;
  delete m_pBrush;
}

/// \brief create a bitmap from resource identifier \a id.
/// \return a pointer to the bitmap that was created with 'new'.
template <class T>
const CBitmap *CAttriPointSetTemplate<T>::CreateBitmap(unsigned int id ///< VS bitmap resource identifier for a Bitmap.
) {
  CBitmap *p;
  p = new CBitmap;
  if (!p->LoadBitmap(id)) {
    delete p;
    p = 0;
  }
  return p;
}

/// \brief free the bitmaps that were allocated for this menu.
//
template <class T> void CAttriPointSetTemplate<T>::FreeBitmaps() {
  std::map<unsigned int, const CBitmap *>::iterator it;
  for (it = m_mpBitmap.begin(); it != m_mpBitmap.end(); ++it)
    if ((*it).second)
      delete (*it).second;
}

/// \brief find a bitmap associated with resource identifier \a id.
/// \return a pointer to the bitmap.
template <class T>
const CBitmap *CAttriPointSetTemplate<T>::FindBitmap(unsigned int ValueTypeId ///< valuetype identifier for a Bitmap.
) {
  std::map<unsigned int, const CBitmap *>::iterator it;
  it = m_mpBitmap.find(ValueTypeId);
  if (it == m_mpBitmap.end())
    return 0;
  else
    return (*it).second;
}

/// \brief Create bitmaps for the ValueTypes
/// \detailed Each ValueType identifier is coupled to a VS Resource
/// bitmap identifier in this function.
template <class T> void CAttriPointSetTemplate<T>::CreateBitmaps() {
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_COHESION] = CreateBitmap((unsigned int)IDB_VT_COHESION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FRICTION_ANGLE] = CreateBitmap((unsigned int)IDB_VT_FRICTION_ANGLE);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POROSITY] = CreateBitmap((unsigned int)IDB_VT_POROSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_BULKSTIFFNESS] = CreateBitmap((unsigned int)IDB_VT_BULK_MODULUS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FLUID_BULK_MOD] = CreateBitmap((unsigned int)IDB_VT_FLUID_BULK_MODULUS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_SHEARMODULUS] = CreateBitmap((unsigned int)IDB_VT_SHEAR_MODULUS);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RHOB] = CreateBitmap((unsigned int)IDB_VT_DENSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_VELOCITYP] = CreateBitmap((unsigned int)IDB_VT_VP);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_VELOCITYS] = CreateBitmap((unsigned int)IDB_VT_VS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_INITFRICTION] = CreateBitmap((unsigned int)IDB_VT_INITFRICTION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_PRECONSOLIDATION] = CreateBitmap((unsigned int)IDB_VT_PRECONSOLIDATION);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CAPSHAPE] = CreateBitmap((unsigned int)IDB_VT_CAPSHAPE);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_DILATATION] = CreateBitmap((unsigned int)IDB_VT_DILATATION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LATRATIO_MAX] = CreateBitmap((unsigned int)IDB_VT_LATRATIO_MAX);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LATRATIO_MIN] = CreateBitmap((unsigned int)IDB_VT_LATRATIO_MIN);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_AZIMUTH] = CreateBitmap((unsigned int)IDB_VT_AZIMUTH);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_A1] = CreateBitmap((unsigned int)IDB_VT_A1);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_A2] = CreateBitmap((unsigned int)IDB_VT_A2);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_N1] = CreateBitmap((unsigned int)IDB_VT_N1);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_N2] = CreateBitmap((unsigned int)IDB_VT_N2);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_DYNUNISTIFFNESS] =
      CreateBitmap((unsigned int)IDB_VT_DYNAMIC_UNIAXIAL_STIFFNESS);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_DYNSHEARSTIFFNESS] =
      CreateBitmap((unsigned int)IDB_VT_DYNAMIC_SHEAR_STIFFNESS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_EQUIV_PLAST_STRAIN1] =
      CreateBitmap((unsigned int)IDB_VT_EQUIV_PLASTIC_STRAIN_1);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_EQUIV_PLAST_STRAIN2] =
      CreateBitmap((unsigned int)IDB_VT_EQUIV_PLASTIC_STRAIN_2);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_EQUIV_PLAST_STRAIN3] =
      CreateBitmap((unsigned int)IDB_VT_EQUIV_PLASTIC_STRAIN_3);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_QR1] = CreateBitmap((unsigned int)IDB_VT_ACTIVATION_ENERGY_Q1R);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_QR2] = CreateBitmap((unsigned int)IDB_VT_ACTIVATION_ENERGY_Q2R);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_CREEP_ALPHA] = CreateBitmap((unsigned int)IDB_VT_CREEP_ALPHA);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_TENSILE_STRETCH] = CreateBitmap((unsigned int)IDB_VT_TENSILE_STRETCH);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_ELASTIC_HARDENING] = CreateBitmap((unsigned int)IDB_VT_ELASTIC_HARDENING);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_SEC_PRECON] =
      CreateBitmap((unsigned int)IDB_VT_SECONDARY_PRECONSOLIDATION_STRESS);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_SEC_HARDENING] = CreateBitmap((unsigned int)IDB_VT_SECONDARY_HARDENING);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_ELASCOMPRES] =
      CreateBitmap((unsigned int)IDB_VT_UNIAXIAL_ELASTIC_COMPRESSIBILITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_PLASCOMPRES] =
      CreateBitmap((unsigned int)IDB_VT_UNIAXIAL_PLASTIC_COMPRESSIBILITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LAYER_NORMAL_INCL] =
      CreateBitmap((unsigned int)IDB_VT_LAYER_NORMAL_INCLINATION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LAYER_NORMAL_AZI] = CreateBitmap((unsigned int)IDB_VT_LAYER_NORMAL_AZIMUTH);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HIGH_FRACT_DENS] = CreateBitmap((unsigned int)IDB_VT_HIGHEST_FRACTURE_DENSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HIGH_FRACT_DENS_INCL] =
      CreateBitmap((unsigned int)IDB_VT_HIGHEST_FRACTURE_DENSITY_INCLINATION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HIGH_FRACT_DENS_AZI] =
      CreateBitmap((unsigned int)IDB_VT_HIGHEST_FRACTURE_DENSITY_AZIMUTH);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LOW_FRACT_DENS] = CreateBitmap((unsigned int)IDB_VT_LOWEST_FRACTURE_DENSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LOW_FRACT_DENS_INCL] =
      CreateBitmap((unsigned int)IDB_VT_LOWEST_FRACTURE_DENSITY_INCLINATION);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_LOW_FRACT_DENS_AZI] =
      CreateBitmap((unsigned int)IDB_VT_LOWEST_FRACTURE_DENSITY_AZIMUTH);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_INTER_FRACT_DENS] =
      CreateBitmap((unsigned int)IDB_VT_INTERMEDIATE_FRACTURE_DENSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_PERMEA] = CreateBitmap((unsigned int)IDB_VT_PERMEABILITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_VISCOSITY] = CreateBitmap((unsigned int)IDB_VT_VISCOSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FLUIDX] = CreateBitmap((unsigned int)IDB_VT_FLUID_THERMAL_EXPANSION);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FLUID_DENSITY] = CreateBitmap((unsigned int)IDB_VT_FLUID_DENSITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_REFERENCESTRESS] = CreateBitmap((unsigned int)IDB_VT_REFERENCE_STRESS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_APERTUREPARAM] = CreateBitmap((unsigned int)IDB_VT_APERTURE);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_GRAINSTIFFNESS] = CreateBitmap((unsigned int)IDB_VT_GRAIN_STIFFNESS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP] =
      CreateBitmap((unsigned int)IDB_VT_ANISOTROPIC_SHEAR_MODULUS_DECOMPACTION);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS] =
      CreateBitmap((unsigned int)IDB_VT_ANISOTROPIC_SHEAR_MODULUS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THOMSEN_EPSILON] = CreateBitmap((unsigned int)IDB_VT_THOMSEN_EPSILON);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THOMSEN_GAMMA] = CreateBitmap((unsigned int)IDB_VT_THOMSEN_GAMMA);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THOMSEN_DELTA] = CreateBitmap((unsigned int)IDB_VT_THOMSEN_DELTA);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNGS_MODULUS] = CreateBitmap((unsigned int)IDB_VT_YOUNGSMODULUS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNGMODULUS_NORM] = CreateBitmap((unsigned int)IDB_VT_YOUNGSMODULUS_NORM);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNGMODULUS_TRANS] = CreateBitmap((unsigned int)IDB_VT_YOUNGSMODULUS_TRANS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNG_DECOMP] = CreateBitmap(IDB_VT_YOUNGSMODULUS_DECOMP);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP] =
      CreateBitmap((unsigned int)IDB_VT_YOUNGSMODULUS_NORM_DECOMP);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP] =
      CreateBitmap((unsigned int)IDB_VT_YOUNGSMODULUS_TRANS_DECOMP);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSONS_RATIO] = CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSONRATIO_NORM] = CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO_NORM);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSONRATIO_TRANS] = CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO_TRANS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSON_DECOMP] = CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO_DECOMP);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP] =
      CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO_NORM_DECOMP);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP] =
      CreateBitmap((unsigned int)IDB_VT_POISSONSRATIO_TRANS_DECOMP);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THERMAL_EXPANSION] = CreateBitmap((unsigned int)IDB_VT_THERMAL_EXPANSION);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THERM_LIN_EXP_NORM] =
      CreateBitmap((unsigned int)IDB_VT_THERMAL_EXPANSION_NORM);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THERM_LIN_EXP_LAT] =
      CreateBitmap((unsigned int)IDB_VT_THERMAL_EXPANSION_TRANS);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THERM_CONDUCT] = CreateBitmap((unsigned int)IDB_VT_THERMAL_CONDUCTIVITY);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_THERM_CAPACI] = CreateBitmap((unsigned int)IDB_VT_THERMAL_CAPACITY);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM1] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_01);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM2] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_02);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM3] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_03);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM4] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_04);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM5] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_05);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM6] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_06);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM7] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_07);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM8] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_08);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM9] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_09);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM10] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_10);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM11] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_11);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM12] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_12);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM13] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_13);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM14] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_14);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM15] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_15);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM16] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_16);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM17] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_17);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM18] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_18);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM19] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_19);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM20] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_20);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_RIGI_PARAM21] = CreateBitmap((unsigned int)IDB_VT_RIGIDITY_21);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARDENING] = CreateBitmap((unsigned int)IDB_VT_HARDENING);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_COHESION1] = CreateBitmap((unsigned int)IDB_VT_HARDENING_COHESION_1);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_COHESION2] = CreateBitmap((unsigned int)IDB_VT_HARDENING_COHESION_2);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_COHESION3] = CreateBitmap((unsigned int)IDB_VT_HARDENING_COHESION_3);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_FRICTION1] =
      CreateBitmap((unsigned int)IDB_VT_HARDENING_FRICTION_ANGLE_1);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_FRICTION2] =
      CreateBitmap((unsigned int)IDB_VT_HARDENING_FRICTION_ANGLE_2);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_HARD_FRICTION3] =
      CreateBitmap((unsigned int)IDB_VT_HARDENING_FRICTION_ANGLE_3);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FRACT_TENSOR] = CreateBitmap((unsigned int)IDB_VT_FRACTURE_TENSOR);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FRACT_STIFF_NORMAL] =
      CreateBitmap((unsigned int)IDB_VT_FRACTURE_STIFFNESS_NORM);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FRACT_STIFF_SHEAR] =
      CreateBitmap((unsigned int)IDB_VT_FRACTURE_STIFFNESS_SHEAR);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_V0_HI] = CreateBitmap((unsigned int)IDB_VT_V0_HI);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_V0_ME] = CreateBitmap((unsigned int)IDB_VT_V0_ME);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_V0_LO] = CreateBitmap((unsigned int)IDB_VT_V0_LO);
  m_mpBitmap[(unsigned int)IDT_VALUETYPE_REFERENCE_APERTURE] = CreateBitmap((unsigned int)IDB_VT_REFERENCE_APERTURE);

  m_mpBitmap[(unsigned int)IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE] =
      CreateBitmap((unsigned int)IDB_VT_FRACTURE_MATRIX_PRESSURE);

  // popup menus under 'material'
  //
  m_mpBitmap[(unsigned int)ID_MENU_YOUNGS_MODULUS] = CreateBitmap((unsigned int)IDB_MENU_VT_YOUNGS_MODULUS);
  m_mpBitmap[(unsigned int)ID_MENU_POISSONS_RATIO] = CreateBitmap((unsigned int)IDB_MENU_VT_POISSONS_RATIO);
  m_mpBitmap[(unsigned int)ID_MENU_THERMAL_PARAMETERS] = CreateBitmap((unsigned int)IDB_MENU_VT_THERMAL_PARAMETERS);
  m_mpBitmap[(unsigned int)ID_MENU_FRACTURE_PARAMETERS] = CreateBitmap((unsigned int)IDB_MENU_VT_FRACTURE_PARAMETERS);
  m_mpBitmap[(unsigned int)ID_MENU_HARDENING_PARAMETERS] = CreateBitmap((unsigned int)IDB_MENU_VT_HARDENING_PARAMETERS);
  m_mpBitmap[(unsigned int)ID_MENU_RIGIDITY_PARAMETERS] = CreateBitmap((unsigned int)IDB_MENU_VT_RIGIDITY_PARAMETERS);
}

/// \brief set the background color for \a menu to white
template <class T>
void CAttriPointSetTemplate<T>::WhiteBackgroundColor(CMenu &menu ///< the menu to change
) {
  // http://msdn.microsoft.com/en-us/library/ms647575%28v=vs.100%29.aspx
  MENUINFO MenuInfo;
  MenuInfo.cbSize = sizeof(MENUINFO);
  MenuInfo.fMask = MIM_BACKGROUND;
  assert(m_pBrush);
  // http://msdn.microsoft.com/en-us/library/ye45d4c7%28v=vs.100%29.aspx
  MenuInfo.hbrBack = HBRUSH(*m_pBrush);
  // http://msdn.microsoft.com/en-us/library/x3hybab1%28v=vs.100%29.aspx
  menu.SetMenuInfo(&MenuInfo);
}

template <class T>
BOOL CAttriPointSetTemplate<T>::OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT *pResult) {
  switch (message) {
  case WM_INITMENUPOPUP: {
    CMenu menu;
    menu.Attach((HMENU)wParam);
    OnInitMenuPopup(&menu, (unsigned int)LOWORD(lParam), (BOOL)HIWORD(lParam));
    *pResult = TRUE;
    return TRUE;
  }
  }

  return CAttributesTemplate<T>::OnWndMsg(message, wParam, lParam, pResult);
}

template <class T> BOOL CAttriPointSetTemplate<T>::OnCommand(WPARAM wParam, LPARAM lParam) {
  unsigned int nID = wParam;

  // Dispatch commands for value type menu
  if ((m_mpCommand.find(nID) != m_mpCommand.end())) {
    m_prValue = m_mpCommand.find(nID)->second;
    ISettings::instance()->writeProfileInt("PointSetDlg", "DefaultValueType", m_prValue.first);
    m_btInsertQuantity.SetWindowText(getStringTableEntry(m_prValue.second));
    OnInsertQuantity();
    return TRUE;
  }

  switch (nID) {
  case IDC_BT_BROWSE_QUANTITY:
    OnBrowseQuantity();
    break;
  case IDC_BT_INSERT_QUANTITY:
    OnInsertQuantity();
    break;
  case IDC_BT_INSERT_VALUESET:
    OnInsertValueSet();
    break;
  case IDC_RB_COORDINATE_FIELD_UNIT:
    OnCoordinateFieldUnit();
    break;
  case IDC_RB_COORDINATE_SI_UNIT:
    OnCoordinateSiUnit();
    break;
  case IDC_RB_VALUE_FIELD_UNIT:
    OnValueFieldUnit();
    break;
  case IDC_RB_VALUE_SI_UNIT:
    OnValueSiUnit();
    break;
  default:
    return CAttributesTemplate<T>::OnCommand(wParam, lParam);
    break;
  }

  return TRUE;
}

template <class T> void CAttriPointSetTemplate<T>::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  // Declare our box variables
  CString strDimension, strName;
  int nPointSize, nElementSize, nAxis, nDim;
  double resizeHullValue = 0;

  CPointSet *pPointSet = dynamic_cast<CPointSet *>(&Copy());

  if (!pDX->m_bSaveAndValidate) {
    // Disable dimension button when we have an elementset or a three column pointset
    IElementSet *pElementSet = dynamic_cast<IElementSet *>(&Copy());
    if (pElementSet)
      nElementSize = pElementSet->ElementSet().ElementSize();
    else
      nElementSize = 0;

    if (pPointSet) {
      GetDlgItem(IDC_EDIT_RESIZEHULL)->EnableWindow(TRUE);
      resizeHullValue = pPointSet->ResizeHullValue();
    } else {
      GetDlgItem(IDC_EDIT_RESIZEHULL)->EnableWindow(FALSE);
    }

    GetDlgItem(IDC_RB_DIM_1)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_DIM_2)->EnableWindow(FALSE);
    GetDlgItem(IDC_RB_DIM_3)->EnableWindow(FALSE);

    // Dimension and property size ...
    switch (Copy().Dimension()) {
    case CPointSet::DIM_1D:
      nDim = 0;
      GetDlgItem(IDC_RB_Z_UP)->EnableWindow(FALSE);
      GetDlgItem(IDC_RB_Z_DOWN)->EnableWindow(FALSE);
      break;
    case CPointSet::DIM_2D:
      nDim = 1;
      GetDlgItem(IDC_RB_Z_UP)->EnableWindow(FALSE);
      GetDlgItem(IDC_RB_Z_DOWN)->EnableWindow(FALSE);
      break;
    case CPointSet::DIM_3D:
      nDim = 2;
      break;
    default:
      assert(FALSE);
      break;
    }

    switch (Copy().IPointSet::ZAxis()) {
    case IPointSet::Z_UP:
      nAxis = 0;
      break;
    case IPointSet::Z_DOWN:
      nAxis = 1;
      break;
    };

    // Name and nr of points
    strName = Copy().Name().toStdString().c_str();
    nPointSize = Copy().PointSize();

    // Boundingbox
    OnUpdateMinMax();
  }

  //
  DDX_Text(pDX, IDC_EDIT_RESIZEHULL, resizeHullValue);
  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ST_POINT_SIZE, nPointSize);
  DDX_Text(pDX, IDC_ST_ELEMENT_SIZE, nElementSize);
  DDX_Control(pDX, IDC_LC_VALUE_SET, *m_pListCtrl);
  DDX_Control(pDX, IDC_TC_DISTRIBUTED, *m_pTreeCtrl);
  DDX_Radio(pDX, IDC_RB_Z_UP, nAxis);
  DDX_Radio(pDX, IDC_RB_DIM_1, nDim);
  DDX_Control(pDX, IDC_BT_INSERT_VALUESET, m_btInsertValueSet);
  DDX_Control(pDX, IDC_BT_INSERT_QUANTITY, m_btInsertQuantity);
  DDX_Control(pDX, IDC_BT_BROWSE_QUANTITY, m_btBrowseQuantity);
  if (resizeHullValue < 0)
    resizeHullValue = 0;

  if (pDX->m_bSaveAndValidate) {
    if (!Validate(pDX))
      return;
    // Name changed?
    if (Copy().Name() != strName)
      Copy().Name((LPCSTR)strName);

    if (Copy().Dimension() == IPointSet::DIM_3D) {
      if (nAxis == 0)
        Copy().ZAxis(IPointSet::Z_UP);
      else
        Copy().ZAxis(IPointSet::Z_DOWN);
    }

    if (pPointSet)
      pPointSet->ResizeHullValue(resizeHullValue);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CAttriPointSetTemplate<T> message handlers

template <class T> CString CAttriPointSetTemplate<T>::PointToString(const geo::IPoint &pt) const {
  CString strRet, strFormat;
  CLengthQuantity l;

  switch (Copy().Dimension()) {
  case CPointSet::DIM_2D:
    strRet.Format(_T("(%6.2f, %6.2f)"), l.Convert(pt.X(), UnitNode().Unit(), CQuantity::SI_UNIT),
                  l.Convert(pt.Y(), UnitNode().Unit(), CQuantity::SI_UNIT));
    break;
  case CPointSet::DIM_3D:
    strRet.Format(_T("(%.2f, %.2f, %.2f)"), l.Convert(pt.X(), UnitNode().Unit(), CQuantity::SI_UNIT),
                  l.Convert(pt.Y(), UnitNode().Unit(), CQuantity::SI_UNIT),
                  l.Convert(pt.Z(), UnitNode().Unit(), CQuantity::SI_UNIT));
    break;
  default:
    break;
  }

  return strRet;
}

template <class T> BOOL CAttriPointSetTemplate<T>::OnInitDialog() {
  CAttributesTemplate<T>::OnInitDialog();

  // Make browse buttons
  m_btBrowseQuantity.SetIcon(AfxGetApp()->LoadIcon(IDI_ARROW));
  m_btBrowseQuantity.RedrawWindow();
  m_btBrowseQuantity.UpdateWindow();

  // Register drop target. Normally this is done by the OnCreate handler, but it is not
  // called right here.
  TreeCtrl().RegisterDropTarget();
  ListCtrl().RegisterDropTarget();

  CRect rect;
  ListCtrl().GetWindowRect(&rect);
  ListCtrl().InsertColumn(0, _T("Name"), LVCFMT_LEFT, (rect.Width() - 3), 0);

  std::vector<CValueType *> vcValueType = Copy().ValueTypes();

  for (size_t i = 0; i < vcValueType.size(); i++) {
    CValueTypeTreeObject *pTreeObject = new CValueTypeTreeObject(ListCtrl(), TreeCtrl(), *vcValueType[i]);
    if (pTreeObject->IsCoordinate())
      m_pCoordinate = pTreeObject;
  }

  for (int i = 0; i < Copy().ValueSetSize(); i++) {
    if (Copy().ValueSet(i).Component() == 0)
      new CValueSetListObject(ListCtrl(), Copy().ValueSet(i));
  }

  m_btInsertQuantity.SetWindowText(getStringTableEntry(m_prValue.second));

  SetUnitRadioButton(IDC_RB_COORDINATE_SI_UNIT, IDC_RB_COORDINATE_FIELD_UNIT, Copy().CoordinateUnit());
  SetUnitRadioButton(IDC_RB_VALUE_SI_UNIT, IDC_RB_VALUE_FIELD_UNIT, Copy().ValueUnit());

  // wjrx mantis 3125
  //

  // no longer allowed to switch; we're leaving the rest of the functionality intact
  GetDlgItem(IDC_RB_VALUE_SI_UNIT)->EnableWindow(false);
  GetDlgItem(IDC_RB_VALUE_FIELD_UNIT)->EnableWindow(false);

  // mcr_20190113
  update_list_cols_width();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

template <class T>
void CAttriPointSetTemplate<T>::SetUnitRadioButton(unsigned int uRbSI, unsigned int uRbField, CQuantity::UNIT unit) {
  CButton *pSI = (CButton *)GetDlgItem(uRbSI);
  CButton *pField = (CButton *)GetDlgItem(uRbField);

  // Unit of the coordinate system
  if (unit == CQuantity::SI_UNIT)
    pSI->SetCheck(TRUE);
  else
    pField->SetCheck(TRUE);
}

template <class T> typename CAttriPointSetTemplate<T>::TTreeCtrl &CAttriPointSetTemplate<T>::TreeCtrl() const {
  return (TTreeCtrl &)(*m_pTreeCtrl);
}

template <class T> typename CAttriPointSetTemplate<T>::TListCtrl &CAttriPointSetTemplate<T>::ListCtrl() const {
  return (TListCtrl &)(*m_pListCtrl);
}

template <class T> BOOL CAttriPointSetTemplate<T>::Validate(CDataExchange *pDX) {
  // Check the value type trees ...
  HTREEITEM hChildItem = TreeCtrl().GetChildItem(TVI_ROOT);
  while (hChildItem) {
    CValueTypeTreeObject *pNode = (CValueTypeTreeObject *)(TreeCtrl().GetItemData(hChildItem));
    pNode->AssertValid();

    CComponentTreeObject *pFailedNode = pNode->Validate();
    if (pFailedNode) {
      // Warn user
      TreeCtrl().Expand(pFailedNode->Handle(), TVE_EXPAND);
      TreeCtrl().SelectItem(pFailedNode->Handle());
      _m()->msg("No value assigned ..");
      pDX->Fail();
      return FALSE;
    }

    hChildItem = TreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
  }
  return TRUE;
}

template <class T>
void CAttriPointSetTemplate<T>::AppendValueTypeToMenu(CMenu &menu, unsigned int uQuantityId,
                                                      unsigned int uQuantityNameId, const CBitmap *pBitmap) {
  // Get the quantity builder
  if (CValueTypeFactory::instance()->ValueTypeAvailable(uQuantityId)) {
    // Append to map
    unsigned int nCommandID = WM_USER + m_mpCommand.size() + COMMAND_OFFSET;
    m_mpCommand.insert(TCommandMap::value_type(nCommandID, std::make_pair(uQuantityId, uQuantityNameId)));
    if (pBitmap)
      menu.AppendMenu(MF_BITMAP, nCommandID, pBitmap);
    else
      menu.AppendMenu(MF_STRING, nCommandID, getStringTableEntry(uQuantityNameId));
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnBrowseQuantity() {
  m_mpCommand.clear();
  m_btBrowseQuantity.SetCheck(1); // Button is pushed ...

  CValueTypeFactory::TValueTypeVec vcValueType = CValueTypeFactory::instance()->ValueTypes();

  // Build menu ...
  CMenu menu;
  menu.CreatePopupMenu();
  WhiteBackgroundColor(menu);

  // Create a popmenu for material parameters
  CMenu material;
  material.CreatePopupMenu();
  WhiteBackgroundColor(material);

  CMenu hardening;
  hardening.CreatePopupMenu();
  WhiteBackgroundColor(hardening);

  CMenu rigidi;
  rigidi.CreatePopupMenu();
  WhiteBackgroundColor(rigidi);

  CMenu young;
  young.CreatePopupMenu();
  WhiteBackgroundColor(young);

  CMenu poisson;
  poisson.CreatePopupMenu();
  WhiteBackgroundColor(poisson);

  CMenu therm;
  therm.CreatePopupMenu();
  WhiteBackgroundColor(therm);

  CMenu fract;
  fract.CreatePopupMenu();
  WhiteBackgroundColor(fract);

  // Fill root menu
  for (size_t i = 0; i < vcValueType.size(); i++) {
    if ((Copy().Dimension() == CPointSet::DIM_2D) || (vcValueType[i].first != (unsigned int)IDT_VALUETYPE_SURFACE)) {
      if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_SURFACE) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_PRESSURE) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_STRESS_TENSOR) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_STRAIN_TENSOR) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_DISPLACEMENT) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_TEMPERATURE) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_VOLUMETRICSTRAIN) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_NRMSTRAIN) ||
          (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_LATSTRAIN))
        AppendValueTypeToMenu(menu, vcValueType[i].first, vcValueType[i].second);
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNGS_MODULUS) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNGMODULUS_NORM) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNGMODULUS_TRANS) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNG_DECOMP) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP))
        AppendValueTypeToMenu(young, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSONS_RATIO) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSONRATIO_NORM) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSONRATIO_TRANS) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSON_DECOMP) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP))
        AppendValueTypeToMenu(poisson, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_THERMAL_EXPANSION) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_THERM_LIN_EXP_NORM) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_THERM_LIN_EXP_LAT) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_THERM_CONDUCT) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_THERM_CAPACI))
        AppendValueTypeToMenu(therm, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_FRACT_TENSOR) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_FRACT_STIFF_NORMAL) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_FRACT_STIFF_SHEAR) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_V0_HI) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_V0_ME) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_V0_LO) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_REFERENCE_APERTURE))
        AppendValueTypeToMenu(fract, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARDENING) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_COHESION1) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_COHESION2) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_COHESION3) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_FRICTION1) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_FRICTION2) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_HARD_FRICTION3))
        AppendValueTypeToMenu(hardening, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else if ((vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM1) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM2) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM3) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM4) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM5) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM6) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM7) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM8) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM9) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM10) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM11) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM12) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM13) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM14) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM15) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM16) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM17) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM18) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM19) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM20) ||
               (vcValueType[i].first == (unsigned int)IDT_VALUETYPE_RIGI_PARAM21))
        AppendValueTypeToMenu(rigidi, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
      else
        AppendValueTypeToMenu(material, vcValueType[i].first, vcValueType[i].second, FindBitmap(vcValueType[i].first));
    }
  }

  material.AppendMenu(MF_POPUP | MF_BITMAP, (UINT_PTR)young.Detach(),
                      FindBitmap((unsigned int)ID_MENU_YOUNGS_MODULUS)); // _T("Young's Modulus"));
  material.AppendMenu(MF_POPUP, (UINT_PTR)poisson.Detach(),
                      FindBitmap((unsigned int)ID_MENU_POISSONS_RATIO)); // _T("Poisson's Ratio"));
  material.AppendMenu(MF_POPUP, (UINT_PTR)therm.Detach(),
                      FindBitmap((unsigned int)ID_MENU_THERMAL_PARAMETERS)); // _T("Thermal Parameters"));
  material.AppendMenu(MF_POPUP, (UINT_PTR)fract.Detach(),
                      FindBitmap((unsigned int)ID_MENU_FRACTURE_PARAMETERS)); // _T("Fracture Parameters"));
  material.AppendMenu(MF_POPUP, (UINT_PTR)hardening.Detach(),
                      FindBitmap((unsigned int)ID_MENU_HARDENING_PARAMETERS)); // _T("Hardening Parameters"));
  material.AppendMenu(MF_POPUP, (UINT_PTR)rigidi.Detach(),
                      FindBitmap((unsigned int)ID_MENU_RIGIDITY_PARAMETERS)); // _T("Rigidity Parameters"));
  menu.AppendMenu(MF_POPUP, (UINT_PTR)material.Detach(), _T("Material"));

  // Create a popmenu ...
  CRect rect;
  m_btBrowseQuantity.GetWindowRect(&rect);
  menu.TrackPopupMenu(TPM_LEFTALIGN, rect.BottomRight().x, rect.BottomRight().y, this);

  m_btBrowseQuantity.SetCheck(0); // Button is un-pushed
}

template <class T> void CAttriPointSetTemplate<T>::OnInsertQuantity() {
  // Create the quantity factory
  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  if (factory->ValueTypeAvailable(m_prValue.first)) {
    // Quantity available. So create it ...
    CValueType *pValueType = factory->BuildValueType(Copy(), m_prValue.first, m_prValue.second);
    assert(pValueType);
    CValueType *pCopy = new CValueType(*pValueType);
    assert(pCopy->Component().IsCopy());
    // Expand the newly added quantity in the tree
    CValueTypeTreeObject *pTreeNode = new CValueTypeTreeObject(ListCtrl(), TreeCtrl(), *pCopy);
    pTreeNode->Ctrl().Expand(pTreeNode->Handle(), TVE_EXPAND);
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnInsertValueSet() {
  // Create a RPNValueSet and execute the RPN dialog
  CRpnValueSet *pSource = new CRpnValueSet(Copy());
  CRpnValueSet *pCopy = new CRpnValueSet(*pSource);
  CRpnValueSet_Delegate *pCopy_Delegate = new CRpnValueSet_Delegate(pCopy);

  // We do dialog and we the user clicked OK he really wants the CRpnValueSet. In
  // that case we add the dialog to list of value sets otherwise we delete the newly
  // added property.
  if (pCopy_Delegate->Attributes()) {
    assert(Copy().IsLinkedTo(*pCopy));
    assert(&Copy().RpnValueSet(Copy().RpnValueSetSize() - 1) == pCopy);
    new CValueSetListObject(ListCtrl(), *pCopy);
    // mcr_20190131
    update_list_cols_width();
  } else {
    delete pCopy_Delegate;
    delete pCopy;
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnCoordinateFieldUnit() {
  CButton *pButton = (CButton *)GetDlgItem(IDC_RB_COORDINATE_FIELD_UNIT);
  if (pButton->GetCheck()) {
    Copy().CoordinateInverseUnit(CQuantity::FIELD_UNIT);
    OnUpdateMinMax();
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnCoordinateSiUnit() {
  CButton *pButton = (CButton *)GetDlgItem(IDC_RB_COORDINATE_SI_UNIT);
  if (pButton->GetCheck()) {
    Copy().CoordinateInverseUnit(CQuantity::SI_UNIT);
    OnUpdateMinMax();
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnValueFieldUnit() {
  CButton *pButton = (CButton *)GetDlgItem(IDC_RB_VALUE_FIELD_UNIT);
  if (pButton->GetCheck()) {
    Copy().ValueUnit(CQuantity::FIELD_UNIT);
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnValueSiUnit() {
  CButton *pButton = (CButton *)GetDlgItem(IDC_RB_VALUE_SI_UNIT);
  if (pButton->GetCheck()) {
    Copy().ValueUnit(CQuantity::SI_UNIT);
  }
}

template <class T> void CAttriPointSetTemplate<T>::OnUpdateMinMax() {
  GetDlgItem(IDC_ST_MAX)->SetWindowText(PointToString(Copy().Max()));
  GetDlgItem(IDC_ST_MIN)->SetWindowText(PointToString(Copy().Min()));
}

#endif // !defined(AFX_ATTRIPOINTSETDLG_H__7DBF9677_72A9_4502_98A6_ECE16C7D4738__INCLUDED_)
