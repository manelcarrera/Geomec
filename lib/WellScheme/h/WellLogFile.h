// WellLogFile.h: interface for the CWellLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLLOGFILE_H__61D89191_293D_4145_A601_AB296AE650D9__INCLUDED_)
#define AFX_WELLLOGFILE_H__61D89191_293D_4145_A601_AB296AE650D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "WellSchemeInclude.h"
#include "FvTextFile.h"


namespace well{
class CWellLog;

class CWellLogFile : public CTextFile
{

  CDoubleQuantity::UNIT m_unit;
  CWellLog *m_pWellLog;
  typedef CTokenTemplate<CWellLogFile> TWellLogToken;

  double m_dSndCut;
  std::vector<double> m_vcTMD;
  std::vector<double> m_vcTVD;
  std::vector<double> m_vcCompressibility;
  std::vector<double> m_vcShalyness;

public:
  CWellLogFile(CWellLog *pWellLog, CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT);
  virtual ~CWellLogFile();

  bool ReadSandCutOff(TInputStream& stream, const QString& sToken);
  bool ReadData(TInputStream& stream, const QString& sToken);
  bool InitLog(TInputStream& stream, const QString& sToken);
  std::vector<double> TMD() const		{ return m_vcTMD; }
  std::vector<double> TVD() const		{ return m_vcTVD; } 
  std::vector<double> Comp() const	{ return m_vcCompressibility; }
  std::vector<double> Shaly() const	{ return m_vcShalyness; }
  double SndCut() const				{ return m_dSndCut; }

};

} // namespace well
#endif // !defined(AFX_WELLLOGFILE_H__61D89191_293D_4145_A601_AB296AE650D9__INCLUDED_)
