#ifndef _NewModel_h_
#define _NewModel_h_

class CFemAppModel;
class CFemAppGUI;

class CNewModel {
public:
  CNewModel();

  virtual ~CNewModel();

  void setModel(CFemAppModel *femAppModel);
  void setModel(CFemAppGUI *femAppGUI);

  bool newModel(bool attachToDocument);

private:
  bool newModelConsole(bool attachToDocument);
  bool newModelGUI(bool attachToDocument);

  typedef bool (CNewModel::*TNewModelFunc)(bool);
  void setNewModelFunction(TNewModelFunc func);

private:
  CNewModel(const CNewModel &rhs);
  CNewModel &operator=(const CNewModel &rhs);

  CFemAppModel *m_modelConsole;
  CFemAppGUI *m_modelGUI;

  TNewModelFunc m_newModelFunc;
};

#endif // _NewModel_h_
