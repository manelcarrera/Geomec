// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CANALYSISPOINT_3B669E030204_INCLUDED
#define _INC_CANALYSISPOINT_3B669E030204_INCLUDED



#include "IComponent.h"
#include "CProductionLoads.h"
#include "CPerforationLoads.h"
#include "CFracturingLoads.h"
#include "CInjectionLoads.h"
#include "CEvacuationLoads.h"
#include "CPriorToCompletionLoads.h"
#include "CTestLoads.h"
#include "CDrillingOperation.h"
#include "CDrillingDirectionSection.h"
#include "CCementingOperation.h"
#include "CFormation.h"

class CDataModel;

//##ModelId=3C0DDDAF0214
class CAnalysisPoint 
: public IComponent
{



private:


	//##ModelId=3CAAA66101C9
	CLengthQuantity m_qTMD;
	//##ModelId=3CAAA66101E8
	CProductionLoads m_ProductionLoads;
	//##ModelId=3CAAA6610217
	CPerforationLoads m_PerforationLoads;
	//##ModelId=3CAAA6610256
	CFracturingLoads m_FracturingLoads;
	//##ModelId=3CAAA6610284
	CInjectionLoads m_InjectionLoads;
	//##ModelId=3CAAA66102B3
	CEvacuationLoads m_EvacuationLoads;
	//##ModelId=3CAAA66102F2
	CPriorToCompletionLoads m_PriorToCompletionLoads;
	//##ModelId=3CAAA6610321
	
	//##ModelId=3CA0119A003F
	CDataModel* m_pDataModel;	

	//##ModelId=3CA0119B02FD
	class CAPDependency1: public CDependency
	{
		public:
	
		//##ModelId=3CA0119B035C
		CAPDependency1(CAnalysisPoint* ap);
		//##ModelId=3CA0119B036A
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const ;
		//##ModelId=3CA0119B036D
		virtual std::string GetDependencyString() const;
		
		private:
		//##ModelId=3CA0119B037B
			CAnalysisPoint* m_pPoint;
		
	};
	friend CAPDependency1;
	
	//##ModelId=3CA0119A006E
	CAPDependency1* m_pAPDependency1;

	//##ModelId=3CA0119A009C
	CSinglePressure m_PorePressAtDepth;

protected:
	

public:
	//##ModelId=3CA0119A00BB
	virtual bool CheckCompleteExtremeOper() const;
	//##ModelId=3CA0119A0149
	virtual bool CheckCompleteProdAndInject() const;
	//##ModelId=3CA0119A01D5
	virtual bool CheckCompleteCompAndTest() const;
	//##ModelId=3CA0119A0252
	virtual bool CheckComplete() const;
	
	//##ModelId=3C2043F70010
	CAnalysisPoint(const double& TMD, CDataModel* pDataModel);
	//##ModelId=3CA0119A03E7
	CAnalysisPoint(const CAnalysisPoint& rhs);
	//##ModelId=3CA0119B01B5
	virtual ~CAnalysisPoint();
	
	
	//##ModelId=3C2043F7010A
	bool operator < (const CAnalysisPoint& rhs) const;

	//From: Depths for Analysis
	//Label: Analysis Point TMD
	//Descr: TVD will be calculated automatically
	//##ModelId=3C0DDDAF02B1
	CLengthQuantity *TMD();
	const CLengthQuantity *TMD() const;
	
	//##ModelId=3C0DDDAF02B2
	CProductionLoads *ProductionLoads();
	const CProductionLoads *ProductionLoads() const;

	//##ModelId=3C0DDDAF02C0
	CPerforationLoads *PerforationLoads();
	const CPerforationLoads *PerforationLoads() const;
	
	//##ModelId=3C0DDDAF02C1
	CFracturingLoads *FracturingLoads();
	const CFracturingLoads *FracturingLoads() const;
	
	//##ModelId=3C0DDDAF02C2
	CInjectionLoads *InjectionLoads();
	const CInjectionLoads *InjectionLoads() const;
	
	//##ModelId=3C0DDDAF02C3
	CEvacuationLoads *EvacuationLoads();
	const CEvacuationLoads *EvacuationLoads() const;

	//##ModelId=3C0DDDAF02C4
	CPriorToCompletionLoads *PriorToCompletionLoads();
	const CPriorToCompletionLoads *PriorToCompletionLoads() const;

  CSinglePressure TestPressure() const;
};

#endif /* _INC_CANALYSISPOINT_3B669E030204_INCLUDED */
