#ifndef OIV2DWELLPATHGRAPHSCENE_H_INCLUDED
#define OIV2DWELLPATHGRAPHSCENE_H_INCLUDED

class CNewWellPath;
class Curve;
class CDepletionStage;
class IResultComponent;

#include "OIV2DGraphValueColorScene.h"

class OIV2DWellPathGraphScene : public OIV2DGraphValueColorScene
{
  std::vector<const CNewWellPath*> m_newWellPaths;

  std::vector<const IValueComponentBase*> m_valueComponents;

  CUnitNode* m_unit;
  const IValueComponentBase * m_rejectedComponent;

  bool m_formationLinesVisible;

  void unlinkWellPaths();
  void unlinkValueComponents();
  void updateGraph();
  void addCurveToLegend(const Curve* curve, size_t numStages);
  void init();

  bool CanConnectValueComponent(const IValueComponentBase* component) const;

  void OnNewWellPath(const CNewWellPath* wellPath);
  void UnLinkIncompatibleValueComponents(const IValueComponentBase* valueComponent);

public:

  OIV2DWellPathGraphScene(CFemAppModel& model);
  OIV2DWellPathGraphScene(const QString& strName, CFemAppModel& model);
  OIV2DWellPathGraphScene(unsigned int uName, CFemAppModel& model);

  ~OIV2DWellPathGraphScene();

  virtual bool operator==(const OIV2DWellPathGraphScene& node) const;

  const CUnitNode& UnitNode() const;

  void toggleFormationLines();
  bool canToggleFormationLines() const;
  bool formationLinesVisible() const;

  void toggleDepthAxis();
  bool canToggleDepthAxis() const;

  void copyDataToClipboard();
  bool canCopyDataToClipboard() const;

  QString copyDataToString() const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual size_t NumberOfValueComponents() const;
  virtual size_t NumberOfResultComponents() const;
  virtual IValueComponentBase* ValueComponent(size_t index) const;
  virtual bool MultipleValueComponentsAllowed() const;
  virtual bool MultipleStagesAllowed() const;

  virtual bool CanConnectToMultipleItems(const CGraphNode& node) const;
  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& node);

  void UnLinkAllValueComponents();
  void UnLinkAllResultComponents();

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  // load/save
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);

  void showContextMenu(const TScreenPoint& point);
  void clearAxis(int group);
  void clearBottomAxis();
  bool bottomAxisFilled() const;
  void clearTopAxis();
  bool topAxisFilled() const;
  size_t countStages() const;

  void collectStages( std::vector<const CDepletionStage *> &stages ) const;


  virtual void setSize(int width, int height);

  void SaveColorToStream (QRgb (OIV2DWellPathGraphScene::*func_ptr)() const, TSTREAM& stream);
  void RestoreColorFromStream (void (OIV2DWellPathGraphScene::*func_ptr)(QRgb), TSTREAM& stream);

  template < class T> 
  void SaveValueToStream (T (OIV2DWellPathGraphScene::*func_ptr)() const, TSTREAM& stream)
  {
    stream << (this->* func_ptr)();
  }

  template < class T >
  void RestoreValueFromStream (void (OIV2DWellPathGraphScene::*func_ptr)(T), TSTREAM& stream)
  {
    T value;
    stream >> value;
    (this->* func_ptr) (value);
  }

  // specialization to account for that we cannot stream to a bool
  template <>
  void RestoreValueFromStream (void (OIV2DWellPathGraphScene::*func_ptr)(bool), TSTREAM& stream)
  {
    int value;
    stream >> value;
    (this->* func_ptr) (value);
  }
  virtual int SelectMode() const;
  virtual void updateLegend();
  void UnLinkAllWellPaths();
  bool IsNewStage( const IResultComponent & resultComponent );
};

#endif
