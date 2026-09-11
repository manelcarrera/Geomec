#pragma once

#include "IT_Data.h"

namespace IT {
class Material : public IT_Data {
public:
  enum eData { Ids, Values };

private:
  TIntV m_ids;
  TValuesV m_values;

public:
  Material() : IT_Data() {}
  void ids(TIntV val) { m_ids = val; };
  void values(TValuesV vals) { m_values = vals; };
  TIntV &ids() { return m_ids; };
  TValuesV &values() { return m_values; };
};
} // namespace IT
