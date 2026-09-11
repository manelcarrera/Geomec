// MeshParam.h: interface for the CMeshParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHPARAM_H__CFD8CB37_B986_474B_85B5_24A2BDC7C05B__INCLUDED_)
#define AFX_MESHPARAM_H__CFD8CB37_B986_474B_85B5_24A2BDC7C05B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CDataModel;

//##ModelId=3C8D9DF4031E
class CMeshParam  
{
friend CDataModel;

private:
	//##ModelId=3CAAA6550081
	CDataModel *m_pDataModel;
	//##ModelId=3C8D9DF5005F
	void UpdateData();
	//##ModelId=3C8D9DF50060
	int m_iNDivTangent;
	//##ModelId=3C8D9DF5007E
	double m_dRing1Radius;
	//##ModelId=3C8D9DF5009E
	double m_dRing2Radius;
	//##ModelId=3C8D9DF5009F
	double m_dRing3Radius;
	//##ModelId=3C8D9DF500AD
	int m_iRing1Factor;
	//##ModelId=3C8D9DF500BD
	int m_iRing2Factor;
	//##ModelId=3C8D9DF500BE
	int m_iRing3Factor;
	//##ModelId=3C8D9DF500CC
	int m_iNDivCasing;
	//##ModelId=3C8D9DF500DC
	int m_iNDivCement;

public:
	//##ModelId=3CA011840148
	void GetMeshRadiRock(std::vector<double>&Vec);
	//##ModelId=3CA0118402ED
	void GetMeshRadiCas(std::vector<double>&Vec);
	//##ModelId=3CA01185007C
	void GetMeshRadiCem(std::vector<double>&Vec);
	
	//##ModelId=3C8D9DF500DD
	double CasingExentricityShift();
	//##ModelId=3C8D9DF500DE
	int NDivTangent();
	//##ModelId=3C8D9DF500EC
	double Ring1Radius();
	//##ModelId=3C8D9DF500FB
	double Ring2Radius();
	//##ModelId=3C8D9DF500FC
	double Ring3Radius();
	//##ModelId=3C8D9DF5010B
	int Ring1Factor();
	//##ModelId=3C8D9DF5010C
	int Ring2Factor();
	//##ModelId=3C8D9DF5010D
	int Ring3Factor();
	//##ModelId=3C8D9DF5011B
	int NDivCasing();
	//##ModelId=3C8D9DF5011C
	int NDivCement();

	//##ModelId=3C8D9DF5011D
	CMeshParam(CDataModel* pDataModel);
	//##ModelId=3C8D9DF5012A
	virtual ~CMeshParam();
};

#endif // !defined(AFX_MESHPARAM_H__CFD8CB37_B986_474B_85B5_24A2BDC7C05B__INCLUDED_)
