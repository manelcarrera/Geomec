#ifndef _LOGASCIISTANDARDFILE_H_
#define _LOGASCIISTANDARDFILE_H_

class CNewWellPath;
class CPointSet;

#include <map>
#include <vector>

#include <qstring.h>

#include "Value.h"

class CLogASCIIStandardFile {
  typedef enum { SEC_NONE = 0, SEC_UNKNOWN, SEC_VERSION, SEC_WELLINFO, SEC_CURVEINFO, SEC_CURVE } TSection;

  class CLasDataLine {
  public:
    CLasDataLine(const QString &strLine, int nLineNr);

    const QString &Mnemonic() const;
    const QString &Unit() const;
    const QString &Data() const;
    const QString &Description() const;

  private:
    QString m_strMnemonic;
    QString m_strUnit;
    QString m_strData;
    QString m_strDescription;
  };

  class CCurveDataLine {
  public:
    CCurveDataLine(const QString &strLine, int nLineNr, const CLogASCIIStandardFile &lasFile);

    const geo::CValue &Value(int nColumn) const;

  private:
    std::vector<geo::CValue> m_vcValues; // holds the converted (SI) values
  };

  class CException {
  public:
    CException(const QString &strError);
    CException(const QString &strError, int nLine);
    const QString &Error() const;

  private:
    QString m_strError;
  };

  class CUnitConverter {
  public:
    CUnitConverter(double dFactor);
    CUnitConverter(const CUnitConverter &rhs);
    virtual CUnitConverter *Clone() const = 0;
    virtual ~CUnitConverter();
    virtual geo::CValue Convert(double dSourceVal) const = 0;
    double Factor() const;

  private:
    double m_dFactor;
  };

  // returns Factor() * dSourceVal
  class CProportionalUnitConverter : public CUnitConverter {
  public:
    CProportionalUnitConverter(double dFactor);
    CProportionalUnitConverter(const CProportionalUnitConverter &rhs);
    virtual CProportionalUnitConverter *Clone() const;
    virtual geo::CValue Convert(double dSourceVal) const;
  };

  // returns Factor() / dSourceVal
  class CInverseUnitConverter : public CUnitConverter {
  public:
    CInverseUnitConverter(double dFactor);
    CInverseUnitConverter(const CInverseUnitConverter &rhs);
    virtual CInverseUnitConverter *Clone() const;
    virtual geo::CValue Convert(double dSourceVal) const;
  };

  class CColumnDef {
  public:
    // CColumnDef becomes the owner of the unit-converter object (if not NULL)
    CColumnDef(unsigned int uiValueType, unsigned int uiValueNameID, const QString &strMnemonic,
               const CUnitConverter *pUnitConverter);
    CColumnDef(const CColumnDef &rhs);
    ~CColumnDef(); // deletes the unitconverter!

    unsigned int ValueTypeID() const;
    const QString &ValueName() const;
    const QString &Mnemonic() const;
    geo::CValue ConvertValue(double dSourceVal) const;

  private:
    unsigned int m_uiValueType;
    QString m_strValueName;
    QString m_strMnemonic;
    const CUnitConverter *m_pUnitConverter;
  };

public:
  CLogASCIIStandardFile(const CNewWellPath &well);
  ~CLogASCIIStandardFile();

  bool Read(const QString &strFileName);
  void Close();
  const QString &Error() const;

  const geo::CValue &NullValue() const;
  char Delimiter() const;

  int ColumnSize() const;
  const CColumnDef &ColumnDef(int nColumn) const;

private:
  void Parse();
  bool IsComment(const char *text) const;
  bool IsSection(const char *text) const;
  void NewSection(const char *text);
  void ReadVersionData(const char *text);
  void ReadWellInfoData(const char *text);
  void ReadCurveInfoData(const char *text);
  void ReadCurveData(const char *text);
  const char *SkipWhiteSpace(const char *text) const;
  void RegisterValueType(unsigned int uiValueType, unsigned int uiValueNameID, const CLasDataLine &lasData, int nLineNr,
                         CUnitConverter *pUnitConverter);
  void ThrowUnrecognizedUnitException(const CLasDataLine &lasData, int nLineNr) const;

private:
  const CNewWellPath *m_pNewWellPath;
  FILE *m_pFile;
  int m_nLine;
  QString m_strError;
  TSection m_section;
  char m_delimiter;
  geo::CValue m_NullValue;
  CPointSet *m_pPointSet;
  int m_iDepthColumn;
  QString m_strLasWellName;

  typedef std::map<unsigned int, std::pair<QString, int>> TValueTypeMap; // from value-type ID to mnemonic and line nr
  TValueTypeMap m_mpValueTypes;

  typedef std::vector<CColumnDef> TColumnVec;
  TColumnVec m_vcColumns;
};

#endif // _LOGASCIISTANDARDFILE_H_
