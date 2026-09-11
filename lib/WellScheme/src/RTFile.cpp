// RTFile.cpp: implementation of the CRTFile class.
//
//////////////////////////////////////////////////////////////////////

#include "RTFile.h"
#include "QuantityInclude.h"
#include "WellLog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {
CRTFile::CRTFile(CWellLog *pWellLog, CDoubleQuantity::UNIT unit /*= CDoubleQuantity::SI_UNIT*/)
    : m_pWellLog(pWellLog), m_unit(unit) {
  DefineComment("#");
  DefineToken(*(new TWellLogToken(*this, "BEGIN", &CRTFile::ReadData)));
  DefineToken(*(new TWellLogToken(*this, "END", &CRTFile::InitLog)));
}

CRTFile::~CRTFile() {}

bool CRTFile::ReadData(TInputStream &stream, const QString & /*sToken*/) {
  std::vector<double> values(3);
  stream.eatwhite();
  while (isdigit(stream.peek())) {
    for (int i = 0; i < 3; i++) {
      stream >> values[i];
      ItemRead();
    }

    CLengthQuantity l;
    values[0] = l.Convert(values[0], CDoubleQuantity::SI_UNIT, m_unit); // tmd
    values[1] = l.Convert(values[1], CDoubleQuantity::SI_UNIT, m_unit); // tvd
    values[2] = l.Convert(values[2], CDoubleQuantity::SI_UNIT, m_unit); // rel. displ.
    m_vcTMD.push_back(values[0]);
    m_vcTVD.push_back(values[1]);
    m_vcDeltaUv.push_back(values[2]);
  }

  return true;
}

bool CRTFile::InitLog(TInputStream & /*stream*/, const QString & /*sToken*/) {
  m_pWellLog->InitRadioactiveTags(TMD(), TVD(), RelDispl());
  ItemRead();

  return false;
}

} // namespace well
