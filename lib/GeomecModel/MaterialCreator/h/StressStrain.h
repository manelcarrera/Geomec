#ifndef _STRESSSTRAIN_H_
#define _STRESSSTRAIN_H_

#include <cmath>

class CStressStrain
{
public:
  CStressStrain()
  : m_dAxialStress(0),
  m_dAxialStrain(0),
  m_dRadialStress(0),
  m_dRadialStrain(0),
  m_dP(0),
  m_dQ(0),
  m_dTime(0)
  {
  }

  CStressStrain(double dAxialStress, double dAxialStrain, double dRadialStress, double dRadialStrain, double dPressure, double dTime)
  : m_dAxialStress(dAxialStress),
  m_dAxialStrain(dAxialStrain),
  m_dRadialStress(dRadialStress),
  m_dRadialStrain(dRadialStrain),
  m_dPressure(dPressure),
  m_dTime(dTime)
  {
  m_dP = (m_dAxialStress + 2 * m_dRadialStress) / 3;
  m_dQ = sqrt(m_dAxialStress * m_dAxialStress + m_dRadialStress * m_dRadialStress - 2 * m_dAxialStress * m_dRadialStress);
  }

  CStressStrain(double dP, double dQ)
  : m_dP(dP),
  m_dQ(dQ)
  {
  }

  CStressStrain(const CStressStrain &SS)
  {
  *this = SS;
  }

  virtual ~CStressStrain()
  {
  }

  CStressStrain& operator=(const CStressStrain &SS)
  {
  m_dAxialStress = SS.m_dAxialStress;
  m_dAxialStrain = SS.m_dAxialStrain;
  m_dRadialStress = SS.m_dRadialStress;
  m_dRadialStrain = SS.m_dRadialStrain;
  m_dPressure = SS.m_dPressure;
  m_dP = SS.m_dP;
  m_dQ = SS.m_dQ;
  m_dTime = SS.m_dTime;
  return *this;
  }

public:
  double m_dAxialStress;
  double m_dAxialStrain;
  double m_dRadialStress;
  double m_dRadialStrain;
  double m_dPressure;
  double m_dP;
  double m_dQ;
  double m_dTime;
};

typedef std::vector<CStressStrain> CStressStrainArray;

#endif
