// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDEPENDENCIESMANAGER_3B67C3D90264_INCLUDED
#define _INC_CDEPENDENCIESMANAGER_3B67C3D90264_INCLUDED

#include "Dependency.h"
#include <vector>

//##ModelId=3C0DC15C01F4
typedef CDoubleQuantity QU;

//##ModelId=3C0DDDAE00CC
class CDependenciesManager 
{
private:

	//##ModelId=3CAAA65B00FE
	std::vector<const CDependency*> m_DependencyVec;

public:

	//##ModelId=3DDA0C4602E3
	QU::UNIT m_UnitSystem;

	
	//##ModelId=3DDA0C4602F1
	QU::UNIT GetUnit();
	//##ModelId=3DDA0C4602F2
	void SetUnit(QU::UNIT us);

	//##ModelId=3C0DDDAE0100
	CDependenciesManager();

	//##ModelId=3C0DDDAE010A
	~CDependenciesManager();
	

	//##ModelId=3C0DDDAE010B
	void AddDependency(const CDependency* pDependency);

	//##ModelId=3C0DDDAE010D
	void RemoveDependency(const CDependency* pDependency);

	//##ModelId=3C0DDDAE010F
	void RemoveDependency(int Index);


	//##ModelId=3C0DDDAE011A
	bool CheckQuantity(const CDoubleQuantity* pQuantity, std::string &strDependency) const;


	//##ModelId=3C60D56900EB
	std::vector<std::string> CheckAllDependencies();
};

#endif /* _INC_CDEPENDENCIESMANAGER_3B67C3D90264_INCLUDED */
