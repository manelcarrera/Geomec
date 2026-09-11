#if !defined(_GVT123DIIMPORT_H_)
#define _GVT123DIIMPORT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QString>
#include <map>

class CGVT123DIImport {
public:
  CGVT123DIImport(void);
  ~CGVT123DIImport(void);

  bool Import(const QString &strFileName);

  int N_BIN() { return m_N_BIN.first; }
  int N_TRACK() { return m_N_TRACK.first; }
  const double &REAL_DELTA_X() { return m_REAL_DELTA_X.first; }
  const double &REAL_DELTA_Y() { return m_REAL_DELTA_Y.first; }
  const double &REAL_GEO_X() { return m_REAL_GEO_X.first; }
  const double &REAL_GEO_Y() { return m_REAL_GEO_Y.first; }
  const double &SKEW_ANGLE() { return m_SKEW_ANGLE.first; }
  int DELTA_BIN_NUM() { return m_DELTA_BIN_NUM; }
  int DELTA_TRACK_NUM() { return m_DELTA_TRACK_NUM; }
  int FIRST_BIN() { return m_FIRST_BIN; }
  int FIRST_TRACK() { return m_FIRST_TRACK; }
  const QString &ErrorMessage() { return m_strError; }

private:
  FILE *m_fp;
  int m_nLineNr;
  QString m_strError;
  typedef bool (CGVT123DIImport::*pReadFunc)(const QString &);
  typedef std::map<QString, pReadFunc> TKeyToFuncMap;
  TKeyToFuncMap m_KeyToFunction;

  std::pair<int, bool> m_N_BIN;
  std::pair<int, bool> m_N_TRACK;
  std::pair<double, bool> m_REAL_DELTA_X;
  std::pair<double, bool> m_REAL_DELTA_Y;
  std::pair<double, bool> m_REAL_GEO_X;
  std::pair<double, bool> m_REAL_GEO_Y;
  std::pair<double, bool> m_SKEW_ANGLE;
  bool m_bFeetUnit;
  int m_DELTA_BIN_NUM;
  int m_DELTA_TRACK_NUM;
  int m_FIRST_BIN;
  int m_FIRST_TRACK;

  bool Parse();
  bool ParseLine(const QString &strLine);
  bool ReadN_BIN(const QString &strLine);
  bool ReadN_TRACK(const QString &strLine);
  bool ReadREAL_DELTA_X(const QString &strLine);
  bool ReadREAL_DELTA_Y(const QString &strLine);
  bool ReadREAL_GEO_X(const QString &strLine);
  bool ReadREAL_GEO_Y(const QString &strLine);
  bool ReadSKEW_ANGLE(const QString &strLine);
  bool ReadUNITS(const QString &strLine);
  bool ReadDELTA_BIN_NUM(const QString &strLine);
  bool ReadDELTA_TRACK_NUM(const QString &strLine);
  bool ReadFIRST_BIN(const QString &strLine);
  bool ReadFIRST_TRACK(const QString &strLine);

  bool ReadIntegerValue(const QString &strLine, int &nVal);
  bool ReadDoubleValue(const QString &strLine, double &nVal);
  bool StringToDouble(const QString &strVal, double &dVal) const;
  bool StringToInt(const QString &strVal, int &iVal) const;

  bool CheckReadParameters();
  void ConvertParameters();
};

#endif