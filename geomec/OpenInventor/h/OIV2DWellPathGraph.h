#ifndef _OIV2DWellPathGraph_h_
#define _OIV2DWellPathGraph_h_

class PoGenAxis;
class PoLinearAxis;

class SoSwitch;
class SoTranslation;
class SoMatrixTransform;
class SoIndexedLineSet;

class CNewWellPath;
class NewWellPathCurve;

#include "OIV2DGraph.h"
#include "CurveGroup.h"
#include "OIV2DRangeInterface.h"

class OIV2DWellPathGraph : public OIV2DGraph
{
public:

  enum DepthAxis
{
    DEPTH_AXIS_AHD,
    DEPTH_AXIS_TVD,
    DEPTH_AXIS_Z
};

private:

  static const int numGraduationsYAxis;
  static const float yAxisOffset;

  /**
     *
     */
  struct FormationSectionParams
  {
    double top;
    double bottom;
    QColor color;
  };

  std::vector<const CNewWellPath*> m_newWellPaths;
  std::vector<const IValueComponentBase*> m_valueComponents;

  CUnitNode::TUnitType m_unit;
  DepthAxis            m_depthAxis;

  class RangeInterface : public OIV2DRangeInterface
  {
      friend class OIV2DWellPathGraph;

      OIV2DWellPathGraph& m_graph;

      float m_top;
      float m_bottom;

      float m_topAHD;
      float m_bottomAHD;

      float m_topTVD;
      float m_bottomTVD;

      float m_topZ;
      float m_bottomZ;

      float m_horizontalMin[2];
      float m_horizontalMax[2];
      float m_horizontalStp[2];

      bool  m_auto[3];
      int   m_tick[3];
      int   m_subtick[3];

      float m_savedTop;
      float m_savedBottom;

  public:
      RangeInterface(OIV2DWellPathGraph& graph, float top, float bottom);
      virtual ~RangeInterface();

      virtual void Enter();
      virtual void Leave(bool revert = false);

      virtual bool HaveAxis(TAxis axis) const;

      virtual void GetRangeMinMax(TAxis axis, float& min, float& max) const;
      virtual bool RangeLargeEnough(TAxis, float min, float max) const;
      virtual void SetMinMax(TAxis axis, float min, float max);

      virtual void ResetMinMax(TAxis axis);

      virtual int Tick(TAxis axis) const;
      virtual void Tick(TAxis axis, int value);

      virtual int SubTick(TAxis axis) const;
      virtual void SubTick(TAxis axis, int value);

      virtual bool Auto(TAxis axis) const;
      virtual void Auto(TAxis axis, bool value);

      virtual void UpdateGraph();

  } m_ranges;

  // m_top and m_bottom are the top and bottom of the well, in AHD, TVD or Z
  // depending on the value of m_depthAxis

  PoDomain*     m_verticalDomain;
  PoGenAxis*    m_yAxisLeft;
  PoGenAxis*    m_yAxisRight;
  SoSwitch*     m_yAxisLeftSwitch;
  SoSwitch*     m_yAxisRightSwitch;

  SoGroup*           m_formationSections;
  SoSwitch*          m_formationSectionsSwitch;
  SoTranslation*     m_formationSectionsTranslation;
  SoSeparator*       m_formationLinesSeparator;
  SoMatrixTransform* m_formationLinesTransform;
  SoSwitch*          m_formationLinesSwitch;
  SoIndexedLineSet*  m_formationLines;

  bool m_formationLinesVisible;
  bool m_markersVisible;
  int  m_lineWidth;

  PoDomain* createVerticalDomain(float top, float bottom);
  PoGenAxis* createVerticalAxis(const char* name, bool gridVisible);
  PoLinearAxis* createHorizontalAxis(CurveGroup::AxisPosition pos, bool gridVisible);
  CurveGroup* createCurveGroup(CurveGroup::AxisPosition axisPosition, int linePattern);

  void buildFormationNodes();

  void setupFormationSections(const std::vector<FormationSectionParams>& params);
  void setupFormationSections(const CNewWellPath& wellPath);
  void setupFormationSections();
  bool setupCurve(Curve* newCurve);
  void createGraph();
  void updateVerticalAxes();
  void updateVerticalDomain();
  void updateFormationSectionsTransforms();
  void updateDomain(CurveGroup* group);
  void setupTopAndBottom();

protected:

  virtual bool HandleMinMaxNearness(size_t coord, float& min, float& max) const;

public:

  OIV2DWellPathGraph(CUnitNode::TUnitType unit, float aspect, int preferredGroup);

  virtual OIV2DRangeInterface& RangeInterface();

  void addWellPath(const CNewWellPath* wellPath);

  void removeWellPath(const CNewWellPath* wellPath);

  bool canAddValueComponent(const IValueComponentBase* component) const;

  bool addValueComponent(const IValueComponentBase* component);

  void updateCurves(const IValueComponentBase* component);

  void removeCurves(const IValueComponentBase* component);

  void removeAllCurves();

  const NewWellPathCurve* getCurve(const CNewWellPath* wellPath, const IValueComponentBase* component) const;

  virtual void updateLabel(Curve * curve, PoDomain* domain, float x, float y) override;

  void showFormationLines(bool visible);

  void showMarkers(bool visible);

  void setUnit(CUnitNode::TUnitType unit);

  void setDepthAxis(DepthAxis axis);

  void UpdateDepthAxes();

  DepthAxis getDepthAxis() const;

  void setLineThickness(int thickness);
  void setForegroundColor( float foregroundColor[3] );
  void collectComponents( int groupindex, std::vector<const IValueComponentBase *> & components );
  int getNumCurves() const;
  int getNumCurves( int group ) const;
  void refreshVisualization();
  void toggleDepthAxis();
};

#endif  // _OIV2DWellPathGraph_h_
