#include "GeneralDataBlock.h"

CGeneralDataBlock::CGeneralDataBlock() : m_name(), m_type(0) {}

CGeneralDataBlock::CGeneralDataBlock(const QString &name, int type) : m_name(name), m_type(type) {}

CGeneralDataBlock::CGeneralDataBlock(const CGeneralDataBlock &rhs)
    : m_name(rhs.m_name), m_type(rhs.m_type), m_intValues(rhs.m_intValues), m_dblValues(rhs.m_dblValues) {}

CGeneralDataBlock::~CGeneralDataBlock() {}

bool CGeneralDataBlock::operator==(const CGeneralDataBlock &rhs) const { return m_name == rhs.m_name; }

bool CGeneralDataBlock::operator<(const CGeneralDataBlock &rhs) const { return m_name < rhs.m_name; }

QString CGeneralDataBlock::Name() const { return m_name; }

void CGeneralDataBlock::Name(const QString &name) { m_name = name; }

int CGeneralDataBlock::Size() const {
  switch (m_type) {
  case 0:
    return int(m_intValues.size());
  case 1:
    return int(m_dblValues.size());
  case 2:
    return int(m_strValues.size());
  }

  return 0;
}

int CGeneralDataBlock::Type() const { return m_type; }

void CGeneralDataBlock::Type(int type) {
  m_type = type;
  switch (m_type) {
  case 0:
    m_dblValues.clear();
    m_strValues.clear();
    break;
  case 1:
    m_intValues.clear();
    m_strValues.clear();
    break;
  case 2:
    m_intValues.clear();
    m_dblValues.clear();
    break;
  default:
    m_intValues.clear();
    m_dblValues.clear();
    m_strValues.clear();
  }
}

void CGeneralDataBlock::getIntValues(std::vector<int> &values) const {
  values.resize(m_intValues.size());
  std::copy(m_intValues.begin(), m_intValues.end(), values.begin());
}

void CGeneralDataBlock::setIntValues(const std::vector<int> &values) {
  m_intValues.resize(values.size());
  std::copy(values.begin(), values.end(), m_intValues.begin());
}

void CGeneralDataBlock::getDblValues(std::vector<double> &values) const {
  values.resize(m_dblValues.size());
  std::copy(m_dblValues.begin(), m_dblValues.end(), values.begin());
}

void CGeneralDataBlock::setDblValues(const std::vector<double> &values) {
  m_dblValues.resize(values.size());
  std::copy(values.begin(), values.end(), m_dblValues.begin());
}

void CGeneralDataBlock::getStrValues(std::vector<QString> &values) const {
  values.resize(m_strValues.size());
  std::copy(m_strValues.begin(), m_strValues.end(), values.begin());
}

void CGeneralDataBlock::setStrValues(const std::vector<QString> &values) {
  m_strValues.resize(values.size());
  std::copy(values.begin(), values.end(), m_strValues.begin());
}
