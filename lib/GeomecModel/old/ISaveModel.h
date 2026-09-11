#ifndef _ISaveModel_h_
#define _ISaveModel_h_

class QString;

class ISaveModel
{
public:
  ISaveModel();
  virtual ~ISaveModel() = 0;

  virtual bool saveModel(const QString& fileName, bool bSwitch) = 0;

private:
  ISaveModel(const ISaveModel& rhs);
  ISaveModel& operator = (const ISaveModel& rhs);
};

#endif  // _ISaveModel_h_
