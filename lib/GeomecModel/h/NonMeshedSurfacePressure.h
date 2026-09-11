#ifndef _NONMESHEDSURFACEPRESSURE_H_
#define _NONMESHEDSURFACEPRESSURE_H_

#include "Result.h"
#include "IValueDataInterface.h"
#include "GeomecModelVisitor.h"

class CNonMeshedSurfacePressure;
class CNonMeshedSurface;
class TPressure;

class CNonMeshedSurfacePressureComponent : public IValueComponentBase
{
public:
  typedef IValueDataInterfaceScalarTempl<CNonMeshedSurfacePressureComponent> IValueDataInterfaceNonSurfPressScalar;

  CNonMeshedSurfacePressureComponent(const QString& sName, CNonMeshedSurfacePressure& pressure);
  CNonMeshedSurfacePressureComponent(const CNonMeshedSurfacePressureComponent& rhs);

  virtual ~CNonMeshedSurfacePressureComponent();

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();

  const CNonMeshedSurfacePressure& Pressure() const;
  CNonMeshedSurfacePressure& Pressure();

  QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

	virtual bool Empty() const;
	virtual long SavedItems() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

	virtual bool CanMap(const COpenGLNode& node) const;

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool Defined() const;
	virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const;

  ACCEPT_GEOMECMODELVISITORS(VisitNonMeshedSurfacePressureComponent);

protected:
  IValueDataInterfaceNonSurfPressScalar m_Data;

private:
  CNonMeshedSurfacePressure& m_pressure;
};

class CNonMeshedSurfacePressure : public IValueComposite
{
public:
  CNonMeshedSurfacePressure(CNonMeshedSurface& surface);
  CNonMeshedSurfacePressure(const CNonMeshedSurfacePressure& rhs);

  CNonMeshedSurfacePressure& operator=(const CNonMeshedSurfacePressure& rhs);
  bool operator==(const CNonMeshedSurfacePressure& rhs) const;

	// distributed values
	size_t DistributedSize() const;
  const TPressure& DistributedValue(size_t nIndex) const;

	// usage of distributed values outside their convex hull
	bool DistributedOnly() const;
	void DistributedOnly(bool bDistributedOnly);

  // manual values, or from formation pressure
  bool ManualValues() const;
  void ManualValues(bool bManual);

  // get/set manual values
  double ReferencePressure() const;
  double ReferenceDepth() const;
  double Gradient() const;

  void ReferencePressure(double dPressure);
  void ReferenceDepth(double dDepth);
  void Gradient(double dGradient);

	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourDeleted(const CGraphNode &node);
	virtual bool CanConnectItem(const CGraphNode &item) const;

	// Save and load
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  const CNonMeshedSurface& Surface() const;
  CNonMeshedSurface& Surface();

	virtual QString ExportLabel(int nComponent) const;
	virtual bool CanMap(const COpenGLNode& node, int nRegister) const;

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

  ACCEPT_GEOMECMODELVISITORS(VisitNonMeshedSurfacePressure);

private:
  CNonMeshedSurface& m_surface;
  std::vector<const TPressure*> m_vcDistributed;
  bool m_bDistributedOnly;
  bool m_bManual;
  double m_dRefPressure;
  double m_dRefDepth;
  double m_dGradient;
};

#endif // _NONMESHEDSURFACEPRESSURE_H_
