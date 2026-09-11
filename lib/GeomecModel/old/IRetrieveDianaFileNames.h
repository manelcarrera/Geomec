#ifndef _IRetrieveDianaFileNames_h_
#define _IRetrieveDianaFileNames_h_

class QString;

#include <string>

class IRetrieveDianaFileNames {
public:
  IRetrieveDianaFileNames();
  virtual ~IRetrieveDianaFileNames() = 0;

  virtual bool retrieveDianaFileNames(QString &newTitle, const std::string &title) const = 0;

private:
  IRetrieveDianaFileNames(const IRetrieveDianaFileNames &rhs);
  IRetrieveDianaFileNames &operator=(const IRetrieveDianaFileNames &rhs);
};

#endif // _IRetrieveDianaFileNames_h_
