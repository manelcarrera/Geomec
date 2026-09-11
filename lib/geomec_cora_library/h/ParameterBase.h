#ifndef _cora_ParameterBase_h_
#define _cora_ParameterBase_h_

#include "safeQSharedPointer.h"

class CGraphNode;

#include "Value.h"

namespace cora
{

class CParameterBase
{
  public:
    CParameterBase();
    virtual ~CParameterBase() = 0;

    virtual int depletionStage() const = 0;
    virtual unsigned int valueTypeID() const = 0;
    virtual const QString& name() const = 0;

    virtual CGraphNode* object() const = 0;

    virtual double min() const = 0;
    virtual double max() const = 0;
    virtual double mean() const = 0;

    virtual const QString lowerLimit() const = 0;
    virtual const QString upperLimit() const = 0;

    virtual std::ostream& operator () (std::ostream& os) const = 0;

  private:
    CParameterBase(const CParameterBase& rhs);
    CParameterBase& operator = (const CParameterBase& rhs);
};

typedef QSharedPointer <CParameterBase> TParameterBase;

} // namespace cora

#endif  // _cora_ParameterBase_h_
