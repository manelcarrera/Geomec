#ifndef _SavePropertyPorosity_h_
#define _SavePropertyPorosity_h_

#include "SavePropertyBase.h"

namespace GeomecRGI
{

class CSavePropertyPorosity : public CSavePropertyBase
{
  public:
  CSavePropertyPorosity(const RGProperty& rgProperty);
  virtual ~CSavePropertyPorosity();

  virtual bool saveProperty(RGInterface& rgi, CModelBase& modelBase,
      const CRockMechProcessor& rmp);

  private:
  CSavePropertyPorosity(const CSavePropertyPorosity& rhs);
  CSavePropertyPorosity& operator = (const CSavePropertyPorosity& rhs);
};

} // namespace GeomecRGI

#endif  // _SavePropertyPorosity_h_
