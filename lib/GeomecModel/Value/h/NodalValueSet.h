// NodalValueSet.h: interface for the CNodalValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODALVALUESET_H__57DB5413_63F0_45ED_AFD6_40F5A56D49A5__INCLUDED_)
#define AFX_NODALVALUESET_H__57DB5413_63F0_45ED_AFD6_40F5A56D49A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IPointSet;

#include "GeomecIValueSet.h"

class CNodalValueSet : public IValueSet
{
	TValueVec m_data;							// Vector with data

	// Properties of the valueset ....
	mutable double m_dMin;						// Cache min
	mutable double m_dMax;						// Cache max
	mutable double m_dSum;						// Cache sum
	mutable double m_dStdDev;					// Cache std dev
	mutable bool m_bDirty;						// Dirty flag
	void CalculateProperties() const;
public:
	CNodalValueSet(const QString &sName, CQuantity::UNIT unit, IPointSet& set);
	CNodalValueSet(IPointSet& point_set, const CNodalValueSet& rhs, TPROGRESS& progress);
	CNodalValueSet(const CNodalValueSet& rhs);
	CNodalValueSet(IPointSet& set);
	virtual ~CNodalValueSet();

  virtual CNodalValueSet* clone();

	// Equal and assignment
	bool operator==(const CNodalValueSet &rhs) const;
	CNodalValueSet& operator=(const CNodalValueSet& rhs);
	bool EqualValue(const CNodalValueSet &set) const;
	
	// Adding and modifying values
	int PushBack(const double& dValue);
	int PushBack(const geo::IValue& value);
	int ValueSize() const;
	const TValue& Value(const int nIndex) const;
	void Value(const int nIndex, const double& value); 
	void Value(const int nIndex, const geo::IValue& value); 

	// Present element and point
  virtual TValue ValuePoint(const geo::IPoint& pt, geo::IParallelInitializationCallback *cb) const;
  virtual TValueVec ValueElement(const geo::IElement& element, geo::IParallelInitializationCallback *cb) const;

	// Graph visualisation
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;

	// Min and max
	virtual TValue Min() const;
	virtual TValue Max() const;
	const double& StdDev() const;
	double Average() const;

	// Save and load
	virtual void LoadStream(TSTREAM& stream, CStreamVersion &version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

  virtual bool PrepareMapping(const geo::IElementSet *);

  ACCEPT_GEOMECMODELVISITORS(VisitNodalValueSet);
};

#endif // !defined(AFX_NODALVALUESET_H__57DB5413_63F0_45ED_AFD6_40F5A56D49A5__INCLUDED_)
