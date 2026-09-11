#ifndef _cora_FilterOutputProperty_h_
#define _cora_FilterOutputProperty_h_

#include <QString>

namespace cora
{

class CObject;

class CFilterOutputProperty
{
  public:
    CFilterOutputProperty();

    bool isOutputPropertyAllowed(const CObject& object, const QString& result)
      const;

  private:
    CFilterOutputProperty(const CFilterOutputProperty& rhs);
    CFilterOutputProperty& operator = (CFilterOutputProperty rhs);
};

} // namespace cora

#endif  // _cora_FilterOutputProperty_h_
