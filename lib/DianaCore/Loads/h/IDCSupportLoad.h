#ifndef _IDCSUPPORTLOAD_H_
#define _IDCSUPPORTLOAD_H_

namespace geo {
class ISupport;
}

#include "IDCDirectedLoad.h"

namespace dia {

class CLoadManager;

class ISupportLoad : public IDirectedLoad  
{
  const geo::ISupport& m_support;

public:
  ISupportLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ISupport &support);
  virtual ~ISupportLoad();

  const geo::ISupport &Support() const;

protected:
  bool WriteFilos(const std::string& sType) const;
};

}

#endif // _IDCSUPPORTLOAD_H_
