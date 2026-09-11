#include "RpnUnitConverter.h"
#include "rpnstack.h"

namespace rpn {

CRpnUnitConverter::CRpnUnitConverter()
: m_conversion(UNKNOWN),
  m_pOperand(0)
{
}

CRpnUnitConverter::CRpnUnitConverter(CRpnStack &stack, TConversion conversion)
: m_conversion(conversion)
{
  assert(stack.StackSize() > 0);
  m_pOperand = &stack.Top();
  stack.Pop();
  stack.Push(*this);
}

CRpnUnitConverter::CRpnUnitConverter(const CRpnUnitConverter& rhs)
: m_conversion(rhs.m_conversion),
  m_pOperand(rhs.m_pOperand)
{
}

CRpnObject* CRpnUnitConverter::Clone(CRpnStack& NewStack) const
{
  CRpnUnitConverter* pClone = new CRpnUnitConverter(*this);
  pClone->m_pOperand = m_pOperand->Clone(NewStack);
  return pClone;
}

CRpnUnitConverter::TValue CRpnUnitConverter::Value(const geo::IPoint &pt, UNIT unit) const
{
  return Operation(m_pOperand->Value(pt, unit));
}

CRpnUnitConverter::TValueVec CRpnUnitConverter::Value(const geo::IElement &el, UNIT unit) const
{
  // Fetch operand and size the return vector
  TValueVec A = m_pOperand->Value(el, unit);
  TValueVec ret(el.NrOfPoints());
  assert(A.size() == ret.size());

  // Do formula for each points
  for(size_t i = 0; i < ret.size(); i++)
  {
    ret[i] = Operation(A[i]);
  }

  return ret;
}

QString CRpnUnitConverter::Formula() const
{
  QString sRet;
  switch(m_conversion)
  {
  case FT_M:
  sRet = "ft_m";
  break;
  case M_FT:
  sRet = "m_ft";
  break;
  case PPG_KGPM3:
  sRet = "ppg_kgm3";
  break;
  case KGPM3_PPG:
  sRet = "kgm3_ppg";
  break;
  case C_K:
  sRet = "C_K";
  break;
  case K_C:
  sRet = "K_C";
  break;
  case F_C:
  sRet = "F_C";
  break;
  case C_F:
  sRet = "C_F";
  break;
  case PSI_MPA:
  sRet = "psi_MPa";
  break;
  case MPA_PSI:
  sRet = "MPa_psi";
  break;
  default:
  assert(false);
  sRet = "unknown";
  }

  sRet += "(" + m_pOperand->Formula() + ")";

  return sRet;
}

void CRpnUnitConverter::Clear(CRpnStack &stack)
{
  assert(&stack.Top() == this);
  stack.Pop();
  stack.Push(*m_pOperand);
  m_pOperand = 0;
  delete this;
}

void CRpnUnitConverter::SaveStream(std::stringstream& stream)
{
  CRpnObject::SaveStream(stream);

  m_pOperand->SaveStream(stream);

  int nConversion = int(m_conversion);
  stream << nConversion << " ";
}

void CRpnUnitConverter::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
  CRpnObject::LoadStream(stream, version, stack);

  m_pOperand = LoadRpnObject(stream, version, stack);

  int nConversion;
  stream >> nConversion;
  m_conversion = (TConversion)nConversion;
}

CRpnUnitConverter::eObjectType CRpnUnitConverter::ObjectType() const
{
  return OT_UNITCONVERTER;
}

bool CRpnUnitConverter::Recursive(TParentSet stParent) const
{
  if(!stParent.insert(this).second)
    return true;
  return m_pOperand->Recursive(stParent);
}

bool CRpnUnitConverter::Defined() const
{
  return true;
}

bool CRpnUnitConverter::exists() const
{
  assert(m_pOperand != 0);

  return (m_pOperand != 0);
}

void CRpnUnitConverter::CollectUsedObjects(TObjectSet& stObjects) const
{
  stObjects.insert(m_pOperand);
  m_pOperand->CollectUsedObjects(stObjects);
}

std::vector<std::pair<unsigned int, QString> > CRpnUnitConverter::Collect()
{
  std::vector<std::pair<unsigned int, QString> > vcRet;
  vcRet.push_back(std::make_pair(FT_M, "ft -> m"));
  vcRet.push_back(std::make_pair(M_FT, "m -> ft"));
  vcRet.push_back(std::make_pair(PPG_KGPM3, "ppg -> kg/m3"));
  vcRet.push_back(std::make_pair(KGPM3_PPG, "kg/m3 -> ppg"));
  vcRet.push_back(std::make_pair(C_K, "C -> K"));
  vcRet.push_back(std::make_pair(K_C, "K -> C"));
  vcRet.push_back(std::make_pair(F_C, "F -> C"));
  vcRet.push_back(std::make_pair(C_F, "C -> F"));
  vcRet.push_back(std::make_pair(PSI_MPA, "psi -> MPa"));
  vcRet.push_back(std::make_pair(MPA_PSI, "MPa -> psi"));

  return vcRet;
}

CRpnUnitConverter::TValue CRpnUnitConverter::Operation(const TValue &value) const
{
  if(value.Valid())
  {
  double v = value.Value();

  switch(m_conversion)
  {
  case FT_M:
      v /= 3.2808399;
      break;
  case M_FT:
      v *= 3.2808399;
      break;
  case PPG_KGPM3:
      v /= 0.008345405;
      break;
  case KGPM3_PPG:
      v *= 0.008345405;
      break;
  case C_K:
      v += 273.15;
      break;
  case K_C:
      v -= 273.15;
      break;
  case F_C:
      v = (v - 32) * 5 / 9;
      break;
  case C_F:
      v = v * 9 / 5 + 32;
      break;
  case PSI_MPA:
      v /= 145.038;
      break;
  case MPA_PSI:
      v *= 145.038;
      break;
  default:
      assert(false);
  }

  return TValue(v);
  }

  return TValue();
}

CRpnObject* CRpnUnitConverter::GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  CRpnUnitConverter* pCopy = new CRpnUnitConverter();
  pCopy->m_conversion = m_conversion;
  pCopy->m_pOperand = m_pOperand->GetExpandedRpnObject(targetstack, lstMessages);

  return pCopy;
}

} // namespace rpn
