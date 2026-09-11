#pragma once

#include <QString>
#include <vector>

// The general data block is a simple way to get some backward compatibility for file formats
// From 5.8.0, if it exists, it will be read and written
// Whenever a higher Geomec (5.8.X) needs to store data, it should store it here
// There is no convenient interface (yet): once the file format can be updated again (presumably 5.9)
// then the data should be moved from this storage to a permanent place.
//
// Assumed usage:
//
// { "SpecialSettingsFor5_8_10" -> values }
// { "ExtraElementsFor5_8_11" -> values }
// etc
//
// Vectors of Ints, Doubles and QStrings can be stored; that should be enough to accommodate any need;
// for a simple setting / flag, it may be overkill to store it as a vector of int, or QString,
// but it will suffice

class CGeneralDataBlock
{
public:
  CGeneralDataBlock();
  CGeneralDataBlock(const QString& name, int type = 0);
  CGeneralDataBlock(const CGeneralDataBlock& rhs);
  virtual ~CGeneralDataBlock();

  bool operator==(const CGeneralDataBlock& rhs) const;
  bool operator<(const CGeneralDataBlock& rhs) const;

  QString Name() const;
  void Name(const QString& name);

  int Size() const;

  int Type() const;
  void Type(int type);

  void getIntValues(std::vector<int>& values) const;
  void setIntValues(const std::vector<int>& values);

  void getDblValues(std::vector<double>& values) const;
  void setDblValues(const std::vector<double>& values);

  void getStrValues(std::vector<QString>& values) const;
  void setStrValues(const std::vector<QString>& values);

private:
  QString m_name;
  int m_type;
  std::vector<int> m_intValues;
  std::vector<double> m_dblValues;
  std::vector<QString> m_strValues;
};
