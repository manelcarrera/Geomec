// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDRILLINGSCHEME_3B667C4B0066_INCLUDED
#define _INC_CDRILLINGSCHEME_3B667C4B0066_INCLUDED



#include "IComponent.h"
#include "CDrillingDirectionSection.h"
#include "CDrillingOperation.h"
#include "Dependency.h"

#include <vector>

class CDataModel;

//##ModelId=3C0DDDAD0262
class CDrillingScheme 
: public IComponent
{
private:

  CDataModel& m_model;
	//##ModelId=3CAAA65A0236
	CLengthQuantity m_qOverburdenBottomTMD;
	//##ModelId=3CAAA65A0264
	std::vector<CDrillingDirectionSection*> m_vecDrillingDirectionSection;
	//##ModelId=3CAAA65A0294
	std::vector<CDrillingOperation*> m_vecDrillingOperation;

	//##ModelId=3C60D56802A1
	std::vector<CDependency*> m_DependencyVec;

public:
	//##ModelId=3CA0118C003E
	virtual bool CheckComplete() const;
	//##ModelId=3C2043F20129
	int NrOfDrillingOperations() const;
	//##ModelId=3C2043F20177
	int NrOfDrillingDirections() const;
	
	//##ModelId=3C0DDDAD030E
	CDrillingScheme(CDataModel& model);
	//##ModelId=3C0DDDAD030F
	~CDrillingScheme();

	//From: Drilling operation
	//Label: TMD at overburden bottom
	//Descr: TMD at overburden bottom
	//##ModelId=3C0DDDAD0310
	CLengthQuantity* OverburdenBottomTMD();
	const CLengthQuantity* OverburdenBottomTMD() const;

	//##ModelId=3B667E0D0252
	//A list with one item (temporary)
	//##ModelId=3C0DDDAD031D
	CDrillingDirectionSection* GetDrillingDirectionSection(int Index);
	const CDrillingDirectionSection* GetDrillingDirectionSection(int Index) const;
  CDrillingDirectionSection* GetDrillingDirectionSectionAt(double dTMD);
  const CDrillingDirectionSection* GetDrillingDirectionSectionAt(double dTMD) const;

	//##ModelId=3B667E240262
	//A list with one item (temporary)
	//##ModelId=3C0DDDAD032D
	CDrillingOperation* GetDrillingOperation(int Index);
	const CDrillingOperation* GetDrillingOperation(int Index) const;

	//Add Drilling Operation
	//##ModelId=3C0DDDAD033D
	bool AddDrillingOperation();

	//Remove Drilling Operation
	//##ModelId=3C2043F201C6
	bool RemoveDrillingOperation(int Index);

	//add Drilling Direction Section
	//##ModelId=3C0DDDAD0340
	bool AddDrillingDirectionSection();

	//Remove Drilling Direction Section
	//##ModelId=3C2043F202DF
	bool RemoveDrillingDirectionSection(int Index);

  void RemoveAllDrillingDirectionSections();

};

#endif /* _INC_CDRILLINGSCHEME_3B667C4B0066_INCLUDED */
