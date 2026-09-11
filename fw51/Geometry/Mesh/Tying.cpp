 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Tying.cpp: implementation of the CTying class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cassert>
#include "IMesh.h"  
#include "INode.h"  
#include "Tying.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo { 

CTying::CTying(IMesh& mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType,
	int nMaster, const IVector &vecMasterDir, TType nMasterType, double dFactor)
: m_mesh(mesh), m_nSlaveNode(nSlave), m_pSlaveDirection(0), m_SlaveType(nSlaveType)
{
	m_pSlaveDirection = new CVector(vecSlaveDir);
	m_vcMaster.push_back(std::make_pair(new CNodeDef(nMaster, vecMasterDir, nMasterType), dFactor));
}

CTying::CTying(IMesh& mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType,
	int nMaster0, const IVector &vecMaster0Dir, TType nMaster0Type, double dFactor0,
	int nMaster1, const IVector &vecMaster1Dir, TType nMaster1Type, double dFactor1)
: m_mesh(mesh), m_nSlaveNode(nSlave), m_pSlaveDirection(0), m_SlaveType(nSlaveType)
{
	m_pSlaveDirection = new CVector(vecSlaveDir);

	m_vcMaster.push_back(std::make_pair(new CNodeDef(nMaster0, vecMaster0Dir, nMaster0Type), dFactor0));
	m_vcMaster.push_back(std::make_pair(new CNodeDef(nMaster1, vecMaster1Dir, nMaster1Type), dFactor1));
}

	// any number of masters (must be greater than 0 of course...)
CTying::CTying(IMesh &mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType,
	const std::vector<TMasterDef> &vcMasters)
: m_mesh(mesh), m_nSlaveNode(nSlave), m_pSlaveDirection(0), m_SlaveType(nSlaveType)
{
	assert(vcMasters.size() > 0);

	m_pSlaveDirection = new CVector(vecSlaveDir);

	for(std::vector<TMasterDef>::const_iterator it = vcMasters.begin(); it != vcMasters.end(); it++)
		m_vcMaster.push_back(std::make_pair(new CNodeDef(it->node, *it->direction, it->type), it->factor));
}

CTying::~CTying()
{
	for(int i = 0; i < m_vcMaster.size(); i++) delete m_vcMaster[i].first;
}

const IMesh &CTying::Mesh() const
{
	return m_mesh;
}

int CTying::SlaveIndex() const
{
	return m_nSlaveNode;
}

const IVector &CTying::SlaveDirection() const
{
	assert(m_pSlaveDirection != 0);
	return *m_pSlaveDirection;
}

int CTying::MasterSize() const
{
	 return (int)m_vcMaster.size();
}

const INode &CTying::Master(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcMaster.size());
	return Mesh().Node(m_vcMaster[nIndex].first->Index());
}

int CTying::MasterIndex(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcMaster.size());
	return m_vcMaster[nIndex].first->Index();
}

const IVector &CTying::MasterDirection(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcMaster.size());
	return m_vcMaster[nIndex].first->Direction();
}

CTying::TType CTying::MasterType(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcMaster.size());
  return m_vcMaster[nIndex].first->Type();
}

std::string CTying::MasterTypeString(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcMaster.size());
	return m_vcMaster[nIndex].first->TypeName();
}

const double &CTying::MasterFactor(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcMaster.size());
	return m_vcMaster[nIndex].second;
}

CTying::TType CTying::SlaveType() const
{
  return m_SlaveType;
}

std::string CTying::SlaveTypeString() const
{
	return CNodeDef::Type2TypeName(m_SlaveType);
}

}
