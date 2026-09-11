#ifndef _IDCFLOWLOAD_H_
#define _IDCFLOWLOAD_H_

#include "DCBoundaCase.h"
#include "IDCLoad.h"

namespace dia {

class IFlowLoad : public ILoad {
  CBoundaCase &m_BoundaCase;

public:
  IFlowLoad(CBoundaCase &boundacase, double size);
  IFlowLoad(CBoundaCase &boundacase, const std::vector<double> &vcValue);

  virtual const CBoundaCase &Case() const;
};

} // namespace dia

#endif // _IDCFLOWLOAD_H_
