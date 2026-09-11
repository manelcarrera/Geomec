// ColorGradient.h: interface for the CColorGradient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORGRADIENT_H__D66ADA3E_F220_4A43_A130_1DFB63FC56BA__INCLUDED_)
#define AFX_COLORGRADIENT_H__D66ADA3E_F220_4A43_A130_1DFB63FC56BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//##ModelId=3BC55D64012E

#include "IColorScaleNode.h"
#include "GeomecModelVisitor.h"

class CColorGradient : public IColorScaleNode
{
	typedef std::map<double, TColor> TColorMap;
	TColorMap m_mpColor;
	bool m_bGlobal;
	std::pair<double, double> m_MinMaxValue;
	void UpdateMinMax();
	void LoadRegistry(const QString& strSection, const QString& strEntry);
	bool m_bSystemColorGradient;
  bool m_bAutoExtremes; // automatically adjust extremes for global colorscale (act like local scale)

public:
	// Construction
	CColorGradient(CFemAppModel &model);
	CColorGradient(CFemAppModel &model, const QString &name); //wjrx mantis 2560
	CColorGradient(unsigned int uName, 
				   CFemAppModel &model, const double& firstValue, const double& secondValue,
				   TColor min_color = qRgb(0, 0, 255),
				   TColor max_color = qRgb(255,0,0),
				   bool bGlobal = true); 
	CColorGradient(const CColorGradient &rhs);
	virtual ~CColorGradient();

	bool SystemColorGradient() const { return m_bSystemColorGradient;} //wjrx mantis 2560
  bool AutoExtremes() const;
  void AutoExtremes(bool bAuto);
	
  TColorMap colorMap() const;
  void colorMap(const TColorMap& colorMap);

  // We only supply a const iterator. The client can only change values in the map with operators
	// on the interface.
	typedef TColorMap::const_iterator color_iterator;
	color_iterator Begin() const;
	color_iterator End() const;
    size_t NumColors() const;
	void SetMinMaxValue(const double& dMin, const double& dMax);
	const std::pair<double, double>& MinMaxValue() const; 

	// Operations
	bool AddValue(const double& dValue, TColor iColor);
	bool AddValue(int iNumerator, int iDenominator, TColor iColor);
	bool RemoveValue(const double &dValue);
	bool ChangeValue(const double &dOldValue, const double &dNewValue);
	bool ChangeColor(const double &dValue, TColor color);

	TColor Color(const double &dValue) const;
	void SaveRegistry(const QString& map, const QString& key);

	double RealValue(int &iNumerator, int &iDenominator);

	bool FindExtents(const double& firstValue, const double& secondValue);

	double Fraction(color_iterator it) const;
	bool IsGlobal() const {return m_bGlobal;}

	// Load and Save...
	virtual bool Empty() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

	bool operator == (const CColorGradient &rhs) const;
	CColorGradient& operator = (const CColorGradient &rhs);

	virtual unsigned int TypeId() const;

	virtual unsigned int IconId() const;
	int MinColor() const;
	int MaxColor() const;
	virtual TColor Color(const TValue& value, const TMinMax &min_max) const; 

  ACCEPT_GEOMECMODELVISITORS(VisitColorGradient);
};

#endif // !defined(AFX_COLORGRADIENT_H__D66ADA3E_F220_4A43_A130_1DFB63FC56BA__INCLUDED_)
