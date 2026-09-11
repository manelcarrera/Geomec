#ifndef _cora_FilterResults_h_
#define _cora_FilterResults_h_

#include <map>

#include <QString>

namespace cora
{

class CFilterResults
{
  public:
    typedef std::map <QString, bool> TFilterMap;

    CFilterResults();

    bool isResultAllowed(const QString& result) const;
    bool isResultPresent(const QString& result) const;

    const TFilterMap& getFilter() const;

  private:
    CFilterResults(const CFilterResults& rhs);
    CFilterResults& operator = (CFilterResults rhs);

    static TFilterMap fillFilter();

    TFilterMap m_filter;
};

} // namespace cora

#endif  // _cora_FilterResults_h_
