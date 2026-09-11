#ifndef _OIDI_MESH_NODE_H__
#define _OIDI_MESH_NODE_H__

class MoMeshAnnotatedIsoline;
class MoCellFilter;
class MoMeshVector;
class MoMeshTensorVector;
class MoMeshTensor;

class SoSwitch;

class DecimatingCellFilterI;

#include <Inventor/nodes/SoGroup.h>

#include <MeshVizXLM/MbVec3.h>

class OIDIMesh;
class OIDIMeshNodeManager;

struct MeshNodeSettings;

class OIDIMeshNode : public SoGroup {
public:
  OIDIMeshNode(const OIDIMesh *meshData, OIDIMeshNodeManager *meshNodeManager);
  void computeIsovalues(std::vector<float> &isovalues, double min, double max);
  const OIDIMesh *getMesh();
  OIDIMeshNodeManager *getMeshNodeManager();

  MoMeshAnnotatedIsoline *CreateIsoline();

  // Set the id of the scalar property set used to color the mesh
  void setColorScalarSetId(int id);
  int getValueColorScalarSetId();
  virtual void updateColorScalarSet() = 0;

  void switchToGeologyColors();
  void switchToValueColors();

  // Tensorset id for beachball viz
  void setTensorSetId(int id);
  int getTensorSetId() const;

  // Set the id of the forward vector property set to use
  void setVectorSetId(int id);
  int getVectorSetId();

  void setTensorVectorSetId(int id);
  int getTensorVectorSetId() const;

  // Set the id of the scalar property set used to color the vectors
  void setVectorScalarSetId(int id);
  int getVectorScalarSetId();

  void setBeachballScalarSetIds(int primaryId, int secondaryid);

  virtual void setIsoScalarSetId(int id);
  int getIsoScalarSetId();
  virtual void updateIsoScalarSet() = 0;

  void setGeologyColor(float red, float green, float blue);
  virtual void updateGeologyColors() = 0;

  MbVec3d getMax();
  MbVec3d getMin();

  SbBox3f getBoundingBox();

  size_t getNumCells();

  virtual void UpdateDisplaySettings(const MeshNodeSettings &meshNodeSettings, bool showColor);
  virtual void setIsoValues(int isocount, double min, double max) = 0;
  virtual void setIsoGap(double gap) = 0;
  bool showIsoLines(const MeshNodeSettings &meshNodeSettings);

  bool showIsoLines();

  void setDisplayBeachBalls(bool display);

  void setDisplayVectors(bool display);

  void setDisplayTensorVectors(bool display);

  void setVectorVisibleFraction(float factor);

  void setVisibility(bool visibility);

  bool isVisible() const;
  virtual void pick(SoPickAction *action);
  virtual bool highlight() { return false; }
  virtual bool dehighlight() { return false; }

protected:
  void buildMeshNode();
  void buildCellFilterNode();
  void buildVectorsNode();
  void buildTensorsNode();

  int m_colorScalarSetId;
  int m_valueColorScalarSetId;
  int m_isoScalarSetId;
  int m_tensorSetId;

  SbColor m_geologyColor;

private:
  OIDIMeshNodeManager *m_meshNodeManager;

  const OIDIMesh *m_mesh;

  // Vector property
  SoSwitch *m_vectorsSwitch;
  SoSwitch *m_cellFilterSwitch;
  MoCellFilter *m_cellFilter;
  DecimatingCellFilterI *m_decimatingCellFilter;
  MoMeshVector *m_vectors;
  MoMeshTensorVector *m_tensorVectors;

  SoSwitch *m_tensorSwitch; // used for tensor vectors

  SoSwitch *m_beachballSwitch;
  MoMeshTensor *m_beachballs;

  bool m_visibility;
};

#endif