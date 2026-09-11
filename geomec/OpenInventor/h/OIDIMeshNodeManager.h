#ifndef _OIDI_MESH_NODE_MANAGER_H__
#define _OIDI_MESH_NODE_MANAGER_H__

#include "OpenInventorDataInterface.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>

#include <map>
#include <memory>

#include "IVectorResult.h"
#include "ITensorGroup.h"
#include "OIDIDataSetI.h"
#include "MeshMode.h"
#include "MeshPointProbeGroup.h"

#include "Deformation.h"

class COpenInventorEventsHandler;

class SoSwitch;
class CrossSectionManipulator;
class MoMeshPlaneSlice;
class MoLegend;

class CNewWellPath;
class CCrossSection;

class OIDISurfaceMeshNode;
class OIDIVolumeMeshNode;
class OIDIMeshNode;
class OIDIScalarSetI;

struct MeshNodeSettings;

class MeshPointProbeGroup;

class CrossSectionGroup;
class MeshCrossSection;

class SoDragger;
class SoLightModel;
class SoScale;

class COpenInventorEventsHandler;
class ValueTracker;
class LegendScene;

class MoScalarSetI;
class MoVec3SetI;
class MoTensorSetI;
class IColorScaleNode;
class IResultComponent;
class MoLinearColorMapping;
class MoLevelColorMapping;
class MoColorMapping;
class MoMeshPointProbe;
class SoFont;
class SoMaterial;

class MoDrawStyle;
class MoMaterial;

template<class T>
class MiDataSetI;

class OIDISet;

class geo::IElementSet;

class OIDIMeshNodeManager : public SoSeparator
{
public:
  OIDIMeshNodeManager(COpenInventorEventsHandler * eventsHandler);
  virtual ~OIDIMeshNodeManager();

  void EnterView();
  void LeaveView();

  static MoColorMapping * GetTensorVectorColorMapping();

  void usePhongLightModel( bool mode );
  bool usingPhongLightModel() const;

  void OnShowCrossSection(const CCrossSection& xsec);

  void UpdateCrossSectionManip( const CCrossSection &xsec );

  void OnHideCrossSection(const CCrossSection& xsec);
  void OnCrossSectionChanged(const CCrossSection& xsec);

  void OnShowWellPath(const CNewWellPath& wellPath);
  void OnHideWellPath(const CNewWellPath& wellPath);
  void OnWellPathChanged(const CNewWellPath& wellPath);
private:
  typedef std::map<const CNewWellPath*, SoSwitch*> TNewWellPathMap;

  void OnWellPathChanged(TNewWellPathMap::iterator iter);
public:

  SbBox3f get_xsec_manipulator_casing_mesh_bounding_box();
  SbBox3f GetAllMeshesBoundingBox();
  SbBox3f GetDisplayedVolumesBoundingBox();
  SbBox3f GetMeshBoundingBox(const OIDIMesh * mesh);
  SbBox3f GetGroupBoundingBox(SoGroup * group);

  size_t GetNumVolumeMeshCells();
  size_t GetNumDisplayedVolumeCells();
  bool DisplayedVolumesContains( const SbVec3f & point );
  SbVec3f getDisplayedVolumesPoint();

  SoScale * getScale();

private:
  OIDIMeshNode * getMainMeshGroupMeshNode() const;

public:
  const MiDataSetI<double> * getFirstScalarDataSet();
  const MiDataSetI<double> * getScalarDataSet(int index);
  const MiDataSetI<double> * getLastScalarDataSet();
  const MiDataSetI<MbVec3d> * getVectorDataSet(int index);
  const MiDataSetI<CTensor> * getTensorDataSet(int index);

  SoGroup * getActiveScalarSetGroup();

  OIDIMeshNode * FindMeshNode(SoGroup * group, const OIDIMesh * mesh);
  OIDIMeshNode * FindMeshNode (const OIDIMesh *mesh, bool &displayed);

  int addScalarDataSet(MiDataSetI<double>* dataSet);
  int addVectorDataSet(MiDataSetI<MbVec3d>* dataSet);
    int addTensorDataSet(MiDataSetI<CTensor>* dataSet);

public:
  void RefreshTensorBaseScale();
  void RefreshVectorBaseScale();
  bool CanRefreshVectorBaseScale();

  void SetVectorBaseScale( float scale );
  void SetBeachBallBaseScale( float scale );

  void UpdateColorMapping();

  void OnValueComponentChanged(const IValueComponentBase *pValueComponent, CDoubleQuantity::UNIT unit, bool settingsChanged = false);
  void OnIsoValueComponentChanged(const IValueComponentBase *pValueComponent, CDoubleQuantity::UNIT unit);

  void OnColorScaleChanged();

  void UpdateValueTrackerDisplay();

  void OnNewFormationNode(const CFormationBase& node);
  void OnFormationNodeModified(const CFormationBase& node, ModifiedHint hint);
  void OnFormationNodeDeleted(const CFormationBase& node);

  void OnNewOpenGLNode(const COpenGLNode& node);
  void OnOpenGLNodeModified(const COpenGLNode& node, enum ModifiedHint uHint = Default);
  void OnOpenGLNodeDeleted(const COpenGLNode& node);
  
  OIDIMeshNode* FindOrCreateMeshNode( const OIDIMesh * mesh );

  COpenGLNode * getOpenGLNode (const OIDIMesh * mesh);

  bool IsMainMesh( const COpenGLNode& node );

  void SetTranslation(bool on);
  const SbVec3d& GetTranslation();

  enum AddedOrDeleted
  {
    Added,
    Deleted
  };

  void OnDeformation( Deformation::Data::Input input );
  Deformation::Data::Input Deformation_();


private:
  void ActUponNoValuesDisplayed(AddedOrDeleted action);

public:
  void ViewBeachBalls(bool bFill);
  bool BeachBallsOnView() const;
  bool CanViewBeachBalls() const;

  void ShowValuesPerElement(bool perElement);
  bool ShowValuesPerElement() const;
  bool CanShowValuesPerElement();

  bool DraggerTrackerSelected() const;
  bool CanSelectDraggerTracker() const;
  void SelectDraggerTracker(bool selected);

  bool MouseTrackerSelected() const;
  bool CanSelectMouseTracker() const;
  void SelectMouseTracker(bool selected);

  void SetDraggerConfined(bool yesOrNo);
  bool DraggerConfined() const;
  bool CanSetDraggerConfined() const;

  void HideLegend(bool yesOrNo);
  bool LegendHidden() const;
  bool CanSelectHideLegend() const;

  void SetTextColor( float * rgb );
  void GetTextColor( float * rgb );

  void SelectMeshMode( MeshMode mode );
  MeshMode MeshModeSelected();
  bool CanSelectMeshMode();

  bool ClipPlaneModeSelected() const;
  bool CanSelectClipPlaneMode() const;
  void SelectClipPlaneMode(bool enable);

  bool ContourLineModeSelected() const;
  bool CanSelectContourLineMode() const;
  void SelectContourLineMode(bool enable);

  bool MeshLinesDisplayed() const;
  bool CanSelectMeshLineDisplayMode() const;
  void SelectMeshLinesDisplayed(bool enable);

  bool FormationOutlinesDisplayed() const;
  bool CanSelectFormationOutlineDisplayMode() const;
  void SelectFormationOutlinesDisplayed(bool enable);

  bool TrackElement() const;
  bool CanTrackElement() const;
  void TrackElement(bool enable);

  bool MeshesBeingDisplayed() const;

  int IsoCount() const;
  void IsoCount (int count);

  double IsoGap() const;
  void IsoGap (double gap);

  int  Transparency() const;
  void Transparency(int transparency);

  void UpdateDisplaySettings ();

  void SetPickStyle(SoPickStyle::Style style);
  SoPickStyle::Style GetPickStyle();

  void Fill(bool bFill);
  bool Fill() const;
  bool CanFill() const;

  void ViewTensorVector( TensorVectorMode mode );
  TensorVectorMode ViewTensorVector();

  bool ViewProportional() const;
  void ViewProportional(bool bProportional);

  void ShowTopView( bool show );
  bool ShowTopView();

  bool PointSetSelected() const;

  void DisplayValuesLegend();

  void MakeValuedMeshesVisible();

  void DisplayGeologyLegend();

  void MakeAllMeshesVisible();

  void UpdateValuesLegend();

  void ActUponNoValuesDisplayedWhenAdded();

  void UpdateValuesLegendAdornments(const IValueComponentBase * valueComponent);

private:
  void UpdateValuesLegendAdornments( );

  void CreateColorScaleAdornment();

  void CreatePropertyAdornments( const IValueComponentBase * valueComponent );

  void AddLegendPropertyContextAdornments( const IResultComponent * pResultComponent );

  void AddValuesLegendAdornment( std::string adornment );

  const std::string GetLegendTitle() const;

public:
  void GetMeshDataMinMax(double & minimum, double & maximum);
  void GetIsoDataMinMax(double & minimum, double & maximum);

  std::size_t GetNumberOfCrossSectionsDisplayed();

  void GetColorScaleRange(double & minimum, double &maximum);

  void adjustMinMax( double &minimum, double &maximum );

  void RemoveAllMeshes();

  bool CanDisplay( const COpenGLNode* pOpenGLNode );
  bool IsVisible( const COpenGLNode* pOpenGLNode );

  void ShowGeologyLegend();

  bool ShowColor();
  void ShowColor(bool show);

  bool CanShowValue() const;

  void removeDataSets();
  void removeIsoDataSets();

  void PointSize( double m_pointSize );

  void SetVectorScale(float scale);
  void SetVectorVisibleFraction(float factor);

  int createTensorVectorDataSet(std::shared_ptr<OIDITensorSetI> tensors);
  int createTensorVectorSignDataSet(std::shared_ptr<OIDITensorSetI> tensors);

  void setVectorDataSetNormal( int vectorSetId, SbVec3f normal);
  void setVectorSignDataSetNormal( int vectorSetId, SbVec3f normal);

  bool isTensorVectorSignDataSet( int scalarSetId );

  double computeDistanceClosestNode( const MiMeshUnstructuredI * mesh, const MbVec3d & coordinates, size_t cellId, size_t & cniClosest );
  void invalidateValueTrackerCellMarkers();
  void computeValueTrackerCellMarkerSet( const MiMeshUnstructuredI * mesh, const MbVec3d & coordinates, size_t cellId, size_t &meshNodeIndexClosest, MbVec3d &meshNodeCoordinatesClosest );
  void computeValueTrackerCellFacetSet( const OIDIMeshVU * mesh, size_t cellId );
  void computeValueTrackerCellFacetSet( const OIDIMeshSU * mesh, size_t cellId);
  void computeValueTrackerCellFacetSet( const OIDIPointSetMesh * mesh, size_t cellId );
  void clearValueTrackerCellFacetSet();

  void updateValueTracker ( const MiMeshUnstructuredI * mesh, const OIDIMesh * oidiMesh, const MbVec3d & coordinates, size_t cellId);

  void handleContext( const OIDIMesh * mesh, size_t cellId, QPoint & point, const MbVec3d & coordinates );

  bool highlightMesh( const OIDIMesh * mesh );
  void dehighlightMesh();

  float GetHeight( const COpenGLNode& node);

  void addToGeologyLegend( const COpenGLNode& node );
  void removeFromGeologyLegend( const COpenGLNode& node );
  void ModifyGeologyLegend( const COpenGLNode & pOpenGLNode );
  void UpdateGeologyLegend();

  bool handleRightMousePress(const QPoint & point );
  void timeOut();
  void resetTimeOut( bool force = false );
  void restoreValueComponent( IValueComponentBase* pValueComponent );

  const OIDISet& getMeshSet() const;

  SoGroup * getMeshes();
  SoGroup * getDisplayedMeshes();
  SoGroup * getMeshPointProbeGroup();

private:

  typedef std::map<const geo::IElementSet*, int> DataSetMap;

  enum ComponentType
  {
    ColorComponent,
    IsoComponent
  };

  void GetDataMinMax (ComponentType componentType, double & minimum, double & maximum);

  void updateMinMax( const MiDataSetI<double>* dataSet, double &minimum, double &maximum );

  void UpdateDataMinMaxForNode( OIDIMeshNode * meshNode, ComponentType componentType, double &minimum, double &maximum );

  static MoColorMapping * s_tensorVectorColorMapping;

  void ActUponValueComponentChanged( const IValueComponentBase * pValueComponent, CDoubleQuantity::UNIT unit, ComponentType why, bool removeDataSets = false );
  void initCrossSectionManip();

  SoSwitch* createWellPathNode(const CNewWellPath& wellPath);

  OIDIMeshNode * CreateMeshNode(const OIDIMesh * mesh);

  void HandleNewOpenGLNode( const COpenGLNode &node );
  bool HandleOpenGLNodeDeleted( const COpenGLNode& node);

  void PointSetWasDeselected(bool wasIt);
  void PointSetWasSelected(bool wasIt);

  std::string GetName(const CGraphNode & node) const;
  bool scalarSetsAvailable();
  bool vectorSetsAvailable();
  bool tensorSetsAvailable();
  bool ValuesBeingDisplayed(AddedOrDeleted action) const;
  
  void UpdateBoundingBox( OIDIMeshNode* meshNode, SbBox3f &bboxOverall );
  void unlinkValueComponent();
  void switchToGeologyColors();
  void switchToValueColors();
  void UpdateMeshBoundingBox( SoGroup * group, SbBox3f & bbox );

  bool refresh_xsec();

public:
  int NumberOfOpenGLNodesLinked();

private:

  COpenInventorEventsHandler *    m_openInventorEventsHandler;

  bool                  m_inView;
  bool                  m_showTracker;

  SoGroup *             m_displayedMeshes;
  SoSeparator *         m_meshes;
  MeshPointProbeGroup * m_meshPointProbeGroup;

  TNewWellPathMap     m_newWellPathMap;
  SoSeparator*        m_wellPaths;
  SoFont *            m_wellPathFont;

  ValueTracker *      m_valueTracker;
  bool                m_elementTrackerSelected;
  bool                m_draggerTrackerSelected;
  bool                m_mouseTrackerSelected;
  bool                m_clipPlaneModeSelected;    

  SoScale  *          m_scale;
  SoLightModel *      m_lightModel;
  LegendScene *       m_legendScene;
  SoSwitch *          m_legendSceneSwitch;

  SoPickStyle *             m_pickStyle;

  SoSwitch *                m_geologyOrValueSwitch;
  SoGroup *                 m_geologyGroup;
  SoGroup *                 m_valueGroup;
  MoColorMapping *          m_valueColorMap;
  MoLevelColorMapping *     m_geologyColorMap;

  const IValueComponentBase *   m_pValueComponentDisplayed;
  const IValueComponentBase *   m_pIsoValueComponentDisplayed;

  SoGroup *                 m_valueScalarSetGroup;
  SoGroup *                 m_vectorSetGroup;
  SoGroup *                 m_tensorSetGroup;

  SoSwitch *                m_crossSectionManipSwitch;
  CrossSectionManipulator * m_crossSectionManip;
  MeshCrossSection*         m_currentCrossSection;
  CrossSectionGroup*        m_xsecGroup;

  OIDISet                   m_meshSet;

  SbVec3d                   m_translation;

  struct MeshInfo
  {
      int colorScalarSetId;
      int isoScalarSetId;
      int vectorSetId;
      int tensorSetId;
      int tensorVectorSetId;
      int eigenValueSignSetId;
      int beachballScalarSetId1; // min direction
      int beachballScalarSetId2; // med direction

      std::shared_ptr<OIDITensorSetI> tensorSet;

      MeshInfo();
  };

  typedef std::map<const OIDIMesh*, MeshInfo> MeshInfoMap;
  MeshInfoMap m_meshInfoMap;

  MeshNodeSettings *        m_meshNodeSettings;
  bool                      m_showTopView;
  int                       m_pointSetCount;

  const COpenGLNode *       m_highlightedNode;
  const OIDIMesh *          m_highlightedMesh; 
};

#endif
