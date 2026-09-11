#include "ResultInfo.h"
#include "resourceIDS.h"
#include "IVectorResult.h"
#include "ITensorGroup.h"

CResultInfo::CResultInfo()
{
  Clear();
}

void CResultInfo::Clear()
{
  m_result= 0;
  m_timeStep= -1;
  m_component= 0;
  m_name= "undetermined";
}

void CResultInfo::Result(const IValueComposite *result)
{
  m_result= result;
}

const IValueComposite *CResultInfo::Result() const
{
  return m_result;
}

void CResultInfo::TimeStep(int ii)
{
  m_timeStep= ii;
}

int CResultInfo::TimeStep() const
{
  return m_timeStep;
}

void CResultInfo::Component(const std::string &cc)
{
  if ( cc.empty() )
    m_component= 0;
  else if ( getStringTableEntry(IDS_ET_VECTOR_X) == cc )
    m_component= IVectorResult::VC_X;
  else if ( getStringTableEntry(IDS_ET_VECTOR_Y) == cc )
    m_component= IVectorResult::VC_Y;
  else if ( getStringTableEntry(IDS_ET_VECTOR_Z) == cc )
    m_component= IVectorResult::VC_Z;
  else if ( getStringTableEntry(IDS_ET_VECTOR_LENGTH) == cc )
    m_component= IVectorResult::VC_LENGTH;
  else if ( getStringTableEntry(IDS_ET_VECTOR_FULL) == cc )
    m_component= IVectorResult::VC_FULLVECTOR;

  else if ( getStringTableEntry(IDS_ET_TENSOR_XX) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_XX;
  else if ( getStringTableEntry(IDS_ET_TENSOR_YY) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_YY;
  else if ( getStringTableEntry(IDS_ET_TENSOR_ZZ) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_ZZ;
  else if ( getStringTableEntry(IDS_ET_TENSOR_XY) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_XY;
  else if ( getStringTableEntry(IDS_ET_TENSOR_YZ) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_YZ;
  else if ( getStringTableEntry(IDS_ET_TENSOR_XZ) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_ZX;
  else if ( getStringTableEntry(IDS_ET_TENSOR_FT) == cc )
    m_component=  ITensorGroup::CComponentComposite::TC_FT;
}	
  
void CResultInfo::Component(unsigned int ii)
{
  m_component= ii;
}

unsigned int CResultInfo::Component() const
{
  return m_component;
}

void CResultInfo::Name(const std::string &nm)
{
  m_name=nm;
}

const std::string CResultInfo::Name() const
{
  return m_name;
}
