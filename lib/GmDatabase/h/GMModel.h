// GMModel.h: interface for the CModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMMODEL_H__1F2F8370_2F4F_4D55_A6B9_2951DEBAD328__INCLUDED_)
#define AFX_GMMODEL_H__1F2F8370_2F4F_4D55_A6B9_2951DEBAD328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

namespace gm {
class CTimestep;
class CModel  
{
protected:
	double	m_dTotalVerticalStressGradient;
	double	m_dWaterColumnStressGradient;
	double	m_dVerticalStressExponent;
	QString m_strProjectInfo;
	bool m_bTimeAnalysis;
	virtual void OnNewTable(CString sTableName);
public:
	CModel();
	virtual ~CModel();
	const double& VerticalStressExponent() const;
	const double& TotalVerticalStressGradient() const;
	const double& WaterColumnStressGradient() const;
	const QString& ProjectInfo() const;
	bool TimeAnalysis() const;
	virtual int TimestepSize() const = 0;
	virtual const CTimestep& Timestep(int nIndex) const = 0;
};

}

#endif // !defined(AFX_GMMODEL_H__1F2F8370_2F4F_4D55_A6B9_2951DEBAD328__INCLUDED_)
