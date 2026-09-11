// modelgeologypg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "modelgeologypg.h"
#include "HexaFormation.h"
#include "BaseEntryTypes.h"
#include "modelbase.h"
#include "MeshBase.h"
#include "horizonbase.h"
#include "BranchState.h"
#include "IInterfaceElement.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelGeologyPg property page



//##ModelId=3BC55D610259
CModelGeologyPg::CModelGeologyPg(CModelBase &model) 
: CPropertyPage(CModelGeologyPg::IDD), m_model(model)
{
  //{{AFX_DATA_INIT(CModelGeologyPg)

  //}}AFX_DATA_INIT
}

//##ModelId=3BC55D61025A
CModelGeologyPg::~CModelGeologyPg()
{
}

//##ModelId=3BC55D61025B
void CModelGeologyPg::DoDataExchange(CDataExchange* pDX)
{
  int nFault = 0;
  int nFormation = 0;
  int nPoints = 0;
  int nElements = 0;
  double dGravity;
  CString strGravityUnit;

  if(!pDX->m_bSaveAndValidate)
  {
//		GetDlgItem(IDC_ST_FAULTS)->EnableWindow(m_model.GraphEntry(MD_GEO_FAULT) != 0);
    GetDlgItem(IDC_ST_FORMATIONS)->EnableWindow(m_model.GraphEntry(MD_BASE_FORMATION) != 0);
//		GetDlgItem(IDC_ST_LABEL_FAULTS)->EnableWindow(m_model.GraphEntry(MD_GEO_FAULT) != 0);
    GetDlgItem(IDC_ST_LABEL_FORMATIONS)->EnableWindow(m_model.GraphEntry(MD_BASE_FORMATION) != 0);

    TFormationBaseEntry *pEntry = dynamic_cast<TFormationBaseEntry*>(m_model.GraphEntry(MD_BASE_FORMATION));
    if(pEntry)
    {
      TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
      nFormation = stNode.size();
    }
    THorizonBaseEntry *pHEntry = dynamic_cast<THorizonBaseEntry*>(m_model.GraphEntry(MD_BASE_HORIZON));
    if(pEntry)
    {
      THorizonBaseEntry::TNodeSet stNode = pHEntry->EntryNodes();
      THorizonBaseEntry::TNodeSet::iterator it = stNode.begin();
      while( it != stNode.end() ) {
        if( (*it)->Slip() ) nFault++;
        it++;
      }
    }

    nPoints = m_model.Mesh().Mesh().PointSize();
    nElements = m_model.Mesh().Mesh().ElementSize();
    if ( m_model.ElementOrder() == CElementOrder::EO_QUADRATIC ) {
      // The number of points will be higher because each edge gets a mid-node
      // Count the edges to add to the number of points
      typedef std::pair<const geo::INode*,const geo::INode*> TNodePair;
      std::set<TNodePair> edges;
      for ( int i = 0; i < nElements; ++i ) {
        const geo::IElement& element = m_model.Mesh().Mesh().Element( i );
    if(!dynamic_cast<const geo::IInterfaceElement*>(&element))
    {
          for ( int j = 0; j < element.NrOfLines(); ++j ) {
            const geo::ILine& line = element.Line( j );
            // Store the edge in one (always the same) direction
            TNodePair edge( &line.Node(0), &line.Node(1) );
            if ( edge.first > edge.second ) std::swap( edge.first, edge.second );
            edges.insert( edge ); // No duplicates because std::set
          }
    }
      }
      nPoints += edges.size();
    }
    dGravity = m_model.Gravity().Value(Unit());
    strGravityUnit = CString(m_model.Gravity().UnitName(Unit()).c_str());
  }


  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CModelGeologyPg)
  DDX_Text(pDX, IDC_ST_FAULTS, nFault);
  DDX_Text(pDX, IDC_ST_FORMATIONS, nFormation);
  DDX_Text(pDX, IDC_ST_POINTS, nPoints);
  DDX_Text(pDX, IDC_ST_ELEMENTS, nElements);
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_ST_GRAVITY_UNIT, strGravityUnit);
  DDX_Text(pDX, IDC_ED_GRAVITY, dGravity);
  DDV_MinMaxDouble(pDX, dGravity, m_model.Gravity().MinValue(Unit()), m_model.Gravity().MaxValue(Unit())); 

  if(pDX->m_bSaveAndValidate)
  {
    m_model.Gravity(dGravity, Unit());
  }
}


BEGIN_MESSAGE_MAP(CModelGeologyPg, CPropertyPage)
  //{{AFX_MSG_MAP(CModelGeologyPg)
  ON_EN_CHANGE(IDC_ED_GRAVITY, OnChangeGravity)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelGeologyPg message handlers

BOOL CModelGeologyPg::OnInitDialog()
{
  CPropertyPage::OnInitDialog();

  if(m_model.BranchState().IsBranch())
  {
    GetDlgItem(IDC_ED_GRAVITY)->EnableWindow(FALSE);
  }

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

CQuantity::UNIT CModelGeologyPg::Unit() const
{
  CGeomecDoc* pDoc = &((CGeomecApp*)AfxGetApp())->GetDoc();

  return pDoc->UnitNode().Unit();
}

void CModelGeologyPg::OnChangeGravity() 
{
  SetModified(true);
}
