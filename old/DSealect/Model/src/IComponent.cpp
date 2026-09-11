// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "IComponent.h"
#include "DSealectGlobal.h"
#include "DSealectDoc.h"

//##ModelId=3C60D563006E
IComponent::IComponent()
{
	m_bComplete=true;
}



//##ModelId=3C0DDDA9031D
bool IComponent::CheckComplete()
{
	return m_bComplete;

	
}

//##ModelId=3C0DDDA9031F
CString IComponent::GetInfo()
{
	return m_strInfo;
}

CDataModel& IComponent::Model()
{
  return *GetDoc()->DSealectModel();
}

const CDataModel& IComponent::Model() const
{
  return *GetDoc()->DSealectModel();
}
