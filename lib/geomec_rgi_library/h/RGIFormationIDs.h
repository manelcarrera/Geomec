#ifndef _RGIFormationIDs_h_
#define _RGIFormationIDs_h_

class RGInterface;

#include <vector>
#include <set>
#include <map>

namespace GeomecRGI
{

class CRGIFormationIDs
{
  public:
  CRGIFormationIDs(RGInterface& rgi);
  ~CRGIFormationIDs();

  std::vector <int> getRGIFormationIDs() const;
  std::set <int> getDistinctRGIFormationIDs() const;

  // returns the offset by which the an element belonging to the formation
  // can be retrieved using 'RGInterface::getElementN'

  std::size_t mapRGIFormation2RGIElement(int rgiFormation) const;

  static std::set <int> getDistinctFormationIDs(
      const std::vector <int>& formationIDs);

  private:
  CRGIFormationIDs(const CRGIFormationIDs& rhs);
  CRGIFormationIDs& operator = (const CRGIFormationIDs& rhs);

  std::vector <int> m_RGIFormationIDs;
  std::set <int> m_DistinctRGIFormationIDs;
  std::map <int, std::size_t> m_RGIFormation2RGIElementMap;
};

} // namespace GeomecRGI

#endif  // _RGIFormationIDs_h_
