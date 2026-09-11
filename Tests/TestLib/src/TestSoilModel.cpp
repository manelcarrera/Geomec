 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "TestSoilModel.h"
#include "TestIModelObjectReceiver.h"
#include "StreamVersion.h"
#include "TestMaterial.h"
#include "TestHorizon.h"
#include "TestFormation.h"

namespace test_lib
{

CSoilModel::CSoilModel(const QString& sName, QTextStream &os)
:CDocumentBase(sName), m_os(&os), m_pMaterials(0), m_pHorizons(0), m_pFormations(0)
{
	new CDocumentReceiver(*this, os);
}

void CSoilModel::createContainers()
{
	m_pMaterials = new CMaterialContainer("Materials", *m_os);
	m_pMaterials->create( this);
	m_pHorizons = new CHorizonContainer("Horizons", *m_os);
	m_pHorizons->create( this );
	m_pFormations = new CFormationContainer("Formations", *m_os);
	m_pFormations->create( this );
}

CSoilModel::~CSoilModel()
{
}

CStreamVersion CSoilModel::documentVersion() const
{
	return CStreamVersion(0, 0, 1);
}

QString CSoilModel::documentType() const
{
	return "SoilModel";
}



}
