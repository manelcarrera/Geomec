#ifndef _OIV2DHistoryScene_h_
#define _OIV2DHistoryScene_h_

class CAnalysisPoint;
class IResultComponent;

#include "OIV2DGraphValueColorScene.h"

class OIV2DHistoryScene : public OIV2DGraphValueColorScene
{
  std::vector<const IValueComponentBase*> m_valueComponents;
  std::vector<const CAnalysisPoint *> m_analysisPoints;

  CUnitNode* m_unit;
  const IValueComponentBase * m_rejectedComponent;

  void unlinkWellPaths();
  void updateGraph();
  void init();

public:

  OIV2DHistoryScene(CFemAppModel& model);
  OIV2DHistoryScene(const QString& strName, CFemAppModel& model);
  OIV2DHistoryScene(unsigned int uName, CFemAppModel& model);

  ~OIV2DHistoryScene();

  virtual bool operator==(const OIV2DHistoryScene& node) const;

  virtual void setSize(int width, int height);

  void unlinkAnalysisPoints();
  void unlinkValueComponents();

  const CUnitNode& UnitNode() const;

  void copyDataToClipboard();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& node);

  virtual bool CanConnectToMultipleItems(const CGraphNode& node) const;

  virtual void OnNewNeighbour(const CGraphNode& node);

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  bool CanDisplayResultComponent(const IResultComponent& result_component) const;

  // load/save
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);
  virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point);

  void showContextMenu(const TScreenPoint& point);

  virtual size_t NumberOfValueComponents() const;
  virtual IValueComponentBase* ValueComponent(size_t index) const;
  virtual bool MultipleValueComponentsAllowed() const;

  virtual bool MultipleStagesAllowed() const;

  virtual size_t NumberOfAnalysisPoints() const;
  virtual CAnalysisPoint * AnalysisPoint(size_t index) const;
  bool canAddUnit( QString unitName ) const;

  void clearLeftAxis();
  bool leftAxisFilled() const;
  void clearRightAxis();
  bool rightAxisFilled() const;
  void clearAxis(int group);

  QString leftAxisUnit() const;
  QString rightAxisUnit() const;

  void SaveColorToStream (QRgb (OIV2DHistoryScene::*func_ptr)() const, TSTREAM& stream);
  void RestoreColorFromStream (void (OIV2DHistoryScene::*func_ptr)(QRgb), TSTREAM& stream);


  template < class T> 
  void SaveValueToStream (T (OIV2DHistoryScene::*func_ptr)() const, TSTREAM& stream)
  {
    stream << (this->* func_ptr)();
  }

  template < class T >
  void RestoreValueFromStream (void (OIV2DHistoryScene::*func_ptr)(T), TSTREAM& stream)
  {
    T value;
    stream >> value;
    (this->* func_ptr) (value);
  }

  // specialization to account for that we cannot stream to a bool
  template <>
  void RestoreValueFromStream (void (OIV2DHistoryScene::*func_ptr)(bool), TSTREAM& stream)
  {
    int value;
    stream >> value;
    (this->* func_ptr) (value);
  }
  virtual int SelectMode() const;
  virtual void updateLegend();
};

#endif  // _OIV2DHistoryScene_h_
