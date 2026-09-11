#ifndef _RetrieveDianaFileNames_h_
#define _RetrieveDianaFileNames_h_

#include "IRetrieveDianaFileNames.h"

class CRetrieveDianaFileNames : public IRetrieveDianaFileNames
{
public:
  CRetrieveDianaFileNames(IRetrieveDianaFileNames* strategy);
  virtual ~CRetrieveDianaFileNames();

  virtual bool retrieveDianaFileNames(QString& newTitle,
  const std::string& title) const;

private:
  CRetrieveDianaFileNames(const CRetrieveDianaFileNames& rhs);
  CRetrieveDianaFileNames& operator = (const CRetrieveDianaFileNames& rhs);

  IRetrieveDianaFileNames* m_strategy;
};

#endif  // _RetrieveDianaFileNames_h_
