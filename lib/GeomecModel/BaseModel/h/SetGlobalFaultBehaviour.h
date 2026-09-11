#ifndef _SetGlobalFaultBehaviour_h_
#define _SetGlobalFaultBehaviour_h_

class CFemAppModel;
class CFaultPressure;
class CModelBase;
class CUnitNode;

#include <vector>

#include <QString>

#include "FaultPressure.h"

class CSetGlobalFaultBehaviour
{
  public:
    CSetGlobalFaultBehaviour(CFemAppModel* femAppModel);

    bool canModifyPressure() const;

    enum
    {
      DEPTH = 0,
      PRESSURE = 1,
      GRADIENT = 2
    };

    std::vector <QString> getUnitNames(const CUnitNode& unitNode) const;
    bool getModeType(CFaultPressure::TModeType& modeType) const;
    bool getValues(std::vector <double>& values, const CUnitNode& unitNode)
      const;

    void setGlobalFaultBehaviour(const std::vector <double>& values,
      const CUnitNode& unitNode, CFaultPressure::TModeType modeType);

  private:
    CSetGlobalFaultBehaviour(const CSetGlobalFaultBehaviour& rhs);
    CSetGlobalFaultBehaviour& operator = (const CSetGlobalFaultBehaviour& rhs);

    bool isLocked(const CFaultPressure* faultPressure) const;
    bool canModifyPressure(const CFaultPressure* faultPressure) const;

    typedef std::vector <CFaultPressure*> TFaultPressures;

    static TFaultPressures collectFaultPressures(CModelBase* modelBase);

    CModelBase* m_modelBase;
    TFaultPressures m_faultPressures;
};

#endif  // _SetGlobalFaultBehaviour_h_
