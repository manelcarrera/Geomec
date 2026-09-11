#ifndef _cora_Parameter_h_
#define _cora_Parameter_h_

#include <vector>

class CFormationBase;
class CLibraryMaterialParameter;
class CHorizonBase;
class CSurfaceBase;

#include "ParameterBase.h"

namespace cora
{

class CParameter
{
  public:
    static struct TFaultParameter {} faultParameter;
    static struct TFormationParameter4Material {} formationParameter4Material;
    static struct TFormationParameter4Pressure {} formationParameter4Pressure;
    static struct TFormationParameter4PressureChange {}
      formationParameter4PressureChange;
    static struct THorizonParameter {} horizonParameter;
    static struct TSurfaceParameter {} surfaceParameter;
    static struct TNonMeshedSurfaceParameter {} nonMeshedSurfaceParameter;
    static struct TWellParameter {} wellParameter;

    CParameter(const TFaultParameter&, const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CHorizonBase* horizonBase, int depletionStage, unsigned int valueTypeID);
    CParameter(const TFormationParameter4Material&, const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CFormationBase* formationBase, int depletionStage,
      CLibraryMaterialParameter& libraryMaterialParameter);
    CParameter(const TFormationParameter4Pressure&, const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CFormationBase* formationBase, int depletionStage,
      unsigned int valueTypeID);
    CParameter(const TFormationParameter4PressureChange&, const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      CFormationBase* formationBase, int depletionStage,
      unsigned int valueTypeID);
    CParameter(const THorizonParameter&);
    CParameter(const TSurfaceParameter&);
    CParameter(const TNonMeshedSurfaceParameter&, const QString& name,
      const geo::CValue& minimum, const geo::CValue& maximum, double mean,
      unsigned int valueTypeID, int depletionStage, CSurfaceBase* surfaceBase);
    CParameter(const TWellParameter&);

    int depletionStage() const;
    unsigned int valueTypeID() const;
    const QString& name() const;

    CGraphNode* object() const;

    double min() const;
    double max() const;
    double mean() const;

    const QString lowerLimit() const;
    const QString upperLimit() const;

    std::ostream& operator () (std::ostream& os) const;

    template <typename ParameterType>
      bool isParameterType() const;

  private:
    CParameter(const CParameter& rhs);
    CParameter& operator = (CParameter rhs);

    TParameterBase m_objectBase;
};

template <typename ParameterType>
  bool CParameter::isParameterType() const
{
  return (dynamic_cast <const ParameterType*> (m_objectBase.data()) != 0);
}

typedef QSharedPointer <CParameter> TParameter;
typedef std::vector <TParameter> TParameters;

} // namespace cora

std::ostream& operator << (std::ostream& os, const cora::CParameter& rhs);
std::ostream& operator << (std::ostream& os, const cora::TParameter& rhs);
std::ostream& operator << (std::ostream& os, const cora::TParameters& rhs);

#endif  // _cora_Parameter_h_
