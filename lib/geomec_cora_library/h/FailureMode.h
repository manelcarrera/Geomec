#ifndef _cora_FailureMode_h_
#define _cora_FailureMode_h_

#include <vector>

#include "safeQSharedPointer.h"

class IResultComponent;

#include "Object.h"

namespace cora
{

class CFailureMode
{
  public:
  CFailureMode(const QString& resultExportLabel,
      const IResultComponent* resultComponent);

  const QString& getResultExportLabel() const;
  const IResultComponent* getResultComponent() const;

  void setObject(const TObject& object);
  void setFailureModeLabel(const QString& failureModeLabel);

  const TObject& getObject() const;
  const QString& getFailureModeLabel() const;

  int getFailureModeType() const;

  private:
  CFailureMode(const CFailureMode& rhs);
  CFailureMode& operator = (CFailureMode rhs);

  const QString m_resultExportLabel;
  const IResultComponent* m_resultComponent;
  TObject m_object;
  QString m_failureModeLabel;
};

typedef QSharedPointer <CFailureMode> TFailureMode;
typedef std::vector <TFailureMode> TFailureModes;

} // namespace cora

#endif  // _cora_FailureMode_h_
