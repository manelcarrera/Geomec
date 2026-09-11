// ResultData.h: interface for the CResultData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTDATA_H__D5CA55ED_E8FC_11D3_B3AD_00A0C941C60B__INCLUDED_)
#define AFX_RESULTDATA_H__D5CA55ED_E8FC_11D3_B3AD_00A0C941C60B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "StressStrain.h"
#include "LibraryMaterial.h"

class CResultData  
{
public:
	CResultData();
	virtual ~CResultData();
	CResultData( const CResultData &RD );
	CResultData &operator=( const CResultData &RD );

	BOOL ReadFromFile( CString strFileName, CLibraryMaterial *pMat );
	BOOL ReadFromFilos();
	CStressStrainArray const *GetStressStrainSteps() const;
	int GetNrExperiments() const {return m_nExperiments;}
  void adjustCalculatedData(int nSelected, const int* piSelected);

	bool GoSubSet( const int *aIndices, int nIndices );

public:

protected:
	BOOL ReadExperiment( FILE *fp, int iExperiment );

protected:
	CString m_strLabel;
	CStressStrainArray *m_paStressStrainStep;
	int m_nExperiments;
};

#endif // !defined(AFX_RESULTDATA_H__D5CA55ED_E8FC_11D3_B3AD_00A0C941C60B__INCLUDED_)
