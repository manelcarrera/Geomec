#ifndef _cora_XMLComponent_h_
#define _cora_XMLComponent_h_

#include <ostream>
#include <vector>

class CModelData;

#include "Object.h"
#include "XMLResponseParameters.h"
#include "XMLInputParameters.h"

namespace cora
{

class CXMLComponent
{
  public:
  CXMLComponent(CModelData& modelData, TObject object);

  std::ostream& operator () (std::ostream& stream) const;

  private:
  CXMLComponent(const CXMLComponent& rhs);
  CXMLComponent& operator = (CXMLComponent rhs);

  TObject m_object;
  CXMLResponseParameters m_responseParameters;
  CXMLInputParameters m_inputParameters;
};

typedef QSharedPointer <CXMLComponent> TXMLComponent;
typedef std::vector <TXMLComponent> TXMLComponents;

} // namespace cora

// global

std::ostream& operator << (std::ostream& stream,
  const cora::CXMLComponent& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLComponent& rhs);
std::ostream& operator << (std::ostream& stream,
  const cora::TXMLComponents& rhs);

#endif  // _cora_XMLComponent_h_
