// GMFormation.cpp: implementation of the CFormation class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GMFormation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm { 

CFormation::CFormation()
: m_bReservoir(false)
{

}

CFormation::~CFormation()
{

}

const QString& CFormation::Name() const
{
	return m_strName;
}

int CFormation::Color() const
{
	return m_lColor;
}

bool CFormation::Reservoir() const
{
	return m_bReservoir;
}

const QString& CFormation::MaterialName() const
{
	return m_strMaterial;
}

}