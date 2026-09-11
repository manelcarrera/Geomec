#ifndef _NewWellPathCurve_h_
#define _NewWellPathCurve_h_

#include "NewWellPath.h"
#include "WellPathCurveBase.h"

class NewWellPathCurve : public WellPathCurveBase {
private:
  const CNewWellPath *m_wellPath;
  CNewWellPath::TPointValueVector m_wellPointValueVector;

  void updatePoints();

protected:
  virtual void onUnitChanged();

public:
  NewWellPathCurve(const CNewWellPath *wellPath, const IValueComponentBase *valueComponent, CUnitNode::TUnitType unit,
                   int colorIndex);

  virtual QString getTextRepresentation() const;

  static QString getTextRepresentation(std::vector<const NewWellPathCurve *> &curves);

  virtual void refresh();

  virtual QString getWellPathName() const;

  const CNewWellPath *getWellPath() const;
};

#endif // _NewWellPathCurve_h_
