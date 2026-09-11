#ifndef UNITCONVERSION_H
#define UNITCONVERSION_H
#include <string>

class IUnitConversion // wjrx mantis 3098
{
public:
  virtual ~IUnitConversion() = 0;
  static IUnitConversion *Create
  ( const std::string &sJustAName // identify by name
  , const std::string &sUnitName  // determines conversion
  );
  static void Remove(IUnitConversion *conversion);
  virtual double ConvertToSi(const double &value) const= 0;
  const std::string &Name() const {return m_name;}
protected:
  IUnitConversion(const std::string &sJustAName):m_name(sJustAName) {};
private:
  std::string m_name;
};

//
// y=(ax+b)/(cx+d)
//
class CLinearRationalUnitConversion : public IUnitConversion // wjrx mantis 3098
{
public:
  static CLinearRationalUnitConversion *Create
  ( const std::string &sJustAName
  , const std::string &sUnitName
  );
  virtual double ConvertToSi(const double &value) const;
private:
  CLinearRationalUnitConversion
  ( const std::string &name
  , const double &divisor_factor= 1
  , const double &divisor_addition= 0
  , const double &denominator_factor= 0
  , const double &denominator_addition= 1
  );
  double m_divisor_factor;
  double m_divisor_addition;
  double m_denominator_factor;
  double m_denominator_addition;
};
#endif // UNITCONVERSION_H

