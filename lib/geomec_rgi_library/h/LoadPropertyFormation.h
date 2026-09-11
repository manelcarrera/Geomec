#ifndef _LoadPropertyFormation_h_
#define _LoadPropertyFormation_h_

#include "LoadPropertyBase.h"

namespace GeomecRGI {

class CLoadPropertyFormation : public CLoadPropertyBase {
public:
  CLoadPropertyFormation(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                         CRockMechProcessor &rmp);
  virtual ~CLoadPropertyFormation();

  virtual bool loadProperty();

private:
  CLoadPropertyFormation(const CLoadPropertyFormation &rhs);
  CLoadPropertyFormation &operator=(const CLoadPropertyFormation &rhs);
};

} // namespace GeomecRGI

#endif // _LoadPropertyFormation_h_
