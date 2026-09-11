#ifndef MOTENSORSET_H_INCLUDED
#define MOTENSORSET_H_INCLUDED

#include <MeshVizXLM/MbVec3.h>
#include <MeshVizXLM/mapping/nodes/MoActionNode.h>
#include <MeshVizXLM/mesh/data/MiDataSetI.h>

typedef MiDataSetI<CTensor> MiTensorSetI;

/**
 * Property node that stores a tensor set for an unstructured mesh
 */
class MoTensorSet : public MoActionNode {
  SO_NODE_HEADER(MoTensorSet);

  const MiTensorSetI *m_tensorSet;
  virtual void doAction(SoAction *action);

protected:
  virtual void GLRender(SoGLRenderAction *action);

public:
  static void initClass();
  static void exitClass();

  MoTensorSet();

  virtual void setTensorSet(const MiTensorSetI *tensorSet);

  virtual const MiTensorSetI *getTensorSet();

  virtual void reset();
};

#endif
