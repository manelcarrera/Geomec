// StressGradOrDensity.cpp: implementation of the CStressGradOrDensity class.
//
//////////////////////////////////////////////////////////////////////

#include <QVectorIterator>

#include "StressGradOrDensity.h"
#include "StressGradientQuantity.h"
#include "DensityQuantity.h"
#include "StreamVersion.h" 
#include "QtDataStream.h"

#define GRAVITY_FACTOR 9.81

//density = gradient * GRAVITY_FACTOR

 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//ALL CONSTRUCTORS ARE IN GRADIENT DEFINITION !!!!

void CStressGradOrDensity::Init()
{
	m_Type = GRAD; //!!!!!!!!!!!!!!!!!!!
}

/*virtual*/ CStressGradOrDensity::~CStressGradOrDensity()
{
	// Destruct
}


CStressGradOrDensity::CStressGradOrDensity(const CStressGradOrDensity &rhs)
: CDoubleQuantity(rhs)
{
	Init();
}

	

CStressGradOrDensity::CStressGradOrDensity()
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Init();

}

CStressGradOrDensity::CStressGradOrDensity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(value, unit);
	Init();
}

CStressGradOrDensity::CStressGradOrDensity(const double& dStressGradient,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

	CDoubleQuantity::MaxValue(max,  unit);
	CDoubleQuantity::MinValue(min,  unit);
	Value(dStressGradient, unit);
	Init();
}

CStressGradOrDensity::CStressGradOrDensity(const double& value,
			                                     const double &min ,
			                                     const double &max,
			                                     bool bIncludeMin,
			                                     bool bIncludeMax,
							                     const UNIT unit)
:CDoubleQuantity()
{
	m_bIncludeMin=bIncludeMin;
	m_bIncludeMax=bIncludeMax;
	MaxValue(max,  unit);
	MinValue(min,  unit);
	Value(value, unit);
	Init();
}




CStressGradOrDensity::TYPE CStressGradOrDensity::GetType() const
{
	return m_Type; 
}

void CStressGradOrDensity::SetGradient(const double& value,const UNIT unit /*=SI_UNIT*/)
{
	m_undefined=false;
	m_value=value;
	if(m_Type==DENS)
		m_value=m_value*GRAVITY_FACTOR;
	
	m_value = Convert(m_value,SI_UNIT,unit);
}

void CStressGradOrDensity::SetDensity(const double& value,const UNIT unit /*=SI_UNIT*/)
{
	m_undefined=false;
	m_value=value;
	if(m_Type==GRAD)
		m_value=m_value/GRAVITY_FACTOR;
	
	m_value = Convert(m_value,SI_UNIT,unit);

}

void CStressGradOrDensity::SetType(CStressGradOrDensity::TYPE type)
{
	if(type != m_Type)
	{
		m_Type = type;

		if(!Undefined())
		{
			if(m_Type==GRAD)
				m_value=m_value*GRAVITY_FACTOR;
			else
				m_value=m_value/GRAVITY_FACTOR;
		}
	}
}

double CStressGradOrDensity::GetGradient(const UNIT unit /*=SI_UNIT*/) const
{
	assert(!Undefined());

	double val = m_value;
	if(m_Type != GRAD)
		val=val*GRAVITY_FACTOR;
	
	return Convert(val,unit,SI_UNIT);

}

double CStressGradOrDensity::GetDensity(const UNIT unit /*=SI_UNIT*/) const
{
	assert(!Undefined());

	double val = m_value;
	if(m_Type != DENS)
		val=val/GRAVITY_FACTOR;
	
	return Convert(val,unit,SI_UNIT);
}


double CStressGradOrDensity::Convert(const double &value, const UNIT out, const UNIT in) const
{

	if(m_Type==GRAD)
		return CStressGradientQuantity().Convert(value,out,in);


	return	CDensityQuantity().Convert(value,out,in);
}

std::string	CStressGradOrDensity::QuantityName() const
{
	return "StressGradient";
}

std::string CStressGradOrDensity::UnitName(const UNIT unit) const 
{
	if(m_Type==GRAD)
		return CStressGradientQuantity().UnitName(unit);

	return	CDensityQuantity().UnitName(unit);

}

// The store and restore functions are used for saving, loading, redo and undo
/*virtual*/ void CStressGradOrDensity::restore(IModelStream& stream)
{

	CStreamVersion version;
	version.restore(stream);

	stream >> m_value;
	stream >> m_max;
	stream >> m_min;

	short i_bool;
	stream >> i_bool ; m_undefined = (i_bool==1);
	stream >> i_bool ; m_bIncludeMax  = (i_bool==1);
	stream >> i_bool ; m_bIncludeMin = (i_bool==1);

	if(version >= CStreamVersion(1,0,2))
	{
		assert(m_Type==GRAD);
	}
	else if(version >= CStreamVersion(1,0,1))
	{
		int type;
		stream >> type;
		m_Type = (CStressGradOrDensity::TYPE)type; 
	}
	else
		m_Type=GRAD;

}

/*virtual*/ void CStressGradOrDensity::store(IModelStream& stream) const
{

	assert(m_Type==GRAD);

	CStreamVersion version(1,0,2);
	version.store(stream);

	stream << m_value;
	stream << m_max;
	stream << m_min;

	stream << (short)(m_undefined ?  1:0);
	stream << (short)(m_bIncludeMax  ? 1:0);
	stream << (short)(m_bIncludeMin  ? 1:0);

}
