#include "GVTVelocityModel.h"

#include <cassert>

CGVTVelocityModel::CGVTVelocityModel(eVelocityModel velmod) : m_ModelType(velmod) {
  switch (velmod) {
  case RFactor:
    m_ModelName = "R";
    AddParameter("R_EX", 5.0);
    AddParameter("R_C", 1.0);
    break;
  case SFactor:
    m_ModelName = "S";
    AddParameter("S", 0.002);
    AddParameter("NB", 1.0);
    break;
  default:
    assert(false);
  }
}

CGVTVelocityModel::CGVTVelocityModel(const CGVTVelocityModel &rhs)
    : m_ModelType(rhs.m_ModelType), m_mpParameters(rhs.m_mpParameters), m_mpFastParameters(rhs.m_mpFastParameters),
      m_ModelName(rhs.m_ModelName) {}

void CGVTVelocityModel::ChangeModel(eVelocityModel velmod) {
  if (velmod == m_ModelType)
    return;

  m_mpParameters.clear();
  m_mpFastParameters.clear();
  switch (velmod) {
  case RFactor:
    m_ModelName = "R";
    AddParameter("R_EX", 5.0);
    AddParameter("R_C", 1.0);
    break;
  case SFactor:
    m_ModelName = "S";
    AddParameter("S", 0.002);
    AddParameter("NB", 1.0);
    break;
  default:
    assert(false);
  }
  m_ModelType = velmod;
}

const QString &CGVTVelocityModel::ModelName() const { return m_ModelName; }

eVelocityModel CGVTVelocityModel::ModelType() const { return m_ModelType; }

eVelocityModelParamNames CGVTVelocityModel::NameString2Enum(const QString &name) {
  if (name == "R_EX")
    return RFactor_EX;
  else if (name == "R_C")
    return RFactor_C;
  else if (name == "S")
    return SFactor_S;

  assert(name == "NB");
  return SFactor_NB;
}

void CGVTVelocityModel::AddParameter(const QString &name, double value) {
  m_mpParameters.insert(std::map<QString, double>::value_type(name, value));
  m_mpFastParameters.insert(std::make_pair(NameString2Enum(name), value));
}

void CGVTVelocityModel::SetParameter(const QString &param, double value) {
  std::map<QString, double>::iterator it;
  it = m_mpParameters.find(param);
  assert(it != m_mpParameters.end());
  it->second = value;

  std::map<eVelocityModelParamNames, double>::iterator fastIt;
  fastIt = m_mpFastParameters.find(NameString2Enum(param));
  assert(fastIt != m_mpFastParameters.end());
  fastIt->second = value;
}

size_t CGVTVelocityModel::ParamCount() const { return m_mpParameters.size(); }

double CGVTVelocityModel::Parameter(const QString &name) const {
  std::map<QString, double>::const_iterator it;
  it = m_mpParameters.find(name);
  assert(it != m_mpParameters.end());
  return it->second;
}

double CGVTVelocityModel::Parameter(eVelocityModelParamNames name) const {
  std::map<eVelocityModelParamNames, double>::const_iterator it;
  it = m_mpFastParameters.find(name);
  assert(it != m_mpFastParameters.end());
  return it->second;
}

const std::map<QString, double> &CGVTVelocityModel::Parameters() const { return m_mpParameters; }

std::map<QString, double> &CGVTVelocityModel::Parameters() { return m_mpParameters; }
