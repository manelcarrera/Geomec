#pragma once

class QString;

#include <string>

class IRetrieveDianaFileNames {
public:
  IRetrieveDianaFileNames();          //{};
  virtual ~IRetrieveDianaFileNames(); //{};

  virtual bool retrieveDianaFileNames(QString &newTitle, const std::string &title) const = 0;
};
