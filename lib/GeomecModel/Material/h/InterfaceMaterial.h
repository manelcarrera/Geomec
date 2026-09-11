#ifndef _INTERFACEMATERIAL_H_
#define _INTERFACEMATERIAL_H_

class CModelBase;

#include "IMaterialBase.h"

class CInterfaceMaterialEntry;

class CInterfaceMaterial : public IMaterialBase
{
public:
  CInterfaceMaterial(CInterfaceMaterialEntry& entry, const QString& strName);
  CInterfaceMaterial(const CInterfaceMaterial& rhs);

  CInterfaceMaterial& operator=(const CInterfaceMaterial& rhs);
  bool operator==(const CInterfaceMaterial& rhs) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

	virtual double ParameterValue(unsigned int ValueTypeID) const;
	virtual bool IsParameter(unsigned int ValueTypeID) const;
	virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;

  double Cohesion() const;
  double Friction() const;
  double NormalStiffness() const;
  double ShearStiffness() const;
  double DFlux() const;

  bool SetCohesion(double dValue);
  bool SetFriction(double dValue);
  bool SetNormalStiffness(double dValue);
  bool SetShearStiffness(double dValue);
  bool SetDFlux(double dValue);

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
  virtual bool Empty() const;

  virtual bool Less(const CGraphNode& node) const;

  // Interface for dia::IElementProperty
  virtual int Type() const;
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitInterfaceMaterial);

private:
  double m_dCohesion;
  double m_dFriction;
  double m_dNormalStiffness;
  double m_dShearStiffness;
  double m_dDFlux;
};

typedef CStorageNode::TSTREAM TSTREAM;
typedef CStorageNode::TPROGRESS TPROGRESS;

class CInterfaceMaterialEntry : public CGraphEntryTemp<CInterfaceMaterial>
{
public:
  CInterfaceMaterialEntry(int nEntryId, CModelBase& model);

	virtual void LoadStream(CFemAppModel& model, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
  CInterfaceMaterial& InsertNew();

  ACCEPT_GEOMECMODELVISITORS(VisitInterfaceMaterialEntry);
};

#endif // _INTERFACEMATERIAL_H_
