#ifndef _PointSet_Delegate_h_
#define _PointSet_Delegate_h_

class CPointSet;

#include "IPointSet_Delegate.h"
#include "PointSet.h"
#include "ValueType_Delegate.h"

class CPointSet_Delegate : public IPointSet_Delegate {
public:
  class CCoordinateType_Delegate : public CValueType_Delegate {
  public:
    CCoordinateType_Delegate(CPointSet::CCoordinateType *coordinateType);

    virtual bool CanDestroy() const;

  private:
    CCoordinateType_Delegate(const CCoordinateType_Delegate &rhs);
    CCoordinateType_Delegate &operator=(const CCoordinateType_Delegate &rhs);

    CPointSet::CCoordinateType *m_coordinateType;

    REGISTER_DELEGATE(CPointSet::CCoordinateType, CCoordinateType_Delegate);
  };

  CPointSet_Delegate(CPointSet *pointSet);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  virtual bool CanDestroy() const;

  virtual void Export();

  virtual void Sample();

  virtual bool CanDestroyConvexHull() const;
  virtual void DestroyConvexHull();

private:
  CPointSet_Delegate(const CPointSet_Delegate &rhs);
  CPointSet_Delegate &operator=(const CPointSet_Delegate &rhs);

  CPointSet *m_pointSet;

  REGISTER_DELEGATE(CPointSet, CPointSet_Delegate);
};

#endif // _PointSet_Delegate_h_
