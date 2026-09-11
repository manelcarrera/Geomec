// NodalValueSet.cpp: implementation of the CNodalValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "NodalValueSet.h"
#include "IPointSet.h"
#include <algorithm>
#include "StreamVersion.h"
#include "resourceIDI.h"
#include "IProgressFactory.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodalValueSet::CNodalValueSet(const QString &sName, CQuantity::UNIT unit, IPointSet& point_set)
: IValueSet(sName, unit, point_set), m_bDirty(true)
{
	// Link to the source point always
	if(!point_set.IsCopy())
		point_set.m_vcNodalValueSet.push_back(this);

	m_data.resize(point_set.PointSize());
}

CNodalValueSet::CNodalValueSet(IPointSet& point_set)
: IValueSet("", CQuantity::SI_UNIT, point_set), m_bDirty(true)
{
	QString sName = QString("Property%1").arg(point_set.NodalValueSetSize());
	Name(sName);

	// Link to the source point always
	if(!point_set.IsCopy())
		point_set.m_vcNodalValueSet.push_back(this);

	m_data.resize(point_set.PointSize());
}

CNodalValueSet::CNodalValueSet(const CNodalValueSet& rhs)
: IValueSet(rhs), 
  m_data(rhs.m_data), 
  m_dMin(rhs.m_dMin),
  m_dMax(rhs.m_dMax),
  m_dSum(rhs.m_dSum),
  m_dStdDev(rhs.m_dStdDev),
  m_bDirty(rhs.m_bDirty)
{
	// Check or we're registered in the m_vcNodalValueSet 
	bool bFound = false;
	for(int i = 0; i < PointSet().m_vcNodalValueSet.size(); i++)
	{
		if(PointSet().m_vcNodalValueSet[i] == &rhs)
			bFound = true;
	}

	if(!bFound)
		PointSet().m_vcNodalValueSet.push_back((CNodalValueSet*) &rhs);
}

CNodalValueSet::CNodalValueSet(IPointSet& point_set, const CNodalValueSet& rhs, TPROGRESS& progress)
: IValueSet(rhs.Name(), CQuantity::SI_UNIT, point_set), m_data(rhs.m_data.size()), m_bDirty(true)
{
	for(int i = 0; i < rhs.m_data.size(); i++)
	{
		m_data[i] = rhs.m_data[i];
		progress.Step();
	}

	// Link to the source point always
	if(!point_set.IsCopy())
		point_set.m_vcNodalValueSet.push_back(this);

}

CNodalValueSet::~CNodalValueSet()
{

}

CNodalValueSet* CNodalValueSet::clone()
{
  /*
     Do not use the copy-constructor.
     The base-class 'CGraphNode' introduces an algorithm that limits the
     number of copies.
  */

  CNodalValueSet* nodalValueSet = new CNodalValueSet(PointSet());

  assert(nodalValueSet->m_data.size() == m_data.size());

  nodalValueSet->m_data = m_data;
  nodalValueSet->m_dMin = m_dMin;
  nodalValueSet->m_dMax = m_dMax;
  nodalValueSet->m_dSum = m_dSum;
  nodalValueSet->m_dStdDev = m_dStdDev;
  nodalValueSet->m_bDirty = m_bDirty;

  return nodalValueSet;
}

bool CNodalValueSet::operator==(const CNodalValueSet &rhs) const
{
	return IValueSet::operator==(rhs);
}

CNodalValueSet& CNodalValueSet::operator=(const CNodalValueSet& rhs)
{
	IValueSet::operator =(rhs);

	m_bDirty = rhs.m_bDirty;
	m_dMax = rhs.m_dMax;
	m_dMin = rhs.m_dMin;
	m_dStdDev = rhs.m_dStdDev;
	m_dSum = rhs.m_dSum;
	m_data = rhs.m_data;

	return *this;
}

unsigned int CNodalValueSet::TypeId() const
{
	return 0;
}

unsigned int CNodalValueSet::IconId() const
{
	return IDI_NODAL_VALUESET;
}

CNodalValueSet::TValue CNodalValueSet::ValuePoint(const geo::IPoint& pt, geo::IParallelInitializationCallback * /*cb*/) const
{
	// Do a present point
	std::vector<int> vcVal = PointSet().PointAt(pt);
	if(vcVal.size() == 1)
		return m_data[vcVal[0]];

	geo::CValue value;
	int nCount = 0;

	for(int i = 0; i < vcVal.size(); i++)
	{
		if(m_data[vcVal[i]].Valid())
		{
			if(value.Valid())
				value = TValue(value.Value() + m_data[vcVal[i]].Value());
			else
				value = m_data[vcVal[i]];

			nCount++;
		}
	}

	if(nCount > 0)
	{
		assert(value.Valid());
		return TValue(value.Value() / nCount);
	}

	return TValue();
}

CNodalValueSet::TValueVec CNodalValueSet::ValueElement(const geo::IElement& element, geo::IParallelInitializationCallback *cb) const
{
	TValueVec vcRet;
	for(int i = 0; i < element.NrOfPoints(); i++)
		vcRet.push_back(ValuePoint(element.Point(i), cb));

	return vcRet;
}

int CNodalValueSet::PushBack(const geo::IValue& value)
{
	int nIndex = m_data.size();

	m_data.push_back(geo::CValue(value));
	m_bDirty = true;

	return nIndex;
}

int CNodalValueSet::PushBack(const double& dValue)
{
	int nIndex = m_data.size();

	m_data.push_back(geo::CValue(dValue));
	m_bDirty = true;

	return nIndex;
}

const CNodalValueSet::TValue& CNodalValueSet::Value(const int nIndex) const
{
	assert(nIndex < m_data.size());
	assert(nIndex >= 0);

	return m_data[nIndex];
}

int CNodalValueSet::ValueSize() const
{
	return m_data.size();
}

void CNodalValueSet::Value(const int nIndex, const double &value)
{
	assert(nIndex < m_data.size());
	assert(nIndex >= 0);

	m_bDirty = true;

	m_data[nIndex] = value;
}

void CNodalValueSet::Value(const int nIndex, const geo::IValue &value)
{
	assert(nIndex < m_data.size());
	assert(nIndex >= 0);

	m_bDirty = true;

	m_data[nIndex] = value;
}


void CNodalValueSet::CalculateProperties() const
{
	assert(m_data.size() == PointSet().PointSize());
	// Set sum on zero ...
	int nCount = 0;
	m_dMin = DBL_MAX;
	m_dMax = -DBL_MAX;
	m_dSum = 0;
  int i;
	for(i = 0; i < m_data.size(); i++)
	{
		if(m_data[i].Valid())
		{
			nCount++;
			m_dMin = std::min(m_data[i].Value(), m_dMin);
			m_dMax = std::max(m_data[i].Value(), m_dMax);
			m_dSum = m_data[i].Value() + m_dSum;
		}
	}


	// Set dirty false to prevent nested endless loops
	m_bDirty = false;

	// Get std dev ...
	nCount = 0;
	for(i = 0; i < m_data.size(); i++)
	{
		if(m_data[i].Valid())
		{
			if(nCount == 0) 
				m_dStdDev = abs(Average() - m_data[i].Value());
			else
				m_dStdDev += abs(Average() - m_data[i].Value());
			nCount++;
		}
	}

	m_dStdDev = sqrt( m_dStdDev / nCount);
}

CNodalValueSet::TValue CNodalValueSet::Min() const
{
	if( m_bDirty )
		CalculateProperties();

	return TValue(m_dMin);
}

CNodalValueSet::TValue CNodalValueSet::Max() const
{
	if( m_bDirty )
		CalculateProperties();

	return TValue(m_dMax);
}

const double& CNodalValueSet::StdDev() const
{
	if( m_bDirty )
		CalculateProperties();

	return m_dStdDev;
}

double CNodalValueSet::Average() const
{
	return m_dSum / m_data.size(); 	
}



bool CNodalValueSet::EqualValue(const CNodalValueSet &set) const
{
//	if(!PointSet().EqualPoints(set.PointSet()))
//		return false;


	std::vector<geo::CValue> vcDiff;
	std::set_difference(m_data.begin(), 
						m_data.end(),
						set.m_data.begin(),
						set.m_data.end(),
						std::back_inserter(vcDiff));

	if(vcDiff.size() > 0)
		return false;
				

	return true;
}

void CNodalValueSet::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
	IValueSet::SaveStream(stream,progress);

	stream << ValueSize();
	for( int iCount = 0; iCount < ValueSize(); iCount++ )
	{
		int bValid = Value(iCount).Valid() == true;
		stream << bValid;
		if(bValid)
			stream << Value(iCount).Value();
		progress.Step();
	}
}

void CNodalValueSet::LoadStream(TSTREAM &stream, CStreamVersion &version,TPROGRESS &progress)
{
	IValueSet::LoadStream(stream,version,progress);

	// This only saved in version greater than 3.0.0 ...
	if((version.majorNr() == 3) && (version.minorNr() == 0) && (version.revisionNr() == 0))
	{
		int iUnit;
		stream >> iUnit;
		PointSet().ValueUnit((CQuantity::UNIT)iUnit);
	}

	int valuesize;
	stream >> valuesize;
	double dValue;
	for( int iCount = 0; iCount < valuesize; iCount++ )
	{
		int bValid = true;
		if(CStreamVersion(3, 0, 18) < version)
			stream >> bValid;
		if(bValid)
		{
			stream >> dValue;
			PushBack( dValue );
		}
		else
			PushBack(TValue());

    try
    {
		  progress.Step();
    }
    catch(CProgressCancel*)
    {
      m_data.clear();
      throw;
    }
	}
}

long CNodalValueSet::SavedItems() const
{
	return IValueSet::SavedItems() + ValueSize();
}


bool CNodalValueSet::PrepareMapping(const geo::IElementSet *pElementSet)
{
  if (m_bDirty)
    CalculateProperties();

  PointSet().PrepareMapping(pElementSet);

  return true;
}
