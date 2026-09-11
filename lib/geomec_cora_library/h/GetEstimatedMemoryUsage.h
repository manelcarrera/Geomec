#ifndef _cora_GetEstimatedMemoryUsage_h_
#define _cora_GetEstimatedMemoryUsage_h_

#include <fstream>

namespace cora
{

class CGetEstimatedMemoryUsage
{
  public:
    CGetEstimatedMemoryUsage();

    std::ostream& operator () (std::ostream& os) const;

  private:
    CGetEstimatedMemoryUsage(const CGetEstimatedMemoryUsage& rhs);
    CGetEstimatedMemoryUsage& operator = (CGetEstimatedMemoryUsage rhs);
};

} // namespace cora

#endif  // _cora_GetEstimatedMemoryUsage_h_
