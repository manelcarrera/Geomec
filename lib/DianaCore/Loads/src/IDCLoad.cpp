// ILoad.cpp: implementation of the ILoad class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"

#include "DCLoadCase.h"
#include "DCLoadManager.h"

#include "IDCDianaRunner.h"

#include "lbcx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

ILoad::ILoad(ICase& icase, double size)
:	m_Manager(icase.Manager()), m_nValueSize( 1 )
{
  m_pValue = (ftn_double_t*) DIMALLOC( sizeof( ftn_double_t ) );
  m_pValue[0] = size;
  m_Manager.InsertLoad(*this);
}

ILoad::ILoad(ICase& icase, const std::vector<double>& vcValue)
:	m_Manager(icase.Manager()), m_nValueSize( vcValue.size() )
{
  m_pValue = (ftn_double_t*) DIMALLOC( vcValue.size() * sizeof( ftn_double_t ) );
  for(int i = 0; i < m_nValueSize; i++)
    m_pValue[i] = vcValue[i];
  m_Manager.InsertLoad(*this);
}

ILoad::~ILoad()
{
  DIFREE( m_pValue );
}

ftn_int_t ILoad::ValueSize() const
{
  return m_nValueSize;
}

ftn_double_t* ILoad::Values() const
{
  return m_pValue;
}

CLoadManager &ILoad::Manager() const
{
  return m_Manager;
}

void ILoad::WriteGroupName(const geo::IElementSet &/*group*/, int /*nIndex*/) const
{
}

void ILoad::WriteGroupName(const geo::CMeshNodeGroup &/*group*/) const
{
}

} // namespace dia
