#ifndef _IMATERIALBASE_H_
#define _IMATERIALBASE_H_

class IMaterialServerBase;
class CFFMaterial;

namespace dia {
  class IDianaRunner;
}

#include "StorageNode.h"
#include "GeomecModelVisitor.h"

class IMaterialBase: public CStorageNode
{
public:
  IMaterialBase(const QString& sName, CFemAppModel& model);
  IMaterialBase(const IMaterialBase& rhs);
  virtual ~IMaterialBase();

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  virtual bool ReadOnly() const;
  virtual bool FixedMaterialModel() const;

  // return the parameter value with the given ID, no support for distributed values!
  virtual double ParameterValue(unsigned int ValueTypeID) const = 0;

  // is the given id a valid parameter?
  virtual bool IsParameter(unsigned int ValueTypeID) const = 0;

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const = 0;

  virtual bool Less(const CGraphNode& node) const;

  // Interface for dia::IElementProperty
  virtual int Type() const = 0;
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const = 0;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const = 0;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIMaterialBase);

protected:
  typedef std::set<const IMaterialServerBase*> TServerSet;
  const TServerSet& Servers() const;

private:
  TServerSet m_stServers;
};

#endif // _IMATERIALBASE_H_
