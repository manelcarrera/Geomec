// GMFormation.h: interface for the CFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMFORMATION_H__424F3D99_4F57_457B_9F1F_9C431E04264F__INCLUDED_)
#define AFX_GMFORMATION_H__424F3D99_4F57_457B_9F1F_9C431E04264F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

#include <qstring.h>

namespace gm {
class CPressure;
class CFormation  
{
protected:
	QString				m_strName;
	long				m_lColor;
	bool				m_bReservoir;
	QString				m_strMaterial;
public:
	CFormation();
	virtual ~CFormation();

	const QString& Name() const;
	int Color() const;
	const QString& MaterialName() const;
	bool Reservoir() const;
	virtual const CPressure& Pressure(const int nDepletionStage) const = 0;
};

}

#endif // !defined(AFX_GMFORMATION_H__424F3D99_4F57_457B_9F1F_9C431E04264F__INCLUDED_)
