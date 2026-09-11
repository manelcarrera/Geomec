// resultselectdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "resultselectdlg.h"
#include "Result.h"
#include "ModelBase.h"
#include "ValueType.h"
#include "ResultTree.h"
#include "BaseEntryTypes.h"
#include "DepletionStage.h"
#include "pointset.h"
#include "MeshBase.h"
#include "SelectionObserver.h"
#include "SelectionObserver_Delegate.h"
#include "IValueComposite_Delegate.h"
#include "GlobalMessage.h"
#include "resourceIDI.h"
#include "DepletionStageObserver.h"
#include "GVTResult.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// Implementatie van CResultSelectDlg::CResultGroupNode
/////////////////////////////////////////////////////////////////////////////
CResultSelectDlg::CResultGroupObserver::CResultGroupObserver(CResultGroup& result_group, CResultSelectDlg& dlg)
: CLocalResultGroupObserver(dlg, result_group, dlg.m_lbValue),
  m_dlg(dlg)
{
	Update();
}

BOOL CResultSelectDlg::CResultGroupObserver::CanInsertResult(const IResult& result) const
{
  const CDerivedResult::CDerivedResultComponent* pResultBeingCreated = m_dlg.ResultBeingCreated();
  if(pResultBeingCreated && &pResultBeingCreated->Parent() == &result)
    return FALSE;

  return CLocalResultGroupObserver::CanInsertResult(result);
}

BOOL CResultSelectDlg::CResultGroupObserver::CanInsertResultGroup(const CResultGroup& group) const
{
  const CDerivedResultGroup* pDerivedResultGroup = dynamic_cast<const CDerivedResultGroup*>(&group);
  if(pDerivedResultGroup && pDerivedResultGroup->StoreOnFile() == (m_dlg.Global() == TRUE))
    return FALSE;

  return CLocalResultGroupObserver::CanInsertResultGroup(group);
}


/////////////////////////////////////////////////////////////////////////////
// Implementation of CResultSelectDlg::CPointSetObserver
/////////////////////////////////////////////////////////////////////////////
CResultSelectDlg::CPointSetObserver::CPointSetObserver(IPointSet& point_set, CResultSelectDlg& dlg)
: CChildEnumerator<CValueType>(dlg.m_lbValue, TVI_ROOT, TVI_LAST), m_point_set(point_set), m_dlg(dlg)
{
	Observer().LinkTo(point_set);
	Ctrl().SetItemData(Handle(), (DWORD_PTR) this);	
	Update();
}

const CGraphNode& CResultSelectDlg::CPointSetObserver::ObservedItem() const
{
	return m_point_set;
}

CGraphNode& CResultSelectDlg::CPointSetObserver::ObservedItem()
{
	return m_point_set;
}

const CGraphNode_Delegate& CResultSelectDlg::CPointSetObserver::Delegate() const
{
	assert(FALSE);
	return *(new CGraphNode_Delegate(0));
}

CGraphNode_Delegate& CResultSelectDlg::CPointSetObserver::Delegate()
{
	assert(FALSE);
	return *(new CGraphNode_Delegate(0));
}


// TODO: these two methods mimic the current way it works, but they are wrong, as this doesn't allow multiple selects
void CResultSelectDlg::CPointSetObserver::ResultToggleState(IValueComposite& result)
{
  if (result.ComponentSize() == 1)
  {
    m_dlg.Select(&result.Component());
  }
}

unsigned int CResultSelectDlg::CPointSetObserver::ResultStateIconId(const IValueComposite& result) const
{
	if (result.ComponentSize() > 1)
		return 0;

	for ( auto &i : m_dlg.m_selected_inputs_v ) 
		if( i == &result.Component() ) 
			return IDI_CHECKED;
	return IDI_UNCHECKED;

}

CTreeNode* CResultSelectDlg::CPointSetObserver::InsertChild(CValueType& value_type)
{
  return new CValueTypeObserverTempl<CPointSetObserver>(value_type, *this);
}

BOOL CResultSelectDlg::CPointSetObserver::OnFilter(const CValueType& t) const
{
	const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&t.PointSet());
	if(pPointSet)
		return &pPointSet->Coordinates() != &t;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CResultSelectDlg dialog
/////////////////////////////////////////////////////////////////////////////

CResultSelectDlg::CResultSelectDlg(CModelBase& model, BOOL bGlobal, const CDerivedResult::CDerivedResultComponent* pResultBeingCreated, CWnd* pParent /*=NULL*/)
: CDialog(CResultSelectDlg::IDD, pParent), 
	m_bInput(FALSE), 
	m_model(model), 
	m_bGlobal(bGlobal),
	//m_pSelection(0),
	m_bGenerateName(TRUE),
  m_pResultBeingCreated(pResultBeingCreated)
, m_RGObserver(0)
{
  init();

	//{{AFX_DATA_INIT(CResultSelectDlg)
	m_sName = _T("");
	//}}AFX_DATA_INIT
}

void CResultSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int		nInput;

	if(!pDX->m_bSaveAndValidate)
	{
		if(m_bInput)
			nInput = 0;
		else
			nInput = 1;

		DDX_Check(pDX, IDC_CK_NAME, m_bGenerateName);
		GetDlgItem(IDC_RB_INPUT)->EnableWindow(!m_bGlobal);
		OnSetLinStage();
	}

	DDX_Radio(pDX, IDC_RB_INPUT, nInput);

	//{{AFX_DATA_MAP(CResultSelectDlg)
	DDX_Control(pDX, IDC_LB_VALUE_TREE, m_lbValue);
	DDX_Text(pDX, IDC_ED_NAME, m_sName);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
    if (m_bSetLinStage && (m_AnalysisTypes.size() == 0) &&
      (m_stTimeStep.size() == 0))
    {
      _m()->msg("No analysis type(s) and depletion stage(s) selected");
      pDX->Fail();
    }

    if (!m_bInput && (m_RGObserver != 0))
    {
      CResultSelectDlg::CResultComponentSetIterator
        resultComponentSetIterator(this);
      const IValueComponentBase* valueComponent = resultComponentSetIterator.get();

      if (valueComponent == 0)
      {
        _m()->msg("Nothing selected or no results as a consequence of the configuration");
        pDX->Fail();
      }
    }

	}
  DDX_Control(pDX, IDC_LB_TIMESTEP, m_lbTimeStep);
}


BEGIN_MESSAGE_MAP(CResultSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CResultSelectDlg)
	ON_BN_CLICKED(IDC_RB_INPUT, OnInput)
	ON_BN_CLICKED(IDC_CK_LINEAR, OnLinear)
	ON_BN_CLICKED(IDC_RB_OUTPUT, OnOutput)
	ON_BN_CLICKED(IDC_CK_NON_LINEAR, OnNonLinear)
  ON_BN_CLICKED(IDC_CK_HEAT, OnHeat)
  ON_BN_CLICKED(IDC_CK_MIXTURE, OnMixture)
  ON_BN_CLICKED(IDC_CK_MIXTURE_CONTAINMENT, OnMixtureContainment)
	ON_BN_CLICKED(IDC_CK_NAME, OnCkName)
	ON_BN_CLICKED(IDC_CHK_SETLINSTAGE, OnSetLinStage)
	ON_EN_CHANGE(IDC_ED_NAME, OnNameChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultSelectDlg message handlers

bool CResultSelectDlg::Composite(const IValueComposite& composite) const 
{ 
  return (m_RGObserver != 0) && m_RGObserver->ResultComposite(composite);
}

void CResultSelectDlg::Composite(const IValueComposite& composite, bool bEnable) 
{ 
  if (m_RGObserver != 0)
  {
    m_RGObserver->ResultComposite(composite, bEnable);
    UpdateSelection();
  }
}

const CDerivedResult::CDerivedResultComponent* CResultSelectDlg::ResultBeingCreated() const
{
  return m_pResultBeingCreated;
}

bool CResultSelectDlg::Valid(const IValueComposite& composite) const
{
  assert(composite.ModeSize() > 0);

  for (int i = 0; i < composite.ComponentSize(); ++i)
  {
    if (Valid(composite.Component(i)))
    {
      return true;
    }
  }

  return false;
}

bool CResultSelectDlg::Valid(const IValueComponentBase& component) const
{
  return true;
}

bool CResultSelectDlg::TimeStep(const CDepletionStage& stage) const
{
  return m_stTimeStep.find(&stage) != m_stTimeStep.end();
}

void CResultSelectDlg::TimeStep(const CDepletionStage& stage, bool bEnable)
{
  if (!bEnable)
  {
    m_stTimeStep.erase(&stage);
  }

  if (bEnable)
  {
    m_stTimeStep.insert(&stage);
  }

  m_lbValue.UpdateTree();
  UpdateSelection();
}

void CResultSelectDlg::BuildTree()
{
	// Clear Tree
	m_lbValue.DeleteAllItems();
	
	if(m_bInput)
	{
		// If we have input, walk over points set
		TPointSetEntry& entry = (TPointSetEntry&)(*Model().GraphEntry(MD_BASE_POINTSET));
		TPointSetEntry::TNodeSet stNode = entry.EntryNodes();
		for(TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
			new CPointSetObserver(**it, *this);

		if (m_RGObserver != 0)
		{
		  // TODO
		  // Although m_RGObserver exists, it is not a valid object and
		  // cannot be savely deleted!
		  // delete m_RGObserver;
		  // The statement 'm_RGObserver = 0;' is not necessary when m_RGObserver
		  // is created before the statement 'new TCompositeEntryObs(...).', however
		  // this omission would alter the order in which both tree's
		  // ('Mesh results' and 'Results') are displayed!

		  m_RGObserver = 0;
		}
	}
	else
	{ 
		typedef CSelectionLeafObserver_Delegate<IValueComposite, IValueComposite_Delegate, CResultSelectDlg, FALSE> TCompositeObs;
		typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs, IValueComposite, CResultSelectDlg, FALSE, FIXED_ITEM> TCompositeEntryObs;
		// Mesh result observer
		new TCompositeEntryObs(*Model().GraphEntry(MD_BASE_MESH_RESULT), m_lbValue, *this,
							   &CResultSelectDlg::Composite, &CResultSelectDlg::Composite);

    
		m_GVTRGObserver = new CResultGroupObserver(Model().GVTResultTree(), *this);

		// If we have output walk over the results
		// TODO
		// m_RGObserver Has to be created anew, something changes that necesitates
		// construction!
		m_RGObserver =
		  new CResultGroupObserver((CResultTree&) Model().ResultTree(), *this);
	}
}

void CResultSelectDlg::UpdateSelection()
{
  GetDlgItem(IDC_CK_LINEAR)->EnableWindow(FALSE);
  GetDlgItem(IDC_CK_NON_LINEAR)->EnableWindow(FALSE);
  GetDlgItem(IDC_CK_HEAT)->EnableWindow(FALSE);
  GetDlgItem(IDC_CK_MIXTURE)->EnableWindow(FALSE);
  GetDlgItem(IDC_CK_MIXTURE_CONTAINMENT)->EnableWindow(FALSE);
  GetDlgItem(IDC_LB_TIMESTEP)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_SETLINSTAGE)->EnableWindow(FALSE);

  TResultComponentSet results;

  if (!m_bInput && (m_RGObserver != 0))
  {
    results = m_RGObserver->Results();
    results.insert(m_GVTRGObserver->Results().begin(), m_GVTRGObserver->Results().end());

    TAnalysisTypes analysisTypes;
    bool resultWithDepletionStage = false;

    for (TResultComponentSet::const_iterator result = results.begin(); result != results.end(); ++result)
    {
      for (unsigned int component = 0;
        component < (*result).first->ComponentSize(); ++component)
      {
        for (unsigned int mode = 0; mode < (*result).first->ModeSize(); ++mode)
        {
          const IValueComponentBase& valueComponent =
            (*result).first->Component(component, mode);
          const CMeshResult::CMeshResultComponent* meshResultComponent =
            dynamic_cast <const CMeshResult::CMeshResultComponent*> (
              &valueComponent);

          const CValueType *vt = dynamic_cast<const CValueType *>((*result).first);

          if (meshResultComponent == 0 && vt == 0)
          {
            const IResultComponent& iResultComponent =
              dynamic_cast <const IResultComponent&> (valueComponent);
            std::pair <TAnalysisTypes::iterator, bool> analysisTypeInserted =
              analysisTypes.insert(
                std::pair <CAnalysisType::TAnalysisType, size_t> (
                  iResultComponent.AnalysisType().AnalysisType(), 1));

            resultWithDepletionStage = true;

            if (!analysisTypeInserted.second)
            {
              ++analysisTypeInserted.first->second;
            }
          }
        }
      }
    }


    mapAnalysisTypes2GUI(analysisTypes);

    GetDlgItem(IDC_CHK_SETLINSTAGE)->EnableWindow(resultWithDepletionStage);
  }

	UpdateName();
}

void CResultSelectDlg::OnSetLinStage()
{
	CButton& button = (CButton&)*GetDlgItem(IDC_CHK_SETLINSTAGE);
	m_bSetLinStage = button.GetCheck() == 1;

  if(::IsWindow(m_lbValue.m_hWnd))
  {
    UpdateSelection();
	  m_lbValue.UpdateTree();
  }
}

void CResultSelectDlg::OnCkName()
{
	CButton& button = (CButton&)*GetDlgItem(IDC_CK_NAME);
	m_bGenerateName = button.GetCheck() == 1;
	UpdateName();
}

void CResultSelectDlg::OnNameChange() 
{
	GetDlgItem(IDC_ED_NAME)->GetWindowText(m_sName);
}

void CResultSelectDlg::OnInput() 
{
	if(!m_bInput)
	{
		m_bInput = TRUE;
		BuildTree();
		UpdateSelection();
		GetDlgItem(IDC_CHK_SETLINSTAGE)->EnableWindow(FALSE);
	}
}

void CResultSelectDlg::OnLinear()
{
  onAnalysisType(IDC_CK_LINEAR, CAnalysisType::AT_LINEAR);
}

void CResultSelectDlg::OnOutput() 
{
	if(m_bInput)
	{
		m_bInput = FALSE;
		m_selected_inputs_v.clear();
		BuildTree();
		UpdateSelection();
	}
}

void CResultSelectDlg::UpdateName()
{
  if (m_RGObserver != 0)
  {
    if ((m_RGObserver->Results().size() > 1) || (m_bSetLinStage &&
      ((m_AnalysisTypes.size() > 1) || (m_stTimeStep.size() > 1))))
    {
      GetDlgItem(IDC_CK_NAME)->EnableWindow(FALSE);
      GetDlgItem(IDC_ED_NAME)->SetWindowText("Multiple results selected");
      GetDlgItem(IDC_ED_NAME)->EnableWindow(FALSE);
      return;
    }
    else
    {
      GetDlgItem(IDC_CK_NAME)->EnableWindow(TRUE);
      GetDlgItem(IDC_ED_NAME)->SetWindowText("");
      GetDlgItem(IDC_ED_NAME)->EnableWindow(TRUE);
    }
  }

	if(m_bGenerateName)
	{
		if( !m_selected_inputs_v.empty() )
		{
			const IValueComponentBase* last_selected_input = m_selected_inputs_v.back();

			const IResult *pDResult= NULL;
		  if ( (m_bSetLinStage && (m_AnalysisTypes.size() == 1) &&
			(m_stTimeStep.size() == 1)) ||
				   ! (pDResult= dynamic_cast<const IResult *>(&last_selected_input->Parent()) )
				 )
				  m_sName = last_selected_input->ExportLabel().toStdString().c_str();
		  else
		  {
			if(pDResult)
			  m_sName = pDResult->ExportLabel(last_selected_input->ComponentIndex()).toStdString().c_str();
			else
			  m_sName = last_selected_input->Parent().ExportLabel().toStdString().c_str();
		  }
		}
		else
		{
		  m_sName = "";
		}

		GetDlgItem(IDC_ED_NAME)->SetWindowText(m_sName);
	}
}

void CResultSelectDlg::OnNonLinear() 
{
  onAnalysisType(IDC_CK_NON_LINEAR, CAnalysisType::AT_NONLIN);
}

void CResultSelectDlg::OnHeat() 
{
  onAnalysisType(IDC_CK_HEAT, CAnalysisType::AT_HEAT);
}

void CResultSelectDlg::OnMixture() 
{
  onAnalysisType(IDC_CK_MIXTURE, CAnalysisType::AT_MIXTURE);
}

void CResultSelectDlg::OnMixtureContainment() 
{
  onAnalysisType(IDC_CK_MIXTURE_CONTAINMENT, CAnalysisType::AT_MIXTURE_CONTAINMENT);
}

QString CResultSelectDlg::Name(const IValueComponentBase* valueComponent) const
{
  QString name;
  const IResult* pDResult = 0;

  if (m_bSetLinStage ||
    !(pDResult = dynamic_cast <const IResult *> (&valueComponent->Parent())))
  {
    name = valueComponent->ExportLabel();
  }
  else
  {
    if (pDResult)
    {
      name = pDResult->ExportLabel(valueComponent->ComponentIndex());
    }
    else
    {
      name = valueComponent->Parent().ExportLabel();
    }
  }

  assert(!name.isEmpty());

  return name;
}

BOOL CResultSelectDlg::Global() const
{
	return m_bGlobal;
}

BOOL CResultSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  UpdateTimeStep();
	UpdateSelection();
	BuildTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CResultSelectDlg::Selected(const IValueComponentBase* pSelection) const
{
	bool found = false;
	for ( auto &i : m_selected_inputs_v ) 
	{
		if( i == pSelection ) 
		{
			found = true;
			break;
		}
	}
	return found;
}

void CResultSelectDlg::Select(const IValueComponentBase* pSelection)
{
	if( !Selected( pSelection ) )
		m_selected_inputs_v.push_back( pSelection );
	else // remove item
	{
		auto tmp_v = m_selected_inputs_v;
		m_selected_inputs_v.clear();

		for ( auto &i : tmp_v ) 
			if( i != pSelection ) 
				m_selected_inputs_v.push_back( i );
	}

	UpdateSelection();
	UpdateName();

	m_lbValue.UpdateTree();
}

// private

void CResultSelectDlg::init()
{
  CollectDepletionStages();
}

void CResultSelectDlg::CollectDepletionStages()
{
  const CDepletionStage* pStage = &m_model.InitialDepletionStage();

  while (pStage)
  {
    VERIFY(m_stTimeStep.insert(pStage).second);

    if (pStage->Last())
    {
      pStage = 0;
    }
    else
    {
      pStage = &pStage->Next();
    }
  }
}

void CResultSelectDlg::UpdateTimeStep()
{
  m_lbTimeStep.DeleteAllItems();
  m_lbTimeStep.InsertColumn(0, "Depletion stage", LVCFMT_LEFT, 130);

  const CDepletionStage* pStage = &m_model.InitialDepletionStage();

  while (pStage)
  {
    CDepletionStageObserver <CResultSelectDlg> * depletionStageObserver =
      new CDepletionStageObserver <CResultSelectDlg> (*pStage, *this,
        m_lbTimeStep);

    depletionStageObserver->ToggleState();

    if (pStage->Last())
    {
      pStage = 0;
    }
    else
    {
      pStage = &pStage->Next();
    }
  }
}

void CResultSelectDlg::onAnalysisType(int dialogItem,
  CAnalysisType::TAnalysisType analysisType)
{
  if (((CButton*) GetDlgItem(dialogItem))->GetCheck() == BST_CHECKED)
  {
    m_AnalysisTypes.insert(CAnalysisType(analysisType));
  }
  else
  {
    m_AnalysisTypes.erase(
      m_AnalysisTypes.find(CAnalysisType(analysisType)));
  }

  UpdateSelection();
}

void CResultSelectDlg::mapAnalysisTypes2GUI(
  const TAnalysisTypes& analysisTypes) const
{
  for (TAnalysisTypes::const_iterator analysisType = analysisTypes.begin();
    analysisType != analysisTypes.end(); ++analysisType)
  {
    if ((*analysisType).first == CAnalysisType::AT_LINEAR)
    {
      GetDlgItem(IDC_CK_LINEAR)->EnableWindow(!m_bInput && m_bSetLinStage);
    }
    else if ((*analysisType).first == CAnalysisType::AT_NONLIN)
    {
      GetDlgItem(IDC_CK_NON_LINEAR)->EnableWindow(!m_bInput && m_bSetLinStage);
    }
    else if ((*analysisType).first == CAnalysisType::AT_HEAT)
    {
      GetDlgItem(IDC_CK_HEAT)->EnableWindow(!m_bInput && m_bSetLinStage);
    }
    else if ((*analysisType).first == CAnalysisType::AT_MIXTURE)
    {
      GetDlgItem(IDC_CK_MIXTURE)->EnableWindow(!m_bInput && m_bSetLinStage);
    }
    else if ((*analysisType).first == CAnalysisType::AT_MIXTURE_CONTAINMENT)
    {
      GetDlgItem(IDC_CK_MIXTURE_CONTAINMENT)->EnableWindow(!m_bInput && m_bSetLinStage);
    }
  }

  GetDlgItem(IDC_LB_TIMESTEP)->EnableWindow(!m_bInput && m_bSetLinStage);
}

// CResultSelectDlg::CResultComponentSetIterator

CResultSelectDlg::CResultComponentSetIterator::TResultsFromDialog
  CResultSelectDlg::CResultComponentSetIterator::resultsFromDialog;

CResultSelectDlg::CResultComponentSetIterator::CResultComponentSetIterator(
  CResultSelectDlg* resultSelectDlg)
: m_ResultComponentSet(resultSelectDlg->m_RGObserver->Results())
, m_Mode(0)
, m_Component(0)
, m_Ended(false)
, m_AnalysisTypes(resultSelectDlg->m_AnalysisTypes)
, m_SetLinStage(resultSelectDlg->m_bSetLinStage)
, m_stTimeStep(resultSelectDlg->m_stTimeStep)
{
  m_ResultComponentSet.insert(resultSelectDlg->m_GVTRGObserver->Results().begin(), resultSelectDlg->m_GVTRGObserver->Results().end());
  m_ResultComponent = m_ResultComponentSet.begin();
}

CResultSelectDlg::CResultComponentSetIterator::CResultComponentSetIterator(
  CResultSelectDlg* resultSelectDlg, TResultsFromDialog& /* resultsFromDialog */)
  : m_ResultComponentSet(resultSelectDlg->m_Results)
, m_ResultComponent(m_ResultComponentSet.begin())
, m_Mode(0)
, m_Component(0)
, m_Ended(false)
, m_AnalysisTypes(resultSelectDlg->m_AnalysisTypes)
, m_SetLinStage(resultSelectDlg->m_bSetLinStage)
, m_stTimeStep(resultSelectDlg->m_stTimeStep)
{
}

const IValueComponentBase* CResultSelectDlg::CResultComponentSetIterator::get()
{
    for (; m_ResultComponent != m_ResultComponentSet.end(); ++m_ResultComponent)
    {

      const CValueType *vt = dynamic_cast<const CValueType *>((*m_ResultComponent).first);
      for (; m_Component < (*m_ResultComponent).first->ComponentSize();
        ++m_Component)
      {
        for (; m_Mode < (*m_ResultComponent).first->ModeSize(); ++m_Mode)
        {
          const IValueComponentBase& valueComponent =
            (*m_ResultComponent).first->Component(m_Component, m_Mode);
          const CMeshResult::CMeshResultComponent* meshResultComponent =
            dynamic_cast <const CMeshResult::CMeshResultComponent*> (
              &valueComponent);

          if (meshResultComponent == 0 && vt == 0)
          {
            const IResultComponent& iResultComponent =
              dynamic_cast <const IResultComponent&> (valueComponent);
            TResultComponent component((*m_ResultComponent).first, m_Component);

            if ((m_ResultComponentSet.find(component) ==
              m_ResultComponentSet.end()) ||
              ((int)m_Component != (*m_ResultComponent).second))
            {
              break;
            }

            // By first verifying 'm_SetLinStage' the contents of
            // 'm_AnalysisTypes' and 'm_stTimeStep' are not checked!

            if (!m_SetLinStage && (m_Mode > 0))
            {
              continue;
            }
            else if ((m_AnalysisTypes.size() > 0) &&
              (m_AnalysisTypes.find(iResultComponent.AnalysisType()) ==
                m_AnalysisTypes.end()))
            {
              continue;
            }
            else if ((m_stTimeStep.size() > 0) &&
              (m_stTimeStep.find(&iResultComponent.Stage()) ==
                m_stTimeStep.end()))
            {
              continue;
            }
          }

          ++m_Mode;

          return &valueComponent;
        }

        m_Mode = 0;
      }

      m_Component = 0;
    }

  m_Ended = true;

  return 0;
}
