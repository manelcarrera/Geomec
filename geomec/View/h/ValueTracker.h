#ifndef _VALUE_TRACKER_H__
#define _VALUE_TRACKER_H__

#include <Inventor/nodes/SoSeparator.h>
#include <MeshVizXLM/mapping/nodes/MoMeshPointProbe.h>
#include <Inventor/SbVec.h>
#include "MeshVizXLM/mesh/data/MiDataSetI.h"

class SoSwitch;
class SoJackDragger;
class MoMeshPointProbe;
class GMoMeshPointProbe;
class ValueTextScene;
class OIDIMeshNodeManager;
class CrossSectionManipulator;
class SoCoordinate3;
class SoMaterial;
class SoMarkerSet;
class SoFaceSet;
class SoVertexProperty;
class OIDIMeshVU;
class OIDIMeshSU;
class OIDIPointSetMesh;
class OIDIMesh;
class SoFieldSensor;
class MiVolumeMeshUnstructured;
class MiMeshUnstructuredI;
class MiCell;


class ValueTracker : public SoSeparator
{
public:
  ValueTracker (OIDIMeshNodeManager * meshNodeManager, CrossSectionManipulator * dragger);
  void Display(bool mode);
  bool IsDisplayed();

  virtual void  motionCallback(GMoMeshPointProbe * probe, const MiVolumeMeshUnstructured * mesh, const std::string & meshName, size_t cellId, const MeXScalardSetI& scalars, const MeXVec3dSetI& vectors);

  void setTextSceneValues( size_t nodeIndex, int scalarSetId, int vectorSetId, int tensorSetId, size_t cellId );

  void updatePropertyName();

  double computeDistanceClosestNode( const MiMeshUnstructuredI * mesh, const MbVec3d & coordinates, size_t cellId, size_t & cellNodeId );
  void invalidateCellMarkers();
  void computeCellMarkerSet( const MiMeshUnstructuredI * mesh, const MbVec3d & coordinates, size_t cellId, size_t &meshNodeIndexClosest, MbVec3d &meshNodeCoordinatesClosest );
  void updateValueTracker( const MiMeshUnstructuredI * mesh, const std::string & meshName, const MbVec3d & coordinates, size_t cellId, int scalarSetId, int vectorSetId, int tensorSetId );

  void GetCellNodeWeights( const MiCell * cell, const MiMeshUnstructuredI * mesh, const MbVec3d & coordinates, std::vector<double> & weights );

  void computeCellFacetSet( const MiVolumeMeshUnstructured * mesh, size_t cellId );
  void computeCellFacetSet( const OIDIMeshSU * mesh, size_t cellId );
  void computeCellFacetSet( const OIDIPointSetMesh * mesh, size_t cellId );
  void clearCellFacetSet();

  CrossSectionManipulator * getDragger();

  void SetTextColor( float * rgb );
  void GetTextColor( float * rgb );

  void UpdateDisplay();
  void normalSensorCallback( SoSensor* sensor );
  void updateTensorText( size_t tensorIndex );
  bool TrackElement();
  void TrackElement(bool track);
  void showDragger(bool show);

private:
  OIDIMeshNodeManager *      m_meshNodeManager;
  SoSwitch*                  m_switch;
  SoSwitch*                  m_draggerSwitch;
  CrossSectionManipulator *  m_dragger;
  ValueTextScene *           m_draggerTextScene;
  bool                       m_firstTime;

  SoSwitch * m_cellMarkerSwitch;
  SoSwitch * m_validCellMarkerSwitch;
  size_t                m_numCellNodes;
  SoVertexProperty *    m_vertexProperty;
  SoMaterial *          m_cellMarkerMaterial;
  SoMarkerSet *         m_cellMarkerSet;
  SoCoordinate3 *       m_cellFacetSetCoordinates;
  SoFaceSet *           m_cellFacetSet;
  bool                  m_volumeCellFacetSetInitialized;
  bool                  m_surfaceCellFacetSetInitialized;

  SoFieldSensor * m_normalSensor;

};
#endif