#ifndef _SaveModelGUI_h_
#define _SaveModelGUI_h_

#include "ISaveModel.h"

class CSaveModelGUI : public ISaveModel
{
public:
  CSaveModelGUI();
  virtual ~CSaveModelGUI();

  virtual bool saveModel(const QString& fileName, bool bSwitch);

private:
  CSaveModelGUI(const CSaveModelGUI& rhs);
  CSaveModelGUI& operator = (const CSaveModelGUI& rhs);
};

#endif  // _SaveModelGUI_h_
