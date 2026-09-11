// PropertyFactory.h: interface for the CPropertyFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYFACTORY_H__39052388_351D_4A47_8F9D_E4FDD227C747__INCLUDED_)
#define AFX_PROPERTYFACTORY_H__39052388_351D_4A47_8F9D_E4FDD227C747__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include "ValueTypeFactory.h" 
#include "IElementSet.h" 

// Warning : This class is only implemented for Mode = 0 :-)

class CPropertyFactory : public CValueTypeFactory
{
public:
	// Represents a for 


public:
	enum FORMAT { UNDEFINED, NODAL, ELEMENT };
	enum STATE	{ NONE, FORMAT_ERROR, 													// Undefined state
				  READ_NONE, READ_NODES, READ_ELEMENTS, READ_READY,		// Read states
				  WRITE_NONE, WRITE_HEADER, WRITE_VALUE, WRITE_READY }; 
private:
	class string_comp
	{
	public:
		bool operator()(const CString& s1, const CString& s2) const
		{	return s1.CompareNoCase(s2) < 0; }
	};

	typedef std::vector<UINT> TComponentVec;
	typedef std::pair<UINT, TComponentVec> TValueType;
	typedef std::map<const CString, TValueType, string_comp> TValueTypeMap;
	typedef std::vector<UINT> THeaderTagVec;
	typedef std::map<UINT, THeaderTagVec> THeaderMap;
	typedef std::set<CString, string_comp> TValueSet;
	TValueTypeMap m_mpValueType;  
	THeaderMap m_mpHeader;
	TValueSet m_stValueSet;
	IPointSet* m_pPointSet;
	std::vector<IValueComposite*> m_vcComposite;
	std::vector<CString> m_vcComment;
	STATE m_state;
	FORMAT m_format;
	INodalFormat	*m_pNodalFormat;
	IElementFormat	*m_pElementFormat;
public:
// Construction 
	CPropertyFactory(CGraphModel& model);	// Constructor for reading
	CPropertyFactory(const IPointSet& point_set, enum FORMAT format);
	virtual ~CPropertyFactory();

	// The current format of the factory ...
	enum FORMAT	Format() const;
	enum STATE State() const;
	virtual void FormatError(const CString& sErrorMessage);

	const IFormat& Format(enum FORMAT format) const;
	IFormat& Format(enum FORMAT format);

	virtual enum CToken::TokenRet ParseHeaderTag(const CString &sHeaderTag);

// Interface output //
	IPointSet& PointSet();
	const IPointSet& PointSet() const;
	BOOL IsPointSet() const;

// Interface input //
	BOOL AddValueComposite(const IValueComposite& composite);

// Write support functions for header 
	int CommentSize() const;
	CString Comment(int nIndex) const;
	BOOL CreatePointSet(int nDim, int nNodalValueSetSize);
	void Unit(CQuantity::UNIT unit) { m_unit = unit;}
protected:

// Definition of the header map
	virtual void DefineComponent(UINT uHeaderID, UINT uValueTypeID, UINT uComponentIndex, UINT uMode=0);
	virtual void DefineValueSet(UINT uHeaderID);

// Export function to create the pointset
	virtual IPointSet::DIMENSION OnSetPointSetDimension() const = 0;
	virtual CString OnSetPointSetName() const = 0;

// Interface for file reading ...


private:
	CQuantity::UNIT m_unit;
	CString HeaderTag(IValueComponent &component) const;
};

#endif // !defined(AFX_PROPERTYFACTORY_H__39052388_351D_4A47_8F9D_E4FDD227C747__INCLUDED_)
