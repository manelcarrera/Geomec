
#include "IDCFlowLoad.h"

namespace dia {

IFlowLoad::IFlowLoad(CBoundaCase &boundacase, double size) : ILoad(boundacase, size), m_BoundaCase(boundacase) {
  boundacase.AddLoad(*this);
}

IFlowLoad::IFlowLoad(CBoundaCase &boundacase, const std::vector<double> &vcValue)
    : ILoad(boundacase, vcValue), m_BoundaCase(boundacase) {
  boundacase.AddLoad(*this);
}

const CBoundaCase &IFlowLoad::Case() const { return m_BoundaCase; }

} // namespace dia