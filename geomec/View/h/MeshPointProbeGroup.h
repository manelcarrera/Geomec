#pragma once
#include "Inventor\nodes\SoSeparator.h"
#include "Inventor\nodes\SoSwitch.h"
#include "MeshVizXLM\mapping\nodes\MoMeshPointProbe.h"

class MiVolumeMeshUnstructured;
class ValueTracker;

class GMoMeshPointProbe : public MoMeshPointProbe {
public:
  GMoMeshPointProbe() : MoMeshPointProbe(), m_tensorSetId(0) {}

  virtual void doAction(SoAction *action);

  void setTensorSetId(int id);

  int getTensorSetId();

private:
  int m_tensorSetId;
};

class MeshPointProbe : public SoSeparator, public MoMeshPointProbe::MoProbeCallback {
public:
  MeshPointProbe(const std::string &meshName, const MiVolumeMeshUnstructured *miVolumeMesh, ValueTracker *valueTracker);
  const MiVolumeMeshUnstructured *getMesh();

  virtual void motionCallback(size_t cellId, const MeXScalardSetI &scalars, const MeXVec3dSetI &vectors);

  bool isDefault();

  void setScalarSetId(int scalarSetId);
  void setVectorSetId(int vectrSetId);
  void setDefault();
  void setTensorSetId(int tensorSetId);

private:
  const std::string m_meshName;
  const MiVolumeMeshUnstructured *m_miVolumeMesh;
  GMoMeshPointProbe *m_meshPointProbe;
  ValueTracker *m_valueTracker;
  bool m_default;
};

class MeshPointProbeGroup : public SoSwitch {
public:
  MeshPointProbeGroup();
  void addProbe(const std::string &meshName, const MiVolumeMeshUnstructured *miVolumeMesh, ValueTracker *valueTracker);
  void removeProbe(const MiVolumeMeshUnstructured *miVolumeMesh);
  void enable(bool state);
  void setScalarSetId(const MiVolumeMeshUnstructured *miVolumeMesh, int scalarSetId);
  void setVectorSetId(const MiVolumeMeshUnstructured *miVolumeMesh, int vectorSetId);
  void setTensorSetId(const MiVolumeMeshUnstructured *miVolumeMesh, int tensorSetId);

private:
};