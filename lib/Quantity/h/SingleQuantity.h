// SingleQuantity.h: interface for the CSingleQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEQUANTITY_H__873C4E7C_8D62_4238_B2BD_836258A5B35F__INCLUDED_)
#define AFX_SINGLEQUANTITY_H__873C4E7C_8D62_4238_B2BD_836258A5B35F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <qobject.h>
#include <cassert>



#define QUANTITY_EPS 1e-5

#define DOUBLE_MIN_ -1e20
#define DOUBLE_MAX_ 1e20


//class CStreamVersion;
class IModelStream; 


class CDoubleQuantity : public QObject
{
	Q_OBJECT

public:
	enum UNIT { FIELD_UNIT, SI_UNIT, US_UNIT, SI_USER_UNIT };
	
signals:

	void OnChanged(const CDoubleQuantity& Quantity);
	void OnValidate(const double& new_value, bool& accept , CDoubleQuantity::UNIT unit , const CDoubleQuantity& Quantity,QString & Description);
	void OnRefuseValue(const CDoubleQuantity& Quantity,const QString & Description);
	void OnCheckValue(const double &value  , bool& accept , CDoubleQuantity::UNIT unit , std::string& reason , const CDoubleQuantity& Quantity);

protected:
	double m_value;
	double m_max;
	double m_min;
	bool m_undefined;
	bool m_bIncludeMax;
	bool m_bIncludeMin;

	// only direved can instantiate direct..
	CDoubleQuantity();

public:
	
	CDoubleQuantity(const CDoubleQuantity &rhs);
	CDoubleQuantity& operator=(const CDoubleQuantity & rhs) ;


	// Quantity description ...
	virtual std::string	QuantityName() const = 0;

	// Unit description ...
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const = 0;

	// Value discription ...
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const = 0;


	// Set and get value ...
	virtual double Value(const UNIT unit = SI_UNIT ) const;

	//default SI setvalue
	void SetValue(const double& value);

	void Value(const double& value, const UNIT unit);

	virtual void MaxValue(const double& newval, const UNIT unit);
	virtual void MinValue(const double& newval, const UNIT unit);
	virtual double MaxValue( const UNIT unit = SI_UNIT) const;
	virtual double MinValue( const UNIT unit = SI_UNIT) const;

	virtual void Invalidate();
	virtual bool Undefined() const;
	virtual bool IncludeMin() const;
	virtual bool IncludeMax() const;
	virtual void IncludeMin(bool v);
	virtual void IncludeMax(bool v);

	virtual bool CheckValue(const double &value, std::string& reason , const UNIT unit=SI_UNIT ) const;

	bool	operator < (const CDoubleQuantity& right) const;
	bool	operator > (const CDoubleQuantity& right) const;
	bool	operator == (const CDoubleQuantity& right) const;


	double 	operator*(const CDoubleQuantity& right) const;
	double 	operator/(const CDoubleQuantity& right) const;
	double 	operator-(const CDoubleQuantity& right) const;
	double 	operator+(const CDoubleQuantity& right) const;

	operator const double& () const;

	// The store and restore functions are used for saving, loading, redo and undo
	virtual void restore(IModelStream& stream);
	virtual void store(IModelStream& stream) const;
};




typedef CDoubleQuantity IQuantityDouble;
typedef IQuantityDouble CQuantity;


#endif // !defined(AFX_SINGLEQUANTITY_H__873C4E7C_8D62_4238_B2BD_836258A5B35F__INCLUDED_)
