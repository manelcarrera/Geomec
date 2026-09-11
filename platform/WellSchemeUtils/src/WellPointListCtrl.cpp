// WellPointListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>

#include "NewWellPathBase.h"
#include "NewWellSectionList.h"
#include "WellPointListCtrl.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellPointListCtrl

CWellPointListCtrl::CWellPointListCtrl(well::CWellPathBase &wellpath)
    : m_pList(0), m_pNewList(0), m_pWellPath(&wellpath), m_pNewWellPath(0) {
  m_ShowPosition = true;
  m_ShowDirections = true;
  m_ShowCoordinates = true;
  m_ShowNumbering = true;
}

CWellPointListCtrl::CWellPointListCtrl(CNewWellPathBase &wellpath)
    : m_pList(0), m_pNewList(0), m_pWellPath(0), m_pNewWellPath(&wellpath) {
  m_ShowPosition = true;
  m_ShowDirections = true;
  m_ShowCoordinates = true;
  m_ShowNumbering = true;
}

CWellPointListCtrl::~CWellPointListCtrl() {}

void CWellPointListCtrl::ShowPosition(bool show) { m_ShowPosition = show; }

void CWellPointListCtrl::ShowDirections(bool show) { m_ShowDirections = show; }

void CWellPointListCtrl::ShowCoordinates(bool show) { m_ShowCoordinates = show; }

void CWellPointListCtrl::ShowNumbering(bool show) { m_ShowNumbering = show; }

void CWellPointListCtrl::ShowTMDOnly() {
  m_ShowPosition = false;
  m_ShowCoordinates = false;
  m_ShowDirections = false;
  m_ShowNumbering = false;
}
void CWellPointListCtrl::OnInitList(well::CWellPointList *pList, CDoubleQuantity::UNIT unit) {
  OnInitList(pList, m_Sections, unit);
}

void CWellPointListCtrl::OnInitList(std::list<CNewWellPoint> &List, CDoubleQuantity::UNIT unit) {
  OnInitList(List, m_NewSections, unit);
}

void CWellPointListCtrl::OnInitList(well::CWellPointList *pList, QVector<well::CWellSectionList *> Sections,
                                    CDoubleQuantity::UNIT unit) {
  m_pList = pList;
  m_Sections = Sections;
  m_Unit = unit;
  m_IndexPoints.clear();

  // Delete all of the columns.
  int nColumnCount;
  if (GetHeaderCtrl())
    nColumnCount = GetHeaderCtrl()->GetItemCount();
  else
    nColumnCount = GetItemCount();

  for (int i = 0; i < nColumnCount; i++) {
    DeleteColumn(0);
  }

  CRect rect;
  GetClientRect(&rect);
  double width = rect.right / 6;

  CLengthQuantity q;
  CAngleQuantity a;

  std::string ahd = "AHD [" + q.UnitName(m_Unit) + "]";
  std::string tvd = "TVD [" + q.UnitName(m_Unit) + "]";
  std::string azi = "Azimuth [" + a.UnitName(m_Unit) + "]";
  std::string inc = "Inclination [" + a.UnitName(m_Unit) + "]";
  std::string nor = "Northing [" + q.UnitName(m_Unit) + "]";
  std::string eas = "Easting  [" + q.UnitName(m_Unit) + "]";
  std::string posx = "Pos X  [" + q.UnitName(m_Unit) + "]";
  std::string posy = "Pos Y  [" + q.UnitName(m_Unit) + "]";
  std::string posz = "Pos Z  [" + q.UnitName(m_Unit) + "]";

  int iColom = 0;

  if (m_ShowNumbering)
    InsertColumn(iColom++, "Nr", LVCFMT_LEFT, width);

  InsertColumn(iColom++, ahd.c_str(), LVCFMT_LEFT, width);

  if (m_ShowPosition) {
    InsertColumn(iColom++, tvd.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, azi.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, inc.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, nor.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, eas.c_str(), LVCFMT_LEFT, width);
  }

  if (m_ShowDirections) {

    InsertColumn(iColom++, "Dir X []", LVCFMT_LEFT, width);
    InsertColumn(iColom++, "Dir Y []", LVCFMT_LEFT, width);
    InsertColumn(iColom++, "Dir Z []", LVCFMT_LEFT, width);
  }

  if (m_ShowCoordinates) {
    InsertColumn(iColom++, posx.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, posy.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, posz.c_str(), LVCFMT_LEFT, width);
  }

  if (m_Sections.count()) {
    for (size_t i = 0; i < m_Sections.size(); i++) {
      well::CWellSectionList *SectionList = m_Sections.at(i);
      InsertColumn(iColom++, (const char *)SectionList->GetDescription().toStdString().c_str(), LVCFMT_LEFT, width);
    }
  }

  SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

// wjrx mantis 3564
//
void CWellPointListCtrl::OnInitList(std::list<CNewWellPoint> &List, std::list<std::list<INewWellSection *>> &Sections,
                                    CDoubleQuantity::UNIT unit) {
  m_pNewList = &List;
  m_NewSections = Sections;
  m_Unit = unit;
  m_NewIndexPoints.clear();

  // Delete all of the columns.
  int nColumnCount;
  if (GetHeaderCtrl())
    nColumnCount = GetHeaderCtrl()->GetItemCount();
  else
    nColumnCount = GetItemCount();

  for (int i = 0; i < nColumnCount; i++) {
    DeleteColumn(0);
  }

  CRect rect;
  GetClientRect(&rect);
  double width = rect.right / 6;

  CLengthQuantity q;
  CAngleQuantity a;

  std::string ahd = "AHD [" + q.UnitName(m_Unit) + "]";
  std::string tvd = "TVD [" + q.UnitName(m_Unit) + "]";
  std::string azi = "Azimuth [" + a.UnitName(m_Unit) + "]";
  std::string inc = "Inclination [" + a.UnitName(m_Unit) + "]";
  std::string nor = "Northing [" + q.UnitName(m_Unit) + "]";
  std::string eas = "Easting  [" + q.UnitName(m_Unit) + "]";
  std::string posx = "Pos X  [" + q.UnitName(m_Unit) + "]";
  std::string posy = "Pos Y  [" + q.UnitName(m_Unit) + "]";
  std::string posz = "Pos Z  [" + q.UnitName(m_Unit) + "]";

  int iColom = 0;

  if (m_ShowNumbering)
    InsertColumn(iColom++, "Nr", LVCFMT_LEFT, width);

  InsertColumn(iColom++, ahd.c_str(), LVCFMT_LEFT, width);

  if (m_ShowPosition) {
    InsertColumn(iColom++, tvd.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, azi.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, inc.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, nor.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, eas.c_str(), LVCFMT_LEFT, width);
  }

  if (m_ShowDirections) {

    // InsertColumn(iColom++, "Dir X []", LVCFMT_LEFT, width);
    // InsertColumn(iColom++, "Dir Y []", LVCFMT_LEFT, width);
    // InsertColumn(iColom++, "Dir Z []", LVCFMT_LEFT, width);
  }

  if (m_ShowCoordinates) {
    InsertColumn(iColom++, posx.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, posy.c_str(), LVCFMT_LEFT, width);
    InsertColumn(iColom++, posz.c_str(), LVCFMT_LEFT, width);
  }

  if (!m_NewSections.empty()) {
    InsertColumn(iColom++, "Formation", LVCFMT_LEFT, width);
  }

  SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

void CWellPointListCtrl::RemoveSelectedPoints(const bool bNewWellPath) {
  CWaitCursor wait;

  POSITION pos = GetFirstSelectedItemPosition();
  std::vector<int> vcIndex;
  std::vector<int>::iterator vcit;

  while (pos) {
    int nItem = GetNextSelectedItem(pos);
    vcIndex.push_back(nItem);
  }

  well::CWellPoint *point = 0;
  CNewWellPoint *newpoint = 0;

  for (vcit = vcIndex.begin(); vcit != vcIndex.end(); vcit++) {
    // DeleteItem(*vcit);
    if (bNewWellPath == false) {
      point = m_IndexPoints[*vcit];
      m_pList->RemovePoint(point);
    } else // wjrx mantis 3564
    {
      newpoint = m_NewIndexPoints[*vcit];
      for (std::list<CNewWellPoint>::const_iterator it = m_pNewList->begin(); it != m_pNewList->end(); ++it) {
        if (*it == *newpoint) {
          m_pNewList->remove(*it);
          break;
        }
      }
    }
  }
  UpdateList();
}

well::CWellPointList CWellPointListCtrl::GetSelecetedPoints() {
  CWaitCursor wait;

  POSITION pos = GetFirstSelectedItemPosition();
  std::vector<int> vcIndex;
  std::vector<int>::iterator vcit;

  while (pos) {
    int nItem = GetNextSelectedItem(pos);
    vcIndex.push_back(nItem);
  }

  well::CWellPointList ret(m_pList->WellPath());
  ret.AutoDelete(false);

  for (vcit = vcIndex.begin(); vcit != vcIndex.end(); vcit++) {
    well::CWellPoint *point = m_IndexPoints[*vcit];
    ret.AddPoint(point);
  }
  return ret;
}

// wjrx mantis 3564
//
std::list<CNewWellPoint> CWellPointListCtrl::GetSelectedPoints() {
  CWaitCursor wait;

  POSITION pos = GetFirstSelectedItemPosition();
  std::vector<int> vcIndex;
  std::vector<int>::iterator vcit;

  while (pos) {
    int nItem = GetNextSelectedItem(pos);
    vcIndex.push_back(nItem);
  }

  std::list<CNewWellPoint> ret;

  for (vcit = vcIndex.begin(); vcit != vcIndex.end(); vcit++) {
    CNewWellPoint point = *m_NewIndexPoints[*vcit];
    ret.push_back(point);
  }
  return ret;
}

void CWellPointListCtrl::UpdateList() {
  DeleteAllItems();
  m_IndexPoints.clear();
  m_NewIndexPoints.clear();

  assert((m_pList != 0) ^ (m_pNewList != 0)); // we want exactly one list

  if ((m_pList == 0 || m_pList->NrOfPoints() == 0) && (m_pNewList == 0 || m_pNewList->empty()))
    return;

  CWaitCursor wait;

  CLengthQuantity qnLength;

  CString strAHD;
  CString strTVD;
  CString strAzi;
  CString strInc;
  CString strNor;
  CString strEas;
  CString strDirX;
  CString strDirY;
  CString strDirZ;
  CString strX;
  CString strY;
  CString strZ;

  int nIndex = 0;
  int i = 0;

  if (m_pList && m_pList->NrOfPoints() != 0) {
    well::CWellPointList::Iterator pit = m_pList->begin();
    well::CWellPoint *p = m_pList->First();

    while (pit != m_pList->end()) {
      p = *pit;
      int iColom = 0;

      nIndex = InsertItem(i, "");
      m_IndexPoints.insert(IndexPointPair(i, p));

      if (m_ShowNumbering) {
        CString strIndex;
        strIndex.Format("%d", i + 1);
        SetItemText(nIndex, iColom++, strIndex);
      }

      strAHD.Format("%f", p->TMD().Value(m_Unit));
      SetItemText(nIndex, iColom++, strAHD);

      if (m_ShowPosition) {
        strTVD.Format("%f", p->TVD().Value(m_Unit));
        strAzi.Format("%f", p->Azimuth().Value(m_Unit));
        strInc.Format("%f", p->Inclination().Value(m_Unit));
        strNor.Format("%f", p->Northing().Value(m_Unit));
        strEas.Format("%f", p->Easting().Value(m_Unit));

        SetItemText(nIndex, iColom++, strTVD);
        SetItemText(nIndex, iColom++, strAzi);
        SetItemText(nIndex, iColom++, strInc);
        SetItemText(nIndex, iColom++, strNor);
        SetItemText(nIndex, iColom++, strEas);
      }

      if (m_ShowDirections) {
        strDirX.Format("%f", p->GetDirection().X());
        strDirY.Format("%f", p->GetDirection().Y());
        strDirZ.Format("%f", p->GetDirection().Z());
        SetItemText(nIndex, iColom++, strDirX);
        SetItemText(nIndex, iColom++, strDirY);
        SetItemText(nIndex, iColom++, strDirZ);
      }

      if (m_ShowDirections) {
        strX.Format("%f", p->X());
        strY.Format("%f", p->Y());
        strZ.Format("%f", p->Z());
        SetItemText(nIndex, iColom++, strX);
        SetItemText(nIndex, iColom++, strY);
        SetItemText(nIndex, iColom++, strZ);
      }

      if (m_Sections.count()) {
        for (int j = 0; j < m_Sections.count(); j++) {
          well::CWellSectionList *pSectionList = m_Sections.at(j);
          well::CWellSectionList lst = pSectionList->GetSections(*p);
          well::CWellSectionList::Iterator it = lst.begin();
          if (it != lst.end()) {
            SetItemText(nIndex, iColom++, (*it)->GetDescription().toStdString().c_str());
          } else {
            SetItemText(nIndex, iColom++, "not found");
          }
          ++it;
        }
      }
      i++;
      ++pit;
    }
  } else if (m_pNewList && !m_pNewList->empty()) // wjrx mantis 3564
  {
    std::list<CNewWellPoint>::iterator pit = m_pNewList->begin();
    int pt_cntr = 0;
    CNewWellPoint *p;

    while (pit != m_pNewList->end()) {
      p = &*pit;
      int iColom = 0;

      nIndex = InsertItem(i, "");
      m_NewIndexPoints.insert(NewIndexPointPair(i, p));

      if (m_ShowNumbering) {
        CString strIndex;
        strIndex.Format("%d", i + 1);
        SetItemText(nIndex, iColom++, strIndex);
      }

      double val = CLengthQuantity(p->TMD()).Value(m_Unit);
      strAHD.Format("%f", val);
      SetItemText(nIndex, iColom++, strAHD);

      if (m_ShowPosition) {
        val = CLengthQuantity(p->TVD()).Value(m_Unit);
        strTVD.Format("%f", val);
        strAzi.Format("%f", m_pNewWellPath->Azimuth(pt_cntr).Value(m_Unit));
        strInc.Format("%f", m_pNewWellPath->Inclination(pt_cntr).Value(m_Unit));
        val = CLengthQuantity(p->Northing()).Value(m_Unit);
        strNor.Format("%f", val);
        val = CLengthQuantity(p->Easting()).Value(m_Unit);
        strEas.Format("%f", val);

        SetItemText(nIndex, iColom++, strTVD);
        SetItemText(nIndex, iColom++, strAzi);
        SetItemText(nIndex, iColom++, strInc);
        SetItemText(nIndex, iColom++, strNor);
        SetItemText(nIndex, iColom++, strEas);
      }

      if (m_ShowDirections) {
        strX.Format("%f", p->X());
        strY.Format("%f", p->Y());
        strZ.Format("%f", p->Z());
        SetItemText(nIndex, iColom++, strX);
        SetItemText(nIndex, iColom++, strY);
        SetItemText(nIndex, iColom++, strZ);
      }

      if (!m_NewSections.empty()) {
        std::list<std::list<INewWellSection *>>::const_iterator it;
        for (it = m_NewSections.begin(); it != m_NewSections.end(); ++it) {
          CNewWellSectionList newWellSectionList(*it);
          std::list<INewWellSection *> lst = newWellSectionList.getSections(*p);

          if (lst.begin() != lst.end()) {
            INewWellSection *section = *(lst.begin());
            SetItemText(nIndex, iColom++, section->GetDescription().c_str());
          } else
            SetItemText(nIndex, iColom++, "not found");
        }
      }

      i++;
      ++pit;
      ++pt_cntr;
    }
  } // if ( m_pNewList && ! m_pNewList->empty() ) // wjrx mantis 3564
}

BEGIN_MESSAGE_MAP(CWellPointListCtrl, CListCtrl)
//{{AFX_MSG_MAP(CWellPointListCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellPointListCtrl message handlers
