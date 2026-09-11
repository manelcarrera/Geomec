// GMModel.cpp: implementation of the CModel class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GMModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm { 

CModel::CModel()
{

}

CModel::~CModel()
{

}

bool CModel::TimeAnalysis() const
{
	return m_bTimeAnalysis;
}

const QString& CModel::ProjectInfo() const 
{
	return m_strProjectInfo;
}

const double& CModel::WaterColumnStressGradient() const
{
	return m_dWaterColumnStressGradient;
}

const double& CModel::TotalVerticalStressGradient() const
{
	return m_dTotalVerticalStressGradient;
}

const double& CModel::VerticalStressExponent() const
{
	return m_dVerticalStressExponent;
}

void CModel::OnNewTable(CString sTableName)
{
}

}