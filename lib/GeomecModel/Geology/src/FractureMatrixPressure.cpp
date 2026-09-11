// FractureMatrixPressure.cpp: implementation of the CFractureMatrixPressure class.
//
//////////////////////////////////////////////////////////////////////
#include "FractureMatrixPressure.h"
#include "FormationBase.h"
#include "GlobalPressure.h"
#include "MeshBase.h"
#include "ValueTypes.h"
#include "DepletionStage.h"
#include "resourceIDI.h"
#include "Pressure.h"

///////////// CFractureMatrixComponentRepeater

CFractureMatrixComponentRepeater::CFractureMatrixComponentRepeater(const CPressure* pressure,
  CFractureMatrixPressure &fractureMatrixPressure)
: TFractureMatrixComponentRepeaterBase(fractureMatrixPressure)
, m_Pressure(pressure)
, m_FractureMatrixPressure(fractureMatrixPressure)
{
}

CFractureMatrixComponentRepeater::CFractureMatrixComponentRepeater(const CFractureMatrixComponentRepeater& rhs)
: TFractureMatrixComponentRepeaterBase(rhs)
, m_Pressure(rhs.m_Pressure)
, m_FractureMatrixPressure(rhs.m_FractureMatrixPressure)
{
}

unsigned int CFractureMatrixComponentRepeater::TypeId() const
{
  const CFractureMatrixPressure *pPressure = dynamic_cast<const CFractureMatrixPressure*> (&FormationLoadBase());
  assert(pPressure);
  switch(pPressure->Type())
  {
  case CFractureMatrixPressure::CONSTANT_MODE:
    return pPressure->Constant().TypeId();
  case CFractureMatrixPressure::REPEAT_MODE:
    return IDT_COMPONENT_PRESSURE_REPEAT;
  case CFractureMatrixPressure::GWC_MODE:
    return pPressure->GWC().TypeId();
  default:
    assert(false); // Bogus
  };

  return 0;
}

unsigned int CFractureMatrixComponentRepeater::IconId() const
{
  const CFractureMatrixPressure *pPressure = dynamic_cast<const CFractureMatrixPressure*> (&FormationLoadBase());
  assert(pPressure);

  switch(pPressure->Type())
  {
  case CFractureMatrixPressure::CONSTANT_MODE:
    return pPressure->Constant().IconId();
  case CFractureMatrixPressure::REPEAT_MODE:
    if(pPressure->DepletionStage().Initial())
      return IDI_COMPONENT_PRESSURE_FRACTURE_PRESSURE_REPEAT;
    else {
      const CDepletionStage *pPrev = &pPressure->DepletionStage().Previous();
      assert(pPrev);
      const CFormationBase *pFormation = dynamic_cast<CFormationBase*>(pPressure->parent());
      assert(pFormation);
      const CFractureMatrixPressure *pPrevPressure = &pFormation->FractureMatrixPressure(*pPrev);
      assert(pPrevPressure);

      // check number of point sets
      if( pPressure->DistributedSize() != 0 && pPressure->DistributedSize() != pPrevPressure->DistributedSize() )
        return IDI_COMPONENT_PRESSURE_REPEAT_CHANGE;
      // check extrapolation setting, only if distributions are attached
      if( pPressure->DistributedSize() != 0 && pPressure->DistributedOnly() != pPrevPressure->DistributedOnly() )
        return IDI_COMPONENT_PRESSURE_REPEAT_CHANGE;
      // check if the list of point sets is equal
      for( size_t i = 0; i < pPressure->DistributedSize(); i++ ) {
    size_t j;
        for( j = 0; j < pPrevPressure->DistributedSize(); j++ )
          if( &pPressure->DistributedValue(i) == &pPrevPressure->DistributedValue(j) )
            break;

        if( j == pPrevPressure->DistributedSize() )
          return IDI_COMPONENT_PRESSURE_REPEAT_CHANGE;
      }
      return IDI_COMPONENT_PRESSURE_FRACTURE_PRESSURE_REPEAT;
    }
  case CFractureMatrixPressure::GWC_MODE:
    return pPressure->GWC().IconId();
  default:
    assert(false);
  };
  
  return 0;
}

QString CFractureMatrixComponentRepeater::TypeName() const
{
  const CFractureMatrixPressure *pPressure = dynamic_cast<const CFractureMatrixPressure*> (&FormationLoadBase());
  assert(pPressure);
  switch(pPressure->Type())
  {
  case CFractureMatrixPressure::CONSTANT_MODE:
    return pPressure->Constant().TypeName();
  case CFractureMatrixPressure::REPEAT_MODE:
    {
      return getStringTableEntry(IDS_COMPONENT_PRESSURE_REPEAT);
    }
  case CFractureMatrixPressure::GWC_MODE:
    return pPressure->GWC().TypeName();
  default:
    assert(false); // Bogus
  };

  return QString();
}

QString CFractureMatrixComponentRepeater::UnitName(const CQuantity::UNIT unit) const
{
  QString sRet;
  if(unit == CQuantity::SI_UNIT)
    sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
  else
    sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

  return sRet;
}

QString CFractureMatrixComponentRepeater::ExportLabel() const
{
   return getStringTableEntry(IDS_ET_FRACTURE_MATRIX_PRESSURE);
}

const CFractureMatrixComponentRepeater::TFormationLoad& CFractureMatrixComponentRepeater::PreviousFormationLoad() const
{
  const CFractureMatrixPressure *pPressure = dynamic_cast<const CFractureMatrixPressure*> (&FormationLoadBase());
  assert(pPressure);
  assert(!pPressure->DepletionStage().Initial());
  return pPressure->Formation()->FractureMatrixPressure(pPressure->DepletionStage().Previous());
}

const IValueComponentBase& CFractureMatrixComponentRepeater::GlobalValues() const
{
  const CModelBase& model = dynamic_cast<const CModelBase&> (Model());
  return model.GlobalPressure().Component();
}

geo::CValue CFractureMatrixComponentRepeater::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  if ((m_FractureMatrixPressure.Type() == CFractureMatrixPressure::REPEAT_MODE))
  {
  // skip baseclass version, we only want distributed values
  geo::CValue val = CFormationLoadComponent<TFractureMatrixPressure>::ValuePoint(pt, unit, cb);
  if(!val.Valid())
      val = m_Pressure->Component().ScalarData().ValuePoint(pt, unit, cb);

  return val;
  }

  return TFractureMatrixComponentRepeaterBase::ValuePoint(pt, unit, cb);
}

IValueDomainScalar::TValueVec CFractureMatrixComponentRepeater::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  if ((m_FractureMatrixPressure.Type() == CFractureMatrixPressure::REPEAT_MODE))
  {
  // skip baseclass version, we only want distributed values
  IValueDomainScalar::TValueVec vcValuesDist = CFormationLoadComponent<TFractureMatrixPressure>::ValueElement(elm, unit, cb);
  IValueDomainScalar::TValueVec vcValuesP = m_Pressure->Component().ScalarData().ValueElement(elm, unit, cb);
  assert(vcValuesDist.size() == vcValuesP.size());
  for(size_t i = 0; i < vcValuesDist.size(); ++i)
  {
      if(!vcValuesDist[i].Valid())
    vcValuesDist[i] = vcValuesP[i];
  }

  return vcValuesDist;
  }

  return TFractureMatrixComponentRepeaterBase::ValueElement(elm, unit, cb);
}

////////// CFractureMatrixComponentGWC

CFractureMatrixComponentGWC::CGWCPressure::CGWCPressure(CFractureMatrixPressure &current, const double &value, const UNIT unit)
: CSinglePressure(value, unit), m_current(current)
{
}

CFractureMatrixComponentGWC::CGWCPressure::CGWCPressure(const CGWCPressure &rhs)
: CSinglePressure(rhs), m_current(rhs.m_current)
{
}

//##ModelId=3BE7AEB90005
double CFractureMatrixComponentGWC::CGWCPressure::MaxValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  // Initialise previous pressure ....
  const CFractureMatrixPressure *pPrev = 0;
  if(!m_current.DepletionStage().Initial())
    pPrev = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Previous());

  while(pPrev)
  {
    switch(pPrev->Mode())
    {
    case CFractureMatrixPressure::PREVIOUS:
      if(!pPrev->DepletionStage().Initial())
        pPrev = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Previous());
      else
        pPrev = 0;		// Exit loop ....
      break;
    case CFractureMatrixPressure::GWC:
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
double CFractureMatrixComponentGWC::CGWCPressure::MinValue(const int /*nComponent*/, const UNIT unit) const
{
/*
    // Initialise previous pressure ....
  const CFractureMatrixPressure *pNext = 0;
  if(!m_current.DepletionStage().Last())
    pNext = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Next());

  while(pNext)
  {
    switch(pNext->Type())
    {
    case CFractureMatrixPressure::PREVIOUS:
      if(!pNext->DepletionStage().Last())
        pNext = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Next());
      else
        pNext = 0;		// Exit loop ....
      break;
    case CFractureMatrixPressure::GWC:
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

CFractureMatrixComponentGWC::CGWCPressure& CFractureMatrixComponentGWC::CGWCPressure::operator=(const CSinglePressure &rhs)
{
  CSinglePressure::operator=(rhs);
  return *this;
}

CFractureMatrixComponentGWC::CGWCPressure& CFractureMatrixComponentGWC::CGWCPressure::operator = (const CGWCPressure& rhs)
{
  if (this != &rhs)
  {
  CSinglePressure::operator = (rhs);

  m_current = rhs.m_current;
  }

  return *this;
}

bool CFractureMatrixComponentGWC::CGWCPressure::operator==(const CGWCPressure &rhs) const
{
  return CSinglePressure::operator==(rhs);
}

//##ModelId=3BE7AEB90091
CFractureMatrixComponentGWC::CGWCDepth::CGWCDepth(CFractureMatrixPressure &current, const double &value, const UNIT unit)
: CLengthQuantity(value, unit), m_current(current)
{
}

CFractureMatrixComponentGWC::CGWCDepth::CGWCDepth(const CGWCDepth &rhs)
: CLengthQuantity(rhs), m_current(rhs.m_current)
{
}

//##ModelId=3BE7AEB900A1
double CFractureMatrixComponentGWC::CGWCDepth::MaxValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  const CFractureMatrixPressure *pNext = 0;
  if(!m_current.DepletionStage().Last())
    pNext = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Next());

  while(pNext)
  {
    switch(pNext->Type())
    {
    case CFractureMatrixPressure::PREVIOUS:
      if(!pNext->DepletionStage().Last())
        pNext = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Next());
      else
        pNext = 0;		// Exit loop ....
      break;
    case CFractureMatrixPressure::GWC:
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
double CFractureMatrixComponentGWC::CGWCDepth::MinValue(const int /*nComponent*/, const UNIT unit) const
{
/*
  const CFractureMatrixPressure *pPrev = 0;
  if(!m_current.DepletionStage().Initial())
    pPrev = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Previous());

  while(pPrev)
  {
    switch(pPrev->Type())
    {
    case CFractureMatrixPressure::PREVIOUS:
      if(!pPrev->DepletionStage().Initial())
        pPrev = &m_current.Formation()->FractureMatrixPressure(m_current.DepletionStage().Previous());
      else
        pPrev = 0;		// Exit loop ....
      break;
    case CFractureMatrixPressure::GWC:
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

CFractureMatrixComponentGWC::CGWCDepth& CFractureMatrixComponentGWC::CGWCDepth::operator=(const CLengthQuantity &rhs)
{
  CLengthQuantity::operator =(rhs);
  return *this;
}

CFractureMatrixComponentGWC::CGWCDepth& CFractureMatrixComponentGWC::CGWCDepth::operator = (const CGWCDepth& rhs)
{
  if (this != &rhs)
  {
  CLengthQuantity::operator = (rhs);

  m_current = rhs.m_current;
  }

  return *this;
}

bool CFractureMatrixComponentGWC::CGWCDepth::operator==(const CGWCDepth &rhs) const
{
  return CLengthQuantity::operator ==(rhs);
}


CFractureMatrixComponentGWC::CFractureMatrixComponentGWC(CFractureMatrixPressure &pressure, bool bEmpty)
: CCommonPressureComponent <TFractureMatrixPressure>(pressure, CFractureMatrixPressure::GWC_MODE, bEmpty),
  m_contact_pressure(pressure, 0),
  m_trans_pressure(pressure, 0),
  m_contact_depth(pressure, 0),
  m_trans_depth(pressure, 0),
  m_bTransitionFromPrevious(true)
{
  const CFractureMatrixPressure *pPressure = dynamic_cast<const CFractureMatrixPressure*> (&PressureBase());
  assert(pPressure);

  m_pUpperGradient = new CPressureGradientQuantity(0);

  if(IsFirstGWC())
    m_pLowerGradient = new CPressureGradientQuantity(0);
  else
    m_pLowerGradient = FirstGWC().m_pLowerGradient;

  if(!IsFirstGWC())
  {
    const CFractureMatrixComponentGWC &prev = PreviousGWC();
    *m_pUpperGradient = prev.UpperGradient();
    m_contact_pressure = prev.ContactPressure();
    m_trans_pressure = prev.TransitionPressure();
    m_contact_depth = prev.ContactDepth();
    m_trans_depth = prev.TransitionDepth();
  }
}

CFractureMatrixComponentGWC::CFractureMatrixComponentGWC(const CFractureMatrixComponentGWC &rhs)
: CCommonPressureComponent <TFractureMatrixPressure>(rhs), 
  m_contact_pressure(rhs.m_contact_pressure),
  m_trans_pressure(rhs.m_trans_pressure),
  m_contact_depth(rhs.m_contact_depth),
  m_trans_depth(rhs.m_trans_depth),
  m_bTransitionFromPrevious(rhs.m_bTransitionFromPrevious)
{
  m_pUpperGradient = new CPressureGradientQuantity(rhs.UpperGradient());
  m_pLowerGradient = new CPressureGradientQuantity(rhs.LowerGradient());
}

CFractureMatrixComponentGWC::~CFractureMatrixComponentGWC()
{
  const CFractureMatrixPressure *pPressure = (const CFractureMatrixPressure*)(&Parent());
  assert(pPressure);

  if(pPressure->AutoDelete() || IsCopy())
    delete m_pLowerGradient;

  delete m_pUpperGradient;
}

const CFractureMatrixComponentGWC &CFractureMatrixComponentGWC::FirstGWC() const
{
  const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
  const CDepletionStage *pStageThis = &pres.DepletionStage();
  const CDepletionStage *pStage = pStageThis;
  pStage = &pStage->InitialStage();
  const CFractureMatrixComponentGWC *pFirstGWC = 0;

  do
  {
    if(pres.Formation()->FractureMatrixPressure(*pStage).Type() == CFractureMatrixPressure::GWC_MODE)
    {
      pFirstGWC = &pres.Formation()->FractureMatrixPressure(*pStage).GWC();
      break;
    }

    if(pStage->Last() || pStage == pStageThis) pStage = 0;
    else pStage = &pStage->Next();
  } while(pStage);

  if(!pFirstGWC) pFirstGWC = &pres.Formation()->FractureMatrixPressure(pStageThis->InitialStage()).GWC();

  return *pFirstGWC;
}

const CFractureMatrixComponentGWC &CFractureMatrixComponentGWC::PreviousGWC() const
{
  // find the previous GWC, not necessarily the first depletion stage
  const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
  const CDepletionStage *pStageThis = &pres.DepletionStage();
  const CDepletionStage *pStage = pStageThis;
  const CFractureMatrixComponentGWC *pPrevGWC = 0;

  if(!pStage->Initial())
  {
    pStage = &pStage->Previous();
    do
    {
      if(pres.Formation()->FractureMatrixPressure(*pStage).Type() == CFractureMatrixPressure::GWC_MODE)
      {
        pPrevGWC = &pres.Formation()->FractureMatrixPressure(*pStage).GWC();
        break;
      }

      if(pStage->Initial()) pStage = 0;
      else pStage = &pStage->Previous();
    } while(pStage);
  }

  if(!pPrevGWC) pPrevGWC = &pres.Formation()->FractureMatrixPressure(pStageThis->InitialStage()).GWC();

  return *pPrevGWC;
}

const CFractureMatrixComponentGWC &CFractureMatrixComponentGWC::NextGWC() const
{
  // find the next GWC

  // there is no next one if this is the last
  assert(!IsLastGWC());

  const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
  const CDepletionStage *pStage = &pres.DepletionStage();
  const CFractureMatrixComponentGWC *pNextGWC = 0;

  if(!pStage->Last())
  {
    pStage = &pStage->Next();
    do
    {
      if(pres.Formation()->FractureMatrixPressure(*pStage).Type() == CFractureMatrixPressure::GWC_MODE)
      {
        pNextGWC = &pres.Formation()->FractureMatrixPressure(*pStage).GWC();
        break;
      }
      if(pStage->Last()) pStage = 0;
      else pStage = &pStage->Next();
    } while(pStage);
  }

  assert(pNextGWC != 0);

  return *pNextGWC;
}

QString CFractureMatrixComponentGWC::TypeName() const
{
  return getStringTableEntry(IDS_COMPONENT_PRESSURE_GWC);
}

const CSinglePressure& CFractureMatrixComponentGWC::ContactPressure() const
{
  return m_contact_pressure;	
}

bool CFractureMatrixComponentGWC::IsFirstGWC() const
{
  const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
  const CFractureMatrixPressure &presfirst = dynamic_cast<const CFractureMatrixPressure &> (FirstGWC().PressureBase());

  return (pres.DepletionStage().Index() == presfirst.DepletionStage().Index());
}

bool CFractureMatrixComponentGWC::IsLastGWC() const
{
  const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
  const CDepletionStage *pStage = &pres.DepletionStage();

  while(!pStage->Last())
  {
    pStage = &pStage->Next();
    if(pres.Formation()->FractureMatrixPressure(*pStage).Type() == CFractureMatrixPressure::GWC_MODE) return false; // a later one was found
  }

  return true; // no later GWC found
}

const CSinglePressure &CFractureMatrixComponentGWC::TransitionPressure() const
{
  // for reference of later stages
  if(IsFirstGWC()) return ContactPressure();

  if(TransitionFromPrevious()) return PreviousGWC().TransitionPressure();

  return m_trans_pressure;
}

const CLengthQuantity& CFractureMatrixComponentGWC::ContactDepth() const
{
  return m_contact_depth;
}

const CLengthQuantity &CFractureMatrixComponentGWC::TransitionDepth() const
{
  // for reference of later stages
  if(IsFirstGWC()) return ContactDepth();

  if(TransitionFromPrevious()) return PreviousGWC().TransitionDepth();

  return m_trans_depth;
}

const CPressureGradientQuantity& CFractureMatrixComponentGWC::UpperGradient() const
{
  assert(m_pUpperGradient);
  return *m_pUpperGradient;
}

const CPressureGradientQuantity& CFractureMatrixComponentGWC::LowerGradient() const
{
  assert(m_pLowerGradient);
  return *m_pLowerGradient;
}

bool CFractureMatrixComponentGWC::TransitionFromPrevious() const
{
  return m_bTransitionFromPrevious;
}

void CFractureMatrixComponentGWC::TransitionFromPrevious(bool bNew)
{
  if(!bNew)
  {
    // copy values from previous
    const CFractureMatrixComponentGWC &prev = PreviousGWC();
    m_trans_depth = prev.TransitionDepth();
    m_trans_pressure = prev.TransitionPressure();
  }

  m_bTransitionFromPrevious = bNew;
}

bool CFractureMatrixComponentGWC::CheckContactDepth(const double &dContactDepth) const
{
  if(!IsFirstGWC())
  {
    // must be smaller than or equal to previous
    const CFractureMatrixComponentGWC &prev = PreviousGWC();
    if(dContactDepth > prev.ContactDepth().Value()) return false;
  }

  if(!IsLastGWC())
  {
    // must be larger than or equal to next
    const CFractureMatrixComponentGWC &next = NextGWC();
    if(dContactDepth < next.ContactDepth().Value()) return false;
  }

  const CModelBase& model = dynamic_cast<const CModelBase &> (Model());
  if(model.Mesh().IsMesh())
  {
    // must be inside formation
    const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
    const CFormationBase &form = *pres.Formation();

    return (dContactDepth >= model.Depth(form.Min()) && dContactDepth <= model.Depth(form.Max()));
  }

  return true;
}

bool CFractureMatrixComponentGWC::CheckTransitionDepth(const double &dTransitionDepth) const
{
  // must be larger than or equal to contact depth
  // must be within formation

  const CModelBase& model = dynamic_cast<const CModelBase &> (Model());
  if(model.Mesh().IsMesh())
  {
    // must be inside formation
    const CFractureMatrixPressure &pres = dynamic_cast<const CFractureMatrixPressure &> (PressureBase());
    const CFormationBase &form = *pres.Formation();

    return (dTransitionDepth >= ContactDepth().Value() && dTransitionDepth < model.Depth(form.Max()));
  }

  return dTransitionDepth >= ContactDepth().Value();

}

void CFractureMatrixComponentGWC::Set(const double &dContactPressure,
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

IValueDomainScalar::TValueVec CFractureMatrixComponentGWC::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet = CCommonPressureComponent <TFractureMatrixPressure>::ValueElement(elm, unit, cb);
  for(int i = 0;i < vcRet.size(); i++)
  {
    if(!vcRet[i].Valid())
      vcRet[i] = geo::CValue(GWCValue(elm.Point(i), unit));
  }

  return vcRet;
}

double CFractureMatrixComponentGWC::GWCValue(const geo::IPoint& pt, const CQuantity::UNIT unit) const
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

geo::CValue CFractureMatrixComponentGWC::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue value = CCommonPressureComponent <TFractureMatrixPressure>::ValuePoint(pt, unit, cb);
  if(value.Valid())
    return value;

  return GWCValue(pt, unit);
}

bool CFractureMatrixComponentGWC::operator==(const CFractureMatrixComponentGWC &rhs) const
{
  if(!CCommonPressureComponent <TFractureMatrixPressure>::operator ==(rhs))
    return false;

  return ((m_contact_depth.Value() == rhs.m_contact_depth.Value()) &&
      (m_contact_pressure.Value() == rhs.m_contact_pressure.Value()) &&
      (m_pLowerGradient->Value() == rhs.m_pLowerGradient->Value()) &&
      (m_pUpperGradient->Value() == rhs.m_pUpperGradient->Value()) &&
      (m_trans_depth.Value() == rhs.m_trans_depth.Value()) &&
      (m_trans_pressure.Value() == rhs.m_trans_pressure.Value()) &&
      (m_bTransitionFromPrevious == rhs.m_bTransitionFromPrevious));
}

CFractureMatrixComponentGWC& CFractureMatrixComponentGWC::operator=(const CFractureMatrixComponentGWC& rhs)
{
  if(!((*this) == rhs))
  {
    CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
    assert(pModel);
  pModel->ResultRegister().ClearLinear(false);
  pModel->ResultRegister().ClearNonLinear(false);
  pModel->ResultRegister().ClearMixture();
  }

  CCommonPressureComponent <TFractureMatrixPressure>::operator =(rhs);

  m_contact_depth = rhs.m_contact_depth;
  m_contact_pressure = rhs.m_contact_pressure;
  m_pLowerGradient->operator=(*rhs.m_pLowerGradient);
  m_pUpperGradient->operator=(*rhs.m_pUpperGradient);
  m_trans_depth = rhs.m_trans_depth;
  m_trans_pressure = rhs.m_trans_pressure;
  m_bTransitionFromPrevious = rhs.m_bTransitionFromPrevious;

  return *this;
}

void CFractureMatrixComponentGWC::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
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

  CCommonPressureComponent <TFractureMatrixPressure>::LoadStream(stream,version,progress);
}

void CFractureMatrixComponentGWC::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << m_contact_pressure.Value();
  stream << m_contact_depth.Value();
  stream << m_pUpperGradient->Value();
  stream << m_pLowerGradient->Value();
  stream << m_trans_pressure.Value();
  stream << m_trans_depth.Value();
  stream << (m_bTransitionFromPrevious ? 1 : 0);
  CCommonPressureComponent <TFractureMatrixPressure>::SaveStream(stream,progress);
}

///////// CFractureMatrixPressure

CFractureMatrixPressure::CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, CDepletionStage &d, bool bEmpty)			// Undefined initialisation
: TFractureMatrixPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
, m_Pressure(pressure)
{
  init(f, d, bEmpty);
}

CFractureMatrixPressure::CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
      CDepletionStage &d,
      const double &dReferencePressure,
      const double &dReferenceDepth,
      const double &dGradient,
      const CQuantity::UNIT unit) // Constant initialisation
: TFractureMatrixPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
, m_Pressure(pressure)
{
  init(f, d);

  Constant().Set(dReferencePressure,
           dReferenceDepth,
           dGradient,
           unit);

  assert(Type() == CONSTANT_MODE);
}

CFractureMatrixPressure::CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
      CDepletionStage &d,
      const double &dContactPressure,
      const double &dTransitionPressure,
      const double &dContactDepth,
      const double &dTransitionDepth,
      const double &dUpperGradient,
      const double &dLowerGradient,
      bool bTransitionFromPrevious,
      const CQuantity::UNIT unit)
: TFractureMatrixPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
, m_Pressure(pressure)
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

CFractureMatrixPressure::CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
      CDepletionStage &d,
      TFractureMatrixPressure &/*p*/)
: TFractureMatrixPressureBase(f, d)
, m_GlobalPressureObserver(this,
  &(static_cast <CModelBase&> (f.Model()).GlobalPressure()))
, m_Pressure(pressure)
{
  init(f, d);
}

CFractureMatrixPressure::CFractureMatrixPressure(const CFractureMatrixPressure& rhs)
: TFractureMatrixPressureBase(rhs)
, m_GlobalPressureObserver(rhs.m_GlobalPressureObserver)
, m_Pressure(rhs.m_Pressure)
{
  new CFractureMatrixComponentGWC(rhs.GWC());					// Copy gwc component
}

CFractureMatrixPressure& CFractureMatrixPressure::operator=(const CFractureMatrixPressure& rhs)
{
  TFractureMatrixPressureBase::operator=(rhs);

  CModelBase& model = static_cast<CModelBase&>(Model());
  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();

  return *this;
}

unsigned int CFractureMatrixPressure::TypeId() const
{
  return IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE;
}

QString CFractureMatrixPressure::TypeName() const
{
  return getStringTableEntry(IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE);
}

void CFractureMatrixPressure::OnNeighbourDeleted(const CGraphNode& node)
{
  if(dynamic_cast<const TFractureMatrixPressure*>(&node))
  {
  CModelBase* pModel = dynamic_cast<CModelBase*> (&Model());
  if(!pModel->getDeleting())
  {
    pModel->ResultRegister().ClearLinear(false);
    pModel->ResultRegister().ClearNonLinear(false);
      pModel->ResultRegister().ClearMixture();
  }
  }

  TFractureMatrixPressureBase::OnNeighbourDeleted(node);
}

void CFractureMatrixPressure::OnNewNeighbour(const CGraphNode& node)
{
  if(dynamic_cast<const TFractureMatrixPressure*>(&node))
  {
  CModelBase* pModel = static_cast<CModelBase*> (&Model());
  pModel->ResultRegister().ClearLinear(false);
  pModel->ResultRegister().ClearNonLinear(false);
  pModel->ResultRegister().ClearMixture();
  }

  TFractureMatrixPressureBase::OnNewNeighbour(node);
}

bool CFractureMatrixPressure::IsFirstValidDepletionStage() const
{
  return (DepletionStage().Initial() != false);
}

void CFractureMatrixPressure::LoadStream(TSTREAM& stream,
  CStreamVersion& version, TPROGRESS& progress)
{
  TFractureMatrixPressureBase::LoadStream(stream, version, progress);
  std::set <TPressure*> pressures = Links <TPressure> ();

  if (version < CStreamVersion(4, 1, 24))
  {
  if (!pressures.empty())
  {
      assert(pressures.size() == 1);

      TPressure* pressure = *(pressures.begin());

      std::set <IValueSet*> valueSets =
    pressure->Component().Links <IValueSet> ();

      assert(valueSets.size() == 1);

      IValueSet* valueSet = *(valueSets.begin());
      IValueSet* clonedValueSet = valueSet->clone();
      TFractureMatrixPressure* fractureMatrixPressure =
    new TFractureMatrixPressure(pressure->PointSet(), pressure->Name());

      clonedValueSet->LinkTo(fractureMatrixPressure->Component());

      UnLink(*pressure);
      LinkTo(*fractureMatrixPressure);
  }
  }
  else
  {
  assert(pressures.size() == 0);
  }
}

void CFractureMatrixPressure::LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  new CFractureMatrixComponentRepeater(m_Pressure, *this);			// Create repeater component

  switch(Type())
  {
  case REPEAT_MODE:
    new CComponentConstant <TFractureMatrixPressure>(*this);
    new CFractureMatrixComponentGWC(*this);
    break;
  case CONSTANT_MODE:
    {
    CComponentConstant <TFractureMatrixPressure>* pConstant = new CComponentConstant <TFractureMatrixPressure>(*this, true);
    pConstant->LoadStream(stream, version, progress);
    new CFractureMatrixComponentGWC(*this);
    }
    break;
  case GWC_MODE:
    {
    new CComponentConstant <TFractureMatrixPressure>(*this);
    CFractureMatrixComponentGWC* pGWC = new CFractureMatrixComponentGWC(*this, true);	
    pGWC->LoadStream(stream, version, progress);
    }
    break;
  default:
    assert(false);
    break;
  };
}

void CFractureMatrixPressure::SaveComponents(TSTREAM& stream, TPROGRESS& progress)
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

TFractureMatrixPressureBase& CFractureMatrixPressure::FormationStageLoad(CFormationBase& formation, CDepletionStage& stage)
{
  return formation.FractureMatrixPressure(stage);
}

// private

void CFractureMatrixPressure::init(CFormationBase &f, CDepletionStage &d, bool bEmpty)
{
  LinkTo(d);	// Link to depletion stage
  reParent(&f);	// Link to formation

  if(!bEmpty)
  {
    new CFractureMatrixComponentRepeater(m_Pressure, *this);		// Create repeater component
    new CComponentConstant <TFractureMatrixPressure>(*this);		// Create constant component
    new CFractureMatrixComponentGWC(*this);		// Create gwc component
  }
}
