// DepthQuantity.h: interface for the CDepthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPTHQUANTITY_H__869BCFB8_E971_43E4_B717_66914830344E__INCLUDED_)
#define AFX_DEPTHQUANTITY_H__869BCFB8_E971_43E4_B717_66914830344E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LengthQuantity.h"
#include "QuantityNode.h"
#include "Boundary.h"

//##ModelId=3BE7AEC40331
class CDistributedDepth : public CQuantityNode
{
	CLengthQuantity m_length;
	//##ModelId=3BE7AEC40350
	CBoundary* m_pBoundary;
public:
	//##ModelId=3BE7AEC50043
	typedef CQuantityEntry<CDistributedDepth> TQuantityEntry;
private:
	//##ModelId=3BE7AEC40371
	TQuantityEntry *m_pEntry;
public:
	//##ModelId=3BE7AEC4037F
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	//##ModelId=3BE7AEC4039F
	virtual UINT IconId() const;
	//##ModelId=3BE7AEC403A1
	CDistributedDepth(CGraphModel& model);
	//##ModelId=3BE7AEC403A3
	virtual const IQuantityEntry& QuantityEntry() const;
	//##ModelId=3BE7AEC403AE
	virtual std::string QuantityName() const ;
	//##ModelId=3BE7AEC403B0
	virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
	//##ModelId=3BE7AEC403BE
	virtual int ComponentSize() const;
	//##ModelId=3BE7AEC403C0
	virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
	//##ModelId=3BE7AEC403DD
	virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
	//##ModelId=3BE7AEC50005
	virtual double Min(const geo::IBox &box, const int nComponent = 0, const UNIT unit = SI_UNIT) const;
	//##ModelId=3BE7AEC50015
	virtual double Max(const geo::IBox &box, const int nComponent = 0, const UNIT unit = SI_UNIT) const;
	//##ModelId=3BE7AEC50026
	virtual double Value(const geo::IPoint& pt, const int nComponent, const UNIT unit = SI_UNIT) const;
	bool Undefined() const { return false; }
};

#endif // !defined(AFX_DEPTHQUANTITY_H__869BCFB8_E971_43E4_B717_66914830344E__INCLUDED_)
