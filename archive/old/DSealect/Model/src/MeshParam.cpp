// MeshParam.cpp: implementation of the CMeshParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDataModel.h"
#include "MeshParam.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C8D9DF5011D
CMeshParam::CMeshParam(CDataModel* pDataModel)
{
	m_pDataModel=pDataModel;
}

//##ModelId=3C8D9DF5012A
CMeshParam::~CMeshParam()
{

}

//##ModelId=3C8D9DF500DE
int CMeshParam::NDivTangent()
{
	return m_iNDivTangent;
}

//##ModelId=3C8D9DF500EC
double CMeshParam::Ring1Radius()
{
	return m_dRing1Radius;
}

//##ModelId=3C8D9DF500FB
double CMeshParam::Ring2Radius()
{
	return m_dRing2Radius;
}

//##ModelId=3C8D9DF500FC
double CMeshParam::Ring3Radius()
{
	return m_dRing3Radius;
}

//##ModelId=3C8D9DF5010B
int CMeshParam::Ring1Factor()
{
	return m_iRing1Factor;
}

//##ModelId=3C8D9DF5010C
int CMeshParam::Ring2Factor()
{
	return m_iRing2Factor;
}

//##ModelId=3C8D9DF5010D
int CMeshParam::Ring3Factor()
{
	return m_iRing3Factor;
}

//##ModelId=3C8D9DF5011B
int CMeshParam::NDivCasing()
{
	return m_iNDivCasing;
}

//##ModelId=3C8D9DF5011C
int CMeshParam::NDivCement()
{
	return m_iNDivCement;
}

//##ModelId=3C8D9DF5005F
void CMeshParam::UpdateData()
{
	double Ri = m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->BoreholeDiameter()->Value()/2.0;			

	switch( m_pDataModel->Control()->RockRefinement() )
	{
		case 1:
			m_iNDivTangent=12;
			m_dRing1Radius=1.91*Ri;
			m_dRing2Radius=7.65*Ri;
			m_dRing3Radius=50.0*Ri;
			m_iRing1Factor=406;
			m_iRing2Factor=306;
			m_iRing3Factor=406;
			break;
		case 2:
			m_iNDivTangent=20;
			m_dRing1Radius=1.72*Ri;
			m_dRing2Radius=7.79*Ri;
			m_dRing3Radius=50.0*Ri;
			m_iRing1Factor=408;
			m_iRing2Factor=510;
			m_iRing3Factor=508;
			break;
		case 3:
			m_iNDivTangent=26;
			m_dRing1Radius=1.70*Ri;
			m_dRing2Radius=9.12*Ri;
			m_dRing3Radius=50.0*Ri;
			m_iRing1Factor=410;
			m_iRing2Factor=714;
			m_iRing3Factor=510;
			break;
		default:
			ASSERT(FALSE);
	}


	m_iNDivCasing = 2 * m_pDataModel->Control()->CasingRefinement();
	m_iNDivCement =	2 * m_pDataModel->Control()->CementRefinement();
	/*
	switch( m_pDataModel->Control()->CasingRefinement() )
	{
		case 2:
			m_iNDivCasing=4;
			break;
		case 3:
			m_iNDivCasing=6;
			break;
		case 4:
			m_iNDivCasing=8;
			break;
		default:
			ASSERT(FALSE);
	}


	switch( m_pDataModel->Control()->CementRefinement() )
	{
		case 3:
			m_iNDivCement=6;
			break;
		case 4:
			m_iNDivCement=8;
			break;
		case 5:
			m_iNDivCement=10;
			break;
		case 6:
			m_iNDivCement=12;
			break;
		default:
			ASSERT(FALSE);
	}
	*/

}

//##ModelId=3C8D9DF500DD
double CMeshParam::CasingExentricityShift()
{
	CDrillingOperation* pDO = m_pDataModel->DrillingScheme()->GetDrillingOperation(0);
	CString Name = pDO->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM= m_pDataModel->CasingMatList()->GetMaterial(Name);

	double dBorehole = pDO->BoreholeDiameter()->Value();
	double dCasingOuter = pCM->CasingOuterDiameter()->Value();
	double dCasingInner = pCM->CasingInnerDiameter()->Value();
	double dPercEccent = pDO->CasingOperation()->PercentageEccentricity()->Value();

	double dEccentrShift = ((dBorehole - dCasingOuter) / 2.0 * dPercEccent); //formula on page 11/59
	return dEccentrShift;
	
}



//##ModelId=3CA011840148
void CMeshParam::GetMeshRadiRock(std::vector<double>&Vec)
{
	CDrillingOperation* pDO = m_pDataModel->DrillingScheme()->GetDrillingOperation(0);
	
	double dBH = pDO->BoreholeDiameter()->Value() / 2.0;
	
	Vec.clear();
	switch( m_pDataModel->Control()->RockRefinement() )
	{
		case 1:
			Vec.push_back(1.0 * dBH);
			Vec.push_back(1.12 * dBH);
			Vec.push_back(1.42 * dBH);
			Vec.push_back(1.91 * dBH);
			Vec.push_back(2.87 * dBH);
			Vec.push_back(4.78 * dBH);
			Vec.push_back(7.65 * dBH);
			Vec.push_back(13.3 * dBH);
			Vec.push_back(27.41 * dBH);
			Vec.push_back(50.0 * dBH);
			break;

		case 2:
			Vec.push_back(1.0 * dBH);
			Vec.push_back(1.07 * dBH);
			Vec.push_back(1.22 * dBH);
			Vec.push_back(1.43 * dBH);
			Vec.push_back(1.72 * dBH);
			Vec.push_back(2.12 * dBH);
			Vec.push_back(2.93 * dBH);
			Vec.push_back(4.15 * dBH);
			Vec.push_back(5.77 * dBH);
			Vec.push_back(7.79 * dBH);
			Vec.push_back(11.31 * dBH);
			Vec.push_back(19.52 * dBH);
			Vec.push_back(32.41 * dBH);
			Vec.push_back(50.0 * dBH);
			break;

		case 3:
			Vec.push_back(1.0 * dBH);
			Vec.push_back(1.06 * dBH);
			Vec.push_back(1.15 * dBH);
			Vec.push_back(1.29 * dBH);
			Vec.push_back(1.48 * dBH);
			Vec.push_back(1.70 * dBH);
			Vec.push_back(1.97 * dBH);
			Vec.push_back(2.5 * dBH);
			Vec.push_back(3.29 * dBH);
			Vec.push_back(4.35 * dBH);
			Vec.push_back(5.68 * dBH);
			Vec.push_back(7.27 * dBH);
			Vec.push_back(9.12 * dBH);
			Vec.push_back(11.85 * dBH);
			Vec.push_back(17.3 * dBH);
			Vec.push_back(25.47 * dBH);
			Vec.push_back(36.37 * dBH);
			Vec.push_back(50.0 * dBH);
			break;

	}

}

//##ModelId=3CA0118402ED
void CMeshParam::GetMeshRadiCas(std::vector<double>&Vec)
{
	CDrillingOperation* pDO = m_pDataModel->DrillingScheme()->GetDrillingOperation(0);
	CString Name = pDO->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM = m_pDataModel->CasingMatList()->GetMaterial(Name);
	
	double dCasingOuter = pCM->CasingOuterDiameter()->Value() / 2.0;
	double dCasingInner = pCM->CasingInnerDiameter()->Value() / 2.0;
	double Delta = (dCasingOuter-dCasingInner) / (m_iNDivCasing / 2);

	Vec.clear();

	for(int i = 0 ; i <= m_iNDivCasing / 2; i++)
	{
		Vec.push_back(dCasingInner + double(i) * Delta);
	}

}

//##ModelId=3CA01185007C
void CMeshParam::GetMeshRadiCem(std::vector<double>&Vec)
{
	CDrillingOperation* pDO = m_pDataModel->DrillingScheme()->GetDrillingOperation(0);
	CString Name = pDO->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM= m_pDataModel->CasingMatList()->GetMaterial(Name);
	
	double dBH = pDO->BoreholeDiameter()->Value() / 2.0;
	double dCasingOuter = pCM->CasingOuterDiameter()->Value() / 2.0;
	
	double Delta = (dBH-dCasingOuter) / (m_iNDivCement / 2);

	Vec.clear();

	for(int i = 0 ; i <= m_iNDivCement / 2; i++)
	{
		Vec.push_back(dCasingOuter + double(i) * Delta);
	}
}
