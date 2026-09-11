#ifndef _GVT_VELOCITY_MODEL_H_
#define _GVT_VELOCITY_MODEL_H_

#include <map>
#include <qstring.h>


enum eVelocityModel
{
  RFactor = 0,
  SFactor
};

enum eVelocityModelParamNames
{
  RFactor_EX = 0,
  RFactor_C,
  SFactor_S,
  SFactor_NB
};

class CGVTVelocityModel
{
  eVelocityModel m_ModelType;
  std::map<QString, double> m_mpParameters;
  std::map<eVelocityModelParamNames, double> m_mpFastParameters;
  QString m_ModelName;

  void AddParameter(const QString &name, double value);

  static eVelocityModelParamNames NameString2Enum(const QString& name);

public:
  CGVTVelocityModel(eVelocityModel velmod);
  CGVTVelocityModel(const CGVTVelocityModel &rhs);

  void ChangeModel(eVelocityModel velmod);

  const QString &ModelName() const; 
  eVelocityModel ModelType() const;

  size_t ParamCount() const;
  double Parameter(const QString &name) const;
  double Parameter(eVelocityModelParamNames name) const;
  void SetParameter(const QString &param, double value);

  const std::map<QString, double> &Parameters() const;
  std::map<QString, double> &Parameters();
};


#endif