
#include <QVectorIterator>

#include "IModelStream.h" 
//#include "..\..\ModelFrame\include\Version.h" 
#include "StreamVersion.h" 
#include "SingleQuantity.h"
#include "QtDataStream.h"

//update if store restore is changed.
//ensure backward compatibility!!
static CStreamVersion current_version(1,0,0);


CDoubleQuantity::CDoubleQuantity()
	:m_undefined(true),
	m_bIncludeMax(true),
	m_bIncludeMin(true)
{
	
}


CDoubleQuantity::CDoubleQuantity(const CDoubleQuantity &rhs)
: QObject(),  // QObject(const QObject&) is private,
              // hence QObject(QObject* parent = 0)
  m_value(rhs.m_value), 
  m_max(rhs.m_max),
  m_min(rhs.m_min), 
  m_undefined(rhs.m_undefined),
  m_bIncludeMax(rhs.m_bIncludeMax),
  m_bIncludeMin(rhs.m_bIncludeMin) 
{
}

CDoubleQuantity& CDoubleQuantity::operator=(const CDoubleQuantity & rhs) 
{
	m_value = rhs.m_value;
	m_max = rhs.m_max;
	m_min = rhs.m_min;
	m_undefined = rhs.m_undefined; 
	m_bIncludeMin = rhs.m_bIncludeMin;
	m_bIncludeMax = rhs.m_bIncludeMax;

	return *this;
}


// Set and get value ...
double CDoubleQuantity::Value(const UNIT unit ) const
{	
	assert(!Undefined());
	return Convert(m_value, unit, SI_UNIT); 
}

//default SI setvalue
void CDoubleQuantity::SetValue(const double& value)
{
	Value(value,SI_UNIT); 
}

void CDoubleQuantity::Value(const double& value, const UNIT unit)
{
	
	bool accept = true;
	QString Description;
	
	emit OnValidate(value ,accept,unit,*this,Description);

	if(!accept)
	{
		OnRefuseValue(*this,Description);
		return;
	}

	m_undefined = false;
	m_value = Convert(value, SI_UNIT, unit);
	emit OnChanged(*this);
}

/*virtual*/ void CDoubleQuantity::MaxValue(const double& newval, const UNIT unit)
{
	m_max=Convert(newval,unit,SI_UNIT);		
}

/*virtual*/ void CDoubleQuantity::MinValue(const double& newval, const UNIT unit)
{
	m_min=Convert(newval,unit,SI_UNIT);		
}

/*virtual*/ double CDoubleQuantity::MaxValue( const UNIT unit ) const
{
	return Convert(m_max,unit,SI_UNIT);		
}

/*virtual*/ double CDoubleQuantity::MinValue( const UNIT unit ) const
{
	return Convert(m_min,unit,SI_UNIT);		
}


/*virtual*/ void CDoubleQuantity::Invalidate()
{ 
	m_undefined = true; 
	emit OnChanged(*this);
}


/*virtual*/ bool CDoubleQuantity::Undefined() const
{	

	return m_undefined; 
}


/*virtual*/ bool CDoubleQuantity::IncludeMin() const
{
	return m_bIncludeMin;
}

/*virtual*/ bool CDoubleQuantity::IncludeMax() const
{
	return m_bIncludeMax;
}

/*virtual*/ void CDoubleQuantity::IncludeMin(bool v)
{
	m_bIncludeMin= v;
}

/*virtual*/ void CDoubleQuantity::IncludeMax(bool v)
{
	m_bIncludeMax= v;
}



/*virtual*/ bool CDoubleQuantity::CheckValue(const double &value, std::string& reason , const UNIT unit ) const
{
	double val= Convert(value, SI_UNIT, unit);

	bool ret;
	if(val<m_max)
	{
		if(val>m_min)
		{
			ret=true;
		}
		else if(val<m_min)
		{
			ret= false;
		}
		else
		{
			ret=m_bIncludeMin;
		}
	}
	else if(val>m_max)
	{
		ret= false;
	}
	else
	{
		ret= m_bIncludeMax;
	}

	if(ret)
	{
		CDoubleQuantity& This = const_cast<CDoubleQuantity&>(*this);
		This.emit OnCheckValue(value,ret,unit,reason,This);
	}
	else
	{
		reason	= "value is outside limits";
	}

	return ret;
} 

bool	CDoubleQuantity::operator < (const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value < (right.m_value-QUANTITY_EPS);
}

bool	CDoubleQuantity::operator > (const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value > (right.m_value+QUANTITY_EPS);
}

bool	CDoubleQuantity::operator == (const CDoubleQuantity& right) const
{
	if(right < *this)
		return false;

	if(*this < right)
		return false;

	return true;
}


// The store and restore functions are used for saving, loading, redo and undo
/*virtual*/ void CDoubleQuantity::restore(IModelStream& stream)
{

	CStreamVersion version;
	version.restore(stream);
	
	assert(version == current_version);

	stream >> m_value;
	stream >> m_max;
	stream >> m_min;


	short i_bool;
	stream >> i_bool ; m_undefined = (i_bool==1);
	stream >> i_bool ; m_bIncludeMax  = (i_bool==1);
	stream >> i_bool ; m_bIncludeMin = (i_bool==1);

}

/*virtual*/ void CDoubleQuantity::store(IModelStream& stream) const
{

	current_version.store(stream);
	stream << m_value;
	stream << m_max;
	stream << m_min;

	stream << (short)(m_undefined ?  1:0);
	stream << (short)(m_bIncludeMax  ? 1:0);
	stream << (short)(m_bIncludeMin  ? 1:0);

}


double CDoubleQuantity::operator*(const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value * right.m_value;
}

double CDoubleQuantity::operator / (const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value / right.m_value;
}

double CDoubleQuantity::operator-(const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value - right.m_value;
}

double CDoubleQuantity::operator+(const CDoubleQuantity& right) const
{
	assert(!Undefined() && !right.Undefined());
	return m_value + right.m_value;
}

CDoubleQuantity::operator const double &() const
{
	if ( Undefined() )
	{
		const_cast <double&> (m_value) = 0;
		assert(false); //mantis 2547 wjrx
	}
	return m_value;
}




