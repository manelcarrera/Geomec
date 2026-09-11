#pragma once

#include "IRetrieveDianaFileNames.h"

class CRetrieveDianaFileNames : public IRetrieveDianaFileNames
{
public:
  CRetrieveDianaFileNames() {};
  virtual ~CRetrieveDianaFileNames() {} // = 0;

  virtual bool retrieveDianaFileNames(	QString& newTitle,
                      const std::string& title) const { return true; } //= 0;

private:
  CRetrieveDianaFileNames(const CRetrieveDianaFileNames& rhs){};
  CRetrieveDianaFileNames& operator = (const CRetrieveDianaFileNames& rhs){};
};