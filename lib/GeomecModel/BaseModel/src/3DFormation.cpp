// 3DFormation.cpp: implementation of the C3DFormation class.
//
//////////////////////////////////////////////////////////////////////

#include "FormationVolume.h"
#include "3DFormation.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DFormation::	C3DFormation(const C3DFormation& rhs)
: CFormationBase(rhs)
{
}

C3DFormation::C3DFormation(CFemAppModel &model)
: CFormationBase(model)
{
}

C3DFormation::C3DFormation(const QString &strName, CModelBase &model)
: CFormationBase(strName, model)
{
}

int C3DFormation::VolumeSize() const
{
	return ElementSetSize();
}

const CFormationVolume& C3DFormation::Volume(int nIndex) const
{
	const CFormationVolume* pVolume = dynamic_cast<const CFormationVolume*>(&ElementSet(nIndex));
	assert(pVolume);
	return *pVolume;
}

CFormationVolume& C3DFormation::Volume(int nIndex)
{
	CFormationVolume* pVolume = dynamic_cast<CFormationVolume*>(&ElementSet(nIndex));
	assert(pVolume);
	return *pVolume;
}

bool C3DFormation::Less(const CGraphNode &node) const
{
	const C3DFormation *pRhs = dynamic_cast<const C3DFormation*>(&node);
	if(pRhs)
	{
		if(!Min().Empty() && !pRhs->Min().Empty())
			return Min() < pRhs->Min();
	}
		
	return CFormationBase::Less(node); 
}

const IFormationElementSet& Casing3DFormation::ElementSet( int nIndex ) const
{
	return m_volume;
}

Casing3DFormation::Casing3DFormation(CFemAppModel &model, const geo::CBodyGroup& bodys)
	: C3DFormation(model),  m_volume(*this, const_cast<geo::CBodyGroup&>(bodys), false)
{
	Name("Casing3DFormation");
}

int Casing3DFormation::ElementSetSize() const
{
	return 1;
	//return m_size;
	//return m_volume.Volume().BodySize();
}

