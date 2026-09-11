#pragma once

#include "GMSkuaConnectorExport.h"

namespace gm_skua {

struct SKUAParseData;

class INTERFACE_GMSKUACONNECTOR SKUAIPostProcessor {
protected:
  SKUAParseData *m_data;
  SKUAIPostProcessor *m_next;

public:
  SKUAIPostProcessor(SKUAParseData *data = nullptr) : m_data(data), m_next(nullptr) {}
  SKUAIPostProcessor(SKUAParseData *data, SKUAIPostProcessor *nextPostProcessor)
      : m_data(data), m_next((*nextPostProcessor)(data)) {}
  virtual ~SKUAIPostProcessor() {}

  void SetNextPostProcessor(SKUAIPostProcessor *nextPostProcessor) { m_next = (*nextPostProcessor)(m_data); }
  SKUAIPostProcessor *operator()(SKUAParseData *data) {
    m_data = data;
    return this;
  }

  virtual void PostProcess() = 0;
};

} // namespace gm_skua
