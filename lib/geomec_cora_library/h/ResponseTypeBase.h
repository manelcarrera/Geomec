#ifndef _cora_ResponseTypeBase_h_
#define _cora_ResponseTypeBase_h_

#include <vector>

#include <QString>

namespace cora
{

class CSummaryResultFile;

} // namespace cora

#include "Object.h"
#include "FailureMode.h"

namespace cora
{

class CResponseTypeBase
{
  public:
    static const QString RESPONSE_TYPE_MAX;
    static const QString RESPONSE_TYPE_MIN;
    static const QString RESPONSE_TYPE_MEAN;
    static const QString RESPONSE_TYPE_FAILURE_AREA_MAX;
    static const QString RESPONSE_TYPE_FAILURE_AREA_MIN;
    static const QString RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MAX;
    static const QString RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MIN;
    static const QString RESPONSE_TYPE_FN;

    static const QString RESPONSE_TYPE_IS_NOT_IMPLEMENTED;

    static const double UNDEFINED_OR_INVALID_RESPONSE_TYPE;

    CResponseTypeBase(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
    virtual ~CResponseTypeBase() = 0;

    virtual double calculate(const TObject& object,
      const TFailureMode& failureMode) = 0;

    const QString getFailureType() const;
    virtual const QString getResponseType() const;

  protected:
    void verifyResponseType(const IValueDomainScalar::TValueVec& valueVec) const;

    static double calculateTotalSize(const TObject& object,
      const geo::IElement* element);

    CSummaryResultFile& m_summaryResultFile;
    const QString m_failureType;
    const QString m_responseType;

  private:
    CResponseTypeBase(const CResponseTypeBase& rhs);
    CResponseTypeBase& operator = (const CResponseTypeBase& rhs);
};

typedef QSharedPointer <CResponseTypeBase> TResponseTypeBase;

} // namespace cora

#endif  // _cora_ResponseTypeBase_h_
