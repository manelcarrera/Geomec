// HexaHorizon.cpp: implementation of the CHexaHorizon class.
//
//////////////////////////////////////////////////////////////////////

#include "HexaEntryTypes.h"
#include "HexaModel.h"
#include "HexaHorizon.h"
#include "BoundaryBase.h"
#include "MeshBase.h"
#include "BranchState.h"
#include "FemAppModel.h"
#include "ModelBase.h"
#include "GlobalMessage.h"
#include "HexaFormation.h"
#include "BodyGroup.h"
#include "ElementGroup.h"

#define ELEMENTS_HEIGHT	2

bool CHexaHorizon::CFaceXYLess::operator()(const geo::IFace* f1, const geo::IFace* f2) const
{
  assert(f1->NrOfPoints() == f2->NrOfPoints());
  int nSize = f1->NrOfPoints();
  for(int i = 0; i < nSize; i++)
  {
    if(f1->Point(i).X() < f2->Point(i).X())
      return true;
    if(f1->Point(i).X() > f2->Point(i).X())
      return false;
    if(f1->Point(i).Y() < f2->Point(i).Y())
      return true;
    if(f1->Point(i).Y() > f2->Point(i).Y())
      return false;
  }

  return false;
}

//default constructor...
CHexaHorizon::CHexaHorizon(CFemAppModel &model)
:C3DHorizon(model),
  m_pInterfaceElementGroup(0)
{
}

CHexaHorizon::CHexaHorizon(const QString& strInstanceName, const double& dDepth, CFemAppModel& model, bool bAttachToModel)
:C3DHorizon(strInstanceName, dDepth, model, bAttachToModel),
  m_pInterfaceElementGroup(0)
{
  if( bAttachToModel )
    AttachToEntry();

  // Link to boundary
  LinkTo(((CModelBase&)model).Boundary());

  // Link to mesher
  LinkTo(((CModelBase&)model).Mesh());
}

void CHexaHorizon::AttachToEntry()
{
  // Link to hexa horizon entry
  assert(Model().GraphEntry(MD_HEXA_HORIZON));
  reParent(Model().GraphEntry(MD_HEXA_HORIZON));
}


CHexaHorizon::CHexaHorizon(CSurfaceBase &surface, CFemAppModel& model, bool bSlip, bool bAttachToModel)
: C3DHorizon(surface, model, bSlip, bAttachToModel),
  m_pInterfaceElementGroup(0)
{
  if(bAttachToModel)
  {
    // Link to hexa horizon entry
    assert(model.GraphEntry(MD_HEXA_HORIZON));
    reParent(model.GraphEntry(MD_HEXA_HORIZON));
  }

  CModelBase& modelbase = static_cast<CModelBase&>(model);

  // Link to boundary
  LinkTo(modelbase.Boundary());

  // Link to mesher
  LinkTo(modelbase.Mesh());
}

CHexaHorizon::CHexaHorizon(const CHexaHorizon &rhs)
: C3DHorizon(rhs), m_pInterfaceElementGroup(rhs.m_pInterfaceElementGroup)
{
}
  
CHexaHorizon::~CHexaHorizon()
{
}

bool CHexaHorizon::ConnectItem(const CGraphNode &item)
{
  CSurfaceBase *pSurf = const_cast<CSurfaceBase*> (dynamic_cast<const CSurfaceBase*> (&item));
  if (pSurf)
  {
    if(!IsLinkedTo(item))
    {
      if(SurfaceSize() == 0)
      {
        LinkTo(const_cast<CGraphNode&>(item));
        Modified();
        return true;
      }
      else
      {
        assert(SurfaceSize() == 1);
        QString sMessage;
        sMessage = QString("Surface %1 is dropped on horizon %2. Do you want to merge them?").arg(item.Name()).arg(Name());
        if(_m()->msg(sMessage, MB_ICONQUESTION | MB_YESNO) == IDYES)
          MergeSurface(*pSurf);
        return true;
      }
    }
  }

  return C3DHorizon::ConnectItem(item);
}

bool CHexaHorizon::CanDisconnectItem(const CGraphNode& item) const
{
  for(int i = 0; i < SurfaceSize(); i++)
  {
    if(&Surface(i) == &item)
      return (SurfaceSize() > 0 && !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch());
  }

  return C3DHorizon::CanDisconnectItem(item);
}

void CHexaHorizon::OnNeighbourModified(const CGraphNode &item, enum ModifiedHint uHint)
{
  CModelBase &model = dynamic_cast<CModelBase&>(Model());
  if(&model.Mesh() == &item) {
    m_stHexFaces.clear();
    if(!model.Mesh().IsMesh())
  {
      m_pInterfaceElementGroup = 0;
      Modified(uHint);
  }
  }

  C3DHorizon::OnNeighbourModified(item, uHint);
}

bool CHexaHorizon::Less(const CGraphNode &node) const
{
  const CHexaHorizon* pHorizon = dynamic_cast<const CHexaHorizon*>(&node);
  if(pHorizon)
  {
    if(pHorizon == this)
      return false;

    while(pHorizon)
    {
      CHexaFormation *pUpper = pHorizon->UpperFormation();
      if(pUpper)
        pHorizon = &pUpper->UpperHorizon();
      else
        pHorizon = 0;

      if(pHorizon == this)
        return true;
    }


    return false;
  }

  return C3DHorizon::Less(node);
}

bool CHexaHorizon::CanConnectItem(const CGraphNode &item) const
{
  CSurfaceBase *pSurf = const_cast<CSurfaceBase*>(dynamic_cast<const CSurfaceBase*> (&item));

  if(pSurf)
  {
    // don't allow modifications in branched state
    if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
      return false;

    if(pSurf->Used() != 0)
      return false;

    // Find upper and lower horizon ....
    const CHexaHorizon *pUpper = 0;
    const CHexaHorizon *pLower = 0;

    if(UpperFormation())
      pUpper = &UpperFormation()->UpperHorizon();

    if(LowerFormation())
      pLower = &LowerFormation()->LowerHorizon();

    if((pUpper) && (pUpper->Min().Z() >= pSurf->Min().Z()))
      return false;

    if((pLower) && (pLower->Max().Z() <= pSurf->Max().Z()))
      return false;

    return true;
  }

  return C3DHorizon::CanConnectItem(item);
}

bool CHexaHorizon::CanDestroy() const
{
  if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
    return false;

  return C3DHorizon::CanDestroy();
}

bool CHexaHorizon::Destroy()
{
  assert(CanDestroy());

  CHexaFormation* pUpper = UpperFormation();
  CHexaFormation* pLower = LowerFormation();
  CHexaFormation* pSelection = 0;

  if(pUpper)
  pSelection = pUpper;
  else if(pLower)
  pSelection = pLower;

  return LocalDestroy(pUpper, pLower, pSelection);
}

bool CHexaHorizon::LocalDestroy(CHexaFormation* pUpper, CHexaFormation* pLower,
  const CHexaFormation* selection)
{
  CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
  assert(pModel);
  pModel->InvalidateMesh();

  if (selection != 0)
    {
      // Really delete the shit ...
      if (selection == pUpper)
      {
        // Delete upper formation 
        if(pLower)
          pLower->UpperHorizon(pUpper->UpperHorizon());
        else
          pUpper->UpperHorizon().Slip(false);
        
        delete pUpper;
      }
      else
      {
        if(pUpper)
          pUpper->LowerHorizon(pLower->LowerHorizon());
        else
          pLower->LowerHorizon().Slip(false);

        delete pLower;
      }
    }

  // Bug 908 (Mantis) When the top or bottom horizon is a sliplayer switch sliplayer off
  if(UpperFormation())
  {
    if((UpperFormation()->UpperHorizon().UpperFormation() == 0) && UpperFormation()->UpperHorizon().Slip())
      UpperFormation()->UpperHorizon().Slip(false);
  }
  else
  {
    if(Slip())
      Slip(false);
  }

  if(LowerFormation())
  {
    if((LowerFormation()->LowerHorizon().LowerFormation() == 0) && LowerFormation()->LowerHorizon().Slip())
      LowerFormation()->LowerHorizon().Slip(false);
  }
  else
  {
    if(Slip())
      Slip(false);
  }

  delete this;

  return true;
}

      
bool CHexaHorizon::operator==(const CHexaHorizon& rhs) const
{
  return C3DHorizon::operator==(rhs);
}

CHexaHorizon& CHexaHorizon::operator=(const CHexaHorizon& rhs)
{
  C3DHorizon::operator =(rhs);
  return *this;
}

int CHexaHorizon::DisplayListSize() const
{
  CHexaModel* pModel = (CHexaModel*)(&Model());

  if(pModel->IsMesh() && !Slip() && (UpperFormation() || LowerFormation()))
      return 1;
  
  return C3DHorizon::DisplayListSize();
}
  
const geo::IObject& CHexaHorizon::DisplayList(int nIndex) const
{
  if(IsCopy())
    return C3DHorizon::DisplayList(nIndex);
  
  CHexaModel* pModel = (CHexaModel*)(&Model());

  if(pModel->IsMesh() && !Slip())
  return MeshedSurface(0);
  
  return C3DHorizon::DisplayList(nIndex);
}

CHexaFormation* CHexaHorizon::UpperFormation() const
{
  for(size_t i = 0; i < referenceSize(); i++)
  {
    const CHexaFormation *pFormation = dynamic_cast<const CHexaFormation*> (&referenceAt(i));
    if(pFormation)
    {
      if(IsCopy())
      {
        if(&pFormation->LowerHorizon() == &Source())
          return const_cast<CHexaFormation*>(pFormation);
      }
      else
      {
        if(&pFormation->LowerHorizon() == this)
          return const_cast<CHexaFormation*>(pFormation);
      }
    }
  }

  return 0;
}

CHexaFormation* CHexaHorizon::LowerFormation() const
{
  for(size_t i = 0; i < referenceSize(); i++)
  {
    const CHexaFormation *pFormation = dynamic_cast<const CHexaFormation*> (&referenceAt(i));
    if(pFormation)
    {
      if(IsCopy())
      {
        if(&pFormation->UpperHorizon() == &Source())
          return const_cast<CHexaFormation*>(pFormation);
      }
      else
      {
        if(&pFormation->UpperHorizon() == this)
          return const_cast<CHexaFormation*>(pFormation);
      }
    }
  }

  return 0;
}

void CHexaHorizon::MoveUp()
{
  CHexaFormation *pLower	= LowerFormation();
  CHexaFormation *pMid		= UpperFormation(); 
  assert(pMid);
  CHexaHorizon *pHorizon		= &pMid->UpperHorizon();
  CHexaFormation *pUpper	= pHorizon->UpperFormation();

  // Unlink the horizons from the formations
  if(pLower)
    UnLink(*pLower);

  if(pUpper)
    pHorizon->UnLink(*pUpper);

  // Set new member variable of the formation and restore the linkage
  if(pLower)
  {
    pLower->LinkTo(*pHorizon);
    pLower->m_pUpper = pHorizon;
  }

  pMid->m_pLower = pHorizon;
  pMid->m_pUpper = this;

  if(pUpper)
  {
    LinkTo(*pUpper);
    pUpper->m_pLower = this;
  }

  Modified();
}

void CHexaHorizon::MoveDown()
{
  CHexaFormation *pUpper	= UpperFormation();
  CHexaFormation *pMid		= LowerFormation(); 
  assert(pMid);
  CHexaHorizon *pHorizon		= &pMid->LowerHorizon(); 
  CHexaFormation *pLower	= pHorizon->LowerFormation();

  // Switch horizons
  if(pUpper)
    UnLink(*pUpper);

  if(pLower)
    pLower->UnLink(*pHorizon);

  // Set new member variable of the formation and restore the linkage
  if(pUpper)
  {
    pUpper->LinkTo(*pHorizon);
    pUpper->m_pLower = pHorizon;
  }

  pMid->m_pUpper = pHorizon;
  pMid->m_pLower = this;

  if(pLower)
  {
    LinkTo(*pLower);
    pLower->m_pUpper = this;
  }

  Modified();
}

bool CHexaHorizon::CanMove() const
{
  return !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();
}

CHexaHorizonEntry::CHexaHorizonEntry(CHexaModel& model)
: CStorageNodeEntry<CHexaHorizon>(MD_HEXA_HORIZON, IDI_HORIZONS, IDS_TREE_HORIZONS, model)
{
}

bool CHexaHorizonEntry::ConnectItem(const CGraphNode &item)
{

  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
  if(pSurface)
  {
    CHexaHorizon *pHorizon = new CHexaHorizon((CSurfaceBase&)(*pSurface), Model());
    InsertHorizon(*pHorizon);
    return true;
  }

  return false;
}

void CHexaHorizonEntry::InsertHorizon(CHexaHorizon &horizon)
{
  CHexaModel* pModel = dynamic_cast<CHexaModel*>(&horizon.Model());
  assert(pModel);

  assert(horizon.LowerFormation() == 0);
  assert(horizon.UpperFormation() == 0);

  TNodeSet stHorizon = EntryNodes();
  if((stHorizon.size() == 2) || (stHorizon.size() == 1))
  {
    // One horizon other is also found ...
    for(TNodeSet::iterator it = stHorizon.begin(); it != stHorizon.end(); it++)
    {
      if((*it) != &horizon)
      {
        CHexaHorizon *pOther = *it;
        if(pOther->Min().Z() > horizon.Min().Z())
          new CHexaFormation(horizon.Name().toStdString().c_str(), horizon, *pOther, ELEMENTS_HEIGHT, (CModelBase&)horizon.Model());
        else
          new CHexaFormation(pOther->Name().toStdString().c_str(), *pOther, horizon, ELEMENTS_HEIGHT, (CModelBase&)horizon.Model());

      }
    }
    // Reorder tree
    horizon.Modified();
    return;
  }

  // Search first (highest formation)
  const CHexaFormation *pFormation = 0;
  for(TNodeSet::iterator it = stHorizon.begin(); it != stHorizon.end(); it++)
  {
    if(((*it) != &horizon) && (*it)->UpperFormation() == 0)
      pFormation = (*it)->LowerFormation();
  }

  // Surface lays above model ..
  if(horizon.Min().Z() < pFormation->UpperHorizon().Min().Z())
  {
    new CHexaFormation(horizon.Name().toStdString().c_str(), horizon, (CHexaHorizon&)(pFormation->UpperHorizon()), ELEMENTS_HEIGHT, (CModelBase&)horizon.Model());
    pModel->InvalidateMesh();
    horizon.Modified();
    return;	// Exit
  }

  while(pFormation)
  {
    if(pFormation->CanSplitFormation(horizon))
    {
      ((CHexaFormation*)pFormation)->SplitFormation(horizon);
      pModel->InvalidateMesh();
      horizon.Modified();
      return;	// Exit
    }

    if(!pFormation->LowerFormation())
    {
      new CHexaFormation(pFormation->LowerHorizon().Name().toStdString().c_str(), (CHexaHorizon&)(pFormation->LowerHorizon()), horizon, ELEMENTS_HEIGHT, (CModelBase&)horizon.Model());
      pModel->InvalidateMesh();
      horizon.Modified();
      return;
    }

    pFormation = pFormation->LowerFormation();
  }

}

bool CHexaHorizonEntry::CanDisconnectItem(const CGraphNode& item) const
{
  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
  if(pSurface)
    return !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

  return CStorageNodeEntry<CHexaHorizon>::CanDisconnectItem(item);
}

bool CHexaHorizonEntry::CanConnectItem(const CGraphNode &item) const
{
  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
  if(pSurface)
  {
    if(!(static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
    {
      // Is surface locked ...
      if(pSurface->Used())
        return false;

      // Search first (highest formation)
      TNodeSet stHorizon = EntryNodes();
      CHexaFormation *pFormation = 0;
      switch(stHorizon.size())
      {
      case 0:
      case 1:
        return true;
        break;
      default:
        // Walk to the upper formation 
    for(size_t i = 0; i < (*stHorizon.begin())->referenceSize(); i++)
        {
          if(dynamic_cast<CHexaFormation*> (&(*stHorizon.begin())->referenceAt(i)))
            pFormation = dynamic_cast<CHexaFormation*> (&(*stHorizon.begin())->referenceAt(i));
        }
        break;
      };

      assert(pFormation);
      while(pFormation->UpperFormation())
        pFormation = (CHexaFormation*) pFormation->UpperFormation();

      if(pSurface->Surface().Min().Z() < pFormation->UpperHorizon().Min().Z())
        return true;

      while(pFormation)
      {
        CHexaHorizon horizon((CSurfaceBase&)(*pSurface), const_cast<CFemAppModel&>(Model()));
        if(pFormation->CanSplitFormation(horizon))
          return true;

        // Is this the last formation?
        if(!pFormation->LowerFormation())
        {
          // Is surface lower than the lowest formation ?
          if(pSurface->Surface().Max().Z() > pFormation->LowerHorizon().Max().Z())
                return true;
        }

        pFormation = (CHexaFormation*)pFormation->LowerFormation();
      }
    
    }
  }

  return false;
}

CHexaHorizonEntry::THorizonVec CHexaHorizonEntry::Horizons() const
{
  THorizonVec vcRet;

  if(EntryNodes().size() > 0)
  {
    CFemAppModel* pModel = &(*EntryNodes().begin())->Model();
    CHexaFormationEntry *pEntry = (CHexaFormationEntry*)pModel->GraphEntry(MD_HEXA_FORMATION);
    CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
    if(stNode.size() > 0)
    {
      // Find upper formation ...
      CHexaFormation *pFormation = 0;
      for(CHexaFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
      {
        if((*it)->UpperFormation() == 0)
          pFormation = *it;
      }
      assert(pFormation);
      
      // Walk over formations
      while(pFormation)
      {
        // Add upper horizon
        vcRet.push_back(&pFormation->UpperHorizon());

        if(!pFormation->LowerFormation())
          vcRet.push_back(&pFormation->LowerHorizon());
        pFormation = (CHexaFormation*) pFormation->LowerFormation();
      }
    }
    else
    {
      // Get the horizons
      TNodeSet stHorizon = EntryNodes();

      // Init the formation
      for(TNodeSet::iterator it = stHorizon.begin(); it != stHorizon.end(); it++)
        vcRet.push_back(*it);
    }
  }

//	This assert is not always valid. During horizon insertion the horizons are NOT linked to formations.
//	The link step is done after the insertion of the horizon, but the mesher is called before the link step.
//	assert(vcRet.size() == stHorizon.size());


  return vcRet;

}

void CHexaHorizon::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  C3DHorizon::LoadStream(stream, version, progress);

  // Link to entry
  reParent(Model().GraphEntry(MD_HEXA_HORIZON));

  // Link to boundary
  CModelBase &model = dynamic_cast<CModelBase&>(Model());
  LinkTo(model.Boundary());

  // Link to mesher
  LinkTo(model.Mesh());
}

void CHexaHorizon::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  C3DHorizon::SaveStream(stream, progress);
}

long CHexaHorizon::SavedItems() const
{
  return C3DHorizon::SavedItems();
}

const geo::IFace* CHexaHorizon::FindXYFace(const geo::IFace& face) const
{
  if(m_stHexFaces.size() == 0)
  {
    // Find out which surface we need ...
    const geo::ISurface *pSurface = 0;
    if(LowerFormation())
    {
      assert(LowerFormation()->VolumeSize() == 1);
      assert(LowerFormation()->Volume(0).Volume().SideSurfaceSize() == 6);
      pSurface = &LowerFormation()->Volume(0).Volume().SideSurface(0);
    }
    else
    {
      assert(UpperFormation());
      assert(UpperFormation()->VolumeSize() == 1);
      assert(UpperFormation()->Volume(0).Volume().SideSurfaceSize() == 6);
      pSurface = &UpperFormation()->Volume(0).Volume().SideSurface(1);
    }

    assert(pSurface);

    // Add face from surface to mesher ...
    for(int i = 0; i < pSurface->FaceSize(); i++)
    {
      VERIFY(m_stHexFaces.insert(&pSurface->Face(i)).second);
    }
  }

  TFaceSet::const_iterator it = m_stHexFaces.find(&face);
  if(it != m_stHexFaces.end())
    return (geo::IFace*)(*it);
  return 0;
}

#ifdef _DEBUG
void CHexaHorizon::AssertValid() const
{
  C3DHorizon::AssertValid();
}
#endif //_DEBUG

/*
//=============================================================================
IValueComponentBase::TValueVec CHexaHorizon::CalculatePorePressure(const geo::IBody& body) const
{
  IValueComponentBase::TValueVec vcPressure;
  // We only support hexahedron interface elements
  const geo::CInterfaceElement *pElement = dynamic_cast<const geo::CInterfaceElement*>(&body);
  if(pElement)
  {
    bool found = false;
    assert(m_pInterfaceElementGroup);
    for(int i = 0; i < m_pInterfaceElementGroup->FaceSize(); i++)
      if(&m_pInterfaceElementGroup->Face(i) == pElement)
        found = true;

    if (found)
    {
      CDepletionStage *pInitialDepletionStage = &((CModelBase&)Model()).InitialDepletionStage();
      CHexaFormation *pUpperFormation = UpperFormation();
      CHexaFormation *pLowerFormation = LowerFormation();

      // Get pressures from the upper and lower elements.
      const geo::IBody& upperBody = pElement->Front().Parent();
      for (i=0; i<4; i++)
        vcPressure.push_back(pUpperFormation->Pressure(*pInitialDepletionStage).Component().ValuePoint(upperBody.Point(i)));
      const geo::IBody& lowerBody = pElement->Back().Parent();
      for (i=4; i<8; i++)
        vcPressure.push_back(pLowerFormation->Pressure(*pInitialDepletionStage).Component().ValuePoint(lowerBody.Point(i)));
    }
  }

  return vcPressure;
}
*/
#if 0
// Calculate the initial stress vector for the 4 nodes (x,y,z positions) of the interface element.
std::vector<geo::CVector*> CHexaHorizon::CalculateInitialStress(const CHexaInterfaceElement& interface_element) const
{
  std::vector<geo::CVector*> vcStress;

  // Get the stess and pressures of the upper and lower elements.
  CDepletionStage *pInitialDepletionStage = &((CModelBase&)Model()).InitialDepletionStage();
  IValueComponentBase::TValueVec vcPressureUpper = UpperFormation()->CalculatePorePressure(interface_element.GetUpperBody(), *pInitialDepletionStage);
  IValueComponentBase::TValueVec vcPressureLower = LowerFormation()->CalculatePorePressure(interface_element.GetLowerBody(), *pInitialDepletionStage);
  std::vector<CTensor> vcStressUpper = UpperFormation()->CalculateEffectiveStress(interface_element.GetUpperBody());
  std::vector<CTensor> vcStressLower = LowerFormation()->CalculateEffectiveStress(interface_element.GetLowerBody());

  geo::CVector v(interface_element.Point(0));
  geo::CVector vn = v.GetNormal();
  geo::CVector vtx(interface_element.Point(0), interface_element.Point(1));
  geo::CVector vty = vn.CrossProduct(vtx);

  geo::CMatrix mn(3,1), mtx(3,1), mty(3,1), mnT(1,3);
  mn.Value(0, 0, vn.X());
  mn.Value(0, 1, vn.Y());
  mn.Value(0, 2, vn.Z());
  mnT = mn.GetTransposed();

  CTensor *pStressTensor;

  for (int i=0; i<4; i++)
  {
    // Take the stresstensor from the element where the porepressure has the largest value.
    if (vcPressureUpper[i].Value() > vcPressureLower[i+4].Value())
      pStressTensor = &vcStressUpper[i];
    else
      pStressTensor = &vcStressLower[i+4];

    vn.X(interface_element.Point(i).X());
    vn.Y(interface_element.Point(i).Y());
    vn.Z(interface_element.Point(i).Z());
    mn.Value(0, 0, vn.X());
    mn.Value(0, 1, vn.Y());
    mn.Value(0, 2, vn.Z());

    const double t1 = (mnT * (pStressTensor->Matrix() * mn)).Value(0, 0);
    const double t2 = (mnT * (pStressTensor->Matrix() * mtx)).Value(0, 0);
    const double t3 = (mnT * (pStressTensor->Matrix() * mty)).Value(0, 0);

    vcStress.push_back(new geo::CVector(t1, t2, t3));
  }

  return vcStress;
}

#endif

int CHexaHorizon::MeshedSurfaceSize() const
{
  return 1;
}

const geo::ISurface& CHexaHorizon::MeshedSurface(int nIndex) const
{
  assert(nIndex == 0); // there can be only one

  if(LowerFormation())
  {
    assert(LowerFormation()->VolumeSize() == 1);
    return LowerFormation()->Volume(0).Volume().SideSurface(0);
  }

  assert(UpperFormation());
  assert(UpperFormation()->VolumeSize() == 1);
  return UpperFormation()->Volume(0).Volume().SideSurface(1);
}

int CHexaHorizon::MeshFaceSize() const
{
  return InterfaceElementGroup()->ElementSize();
}

const geo::IFace &CHexaHorizon::MeshFace(int nIndex) const
{
  const geo::IFace *pFace = dynamic_cast<const geo::IFace*>(&InterfaceElementGroup()->Element(nIndex));
  assert(pFace);
  return *pFace;
}

int CHexaHorizon::BodyFaceSize() const
{
  CHexaModel* pModel = (CHexaModel*)(&Model());
  int sz = 0;

  if(pModel->IsMesh())
  {
    if(UpperFormation())
    {
      assert(UpperFormation()->VolumeSize() == 1);
      sz += UpperFormation()->Volume(0).Volume().SideSurface(1).FaceSize();
    }
    if(LowerFormation())
    {
      assert(LowerFormation()->VolumeSize() == 1);
      sz += LowerFormation()->Volume(0).Volume().SideSurface(0).FaceSize();
    }
  }

  return sz;
}

const geo::IFace &CHexaHorizon::BodyFace(int nIndex) const
{
  CHexaModel* pModel = (CHexaModel*)(&Model());
  assert(pModel->IsMesh());
  const geo::IFace *pFace = 0;

  if(UpperFormation())
  {
    if(nIndex < UpperFormation()->Volume(0).Volume().SideSurface(1).FaceSize())
      pFace = &UpperFormation()->Volume(0).Volume().SideSurface(1).Face(nIndex);
    else
    {
      assert(LowerFormation());
      pFace = &LowerFormation()->Volume(0).Volume().SideSurface(0).Face(nIndex - UpperFormation()->Volume(0).Volume().SideSurface(1).FaceSize());
    }
  }
  if(!pFace)
  {
    assert(LowerFormation());
    assert(nIndex < LowerFormation()->Volume(0).Volume().SideSurface(0).FaceSize());
    pFace = &LowerFormation()->Volume(0).Volume().SideSurface(0).Face(nIndex);
  }

  assert(pFace != 0);
  return *pFace;
}

bool CHexaHorizon::CanSlip() const
{
  return (UpperFormation() != 0 && LowerFormation() != 0);
}

bool CHexaHorizon::IsTopHorizon() const
{
  return UpperFormation() == 0;
}

void CHexaHorizon::InterfaceElementGroup(const geo::CElementGroup& group)
{
  assert(Slip());
  m_pInterfaceElementGroup = &group;
}

const geo::CElementGroup* CHexaHorizon::InterfaceElementGroup() const
{
  assert(Slip());
  return m_pInterfaceElementGroup;
}


/*
CHexaHorizonEntryObserver::CHexaHorizonEntryObserver(CHexaHorizonEntry& entry, CTreeCtrl& ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
: THexaHorizonEntryObserver(entry, ctrl, false, DELETE_ITEM, hParent, hInsertAfter, true, false)
{
  Update();
}

CTreeNode* CHexaHorizonEntryObserver::InsertChild(CHexaHorizon& t)
{
  CTreeNode* pTreeObserver = THexaHorizonEntryObserver::InsertChild(t);

  if(t.Slip())
    new CFaultPressureEnumerator(*pTreeObserver);

  return pTreeObserver;
}

void CHexaHorizonEntryObserver::OnChildModified(CTreeNode &child)
{
  CHexaHorizon& hor = dynamic_cast<CHexaHorizon&>(child.ObservedItem());

  if(hor.Slip())
  {
    bool bFound = false;
    CTreeNode::TChildren vcChildren = child.Children();
    int i;
    for(size_t i = 0; i < vcChildren.size(); ++i)
    {
      if(dynamic_cast<CFaultPressureEnumerator*>(vcChildren[i]))
      {
        bFound = true;
        break;
      }
    }

    if(!bFound)
      new CFaultPressureEnumerator(child);
  }
  else
  {
    CTreeNode::TChildren vcChildren = child.Children();
    int i;
    for(size_t i = 0; i < vcChildren.size(); ++i)
    {
      CFaultPressureEnumerator* fpe = dynamic_cast<CFaultPressureEnumerator*>(vcChildren[i]);
      if(fpe)
      {
        delete fpe;
        break;
      }
    }
  }

  THexaHorizonEntryObserver::OnChildModified(child);
}



CFaultPressureEnumerator::CFaultPressureEnumerator(CTreeNode& parent)
: TFaultPressureEnumerator(parent, _T("Pressures"), IDI_PRESSURES)
{
}
*/