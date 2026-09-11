// QuantityNode.h: interface for the CQuantityNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUANTITYNODE_H__90B821AE_3130_4992_918A_0DFB58312C8B__INCLUDED_)
#define AFX_QUANTITYNODE_H__90B821AE_3130_4992_918A_0DFB58312C8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Quantity.h"
#include "QuantityEntry.h"
#include "UnitNode.h"

class COpenGLSceneBase;
//##ModelId=3BE7AEB70053
class CQuantityNode : public CGraphNode, public CQuantity 
{
public:
	//##ModelId=3BE7AEB70073
	CQuantityNode(const CString& strInstanceName);
	//##ModelId=3BE7AEB70075
	CQuantityNode(UINT uInstanceName);	
	//##ModelId=3BE7AEB70083
	CQuantityNode(const CQuantityNode & rhs)
		: CGraphNode(rhs) {}

	//##ModelId=3BE7AEB70092
	virtual const IQuantityEntry& QuantityEntry() const = 0;
	//##ModelId=3BE7AEB70094
	virtual const COpenGLNode* ProjectOn() const;
	//##ModelId=3BE7AEB70096
	virtual UINT TypeNameId() const;

};

#endif // !defined(AFX_QUANTITYNODE_H__90B821AE_3130_4992_918A_0DFB58312C8B__INCLUDED_)
