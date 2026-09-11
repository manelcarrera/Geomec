#if !defined(AFX_WELLPOINTLISTCTRL_H__AFA06BCE_A709_456F_8AB0_AA1BE7159E41__INCLUDED_)
#define AFX_WELLPOINTLISTCTRL_H__AFA06BCE_A709_456F_8AB0_AA1BE7159E41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewWellPoint.h"
#include "NewWellSection.h"

// WellPointListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellPointListCtrl window

class CNewWellPathBase;
namespace well
{
  class CWellPathBase;
};

typedef std::map<int, well::CWellPoint*> IndexPointMap;
typedef std::pair<int, well::CWellPoint*> IndexPointPair;
typedef std::map<int, CNewWellPoint*> NewIndexPointMap;
typedef std::pair<int, CNewWellPoint*> NewIndexPointPair;

class AFX_EXT_CLASS CWellPointListCtrl : public CListCtrl
{
// Construction
public:
  CWellPointListCtrl(well::CWellPathBase &wellpath);
  CWellPointListCtrl(CNewWellPathBase &wellpath);

  void UpdateList();
  void OnInitList
  (well::CWellPointList* pList
  , CDoubleQuantity::UNIT unit=CDoubleQuantity::SI_UNIT
  );

  void OnInitList
  ( std::list<CNewWellPoint> & List
  , CDoubleQuantity::UNIT unit=CDoubleQuantity::SI_UNIT
  );

  void OnInitList
  ( well::CWellPointList* pList
  , QVector<well::CWellSectionList*> Sections
  , CDoubleQuantity::UNIT unit=CDoubleQuantity::SI_UNIT
  );

  void OnInitList // wjrx mantis 3564
  ( std::list<CNewWellPoint> &List
  , std::list<std::list<INewWellSection *> > &Sections
  , CDoubleQuantity::UNIT unit
  );
  std::list<CNewWellPoint> GetSelectedPoints();

  void ShowPosition(bool show);
  void ShowDirections(bool show);
  void ShowCoordinates(bool show);
  void ShowNumbering(bool show);
  void ShowTMDOnly();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellPointListCtrl)
  //}}AFX_VIRTUAL

  virtual ~CWellPointListCtrl();
  void RemoveSelectedPoints(const bool bNewWellPath= false);
  well::CWellPointList GetSelecetedPoints();
  // Generated message map functions
protected:
  //{{AFX_MSG(CWellPointListCtrl)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()

private:
  IndexPointMap& IndexPoints(){return m_IndexPoints;}
  NewIndexPointMap& NewIndexPoints(){return m_NewIndexPoints;}

  bool m_ShowCoordinates;
  bool m_ShowDirections;
  bool m_ShowPosition;
  bool m_ShowNumbering;

  well::CWellPointList* m_pList;
  std::list<CNewWellPoint> *m_pNewList;
  IndexPointMap m_IndexPoints;
  NewIndexPointMap m_NewIndexPoints;
  CDoubleQuantity::UNIT m_Unit;
  QVector<well::CWellSectionList*> m_Sections;
  std::list<std::list<INewWellSection *> > m_NewSections;

  well::CWellPathBase *m_pWellPath;
  CNewWellPathBase *m_pNewWellPath;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLPOINTLISTCTRL_H__AFA06BCE_A709_456F_8AB0_AA1BE7159E41__INCLUDED_)
