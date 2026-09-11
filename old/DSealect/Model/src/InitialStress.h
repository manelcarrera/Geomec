// InitialStress.h: interface for the CInitialStress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITIALSTRESS_H__69CDB5B7_7B16_47FC_8CFB_861218067EC7__INCLUDED_)
#define AFX_INITIALSTRESS_H__69CDB5B7_7B16_47FC_8CFB_861218067EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBatchTableRow;
class CDataModel;

#include "mfTensor.h"

//##ModelId=3C8D9DF50178
class CInitialStress  
{
private:
	
	//##ModelId=3CAAA655013C
	CTensor m_LocalEffectiveStress;

	//##ModelId=3CAAA655016B
	CDataModel     *m_pDatamodel;
	//##ModelId=3CAAA655019A
	CBatchTableRow *m_pBatchTableRow;
public:
	//##ModelId=3C8D9DF50206
	void CalculateStress();
	//##ModelId=3C8D9DF50207
	double EffStressXX() const;
	//##ModelId=3C8D9DF50215
	double EffStressYY() const;
	//##ModelId=3C8D9DF50217
	double EffStressZZ() const;
	//##ModelId=3C8D9DF50219
	double EffStressXY() const;
	//##ModelId=3C8D9DF50224
	double EffStressYZ() const;
	//##ModelId=3C8D9DF50226
	double EffStressZX() const;

	//##ModelId=3C8D9DF50243
	double PorePressure() const;

	//##ModelId=3DDA0C430023
	double UndrainedPorePressure() const;
	//##ModelId=3C8D9DF50254
	double DrillingMudPressure() const;
	//##ModelId=3C8D9DF50263
	double CementSlurryPressure() const;

	//##ModelId=3C8D9DF50265
	double CasingAxialStress() const;
	//##ModelId=3C8D9DF50267
	double WaitingOnCementFluidPressure() const;


	//##ModelId=3C8D9DF50269
	CInitialStress(CDataModel* pDatamodel,CBatchTableRow *pBatchTableRow);
	//##ModelId=3C8D9DF50274
	virtual ~CInitialStress();

};

#endif // !defined(AFX_INITIALSTRESS_H__69CDB5B7_7B16_47FC_8CFB_861218067EC7__INCLUDED_)
