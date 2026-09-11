#ifndef _MLXMLEXCEPTION_H_
#define _MLXMLEXCEPTION_H_

#include <QtCore/QString>

namespace mlxml {

class CException
{
public:
  CException(const QString& strError);
  const QString& error() const;

private:
  QString m_strError;
};

} // namespace mlxml

#endif // _MLXMLEXCEPTION_H_
