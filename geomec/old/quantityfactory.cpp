// importquantityfactory.cpp: implementation of the CImportQuantityFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "quantityfactory.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuantityFactory::CQuantityFactory(CGraphModel& model)
: m_model(model)
{
	// Stresses
	AppendBuilder(MD_QUANTITY_STRESS, IDS_QUANTITY_STRESS_TENSOR, *(new CQuantityBuilderTemp<TStressTensor>));
	AppendBuilder(MD_QUANTITY_STRESS, IDS_QUANTITY_MINIMAL_PRINCIPAL_STRESS, *(new CQuantityBuilderTemp<TMinimalPrincipalStress>));
	AppendBuilder(MD_QUANTITY_STRESS, IDS_QUANTITY_MAXIMAL_PRINCIPAL_STRESS, *(new CQuantityBuilderTemp<TMaximalPrincipalStress>));
	AppendBuilder(MD_QUANTITY_STRESS, IDS_QUANTITY_VERTICAL_STRESS, *(new CQuantityBuilderTemp<TVerticalStress>));

	// Materials
	AppendBuilder(MD_QUANTITY_YOUNGS_MODULUS, IDS_QUANTITY_YOUNGS_MODULUS, *(new CQuantityBuilderTemp<TYoungsModulus>));
	AppendBuilder(MD_QUANTITY_POISSONS_RATIO, IDS_QUANTITY_POISSONS_RATIO, *(new CQuantityBuilderTemp<TPoissonsRatio>));
	AppendBuilder(MD_QUANTITY_COHESION, IDS_QUANTITY_COHESION, *(new CQuantityBuilderTemp<TCohesion>));
	AppendBuilder(MD_QUANTITY_FRICTION_ANGLE, IDS_QUANTITY_FRICTION_ANGLE, *(new CQuantityBuilderTemp<TFrictionAngle>));
	AppendBuilder(MD_QUANTITY_POROSITY, IDS_QUANTITY_POROSITY, *(new CQuantityBuilderTemp<TPorosity>));
	AppendBuilder(MD_QUANTITY_BULK_DENSITY, IDS_QUANTITY_BULK_DENSITY, *(new CQuantityBuilderTemp<TBulkDensity>));
	AppendBuilder(MD_QUANTITY_COMPRESSIONAL_VELOCITY, IDS_QUANTITY_COMPRESSIONAL_VELOCITY, *(new CQuantityBuilderTemp<TCompressionalVelocity>));
	AppendBuilder(MD_QUANTITY_SHEAR_VELOCITY, IDS_QUANTITY_SHEAR_VELOCITY, *(new CQuantityBuilderTemp<TShearVelocity>));
	AppendBuilder(MD_QUANTITY_PERMEABILITY, IDS_QUANTITY_VERTICAL_PERMEABILITY, *(new CQuantityBuilderTemp<TVerticalPermeability>));
	AppendBuilder(MD_QUANTITY_PERMEABILITY, IDS_QUANTITY_HORIZONTAL_PERMEABILITY, *(new CQuantityBuilderTemp<THorizontalPermeability>));
	
	// Pressure
	AppendBuilder(MD_QUANTITY_PRESSURE, IDS_QUANTITY_PRESSURE, *(new CQuantityBuilderTemp<TPressure>));


}

CQuantityFactory::~CQuantityFactory()
{
	// Kill map ...
	for(TQuantityMap::iterator it = m_mpQuantity.begin(); it != m_mpQuantity.end(); it++)
		delete it->second;
}

void CQuantityFactory::AppendBuilder(UINT uQuantityEntryID, UINT uQuantityID, CQuantityBuilder& builder)
{
	m_mpQuantity.insert(TQuantityMap::value_type(uQuantityID, &builder));	
	m_mpQuantityEntry.insert(TQuantityEntryMap::value_type(uQuantityID, uQuantityEntryID));
}

BOOL CQuantityFactory::QuantityAvailable(UINT uQuantityID) const
{
	if(m_mpQuantity.find(uQuantityID) != m_mpQuantity.end())
		return m_model.GraphEntry(m_mpQuantityEntry.find(uQuantityID)->second) != 0;
	
	return FALSE;
}
	
CDistributedQuantity* CQuantityFactory::BuildQuantity(CPointSet &point_set, UINT uQuantityID)
{
	if(QuantityAvailable(uQuantityID))
	{
		CQuantityBuilder *pBuilder = m_mpQuantity.find(uQuantityID)->second;
		return pBuilder->Build(point_set);
	}

	return 0;
}