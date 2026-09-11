// Pressure.cpp: implementation of the CPressure class.
//
//////////////////////////////////////////////////////////////////////
#include "Pressure.h"
#include "FormationBase.h"
#include "GlobalPressure.h"
#include "MeshBase.h"
#include "ValueTypes.h"
#include "DepletionStage.h"
#include "resourceIDI.h"

///////////// CComponentRepeater

CComponentRepeater::CComponentRepeater(CPressure &pressure)
: CFormationLoadRepeaterComponent<TPressure, CComponentConstant <TPressure>, CComponentRepeater, CComponentGWC>(pressure)
{
}

CComponentRepeater::CComponentRepeater(const CComponentRepeater& rhs)
: CFormationLoadRepeaterComponent<TPressure, CComponentConstant <TPressure>, CComponentRepeater, CComponentGWC>(rhs)
{
}

unsigned int CComponentRepeater::TypeId() const
{
  const CPressure *pPressure = dynamic_cast<const CPressure*> (&FormationLoadBase());
  assert(pPressure);
  switch(pPressure->Type())
  {
  case CPressure::CONSTANT_MODE:
    return pPressure->Constant().TypeId();
  case CPressure::REPEAT_MODE:
    return IDT_COMPONENT_PRESSURE_REPEAT;
  case CPressure::GWC_MODE:
    return pPressure->GWC().TypeId();
  default:
    assert(false); // Bogus
  };

  return 0;
}

unsigned int CComponentRepeater::IconId() const
{
  const CPressure *pPressure = dynamic_cast<const CPressure*> (&FormationLoadBase());
  assert(pPressure);

  const CPressure *pParentPressure = pPressure->ParentPressure();

  bool bSimilarToParent = pParentPressure && pPressure->IsSimilar(*pParentPressure, false);

  switch(pPressure->Type())
  {
  case CPressure::CONSTANT_MODE:
  if (bSimilarToParent && pPressure->Constant() == pParentPressure->Constant())
      return IDI_COMPONENT_PRESSURE_PARENT;
  else
      return pPressure->Constant().IconId();
  case CPressure::REPEAT_MODE:
  if (pPressure->DepletionStage().Initial())
  {
      if (bSimilarToParent)
    return IDI_COMPONENT_PRESSURE_PARENT;
      else
    return IDI_COMPONENT_PRESSURE_GLOBAL;
  }
    else
  {
      const CPressure *pPrevPressure = &pPressure->Formation()->Pressure(pPressure->DepletionStage().Previous());
      assert(pPrevPressure);

      if (pPressure->IsSimilar(*pPrevPressure))
    return IDI_COMPONENT_PRESSURE_REPEAT;

      if (bSimilarToParent)
    return IDI_COMPONENT_PRESSURE_PARENT;

      return IDI_COMPONENT_PRESSURE_REPEAT_CHANGE;
    }
  case CPressure::GWC_MODE:
  if (bSimilarToParent && pPressure->GWC() == pParentPressure->GWC())
      return IDI_COMPONENT_PRESSURE_PARENT;
  else
      return pPressure->GWC().IconId();
  default:
    assert(false);
  };
  
  return 0;
}

QString CComponentRepeater::TypeName() const
{
  const CPressure *pPressure = dynamic_cast<const CPressure*> (&FormationLoadBase());
  assert(pPressure);
  switch(pPressure->Type())
  {
  case CPressure::CONSTANT_MODE:
    return pPressure->Constant().TypeName();
  case CPressure::REPEAT_MODE:
    {
      return getStringTableEntry(IDS_COMPONENT_PRESSURE_REPEAT);
    }
  case CPressure::GWC_MODE:
    return pPressure->GWC().TypeName();
  default:
    assert(false); // Bogus
  };

  return QString();
}

QString CComponentRepeater::UnitName(const CQuantity::UNIT unit) const
{
  QString sRet;
  if(unit == CQuantity::SI_UNIT)
    sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
  else
    sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

  return sRet;
}

QString CComponentRepeater::ExportLabel() const
{
   return getStringTableEntry(IDS_ET_PRESSURE);
}

const CComponentRepeater::TFormationLoad& CComponentRepeater::PreviousFormationLoad() const
{
  const CPressure *pPressure = dynamic_cast<const CPressure*> (&FormationLoadBase());
  assert(pPressure);
  assert(!pPressure->DepletionStage().Initial());
  return pPressure->Formation()->Pressure(pPressure->DepletionStage().Previous());
}

const IValueComponentBase& CComponentRepeater::GlobalValues() const
{
  const CModelBase& model = dynamic_cast<const CModelBase&> (Model());
  return model.GlobalPressure().Component();
}


////////// CComponentGWC

CComponentGWC::CGWCPressure::CGWCPressure(CPressure &current, const double &value, const UNIT unit)
: CSinglePressure(value, unit), m_current(current)
{
}

CComponentGWC::CGWCPressure::CGWCPressure(const CGWCPressure &rhs)
: CSinglePressure(rhs), m_current(rhs.m_current)
{
}

//##ModelId=3BE7AEB90005
double CComponentGWC::CGWCPressure::MaxValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  // Initialise previous pressure ....
  const CPressure *pPrev = 0;
  if(!m_current.DepletionStage().Initial())
    pPrev = &m_current.Formation()->Pressure(m_current.DepletionStage().Previous());

  while(pPrev)
  {
    switch(pPrev->Mode())
    {
    case CPressure::PREVIOUS:
      if(!pPrev->DepletionStage().Initial())
        pPrev = &m_current.Formation()->Pressure(m_current.DepletionStage().Previous());
      else
        pPrev = 0;		// Exit loop ....
      break;
    case CPressure::GWC:
      return pPrev->ReferencePressure().Value(unit);
      break;
    default:
      pPrev = 0;			// Exit loop ....
      break;
    }
  }
*/
  // Defaultly we return the max value ....
  return CSinglePressure::MaxValue( unit);
}


//##ModelId=3BE7AEB90015
double CComponentGWC::CGWCPressure::MinValue(const int /*nComponent*/, const UNIT unit) const
{
/*
    // Initialise previous pressure ....
  const CPressure *pNext = 0;
  if(!m_current.DepletionStage().Last())
    pNext = &m_current.Formation()->Pressure(m_current.DepletionStage().Next());

  while(pNext)
  {
    switch(pNext->Type())
    {
    case CPressure::PREVIOUS:
      if(!pNext->DepletionStage().Last())
        pNext = &m_current.Formation()->Pressure(m_current.DepletionStage().Next());
      else
        pNext = 0;		// Exit loop ....
      break;
    case CPressure::GWC:
      return pNext->ReferencePressure().Value(unit);
      break;
    default:
      pNext = 0;			// Exit loop ....
      break;
    }
  }
*/
  // Defaultly we return the max value ....
  return CSinglePressure::MinValue(unit);
}

CComponentGWC::CGWCPressure& CComponentGWC::CGWCPressure::operator=(const CSinglePressure &rhs)
{
  CSinglePressure::operator=(rhs);
  return *this;
}

CComponentGWC::CGWCPressure& CComponentGWC::CGWCPressure::operator = (const CGWCPressure& rhs)
{
  if (this != &rhs)
  {
  CSinglePressure::operator = (rhs);

  m_current = rhs.m_current;
  }

  return *this;
}

bool CComponentGWC::CGWCPressure::operator==(const CGWCPressure &rhs) const
{
  return CSinglePressure::operator==(rhs);
}

//##ModelId=3BE7AEB90091
CComponentGWC::CGWCDepth::CGWCDepth(CPressure &current, const double &value, const UNIT unit)
: CLengthQuantity(value, unit), m_current(current)
{
}

CComponentGWC::CGWCDepth::CGWCDepth(const CGWCDepth &rhs)
: CLengthQuantity(rhs), m_current(rhs.m_current)
{
}

//##ModelId=3BE7AEB900A1
double CComponentGWC::CGWCDepth::MaxValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  const CPressure *pNext = 0;
  if(!m_current.DepletionStage().Last())
    pNext = &m_current.Formation()->Pressure(m_current.DepletionStage().Next());

  while(pNext)
  {
    switch(pNext->Type())
    {
    case CPressure::PREVIOUS:
      if(!pNext->DepletionStage().Last())
        pNext = &m_current.Formation()->Pressure(m_current.DepletionStage().Next());
      else
        pNext = 0;		// Exit loop ....
      break;
    case CPressure::GWC:
      return pNext->ReferenceDepth().Value(unit);
      break;
    default:
      pNext = 0;			// Exit loop ....
      break;
    }
  }
*/
  // Default we return the max depth of the current formation ....
  const CModelBase& model = dynamic_cast<const CModelBase &> (m_current.Model());
  CLengthQuantity d(model.Depth(m_current.Formation()->Max()), unit);

  return d.Value(unit);
}

//##ModelId=3BE7AEB900A5
double CComponentGWC::CGWCDepth::MinValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  const CPressure *pPrev = 0;
  if(!m_current.DepletionStage().Initial())
    pPrev = &m_current.Formation()->Pressure(m_current.DepletionStage().Previous());

  while(pPrev)
  {
    switch(pPrev->Type())
    {
    case CPressure::PREVIOUS:
      if(!pPrev->DepletionStage().Initial())
        pPrev = &m_current.Formation()->Pressure(m_current.DepletionStage().Previous());
      else
        pPrev = 0;		// Exit loop ....
      break;
    case CPressure::GWC:
      return pPrev->ReferenceDepth().Value(unit);
      break;
    default:
      pPrev = 0;			// Exit loop ....
      break;
    }
  }
*/
  // Default we return the min depth of the current formation ....
  const CModelBase& model = dynamic_cast<const CModelBase &> (m_current.Model());
  CLengthQuantity d(model.Depth(m_current.Formation()->Min()), unit);

  return d.Value(unit);
}

CComponentGWC::CGWCDepth& CComponentGWC::CGWCDepth::operator=(const CLengthQuantity &rhs)
{
  CLengthQuantity::operator =(rhs);
  return *this;
}

CComponentGWC::CGWCDepth& CComponentGWC::CGWCDepth::operator = (const CGWCDepth& rhs)
{
  if (this != &rhs)
  {
  CLengthQuantity::operator = (rhs);

  m_current = rhs.m_current;
  }

  return *this;
}

bool CComponentGWC::CGWCDepth::operator==(const CGWCDepth &rhs) const
{
  return CLengthQuantity::operator ==(rhs);
}


CComponentGWC::CComponentGWC(CPressure &pressure, bool bEmpty)
: CCommonPressureComponent<TPressure>(pressure, CPressure::GWC_MODE, bEmpty),
  m_contact_pressure(pressure, 0),
  m_trans_pressure(pressure, 0),
  m_contact_depth(pressure, 0),
  m_trans_depth(pressure, 0),
  m_bTransitionFromPrevious(true)
{
  const CPressure *pPressure = dynamic_cast<const CPressure*> (&PressureBase());
  assert(pPressure);

  m_pUpperGradient = new CPressureGradientQuantity(0);

  if(IsFirstGWC())
    m_pLowerGradient = new CPressureGradientQuantity(0);
  else
    m_pLowerGradient = FirstGWC().m_pLowerGradient;

  if(!IsFirstGWC())
  {
    const CComponentGWC &prev = PreviousGWC();
    *m_pUpperGradient = prev.UpperGradient();
    m_contact_pressure = prev.ContactPressure();
    m_trans_pressure = prev.TransitionPressure();
    m_contact_depth = prev.ContactDepth();
    m_trans_depth = prev.TransitionDepth();
  }
}

CComponentGWC::CComponentGWC(const CComponentGWC &rhs)
: CCommonPressureComponent<TPressure>(rhs), 
  m_contact_pressure(rhs.m_contact_pressure),
  m_trans_pressure(rhs.m_trans_pressure),
  m_contact_depth(rhs.m_contact_depth),
  m_trans_depth(rhs.m_trans_depth),
  m_bTransitionFromPrevious(rhs.m_bTransitionFromPrevious)
{
  m_pUpperGradient = new CPressureGradientQuantity(rhs.UpperGradient());
  m_pLowerGradient = new CPressureGradientQuantity(rhs.LowerGradient());
}

CComponentGWC::~CComponentGWC()
{
  const CPressure *pPressure = (const CPressure*)(&Parent());
  assert(pPressure);

  if(pPressure->AutoDelete() || IsCopy())
    delete m_pLowerGradient;

  delete m_pUpperGradient;
}

const CComponentGWC &CComponentGWC::FirstGWC() const
{
  const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
  const CDepletionStage *pStageThis = &pres.DepletionStage();
  const CDepletionStage *pStage = pStageThis;
  pStage = &pStage->InitialStage();
  const CComponentGWC *pFirstGWC = 0;

  do
  {
    if(pres.Formation()->Pressure(*pStage).Type() == CPressure::GWC_MODE)
    {
      pFirstGWC = &pres.Formation()->Pressure(*pStage).GWC();
      break;
    }

    if(pStage->Last() || pStage == pStageThis) pStage = 0;
    else pStage = &pStage->Next();
  } while(pStage);

  if(!pFirstGWC) pFirstGWC = &pres.Formation()->Pressure(pStageThis->InitialStage()).GWC();

  return *pFirstGWC;
}

const CComponentGWC &CComponentGWC::PreviousGWC() const
{
  // find the previous GWC, not necessarily the first depletion stage
  const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
  const CDepletionStage *pStageThis = &pres.DepletionStage();
  const CDepletionStage *pStage = pStageThis;
  const CComponentGWC *pPrevGWC = 0;

  if(!pStage->Initial())
  {
    pStage = &pStage->Previous();
    do
    {
      if(pres.Formation()->Pressure(*pStage).Type() == CPressure::GWC_MODE)
      {
        pPrevGWC = &pres.Formation()->Pressure(*pStage).GWC();
        break;
      }

      if(pStage->Initial()) pStage = 0;
      else pStage = &pStage->Previous();
    } while(pStage);
  }

  if(!pPrevGWC) pPrevGWC = &pres.Formation()->Pressure(pStageThis->InitialStage()).GWC();

  return *pPrevGWC;
}

const CComponentGWC &CComponentGWC::NextGWC() const
{
  // find the next GWC

  // there is no next one if this is the last
  assert(!IsLastGWC());

  const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
  const CDepletionStage *pStage = &pres.DepletionStage();
  const CComponentGWC *pNextGWC = 0;

  if(!pStage->Last())
  {
    pStage = &pStage->Next();
    do
    {
      if(pres.Formation()->Pressure(*pStage).Type() == CPressure::GWC_MODE)
      {
        pNextGWC = &pres.Formation()->Pressure(*pStage).GWC();
        break;
      }
      if(pStage->Last()) pStage = 0;
      else pStage = &pStage->Next();
    } while(pStage);
  }

  assert(pNextGWC != 0);

  return *pNextGWC;
}

QString CComponentGWC::TypeName() const
{
  return getStringTableEntry(IDS_COMPONENT_PRESSURE_GWC);
}

const CSinglePressure& CComponentGWC::ContactPressure() const
{
  return m_contact_pressure;	
}

bool CComponentGWC::IsFirstGWC() const
{
  const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
  const CPressure &presfirst = dynamic_cast<const CPressure &> (FirstGWC().PressureBase());

  return (pres.DepletionStage().Index() == presfirst.DepletionStage().Index());
}

bool CComponentGWC::IsLastGWC() const
{
  const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
  const CDepletionStage *pStage = &pres.DepletionStage();

  while(!pStage->Last())
  {
    pStage = &pStage->Next();
    if(pres.Formation()->Pressure(*pStage).Type() == CPressure::GWC_MODE) return false; // a later one was found
  }

  return true; // no later GWC found
}

const CSinglePressure &CComponentGWC::TransitionPressure() const
{
  // for reference of later stages
  if(IsFirstGWC()) return ContactPressure();

  if(TransitionFromPrevious()) return PreviousGWC().TransitionPressure();

  return m_trans_pressure;
}

const CLengthQuantity& CComponentGWC::ContactDepth() const
{
  return m_contact_depth;
}

const CLengthQuantity &CComponentGWC::TransitionDepth() const
{
  // for reference of later stages
  if(IsFirstGWC()) return ContactDepth();

  if(TransitionFromPrevious()) return PreviousGWC().TransitionDepth();

  return m_trans_depth;
}

const CPressureGradientQuantity& CComponentGWC::UpperGradient() const
{
  assert(m_pUpperGradient);
  return *m_pUpperGradient;
}

const CPressureGradientQuantity& CComponentGWC::LowerGradient() const
{
  assert(m_pLowerGradient);
  return *m_pLowerGradient;
}

bool CComponentGWC::TransitionFromPrevious() const
{
  return m_bTransitionFromPrevious;
}

void CComponentGWC::TransitionFromPrevious(bool bNew)
{
  if(!bNew)
  {
    // copy values from previous
    const CComponentGWC &prev = PreviousGWC();
    m_trans_depth = prev.TransitionDepth();
    m_trans_pressure = prev.TransitionPressure();
  }

  m_bTransitionFromPrevious = bNew;
}

bool CComponentGWC::CheckContactDepth(const double &dContactDepth) const
{
  if(!IsFirstGWC())
  {
    // must be smaller than or equal to previous
    const CComponentGWC &prev = PreviousGWC();
    if(dContactDepth > prev.ContactDepth().Value()) return false;
  }

  if(!IsLastGWC())
  {
    // must be larger than or equal to next
    const CComponentGWC &next = NextGWC();
    if(dContactDepth < next.ContactDepth().Value()) return false;
  }

  const CModelBase& model = dynamic_cast<const CModelBase &> (Model());
  if(model.Mesh().IsMesh())
  {
    // must be inside formation
    const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
    const CFormationBase &form = *pres.Formation();

    return (dContactDepth >= model.Depth(form.Min()) && dContactDepth <= model.Depth(form.Max()));
  }

  return true;
}

bool CComponentGWC::CheckTransitionDepth(const double &dTransitionDepth) const
{
  // must be larger than or equal to contact depth
  // must be within formation

  const CModelBase& model = dynamic_cast<const CModelBase &> (Model());
  if(model.Mesh().IsMesh())
  {
    // must be inside formation
    const CPressure &pres = dynamic_cast<const CPressure &> (PressureBase());
    const CFormationBase &form = *pres.Formation();

    return (dTransitionDepth >= ContactDepth().Value() && dTransitionDepth < model.Depth(form.Max()));
  }

  return dTransitionDepth >= ContactDepth().Value();

}

void CComponentGWC::Set(const double &dContactPressure,
            const double &dTransitionPressure,
            const double &dContactDepth,
            const double &dTransitionDepth,
            const double &dUpperGradient,
            const double &dLowerGradient,
            bool bTransitionFromPrevious,
            const CQuantity::UNIT unit)
{
  m_contact_pressure.Value(dContactPressure, unit);
  m_trans_pressure.Value(dTransitionPressure, unit);
  m_contact_depth.Value(dContactDepth, unit);
  m_trans_depth.Value(dTransitionDepth, unit);
  m_pUpperGradient->Value(dUpperGradient, unit);
  m_pLowerGradient->Value(dLowerGradient, unit);
  m_bTransitionFromPrevious = bTransitionFromPrevious;
}

IValueDomainScalar::TValueVec CComponentGWC::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet = CCommonPressureComponent<TPressure>::ValueElement(elm, unit, cb);
  for(int i = 0;i < vcRet.size(); i++)
  {
    if(!vcRet[i].Valid())
      vcRet[i] = geo::CValue(GWCValue(elm.Point(i), unit));
  }

  return vcRet;
}

double CComponentGWC::GWCValue(const geo::IPoint& pt, const CQuantity::UNIT unit) const
{
  const CModelBase& model = dynamic_cast<const CModelBase &> (Model());
  CLengthQuantity Z;
  Z.Value(model.Depth(pt), CLengthQuantity::SI_UNIT);

  // above the contact
  if(model.Depth(pt) < ContactDepth().Value())
    return (Z.Value(unit) - ContactDepth().Value(unit)) * UpperGradient().Value(unit) + ContactPressure().Value(unit);

  // below the transition zone
  if(model.Depth(pt) > TransitionDepth().Value())
    return TransitionPressure().Value(unit) + (Z.Value(unit) - TransitionDepth().Value(unit)) * LowerGradient().Value(unit);

  if(fabs(TransitionDepth().Value(unit) - ContactDepth().Value(unit)) < EPS) return ContactPressure().Value(unit);

  // inside the transition zone
  double grad = (TransitionPressure().Value(unit) - ContactPressure().Value(unit)) / (TransitionDepth().Value(unit) - ContactDepth().Value(unit));
  return (Z.Value(unit) - ContactDepth().Value(unit)) * grad + ContactPressure().Value(unit);
}

geo::CValue CComponentGWC::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue value = CCommonPressureComponent<TPressure>::ValuePoint(pt, unit, cb);
  if(value.Valid())
    return value;

  return GWCValue(pt, unit);
}

bool CComponentGWC::operator==(const CComponentGWC &rhs) const
{
  if(!CCommonPressureComponent<TPressure>::operator ==(rhs))
    return false;

  return ((m_contact_depth.Value() == rhs.m_contact_depth.Value()) &&
      (m_contact_pressure.Value() == rhs.m_contact_pressure.Value()) &&
      (m_pLowerGradient->Value() == rhs.m_pLowerGradient->Value()) &&
      (m_pUpperGradient->Value() == rhs.m_pUpperGradient->Value()) &&
      (m_trans_depth.Value() == rhs.m_trans_depth.Value()) &&
      (m_trans_pressure.Value() == rhs.m_trans_pressure.Value()) &&
      (m_bTransitionFromPrevious == rhs.m_bTransitionFromPrevious));
}

CComponentGWC& CComponentGWC::operator=(const CComponentGWC& rhs)
{
  if(!((*this) == rhs))
  {
    CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
    assert(pModel);
  pModel->ResultRegister().ClearLinear(false);
  pModel->ResultRegister().ClearNonLinear(false);
  pModel->ResultRegister().ClearMixture();
  }

  CCommonPressureComponent<TPressure>::operator =(rhs);

  m_contact_depth = rhs.m_contact_depth;
  m_contact_pressure = rhs.m_contact_pressure;
  m_pLowerGradient->operator=(*rhs.m_pLowerGradient);
  m_pUpperGradient->operator=(*rhs.m_pUpperGradient);
  m_trans_depth = rhs.m_trans_depth;
  m_trans_pressure = rhs.m_trans_pressure;
  m_bTransitionFromPrevious = rhs.m_bTransitionFromPrevious;

  return *this;
}

void CComponentGWC::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
{
  double dVal;
  stream >> dVal;
  m_contact_pressure.Value(dVal,CQuantity::SI_UNIT);
  stream >> dVal;
  m_contact_depth.Value(dVal,CQuantity::SI_UNIT);
  stream >> dVal;
  m_pUpperGradient->Value(dVal,CQuantity::SI_UNIT);
  stream >> dVal;
  m_pLowerGradient->Value(dVal,CQuantity::SI_UNIT);

  if(CStreamVersion(3, 0, 47) < version)
  {
    stream >> dVal;
    m_trans_pressure.Value(dVal, CQuantity::SI_UNIT);
    stream >> dVal;
    m_trans_depth.Value(dVal, CQuantity::SI_UNIT);
    int iVal;
    stream >> iVal;
    m_bTransitionFromPrevious = (iVal != 0);
  }

  CCommonPressureComponent<TPressure>::LoadStream(stream,version,progress);
}

void CComponentGWC::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << m_contact_pressure.Value();
  stream << m_contact_depth.Value();
  stream << m_pUpperGradient->Value();
  stream << m_pLowerGradient->Value();
  stream << m_trans_pressure.Value();
  stream << m_trans_depth.Value();
  stream << (m_bTransitionFromPrevious ? 1 : 0);
  CCommonPressureComponent<TPressure>::SaveStream(stream,progress);
}

///////// CPressure

CPressure::CPressure(CFormationBase &f, CDepletionStage &d, bool bEmpty)			// Undefined initialisation
: TPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
{
  init(f, d, bEmpty);
}

CPressure::CPressure(CFormationBase &f, 
      CDepletionStage &d,
      const double &dReferencePressure,
      const double &dReferenceDepth,
      const double &dGradient,
      const CQuantity::UNIT unit) // Constant initialisation
: TPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
{
  init(f, d);

  Constant().Set(dReferencePressure,
           dReferenceDepth,
           dGradient,
           unit);

  assert(Type() == CONSTANT_MODE);
}

CPressure::CPressure(CFormationBase &f, 
      CDepletionStage &d,
      const double &dContactPressure,
      const double &dTransitionPressure,
      const double &dContactDepth,
      const double &dTransitionDepth,
      const double &dUpperGradient,
      const double &dLowerGradient,
      bool bTransitionFromPrevious,
      const CQuantity::UNIT unit)
: TPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
{
  init(f, d);

  GWC().Set(dContactPressure,
        dTransitionPressure,
        dContactDepth,
        dTransitionDepth,
        dUpperGradient,
        dLowerGradient,
        bTransitionFromPrevious,
        unit);

  assert(Type() == GWC_MODE);
}

CPressure::CPressure(CFormationBase &f, 
      CDepletionStage &d,
      TPressure &/*p*/)
: TPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
{
  init(f, d);
}

CPressure::CPressure(const CPressure& rhs)
: TPressureBase(rhs)
, m_GlobalPressureObserver(rhs.m_GlobalPressureObserver)
{
  new CComponentGWC(rhs.GWC());					// Copy gwc component
}

CPressure& CPressure::operator=(const CPressure& rhs)
{
  TPressureBase::operator=(rhs);

  CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();

  return *this;
}

unsigned int CPressure::TypeId() const
{
  return IDT_VALUETYPE_PRESSURE;
}

QString CPressure::TypeName() const
{
  return getStringTableEntry(IDS_VALUENAME_PRESSURE);
}

void CPressure::OnNeighbourDeleted(const CGraphNode& node)
{
  if(dynamic_cast<const TPressure*>(&node))
  {
  CModelBase* pModel = dynamic_cast<CModelBase*> (&Model());
  if(!pModel->getDeleting())
  {
    pModel->ResultRegister().ClearLinear(false);
    pModel->ResultRegister().ClearNonLinear(false);
      pModel->ResultRegister().ClearMixture();
  }
  }

  TPressureBase::OnNeighbourDeleted(node);
}

void CPressure::OnNewNeighbour(const CGraphNode& node)
{
  if(dynamic_cast<const TPressure*>(&node))
  {
  CModelBase* pModel = static_cast<CModelBase*> (&Model());
  pModel->ResultRegister().ClearLinear(false);
  pModel->ResultRegister().ClearNonLinear(false);
  pModel->ResultRegister().ClearMixture();
  }

  TPressureBase::OnNewNeighbour(node);
}

bool CPressure::IsFirstValidDepletionStage() const
{
  return (DepletionStage().Initial() != false);
}

void CPressure::LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  new CComponentRepeater(*this);			// Create repeater component

  switch(Type())
  {
  case REPEAT_MODE:
    new CComponentConstant <TPressure>(*this);
    new CComponentGWC(*this);
    break;
  case CONSTANT_MODE:
    {
    CComponentConstant <TPressure>* pConstant = new CComponentConstant <TPressure>(*this, true);
    pConstant->LoadStream(stream, version, progress);
    new CComponentGWC(*this);
    }
    break;
  case GWC_MODE:
    {
    new CComponentConstant <TPressure>(*this);
    CComponentGWC* pGWC = new CComponentGWC(*this, true);	
    pGWC->LoadStream(stream, version, progress);
    }
    break;
  default:
    assert(false);
    break;
  };
}

void CPressure::SaveComponents(TSTREAM& stream, TPROGRESS& progress)
{
  switch(Type())
  {
  case CONSTANT_MODE:
    Constant().SaveStream(stream, progress);
    break;
  case GWC_MODE:
    GWC().SaveStream(stream, progress);
    break;
  case REPEAT_MODE:
    break;
  }
}

TPressureBase& CPressure::FormationStageLoad(CFormationBase& formation, CDepletionStage& stage)
{
  return formation.Pressure(stage);
}

// private

void CPressure::init(CFormationBase &f, CDepletionStage &d, bool bEmpty)
{
  LinkTo(d);	// Link to depletion stage
  reParent(&f);	// Link to formation

  if(!bEmpty)
  {
    new CComponentRepeater(*this);		// Create repeater component
    new CComponentConstant <TPressure>(*this);		// Create constant component
    new CComponentGWC(*this);		// Create gwc component
  }
}

const CPressure *CPressure::ParentPressure() const
{
  const CFormationBase *pFormation = Formation();
  const CPressure *pParentPressure = 0;

  if (pFormation && pFormation->ParentFormation())
  pParentPressure = &pFormation->ParentFormation()->Pressure(static_cast<const CModelBase&>(pFormation->Model()).ParentEquivalentDepletionStage(DepletionStage()));

  return pParentPressure;
}


