// importquantityfactory.h: interface for the CImportQuantityFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUANTITYFACTORY_H__CBEF114B_C3AA_401A_A387_C2D291D630E2__INCLUDED_)
#define AFX_QUANTITYFACTORY_H__CBEF114B_C3AA_401A_A387_C2D291D630E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DistributedQuantity.h"

class CQuantityFactory  
{
	CGraphModel& m_model;
	typedef std::map<UINT, CQuantityBuilder*> TQuantityMap;
	typedef std::map<UINT, UINT> TQuantityEntryMap;
	TQuantityMap m_mpQuantity;
	TQuantityEntryMap m_mpQuantityEntry;
// Initialisation 
	void AppendBuilder(UINT uQuantityEntryID, UINT uQuantityID, CQuantityBuilder& builder);
public:
	// Construction ...
	CQuantityFactory(CGraphModel &model);
	virtual ~CQuantityFactory();

	// Function to build a quantity
	BOOL QuantityAvailable(UINT uQuantityID) const;
	CDistributedQuantity* BuildQuantity(CPointSet &point_set, UINT uQuantityID);
};

#endif // !defined(AFX_QUANTITYFACTORY_H__CBEF114B_C3AA_401A_A387_C2D291D630E2__INCLUDED_)
