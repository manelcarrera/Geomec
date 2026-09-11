// ResultData.cpp: implementation of the CResultData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ResultData.h"
#include "lbfl.h"
//#include "lbds.h"
//#include "lbcx.h"
#include "Materials.h"
#include "StressStrain.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

#define MAX_HEADER_LEN 255
#define COLWIDTH 14
#define COLSPERGAMMMA 4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultData::CResultData()
{
//	m_paAxialStrainStep = NULL;
//	m_paRadialStrainStep = NULL;
  m_paStressStrainStep = NULL;
  m_nExperiments = 0;
}

CResultData::~CResultData()
{
/*
  if(m_paAxialStrainStep)
  {
    delete[] m_paAxialStrainStep;
  }

  if(m_paRadialStrainStep)
  {
    delete[] m_paRadialStrainStep;
  }
*/
  if(m_paStressStrainStep)
  {
    delete[] m_paStressStrainStep;
  }
}

CResultData::CResultData(const CResultData &RD)
{
  if(RD.m_paStressStrainStep != NULL)
  {
    assert(RD.m_nExperiments != 0);
//		if(m_paAxialStrainStep != NULL) delete[] m_paAxialStrainStep;
    m_paStressStrainStep = new CStressStrainArray[RD.m_nExperiments];
    for(int j=0; j<RD.m_nExperiments; ++j)
    {
      m_paStressStrainStep[j].resize(RD.m_paStressStrainStep[j].size());
      for(size_t i=0; i<RD.m_paStressStrainStep[j].size(); ++i)
      {
        m_paStressStrainStep[j][i] = RD.m_paStressStrainStep[j][i];
      }
    }
  }

  m_nExperiments = RD.m_nExperiments;
  m_strLabel = RD.m_strLabel;
}

CResultData &CResultData::operator =(const CResultData &RD)
{
  if(RD.m_paStressStrainStep != NULL)
  {
    assert(RD.m_nExperiments != 0);
//		if(m_paAxialStrainStep != NULL) delete[] m_paAxialStrainStep;
    m_paStressStrainStep = new CStressStrainArray[RD.m_nExperiments];
    for(int j=0; j<RD.m_nExperiments; ++j)
    {
      m_paStressStrainStep[j].resize(RD.m_paStressStrainStep[j].size());
      for(size_t i=0; i<RD.m_paStressStrainStep[j].size(); ++i)
      {
        m_paStressStrainStep[j][i] = RD.m_paStressStrainStep[j][i];
      }
    }
  }

  m_nExperiments = RD.m_nExperiments;
  m_strLabel = RD.m_strLabel;

  return *this;
}

BOOL CResultData::ReadExperiment(FILE *fp, int iExperiment)
{
  char buf[MAX_HEADER_LEN];
  char *p;
  CString strHeader;
  strHeader.Format("EXPERIMENT %d", iExperiment + 1);

  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    if(strstr(buf, strHeader)) break;
  }

  // found header, now get the number of points
  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    strHeader = "Number of points";
    if(strstr(buf, strHeader)) break;
  }

  p = buf;
  while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
  p += strHeader.GetLength();
  while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
  char *q = p;
  while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
  *p = '\0';
  int nPoints = atoi(q);
  assert(nPoints > 0);
  if(nPoints <= 0) return FALSE;
  m_paStressStrainStep[iExperiment].resize(nPoints);

  // skip weight factor
  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    strHeader = "Weight factor";
    if(strstr(buf, strHeader)) break;
  }

  for(int i=0; i<nPoints; i++)
  {
    double dTime;
    double dAxStress;
    double dRadStress;
    double dAxStrain;
    double dRadStrain;
    fgets(buf, MAX_HEADER_LEN, fp);
    p = buf;
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
    if(*p == '!' || *p == '\0' || *p == '\n' || *p == '\r') continue; // comment or empty line
    if(!p || !*p) return FALSE;

    dTime = atof(p);

    while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
    if(!p || !*p) return FALSE;

    dAxStress = -atof(p);

    while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
    if(!p || !*p) return FALSE;

    dRadStress = -atof(p);

    while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
    if(!p || !*p) return FALSE;

    dAxStrain = -atof(p);

    while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
    if(!p || !*p) return FALSE;

    dRadStrain = -atof(p);

    m_paStressStrainStep[iExperiment][i] = CStressStrain(dAxStress, dAxStrain, dRadStress, dRadStrain, 0, dTime);
  }

  return TRUE;
}

BOOL CResultData::ReadFromFile(CString strFileName, CLibraryMaterial *pMat)
{
  FILE *fp;
  fp = fopen(strFileName, "r");
  if(!fp) return FALSE;

  char buf[MAX_HEADER_LEN];
  char *p = 0;
  char *q;

  m_nExperiments = 0;
  CString strMatModel;
  strMatModel = "MATERIAL MODEL";
  CString strHeader;

  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    if(strstr(buf, strMatModel)) break;
  }
    // ok, found the material model header, now get the material model name
  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    p = buf;
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
    if(*p == '!' || *p == '\0' || *p == '\n' || *p == '\r') continue; // comment or empty line
    break;
  }

  strHeader = "Material Type";
  p += strHeader.GetLength();
  while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
  q = p;
  while(q && *q && (*q != '\n' || *q == '\r')) q++;
  *q = '\0';

//  int nCalibrationGroup = (pMat->GroupSize() > 0 ? 0 : -1);
/*
  switch(pMat->MaterialModel())
  {
  case MM_LINEAR:
    nCalibrationGroup = 0;
    break;
  case MM_CAMCLAY:
  case MM_MOHRCOULOMB:
  case MM_MODIFIEDMOHRCOULOMB:
  case MM_CREEP:
    break;
  case MM_UNDRAINED:
    nCalibrationGroup = 0;
    break;
  default:
    assert(FALSE);
    return FALSE;
  }
*/
  // now get the parameters
  size_t n = pMat->ParameterSize();
  if(pMat->GroupSize() > 0)
  n = pMat->Group(0).ParameterSize();

  for(size_t i = 0; i < n && !feof(fp); i++)
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    bool bFound = false;
  size_t j;
  for(j = 0; j < n; ++j)
    {
      ml::CMatParam* pMatParam;
      if(pMat->GroupSize() > 0)
    pMatParam = &pMat->Group(0).Parameter(j);
      else
    pMatParam = &pMat->Parameter(j);
      if(strstr(buf, pMatParam->Name().toStdString().c_str()))
      {
        p = buf;
        while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
        p += pMatParam->Name().length();
        while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
        q = p;
        while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
        *p = '\0';

        double dNew;

        dNew = atof(q);
        if(fabs(dNew - pMatParam->Value()) < 1e-8 * dNew)
          pMatParam->Value(dNew);

        bFound = true;
        break;
      }
    }

    if(!bFound) break;
  }

  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    strHeader = "EXPERIMENTS";
    if(strstr(buf, strHeader)) break;
  }

  while(!feof(fp))
  {
    fgets(buf, MAX_HEADER_LEN, fp);
    p = buf;
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
    if(*p == '!' || *p == '\0' || *p == '\n' || *p == '\r') continue; // comment or empty line
    break;
  }

  strHeader = "Number of experiments";
  p += strHeader.GetLength();
  while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
  q = p;
  while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
  *p = '\0';
  m_nExperiments = atoi(q);

  if(m_nExperiments <= 0)
  {
    m_nExperiments = 0;
    fclose(fp);
    return FALSE;
  }

  if(m_paStressStrainStep != NULL) delete[] m_paStressStrainStep;
  m_paStressStrainStep = new CStressStrainArray[m_nExperiments];

  for(int i=0; i<m_nExperiments; i++)
  {
    if(!ReadExperiment(fp, i))
    {
      fclose(fp);
      delete[] m_paStressStrainStep;
      m_paStressStrainStep = NULL;
      m_nExperiments = 0;
      return FALSE;
    }
  }
/*
  CString strColumn;
  strColumn = getStringTableEntry(IDS_COLUMN).toStdString().c_str();
  char *p;

  {
    // start counting gammas
    char buf[MAX_HEADER_LEN];

    while(!feof(fp))
    {
      fgets(buf, MAX_HEADER_LEN, fp);
      p = buf;
      while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
      if(*p == '!' || *p == '\0' || *p == '\n' || *p == '\r') continue; // comment or empty line
      if(!strstr(buf, strColumn)) break;
      ++m_nExperiments;
    }
  }

  if(!m_nExperiments) return FALSE;

  rewind(fp);
  int siz = COLSPERGAMMMA * COLWIDTH * m_nExperiments;
  siz = max(siz, MAX_HEADER_LEN);
  assert(siz > 0);
  char *buf = (char *) malloc(siz * sizeof(char));

  if(m_paStressStrainStep != NULL) delete[] m_paStressStrainStep;
  m_paStressStrainStep = new CStressStrainArray[m_nExperiments];

  // dRead:
  // 0: effective axial stress
  // 1: effective radial stress
  // 2: axial strain
  // 3: radial strain
  double dRead[4];

  while(!feof(fp))
  {
    fgets(buf, siz, fp);
    if(strstr(buf, strColumn)) continue; // skip heading lines
    p = buf;
    while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip leading spaces
    if(*p == '!' || *p == '\0' || *p == '\n' || *p == '\r') continue; // comment or empty line

    for(int i=0; i<m_nExperiments; ++i)
    {
      if(!p || !*p) return FALSE;

      dRead[0] = atof(p);

      while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
      while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
      if(!p || !*p) return FALSE;

      dRead[1] = atof(p);

      while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
      while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
      if(!p || !*p) return FALSE;

      dRead[2] = atof(p);

      while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
      while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces
      if(!p || !*p) return FALSE;

      dRead[3] = atof(p);

      while(p && *p && *p != ' ' && *p != '\t') p++; // skip to next spaces
      while(p && *p && (*p == ' ' || *p == '\t')) p++; // skip next spaces

      m_paStressStrainStep[i].Add(CStressStrain(dRead[0], dRead[2], dRead[1], dRead[3]));
    }
  }

  free(buf);
*/
  fclose(fp);

  return TRUE;
}

BOOL CResultData::ReadFromFilos()
{
  int nNoOfSteps;
  ftn_int_t inq_ind;
  
  PushDir();
    assert(XistIndexed("/MATCAL", 0));
    ChangeDir("/MATCAL");
  
    m_nExperiments = Inquire("SIMULA", "DIM");
  if(m_nExperiments <= 0)
  {
      PopDir();
      return FALSE;
  }

    if(m_paStressStrainStep != NULL) delete[] m_paStressStrainStep;
    m_paStressStrainStep = new CStressStrainArray[m_nExperiments];

    for(int i = 0; i < m_nExperiments; i++) // filos is 1 based
    {
      PushDir();
        inq_ind = i + 1;

        assert(XistIndexed("SIMULA/", &inq_ind));
        ChangeIndexedDir("SIMULA/", &inq_ind);

        nNoOfSteps = Inquire("STEP", "DIM");
        m_paStressStrainStep[i].resize(nNoOfSteps);
        double dSAx, dSRad;
        for(int j = 0; j < nNoOfSteps; j++)
        {
          PushDir();
            inq_ind = j + 1;
            assert(XistIndexed("STEP/", &inq_ind));
            ChangeIndexedDir("STEP/", &inq_ind);

            ftn_double_t stress[2];
            GetItemLength("STRESS", stress, 2);

            dSAx = -1.0 * stress[0];
            dSRad = -1.0 * stress[1];

            m_paStressStrainStep[i][j].m_dAxialStress = dSAx;
            m_paStressStrainStep[i][j].m_dRadialStress = dSRad;
            m_paStressStrainStep[i][j].m_dP = (dSAx + 2 * dSRad) / 3;
            m_paStressStrainStep[i][j].m_dQ = sqrt(dSAx * dSAx + dSRad * dSRad - 2 * dSAx * dSRad);
          PopDir();
        }
      PopDir();

      PushDir();
        inq_ind = i + 1;
      
        assert(XistIndexed("EXPERI/", &inq_ind));
        ChangeIndexedDir("EXPERI/", &inq_ind);

        nNoOfSteps = Inquire("STEP", "DIM");
        for(int k = 0; k < nNoOfSteps; k++)
        {
          PushDir();
            inq_ind = k + 1;
            assert(XistIndexed("STEP/", &inq_ind));
            ChangeIndexedDir("STEP/", &inq_ind);

            ftn_double_t strain[2];
            GetItemLength("STRAIN", strain, 2);

      ftn_double_t pressure;
      GetItem("PRESSU", &pressure);

            m_paStressStrainStep[i][k].m_dAxialStrain = -1.0 * strain[0];
            m_paStressStrainStep[i][k].m_dRadialStrain = -1.0 * strain[1];
      m_paStressStrainStep[i][k].m_dPressure = pressure;
          PopDir();
        }
      PopDir();
    }

  PopDir();

  return TRUE;
}


CStressStrainArray const *CResultData::GetStressStrainSteps() const
{
  return m_paStressStrainStep;
}

void CResultData::adjustCalculatedData(int nSelected, const int* piSelected)
{
  if (m_paStressStrainStep != 0)
  {
  for (int i = 0; i < nSelected; ++i)
  {
      if (piSelected[i] < (m_nExperiments - 1))
      {
    std::swap(m_paStressStrainStep[piSelected[i]],
          m_paStressStrainStep[m_nExperiments - 1]);
      }

      m_paStressStrainStep[m_nExperiments - 1].resize(0);

      --m_nExperiments;
  }
  }
}

bool CResultData::GoSubSet(const int *aIndices, int nIndices)
{
  if(!nIndices)
  {
    delete[] m_paStressStrainStep;
    m_paStressStrainStep = NULL;
    m_nExperiments = 0;
    return TRUE;
  }

  int i;
  for(i=0; i<nIndices; ++i)
  {
    if(aIndices[i] > m_nExperiments) return FALSE;
  }

  assert(m_paStressStrainStep != NULL);

  CStressStrainArray *pStressStrain = new CStressStrainArray[nIndices];

  for(i=0; i<nIndices; ++i)
  {
    pStressStrain[i].resize(m_paStressStrainStep[aIndices[i]].size());

    for(size_t j=0; j<pStressStrain[i].size(); ++j)
    {
      pStressStrain[i][j] = m_paStressStrainStep[aIndices[i]][j];
    }
  }

  delete[] m_paStressStrainStep;

  m_paStressStrainStep = pStressStrain;
  m_nExperiments = nIndices;

  return TRUE;
}
