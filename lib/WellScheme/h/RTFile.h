// RTFile.h: interface for the CRTFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTFILE_H__2424EB22_8254_42CF_88E1_A1681FB252CA__INCLUDED_)
#define AFX_RTFILE_H__2424EB22_8254_42CF_88E1_A1681FB252CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FvTextFile.h"
#include "WellSchemeInclude.h"

namespace well {
class CWellLog;
class CRTFile : public CTextFile {
  std::vector<double> m_vcTMD;
  std::vector<double> m_vcTVD;
  std::vector<double> m_vcDeltaUv;
  typedef CTokenTemplate<CRTFile> TWellLogToken;

  CWellLog *m_pWellLog;
  CDoubleQuantity::UNIT m_unit;

public:
  CRTFile(CWellLog *pWellLog, CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT);
  virtual ~CRTFile();

  bool ReadData(TInputStream &stream, const QString &sToken);
  bool InitLog(TInputStream &stream, const QString &sToken);

  std::vector<double> TMD() { return m_vcTMD; }
  std::vector<double> TVD() { return m_vcTVD; }
  std::vector<double> RelDispl() { return m_vcDeltaUv; }
};
} // namespace well
#endif // !defined(AFX_RTFILE_H__2424EB22_8254_42CF_88E1_A1681FB252CA__INCLUDED_)
