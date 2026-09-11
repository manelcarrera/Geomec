#ifndef IMPORTPETREL_H
#define IMPORTPETREL_H

#include <fstream>

#include <QString>

class CNewWellPathInput;
class IProgressBase;

#include "SingleQuantity.h"

class CImportPetrel
{
public:
  CImportPetrel
  ( const QString &strFileName
  , CNewWellPathInput *wellpathInput
  , IProgressBase &dlg
  , const CQuantity::UNIT& defaultLateralUnit
  , const CQuantity::UNIT& defaultDepthUnit
  );
  CImportPetrel
  ( const QString &strFileName
  , IProgressBase &dlg
  , const CQuantity::UNIT& defaultLateralUnit
  , const CQuantity::UNIT& defaultDepthUnit
  );
  ~CImportPetrel();
  bool Import();
  const QString &Message() const;
  typedef enum { e_unknown, e_si_unit, e_field_unit } t_unit;
  bool Warning();
  t_unit getLateralUnit() const;
  t_unit getDepthUnit() const;

  static bool hasPetrelHeader(char* const buffer);

private:
  bool ReadIdentifier();
  bool ReadHeaders();
  bool ReadTable();
  bool CheckColumnNames();
  bool FindColumnNumbers();
  bool CheckColumnNumbers();
  bool CheckCoordUnit( const QString &s_unit, const t_unit unit);
  bool CheckDepthUnit( const QString &s_unit, const t_unit unit);
  bool _CheckUnit
  ( const QString &s_unit
  , const CImportPetrel::t_unit unit
  , const QString &s_regex
  );
  void SetProgressSteps();
  void ProgressStep();

  static const size_t m_bufferSize;
  char * const m_cBuffer;
  size_t m_lineCounter;
  const QString &m_strFileName;
  CNewWellPathInput *m_wellpathInput;
  QString m_message;
  std::ifstream m_stream;
  t_unit m_coordUnit;
  t_unit m_depthUnit;
  int m_x_column;
  int m_y_column;
  int m_z_column;
  int m_column_count;
  bool m_warning;
  IProgressBase &m_progress;
  unsigned long m_filesize;
  unsigned int m_currentProgressStep;
  bool m_onlyReadHeaders;
};
#endif // IMPORTPETREL_H
