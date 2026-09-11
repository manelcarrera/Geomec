#ifndef _cora_FaultParameters_h_
#define _cora_FaultParameters_h_

class CModelBase;
class CDepletionStage;

#include "Parameter.h"

namespace cora
{

class CFaultParameters
{
  public:
  CFaultParameters();

  static TParameters getParameters(CModelBase* modelBase,
      CHorizonBase* horizonBase);

  private:
  CFaultParameters(const CFaultParameters& rhs);
  CFaultParameters& operator = (const CFaultParameters& rhs);

  static void getParameters(TParameters& faultParameters,
      CModelBase* modelBase, const CDepletionStage& depletionStage,
      CHorizonBase* horizonBase);
  static void getParameters(TParameters& faultParameters,
      CModelBase* modelBase, const CDepletionStage& depletionStage,
      CHorizonBase* horizonBase, unsigned int valueTypeID,
      unsigned int valueTypeName);
  static void getFaultParameters(TParameters& faultParameters,
      CModelBase* modelBase, const CDepletionStage& depletionStage,
      CHorizonBase* horizonBase, unsigned int valueTypeID,
      unsigned int valueTypeName);
};

} // namespace cora

#endif  // _cora_FaultParameters_h_
