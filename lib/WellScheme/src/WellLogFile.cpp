// WellLogFile.cpp: implementation of the CWellLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "QuantityInclude.h"
#include "WellLogFile.h"
#include "WellLog.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well{

CWellLogFile::CWellLogFile(CWellLog *pWellLog, CDoubleQuantity::UNIT unit): m_unit(unit), m_pWellLog(pWellLog)
{
  DefineComment("#");
  DefineToken(*(new TWellLogToken(*this, "Sand", &CWellLogFile::ReadSandCutOff)));
  DefineToken(*(new TWellLogToken(*this, "BEGIN", &CWellLogFile::ReadData)));
  DefineToken(*(new TWellLogToken(*this, "END", &CWellLogFile::InitLog)));
}

CWellLogFile::~CWellLogFile()
{

}


bool CWellLogFile::ReadSandCutOff(TInputStream& stream, const QString& /*sToken*/)
{
  QString sSandCutOff, sVc;
  stream >> sSandCutOff;
  stream >> sVc;
  if( sSandCutOff.toUpper() != "CUT-OFF" ||
    sVc.toUpper()         != "VC=" ) {
    throw CReadException("\"Sand Cut-Off Vc=\" expected");
  }
  
  stream >> m_dSndCut;
  ItemRead();
  
  return true;
}

bool CWellLogFile::ReadData(TInputStream& stream, const QString& /*sToken*/)
{
  while(stream.get() != '\n')
  {// go to end of line
  }

  
  std::vector<double> values(4);
  while(isdigit(stream.peek()))
  {
    for(int i = 0; i < 4; i++)
          stream >> values[i];
      
    ItemRead();

  		CLengthQuantity l;
    values[0] = l.Convert(values[0], CDoubleQuantity::SI_UNIT, m_unit); //tmd
    values[1] = l.Convert(values[1], CDoubleQuantity::SI_UNIT, m_unit); //tvd
    m_vcTMD.push_back(values[0]);
    m_vcTVD.push_back(values[1]);
      
    CCompressibilityQuantity c;
    values[2] = c.Convert(values[2], CDoubleQuantity::SI_UNIT, m_unit); //compressibility
    m_vcCompressibility.push_back(values[2]);
    m_vcShalyness.push_back(values[3]);
  }

  return true;
}

bool CWellLogFile::InitLog(TInputStream& /*stream*/, const QString& /*sToken*/)
{
  m_pWellLog->InitLog(TMD(), TVD(), Shaly(), Comp(), SndCut());
  return false;
}

}// namespace well