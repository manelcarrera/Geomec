#include "DCStrainLoad.h"
#include "DCLoadManager.h"
#include "IDCDianaRunner.h"

#include <cassert>
#include "lbcx.h"
#include "lbfl.h"

#include "IElement.h"
#include "Vector.h"

namespace dia {

CStrainLoad::CStrainLoad(CLoadCase& loadcase, const CTensor& values, const geo::IElement& element, bool writePrinciples)
: IStructuralLoad(loadcase, 0),
  m_Element(element),
  m_WritePrinciples(writePrinciples)
{
  m_vcTensors.push_back(values);
}


CStrainLoad::CStrainLoad(CLoadCase& loadcase, const std::vector<CTensor>& vcValues, const geo::IElement& element, bool writePrinciples)
: IStructuralLoad(loadcase, 0),
  m_Element(element),
  m_WritePrinciples(writePrinciples)
{
  assert(vcValues.size() == element.NrOfNodes());
  m_vcTensors.reserve(vcValues.size());
  for (size_t i = 0; i < vcValues.size(); ++i)
  m_vcTensors.push_back(vcValues[i]);
}




bool CStrainLoad::WriteFilos() const
{
  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if(idx < 0) idx = 0;
  ++idx;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t) (Element().Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t) (LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  if (m_WritePrinciples) // Principle values are in global coordinate system
  {

  ftn_double_t *pStrain = (ftn_double_t *)DiMalloc(3 * m_vcTensors.size() *  sizeof(ftn_double_t), "CStrainLoad::WriteFilos");
  ftn_int_t *pPrince = (ftn_int_t *)DiMalloc(2 * m_vcTensors.size() *  sizeof(ftn_int_t), "CStrainLoad::WriteFilos");

  size_t index = 0;

  std::vector<geo::CMatrix> vcStrains, vcDirs;
  vcStrains.reserve(m_vcTensors.size());
  vcDirs.reserve(m_vcTensors.size());

  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      vcStrains.push_back(m_vcTensors[i].GetPrincipalStr());
      vcDirs.push_back(m_vcTensors[i].GetPrincipalDir());
  }

  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      pStrain[index++] = -vcStrains[i].Value(0, 0);
  }
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      pStrain[index++] = -vcStrains[i].Value(1, 1);
  }
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      pStrain[index++] = -vcStrains[i].Value(2, 2);
  }

  PutItemLength("STRAIN", pStrain, index);

  index = 0;

  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      geo::CVector dir(vcDirs[i].Value(0, 0), vcDirs[i].Value(1, 0), vcDirs[i].Value(2, 0));
      pPrince[index++] = (ftn_int_t)Manager().Runner().AddDirection(dir);
  }
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
  {
      geo::CVector dir(vcDirs[i].Value(0, 1), vcDirs[i].Value(1, 1), vcDirs[i].Value(2, 1));
      pPrince[index++] = (ftn_int_t)Manager().Runner().AddDirection(dir);
  }

  PutItemLength("PRINCI", pPrince, index);

  DiFree(pStrain, "CStrainLoad::WriteFilos");
  DiFree(pPrince, "CStrainLoad::WriteFilos");

  }
  else // Diana's full tensor is in local coordinate system
  {

  ftn_double_t *pTensor = (ftn_double_t *)DiMalloc(6 * m_vcTensors.size() * sizeof(ftn_double_t), "CStrainLoad::WriteFilos");

  size_t index = 0;

  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].XX();
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].YY();
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].ZZ();
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].XY();
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].YZ();
  for (size_t i = 0; i < m_vcTensors.size(); ++i)
      pTensor[index++] = -1 * m_vcTensors[i].XZ();
  
  PutItemLength("STRAIN", pTensor, index);

  DiFree(pTensor, "CStrainLoad::WriteFilos");

  }

  PopDir();

  return true;
}

const geo::IElement& CStrainLoad::Element() const
{
  return m_Element;
}

} // namespace dia