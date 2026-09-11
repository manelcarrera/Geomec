// WellLog.cpp: implementation of the CWellLog class.
//
//////////////////////////////////////////////////////////////////////
#include <map>
#include <vector>
#include <cmath>
#include "WellLog.h"
#include "wellpathbase.h"
#include "WellSection.h"
#include "WellPoint.h"



namespace well {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWellLog::CWellLog(CWellPathBase *pWellPath, IWellSection *pSection)
: m_pWellPath(pWellPath), m_pSection(pSection), m_dVc(0), 
  m_point_log_tmd_list(*pWellPath, true, false),
  m_point_avax_rat_list(*pWellPath, true, true),
  m_compress_map("Compressibility", m_point_log_tmd_list, 0),
  m_modul_map("Modulation", m_point_log_tmd_list, 0),
  m_avax_map("Average axial strain", m_point_avax_rat_list, 0)
{
}

CWellLog::~CWellLog()
{

}

CWellLog::CWellLog()
: m_pWellPath(0), m_pSection(0), m_dVc(0),
  m_compress_map("Compressibility", m_point_log_tmd_list, 0),
  m_modul_map("Modulation", m_point_log_tmd_list, 0),
  m_avax_map("Average axial strain", m_point_avax_rat_list, 0)
{
  m_point_avax_rat_list.AllowDuplicateTMD(true);
}

CWellLog::CWellLog(const CWellLog &welllog):
  m_compress_map("Compressibility", m_point_log_tmd_list, 0),
  m_modul_map("Modulation", m_point_log_tmd_list, 0),
  m_avax_map("Average axial strain", m_point_avax_rat_list, 0)
{
  m_pWellPath = welllog.WellPath();
  m_point_avax_rat_list.WellPath(const_cast<CWellPathBase*>(welllog.WellPath()));
  m_point_avax_rat_list.AllowDuplicateTMD(true);
  m_point_log_tmd_list.WellPath(const_cast<CWellPathBase*>(welllog.WellPath()));
  m_pSection = welllog.WellSection();
  if(welllog.LogPointSize() > 0)
    InitLog(welllog.m_vcOriginalTVD, welllog.m_vcOriginalTVD, welllog.m_vcShalyness, welllog.m_vcCompressibility,
    welllog.m_dVc);
  if(welllog.RTagSize() > 0)
    InitRadioactiveTags(welllog.m_vcRTTMD, welllog.m_vcRTTVD, welllog.m_vcRTdeltaUv);
}

void CWellLog::InitLog(const std::vector<double> &vcTMD, const std::vector<double> &vcTVD, 
         const std::vector<double> &vcShalyness, const std::vector<double> &vcCompressibility, 
         const double dSandCutOff)
{
  assert(vcTMD.size() > 1);
  m_vcOriginalTMD.clear();
  m_vcOriginalTVD.clear();
  m_vcShalyness.clear();
  m_vcCompressibility.clear();

#ifdef DEBUG
  size_t size = vcTMD.size();
  assert(vcTVD.size() == size && vcShalyness.size() == size && vcCompressibility.size() == size);
#endif
  for(size_t i = 0; i < vcTMD.size(); i++)
  {
    m_vcOriginalTMD.push_back(geo::CValue(vcTMD[i]));
  }
  for(size_t i = 0; i < vcTVD.size(); i++)
  {
    m_vcOriginalTVD.push_back(geo::CValue(vcTVD[i]));	
  }
  for(size_t i = 0; i < vcShalyness.size(); i++)
  {
    m_vcShalyness.push_back(geo::CValue(vcShalyness[i]));
  }
  for(size_t i = 0; i < vcCompressibility.size(); i++)
  {
    m_vcCompressibility.push_back(geo::CValue(vcCompressibility[i]));
  }
    
  m_dVc = dSandCutOff;
  StretchLogData();
  ExtractSandLayers();
  ExtractNonSandLayers();
  
  FillLogPointList();
  FillCompressMap();

  if(RTagSize() > 2)
  {
    CalculateAxialStrains();
    AvgCompRTIntervals();
    CalculateModulation();
    FillModulMap();
  }
}

void CWellLog::InitLog(const std::vector<geo::CValue> &vcTMD, const std::vector<geo::CValue> &vcTVD,
         const std::vector<geo::CValue> &vcShalyness, const std::vector<geo::CValue> &vcCompressibility,
         const double dSandCutOff)
{
  assert(vcTMD.size() > 1);
#ifdef DEBUG
  size_t size = vcTMD.size();
  assert(vcTVD.size() == size && vcShalyness.size() == size && vcCompressibility.size() == size);
#endif
  
  m_vcOriginalTMD = vcTMD;
  m_vcOriginalTVD = vcTVD;
  m_vcShalyness = vcShalyness;
  m_vcCompressibility = vcCompressibility;
  m_dVc = dSandCutOff;
  StretchLogData();
  ExtractSandLayers();
  ExtractNonSandLayers();

  FillLogPointList();
  FillCompressMap();
  
  if(RTagSize() > 2)
  {
    CalculateAxialStrains();
    AvgCompRTIntervals();
    CalculateModulation();
    FillModulMap();
  }
}


void CWellLog::InitRadioactiveTags(const std::vector<geo::CValue> &vcTMD, const std::vector<geo::CValue> &vcTVD,
               const std::vector<geo::CValue> &vcRelDispl)
{
  assert(vcTMD.size() > 1);
#ifdef DEBUG
  size_t size = vcTMD.size();
  assert(vcTVD.size() == size && vcRelDispl.size() == size);
#endif
  
  m_vcRTTMD = vcTMD;
  m_vcRTTVD = vcTVD;
  m_vcRTdeltaUv = vcRelDispl;
  CalculateAxialStrains();

  FillAvAxPointList();
  FillAvAxValueMap();
  
  if(LogPointSize() > 1)
  {
    AvgCompRTIntervals();
    CalculateModulation();
    FillModulMap();
  }
}

void CWellLog::InitRadioactiveTags(const std::vector<double> &vcTMD, const std::vector<double> &vcTVD,
                   const std::vector<double> &vcRelDispl)
{
  assert(vcTMD.size() > 1);

  m_vcRTTMD.clear();
  m_vcRTTVD.clear();
  m_vcRTdeltaUv.clear();
#ifdef DEBUG
  size_t size = vcTMD.size();
  assert(vcTVD.size() == size && vcRelDispl.size() == size);
#endif
  for(size_t i = 0; i < vcTMD.size(); i++)
  {
    m_vcRTTMD.push_back(geo::CValue(vcTMD[i]));
  }
  for(size_t i = 0; i < vcTVD.size(); i++)
  {
    m_vcRTTVD.push_back(geo::CValue(vcTVD[i]));
  }
  for(size_t i = 0; i < vcRelDispl.size(); i++)
  {
    m_vcRTdeltaUv.push_back(geo::CValue(vcRelDispl[i]));
  }

  CalculateAxialStrains();

  FillAvAxPointList();
  FillAvAxValueMap();
  
  if(LogPointSize() > 1)
  {
    AvgCompRTIntervals();
    CalculateModulation();
    FillModulMap();
  }

}

void CWellLog::StretchLogData()
{
  assert(m_vcOriginalTMD.size() > 1);
  assert(m_vcOriginalTVD.size() > 1);
  m_vcStretchedTMD.clear();
//	m_vcStretchedTVD.clear();
  
  double dConversion, dNewValue;
  double dNewBegin = m_pSection->Top().TMD().Value();
  double dOldBegin = m_vcOriginalTMD[0].Value();
  double dDelta_old = m_vcOriginalTMD[m_vcOriginalTMD.size() - 1].Value() - dOldBegin;
  assert(dDelta_old > 0.0);
  double dDelta_new = m_pSection->Bottom().TMD().Value() - dNewBegin;

  // The TMDs...
  m_vcStretchedTMD.push_back(geo::CValue(dNewBegin));
  
  for(size_t i = 1; i < m_vcOriginalTMD.size(); i++)
  {
    dConversion = (m_vcOriginalTMD[i].Value() - dOldBegin) / dDelta_old;

    dNewValue = (dConversion * dDelta_new) + dNewBegin;
    m_vcStretchedTMD.push_back(geo::CValue(dNewValue));
  }

//	assert(m_vcStretchedTMD.size() == m_vcStretchedTVD.size());
}

void CWellLog::CalculateAxialStrains()
{
  assert(m_vcRTTVD.size() == m_vcRTdeltaUv.size());
  assert(m_vcRTTVD.size() > 1);
  assert(m_vcRTdeltaUv.size() > 1);

  m_vcRTStrains.clear();

  m_vcRTStrains.push_back(-1.0); // the first value can not be calculated, so it is set to -1
  double dStrain, denom;

  for(size_t i = 1; i < m_vcRTdeltaUv.size(); i++)
  {
    denom = SandDelta(m_vcRTTMD[i - 1].Value(), m_vcRTTMD[i].Value());
    
    if(denom < EPS)
    {
      m_vcRTStrains.push_back(0.0);
      continue;
    }

    assert(denom > 0.0);
    dStrain = m_vcRTdeltaUv[i].Value() / denom;
    m_vcRTStrains.push_back(dStrain);
  }
}


// This function finds the sandlayers. A sand layer is defined as a layer of minimal 1.0 m thick where the 
// shalyness is smaller than the sand-cut off value. A shale layer is defined as a layer of minimal 1.0 m thick
// where the shalyness is larger than the sand-cut off value;
void CWellLog::ExtractSandLayers()
{
  typedef std::pair<int, bool> TBorder; // describes a transition from a shale to sand layer and vice versa...
  // true == sand, else shale. int is index BELOW the transition.

  std::vector<TBorder> vcBorders;
  bool bCurrentMaterial = false;

  m_vcSandLayers.clear();

  // find all the borders...
  for(int i = 0; i < LogPointSize(); i++)
  {
    if(i == 0)
    {
      if(m_vcShalyness[i].Value() < m_dVc)
      {
        vcBorders.push_back(TBorder(-1, true));// beginning is set -1;
        bCurrentMaterial = true;
      }
      else
        vcBorders.push_back(TBorder(-1, false));

      continue;
    }

    if(m_vcShalyness[i].Value() > m_dVc && bCurrentMaterial)
    {
      vcBorders.push_back(TBorder(i, false));
      bCurrentMaterial = false;
      continue;
    }

    if(m_vcShalyness[i].Value() < m_dVc && !bCurrentMaterial)
    {
      vcBorders.push_back(TBorder(i, true));
      bCurrentMaterial = true;
      continue;
    }
  }

  std::vector<TBorder> vcTrueBorders;
  if(vcBorders.size() > 1)
  {
    // remove all borders that describe a layer smaller than -1.0 m
    double dBottom = Depth(STRETCHED_TMD, LogPointSize() - 1);
    double dTop = Depth(STRETCHED_TMD, 0);
    
    std::set<size_t> vcUseless;
    for(size_t j = 0; j < vcBorders.size(); j++) 
    {
      if(j == vcBorders.size() - 1)
      {
        if(dBottom - Depth(STRETCHED_TMD, vcBorders[j].first) < 1.0)
          vcUseless.insert(j);
        continue;
      }

      if(j == 0)
      {
        if(Depth(STRETCHED_TMD, vcBorders[j + 1].first) - dTop < 1.0)
          vcUseless.insert(j);
        continue;
      }

      if(Depth(STRETCHED_TMD, vcBorders[j + 1].first) - Depth(STRETCHED_TMD, vcBorders[j].first) < 1.0)
      {
        vcUseless.insert(j);
        continue;
      }
    }

    // create a new vector with the only usefull borders...
  std::pair<std::set<size_t>::iterator, bool> pr;
    for(size_t k = 0; k < vcBorders.size(); k++)
    {
      pr = vcUseless.insert(k);
      if(pr.second)
        vcTrueBorders.push_back(vcBorders[k]);
    }
  }
  else 
  { // trivial cases...
    if(vcBorders[0].second)
    {
      m_vcSandLayers.push_back(TSandLayer(0, LogPointSize() - 1));
      return; // everything is sand...
    }
    else
    {
      m_vcSandLayers.clear();
      return; // no sand at all...
    }
  }

  // Now create the sandlayers...
  size_t top;
  for(size_t l = 0; l < vcTrueBorders.size(); l++)
  {
    if(!vcTrueBorders[l].second)
      continue;
    else
    {
      top = l;
      int nTopSand;
      int nBottomSand;

      while(vcTrueBorders[l].second)
      {
        l++;
        if(l == vcTrueBorders.size())
        {
          nTopSand = vcTrueBorders[top].first;
          if(nTopSand == -1)
            nTopSand = 0;
          nBottomSand = LogPointSize() - 1;
          m_vcSandLayers.push_back(TSandLayer(nTopSand, nBottomSand));
          return; // must be last sandlayer....
        }
      }

      nTopSand = vcTrueBorders[top].first;
      if(nTopSand == -1)
        nTopSand = 0;
      nBottomSand = vcTrueBorders[l].first - 1;

      m_vcSandLayers.push_back(TSandLayer(nTopSand, nBottomSand));
      continue;
    }
  }
}

void CWellLog::ExtractNonSandLayers()
{
  m_vcNonSandLayers.clear();
  if(m_vcSandLayers.size() == 0)
  {
    m_vcNonSandLayers.push_back(TSandLayer(0, LogPointSize() - 1));
    return;
  }


  int nBegin, nEnd;
  TSandLayer sand, next_sand;
  sand = SandLayer(0);
  
  // The first non-sandlayer....
  nBegin = 0;
  nEnd = sand.first;
  if(nBegin != nEnd)
    m_vcNonSandLayers.push_back(TSandLayer(nBegin, nEnd));
  
  for(int i = 0; i < SandLayerSize() - 1; i++)
  {
    sand = SandLayer(i);
    next_sand = SandLayer(i + 1);
    m_vcNonSandLayers.push_back(TSandLayer(sand.second, next_sand.first));
  }

  // The last non-sandlayer....
  nBegin = SandLayer(SandLayerSize() - 1).second;
  nEnd = LogPointSize() - 1;
  if(nBegin != nEnd)
    m_vcNonSandLayers.push_back(TSandLayer(nBegin ,nEnd));

}

size_t CWellLog::NonSandLayerSize() const
{
  return m_vcNonSandLayers.size();
}

CWellLog::TSandLayer CWellLog::NonSandLayer(size_t nIndex) const
{
  assert(nIndex < m_vcNonSandLayers.size());

  return m_vcNonSandLayers[nIndex];
}


double CWellLog::MaxDeltaTVD() const
{
  double dMin, dMax;
  dMin = dMax = Depth(STRETCHED_TVD, 0);

  double dDepth;
  for(int i = 1; i < LogPointSize(); i++)
  {
    dDepth = Depth(STRETCHED_TVD, i);
    if(dDepth < dMin)
    {
      dMin = dDepth;
      continue;
    }
    if(dDepth > dMax)
    {
      dMax = dDepth;
      continue;
    }
  }

  return dMax - dMin;
}

// function returns the distance between dStartTMD and dEndTMD that actuallly passes through sand;
double CWellLog::SandDelta(const double &dStartTMD, const double &dEndTMD) const
{
  assert(dEndTMD > dStartTMD);

  if(LogPointSize() == 0)
    return dEndTMD - dStartTMD; // no log data yet, assume everything is sand.

  if(SandLayerSize() == 0)
    return 0.0; // no sand at all...

  int i;
  double dTotal;
  std::pair<int, bool> prStart = NearestSandLayer(dStartTMD);
  std::pair<int, bool> prEnd = NearestSandLayer(dEndTMD);

  if(prStart.first == prEnd.first && prStart.second && prEnd.second)
    return dEndTMD - dStartTMD; // in same sandlayer

  if(prStart.first == prEnd.first && !prStart.second && !prEnd.second)
    return 0.0; // both in the same non-sandlayer

  if(prStart.first == prEnd.first && prStart.second && !prEnd.second)
    return BottomSand(prEnd.first, STRETCHED_TMD) - dStartTMD; // start in sand end below the same sand.

  if(prStart.first != prEnd.first && prStart.second && prEnd.second)
  { // start and end are both in a sand, not the same.
    dTotal = 0.0;

    for(i = prStart.first; i <= prEnd.first; i++)
    {
      if(i == prStart.first)
        dTotal += BottomSand(i, STRETCHED_TMD) - dStartTMD;
      else if(i == prEnd.first)
        dTotal += dEndTMD - TopSand(i, STRETCHED_TMD);
      else
        dTotal += BottomSand(i, STRETCHED_TMD) - TopSand(i, STRETCHED_TMD);
    }

    return dTotal;
  }

  if(prStart.first != prEnd.first && prStart.second && !prEnd.second)
  { // start in a sand, end below a sand (not the same)
    dTotal = 0.0;
    for(i = prStart.first; i <= prEnd.first; i++)
    {
      if(i == prStart.first)
        dTotal += BottomSand(i, STRETCHED_TMD) - dStartTMD;
      else
        dTotal += BottomSand(i, STRETCHED_TMD) - TopSand(i, STRETCHED_TMD);
    }

    return dTotal;
  }

  if(prStart.first != prEnd.first && !prStart.second && prEnd.second)
  { // start in a non-sand, end in a sand
    if(prStart.first + 1 == prEnd.first)
      return dEndTMD - TopSand(prEnd.first, STRETCHED_TMD);

    dTotal = 0.0;

    for(i = prStart.first + 1; i <= prEnd.first; i++)
    {
      if(i == prEnd.first)
        dTotal += dEndTMD - TopSand(i, STRETCHED_TMD);
      else
        dTotal += BottomSand(i, STRETCHED_TMD) - TopSand(i, STRETCHED_TMD);
    }

    return dTotal;
  }

  if(prStart.first == -1 && prEnd.first == -1)
    return 0.0; // start and end above all sandlayers.

  if(prStart.first == -1 && prEnd.second)
  { // start above all sandlayers, end in a sandlayer.
    dTotal = 0.0;

    for(i = 0; i <= prEnd.first; i++)
    {
      if(i == prEnd.first)
        dTotal += dEndTMD - TopSand(i, STRETCHED_TMD);
      else
        dTotal += BottomSand(i, STRETCHED_TMD) - TopSand(i, STRETCHED_TMD);
    }
    
    return dTotal;
  }

  if(prStart.first == -1 && !prEnd.second)
  { // start above all sandlayers, end in non-sandlayer
    dTotal = 0.0;

    for(i = 0; i <= prEnd.first; i++)
    {
      dTotal += BottomSand(i, STRETCHED_TMD) - TopSand(i, STRETCHED_TMD);
    }

    return dTotal;
  }
  
  assert(false); // situation should be handled by now...
  return -1.0;
}

// function returns an index to the sandlayer closest to (above) the TMD and a bool indicating of the TMD is
// inside the sandlayer. If the index is -1 the TMD is above the top sand layer.
std::pair<int, bool> CWellLog::NearestSandLayer(const double &TMD) const
{
  double dTop, dBottom;
  
  assert(SandLayerSize() > 0);

  for(int i = 0; i < SandLayerSize(); i++)
  {
    if(InsideSand(i, TMD))
      return std::pair<int, bool>(i, true);

    if(i == SandLayerSize() - 1) // the TMD is below the lowest sandlayer.
      return std::pair<int, bool>(i, false);
    
    dTop = TopSand(i + 1, STRETCHED_TMD); //  top of the next layer.
    dBottom = BottomSand(i, STRETCHED_TMD); // bottom of the current layer.

    if(dTop > TMD && dBottom < TMD)
      return std::pair<int, bool>(i, false);
    if(dBottom > TMD) // TMD is above the first layer.
      return std::pair<int, bool>(-1, false);
  }


  assert(false);
  std::pair<int, bool> pr;
  return pr;
}

double CWellLog::TopSand(int nSandIndex, eDepthType type) const
{
  assert(nSandIndex >= 0 && nSandIndex < m_vcSandLayers.size());
  return Depth(type, m_vcSandLayers[nSandIndex].first);
}

double CWellLog::BottomSand(int nSandIndex, eDepthType type) const
{
  assert(nSandIndex >= 0 && nSandIndex < m_vcSandLayers.size());
  return Depth(type, m_vcSandLayers[nSandIndex].second);
}

bool CWellLog::InsideSand(int nSandIndex, const double &TMD) const
{
  assert(nSandIndex >= 0 && nSandIndex < m_vcSandLayers.size());
  if(TMD >= Depth(STRETCHED_TMD, m_vcSandLayers[nSandIndex].first) && 
     TMD <= Depth(STRETCHED_TMD, m_vcSandLayers[nSandIndex].second))
     return true;

  return false;
}

int CWellLog::RTagSize() const
{
  return m_vcRTTMD.size();
}

int CWellLog::SandLayerSize() const
{
  return m_vcSandLayers.size();
}

CWellLog::TSandLayer CWellLog::SandLayer(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcSandLayers.size());
  return m_vcSandLayers[nIndex];
}



// average compressibility for the whole log. Compressibility is only taken into account when inside a sand.
// In the end the total depth is actually the total vertical depth!! Not jus the depths of the sands!
double CWellLog::TotalAverageCompressibility() const
{
  double nom, denom, delta;
  int first, second;

  nom = 0.0;
  denom = 0.0;

  for(size_t i = 0; i < m_vcSandLayers.size(); i++)
  {
    first = m_vcSandLayers[i].first;
    second = m_vcSandLayers[i].second;
    for(int j = first; j <= second; j++)
    {
      //if(j == first)
      if(j == 0)
        delta = fabs((Depth(STRETCHED_TVD, j + 1) - Depth(STRETCHED_TVD, j)) / 2.0);
      //else if(j == second)
      else if(j == LogPointSize() - 1)
        delta = fabs((Depth(STRETCHED_TVD, j) - Depth(STRETCHED_TVD, j - 1)) / 2.0);
      else
        delta = fabs((-1.0 * (Depth(STRETCHED_TVD, j - 1)) + Depth(STRETCHED_TVD, j + 1)) / 2.0);

      nom += getCompressibility(j) * delta;
    }
  }

  denom = MaxDeltaTVD();
  assert(denom > 0.0);
  return nom/denom;
}


// The average compressibility is calculated for sandlayers only. The delta TMD is also only calculated for 
// sands. (in TotalAverageCompressibility() the delta TMD is calculated using the shales as well)
double CWellLog::AverageCompressibility(const double &dStartTMD, const double &dEndTMD) const
{
  assert(dStartTMD < dEndTMD);
  std::pair<int, int> prIndices1 = NearestIndices(dStartTMD);
  std::pair<int, int> prIndices2 = NearestIndices(dEndTMD);
  std::pair<double, double> Components;

  if(prIndices1.first == prIndices1.second && prIndices2.first == prIndices2.second)
  {
    Components = AvgCompComponents(prIndices1.first, prIndices2.first, STRETCHED_TMD);
    return Components.first / Components.second;
  }

  if((prIndices1.first == -1 && prIndices2.first == -1) ||
     (prIndices1.second == -1 && prIndices2.second == -1))
     return 0.0; // either both indices above or below all other indices

  if(prIndices1.first == -1 && prIndices2.second == -1)
  {
    Components = AvgCompComponents(0, LogPointSize() - 1, STRETCHED_TMD); // All sandlayers....
    return Components.first / Components.second;
  }

  if(prIndices1.first == -1 && prIndices2.first == 0 && prIndices2.second == 0)
    return 0.0; // start is above all indices and end is exactly on first index

  if(prIndices1.first == LogPointSize() - 1 && prIndices1.second == LogPointSize() -1 && prIndices2.second == -1)
    return 0.0; // start is exactly the last index and the end is below all indices

  double nom = 0.0;
  double denom = 0.0;
  double dBeginValue, dEndValue;
  double dComp1, dComp2;
  double dDepth1, dDepth2;
  if(prIndices1.first == -1)
  { 
    assert(prIndices2.second != -1);

    Components = AvgCompComponents(0, prIndices2.first, STRETCHED_TMD);
    nom += Components.first;
    denom += Components.second;
    
    if(NearestSandLayer(dEndTMD).second) // Is the end in a sand?
    {
      // The compressibility has to be calculated at dEndTMD using a weighted average method:
      dComp1 = getCompressibility(prIndices2.first);
      dComp2 = getCompressibility(prIndices2.second);
      dDepth1 = Depth(STRETCHED_TMD, prIndices2.first);
      dDepth2 = Depth(STRETCHED_TMD, prIndices2.second);

      dEndValue = dComp1 * ( (dDepth2 - dEndTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dEndTMD - dDepth1) / (dDepth2 - dDepth1) );

      nom += dComp1 * ((dEndTMD - dDepth1) / 2.0);
      nom += dEndValue * ((dEndTMD - dDepth1) / 2.0);

      denom += (dEndTMD - dDepth1);
    }

    return nom / denom;
  }

  if(prIndices2.second == -1)
  {	
    assert(prIndices1.first != -1);

    Components = AvgCompComponents(prIndices1.second, LogPointSize() - 1, STRETCHED_TMD);
    nom += Components.first;
    denom += Components.second;

    if(NearestSandLayer(dStartTMD).second) // is the start in a sand?
    {
      // The compressibility has to be calculated at dStartTMD using a weighted average method:
      dComp1 = getCompressibility(prIndices1.first);
      dComp2 = getCompressibility(prIndices1.second);
      dDepth1 = Depth(STRETCHED_TMD, prIndices1.first);
      dDepth2 = Depth(STRETCHED_TMD, prIndices1.second);

      dBeginValue = dComp1 * ( (dDepth2 - dStartTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dStartTMD - dDepth1) / (dDepth2 - dDepth1) );

      nom += dComp2 * ((dStartTMD - dDepth2) / 2.0);
      nom += dBeginValue * ((dStartTMD - dDepth2) / 2.0);
      denom += (dDepth2 - dStartTMD);
    }
    
    return nom / denom;
  }	

  assert(prIndices1.first != -1 && prIndices2.second != -1);
  
  if(prIndices1.second == prIndices2.first || prIndices1 == prIndices2) // start and end in between the same indices
  {
    std::pair<int, bool> InSand1 = NearestSandLayer(dStartTMD);
    std::pair<int, bool> InSand2 = NearestSandLayer(dEndTMD);
    if(!InSand1.second || !InSand2.second)
      return 0.0;
    
    assert(InSand1.second && InSand2.second);
    
    if(prIndices1.first == prIndices1.second)
      dBeginValue = getCompressibility(prIndices1.first);
    else
    {
      dComp1 = getCompressibility(prIndices1.first);
      dComp2 = getCompressibility(prIndices1.second);
      dDepth1 = Depth(STRETCHED_TMD, prIndices1.first);
      dDepth2 = Depth(STRETCHED_TMD, prIndices1.second);
      dBeginValue = dComp1 * ( (dDepth2 - dStartTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dStartTMD - dDepth1) / (dDepth2 - dDepth1) ); 
    }

    if(prIndices2.first == prIndices2.second)
      dEndValue = getCompressibility(prIndices2.first);
    else
    {
      dComp1 = getCompressibility(prIndices2.first);
      dComp2 = getCompressibility(prIndices2.second);
      dDepth1 = Depth(STRETCHED_TMD, prIndices2.first);
      dDepth2 = Depth(STRETCHED_TMD, prIndices2.second);

      dEndValue = dComp1 * ( (dDepth2 - dEndTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dEndTMD - dDepth1) / (dDepth2 - dDepth1) );
    }

    nom += dBeginValue * ((dEndTMD - dStartTMD) / 2.0);
    nom += dEndValue * ((dEndTMD - dStartTMD) / 2.0);
    denom += dEndTMD - dStartTMD;

    return nom / denom;
  }
  
  Components = AvgCompComponents(prIndices1.second, prIndices2.first, STRETCHED_TMD);
  nom += Components.first;
  denom += Components.second;

  if(NearestSandLayer(dStartTMD).second)
  {
    dComp1 = getCompressibility(prIndices1.first);
    dComp2 = getCompressibility(prIndices1.second);
    dDepth1 = Depth(STRETCHED_TMD, prIndices1.first);
    dDepth2 = Depth(STRETCHED_TMD, prIndices1.second);
    if(dDepth2 - dDepth1 < EPS)
      dBeginValue = dComp1;
    else
      dBeginValue = dComp1 * ( (dDepth2 - dStartTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dStartTMD - dDepth1) / (dDepth2 - dDepth1) );
    
    nom += dComp2 * ((dStartTMD - dDepth2) / 2.0);
    nom += dBeginValue * ((dStartTMD - dDepth2) / 2.0);
    denom += (dDepth2 - dStartTMD);
  }

  if(NearestSandLayer(dEndTMD).second)
  {
    dComp1 = getCompressibility(prIndices2.first);
    dComp2 = getCompressibility(prIndices2.second);
    dDepth1 = Depth(STRETCHED_TMD, prIndices2.first);
    dDepth2 = Depth(STRETCHED_TMD, prIndices2.second);

    if(dDepth2 - dDepth1 < EPS)
      dEndValue = dComp1;
    else
      dEndValue = dComp1 * ( (dDepth2 - dEndTMD) / (dDepth2 - dDepth1) )
            + dComp2 * ( (dEndTMD - dDepth1) / (dDepth2 - dDepth1) );

    nom += dComp1 * ((dEndTMD - dDepth1) / 2.0);
    nom += dEndValue * ((dEndTMD - dDepth1) / 2.0);
    denom += (dEndTMD - dDepth1);
  }

  return nom / denom;
}

// For each radioactive tag interval the average compressibility is calculated.
void CWellLog::AvgCompRTIntervals()
{
  assert(m_vcRTTMD.size() > 1);
  m_vcRTAvgCompInterval.clear();
  for(size_t i = 0; i < m_vcRTTMD.size(); i++)
  {
    if(i == m_vcRTTMD.size() - 1)
    {
      m_vcRTAvgCompInterval.push_back(0.0); // last index, just set to 0;
      break;
    }
    m_vcRTAvgCompInterval.push_back(AverageCompressibility(m_vcRTTMD[i].Value(), m_vcRTTMD[i + 1].Value()));
  }

}

void CWellLog::CalculateModulation()
{
  assert(m_vcRTAvgCompInterval.size() > 0);
  m_vcModulation.clear();

  int nInterval;
  double dDepth, dComp, dAvgComp, dModulation, dAvgAxialStrain;
  
  for(int i = 0; i < LogPointSize(); i++)
  {
    dDepth = Depth(STRETCHED_TMD, i);
    nInterval = RTInterval(dDepth);

    if(nInterval == -1)
    {
      m_vcModulation.push_back(0.0);
      continue;
    }

    dComp = m_vcCompressibility[i].Value();
    dAvgComp = m_vcRTAvgCompInterval[nInterval].Value();
    dAvgAxialStrain = m_vcRTStrains[nInterval + 1].Value();
    
    if(fabs(dAvgComp) < EPS)
    {
      // The following line of code is correct! but the graph does not know ow to handle it...
      m_vcModulation.push_back(geo::CValue()); 
    //	m_vcModulation.push_back(0.0);
      return;
    }

    dModulation = (dComp / dAvgComp) * dAvgAxialStrain;
    m_vcModulation.push_back(dModulation);
  }
}


double CWellLog::Modulation(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < LogPointSize());
  return m_vcModulation[nIndex].Value();
}

// function returns an index to an interval: 0 means the interval between the first and second radioactive tag.
int CWellLog::RTInterval(double &TMD) const
{
  if(m_vcRTTMD.size() < 2)
    return -1;

  if(TMD < m_vcRTTMD[0].Value() || TMD > m_vcRTTMD[RTagSize() - 1].Value())
    return -1;

  for(int i = 0; i < RTagSize() - 1; i++)
  {
    if(m_vcRTTMD[i].Value() <= TMD && m_vcRTTMD[i + 1].Value() >= TMD)
      return i;
  }

  assert(false);

  return -1;
}

double CWellLog::RTTMD(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < RTagSize());
  return m_vcRTTMD[nIndex].Value();
}

double CWellLog::RTTVD(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < RTagSize());
  return m_vcRTTVD[nIndex].Value();
}

double CWellLog::RTAvgComp(int nInterval) const
{
  assert(nInterval >= 0 && nInterval < RTagSize());
  return m_vcRTAvgCompInterval[nInterval].Value();
}

double CWellLog::RTAxialStrain(int nInterval) const
{
  assert(nInterval >=0 && nInterval < RTagSize());
  return m_vcRTStrains[nInterval].Value();
}

double CWellLog::RTRelDispl(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < RTagSize());
  return m_vcRTdeltaUv[nIndex].Value();
}

// returns the axial strain at any given TMD
double CWellLog::AxialStrain(double &TMD) const
{
  int nInterval = RTInterval(TMD);
  if(nInterval == -1)
    return 0.0;	
  return RTAxialStrain(nInterval);
}

void CWellLog::FillLogPointList()
{
  m_point_log_tmd_list.RemoveAllPoints();
  for(int i = 0; i < LogPointSize(); i++)
  {
    CWellPoint *pPt = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), Depth(STRETCHED_TMD, i));
    m_point_log_tmd_list.AddPoint(pPt);
  }
}

void CWellLog::FillAvAxPointList()
{
  m_point_avax_rat_list.RemoveAllPoints();

  for(int i = 0; i < RTagSize(); i++)
  {
    CWellPoint *pPt = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), m_vcRTTMD[i].Value());
    m_point_avax_rat_list.AddPoint(pPt);
    if(i != 0 && i != RTagSize() - 1)
    {
      CWellPoint *pPt2 = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), m_vcRTTMD[i].Value());
      m_point_avax_rat_list.AddPoint(pPt2); // add the point again since we want a stepped function
    }
  }
}

void CWellLog::FillAvAxValueMap()
{
  CWellPointList::Iterator it_list = m_point_avax_rat_list.begin();
  m_avax_map.ClearMap();
  
  int j = 1;
  while(it_list != m_point_avax_rat_list.end())
  {	
    assert(j != RTagSize());
    m_avax_map.InsertItem(*it_list, (m_vcRTStrains[j]));
    ++it_list;
    if(it_list == m_point_avax_rat_list.end())
      break;
    m_avax_map.InsertItem(*it_list, (m_vcRTStrains[j]));
    ++it_list;
    j++;
  }
  
}

void CWellLog::FillCompressMap()
{
  CWellPointList::Iterator it_list = m_point_log_tmd_list.begin();
  m_compress_map.ClearMap();
  int j = 0;
  while(it_list != m_point_log_tmd_list.end())
  {	
    assert(j != LogPointSize());
    m_compress_map.InsertItem(*it_list, const_cast<geo::CValue&>(m_vcCompressibility[j]));
    ++it_list;
    j++;
  }
}

void CWellLog::FillModulMap()
{
  CWellPointList::Iterator it_list = m_point_log_tmd_list.begin();
  m_modul_map.ClearMap();
  int j = 0;
  while(it_list != m_point_log_tmd_list.end())
  {	
    assert(j != LogPointSize());
    m_modul_map.InsertItem(*it_list, const_cast<geo::CValue&>(m_vcModulation[j]));
    ++it_list;
    j++;
  }
}

const CWellPointList &CWellLog::LogPoints() const
{
  return m_point_log_tmd_list;
}

const CWellPointList &CWellLog::RatAvAxPoints() const
{
  return m_point_avax_rat_list;
}

const CWellPointValueMap &CWellLog::AvAxMap() const
{
  return m_avax_map;
}

const CWellPointValueMap &CWellLog::ModulMap() const
{
  return m_modul_map;
}

const CWellPointValueMap &CWellLog::CompressMap() const
{
  return m_compress_map;
}

/*
void CWellLog::WellPointModul(CWellPointList &point_list, CWellPointValueMap &vlmap) const
{
  assert(&point_list == &vlmap.List());
  assert(&point_list.WellPath() == m_pWellPath);

  //CWellPointList point_list(const_cast<CWellPathBase&>(*m_pWellPath));
  //std::pair<CWellPointList, CWellPointValueMap> retpr(point_list, CWellPointValueMap("Compressibility", point_list, 0));

  point_list.RemoveAllPoints();
  vlmap.ClearMap();
  for(int i = 0; i < LogPointSize(); i++)
  {
    CWellPoint *pPt = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), Depth(STRETCHED_TMD, i));
    point_list.AddPoint(pPt);
  }

//	geo::CValue *val = 0;
  //CWellPointValueMap vlmap("Modulation", point_list, 0);
  CWellPointList::Iterator it_list = point_list.GetIterator();
  int j = 0;
  while(it_list.current())
  {	
    assert(j != LogPointSize());
  //	val = new geo::CValue(Modulation(j));
    vlmap.InsertItem(it_list.current(), const_cast<geo::CValue*>(&m_vcModulation[j]));
    ++it_list;
    j++;
  }
}

void CWellLog::AxialStrainIntervals(CWellPointList &point_list, CWellPointValueMap &vlmap) const
{
  assert(&point_list == &vlmap.List());
  assert(&point_list.WellPath() == m_pWellPath);

  //CWellPointList point_list(const_cast<CWellPathBase&>(*m_pWellPath));
  //std::pair<CWellPointList, CWellPointValueMap> retpr(point_list, CWellPointValueMap("Axial strain", point_list, 0));
  
  point_list.RemoveAllPoints();
  vlmap.ClearMap();
  for(int i = 0; i < RTagSize(); i++)
  {
    CWellPoint *pPt = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), m_vcRTTMD[i].Value());
    point_list.AddPoint(pPt);
    if(i != 0 && i != RTagSize() - 1)
    {
      CWellPoint *pPt2 = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), m_vcRTTMD[i].Value());
      point_list.AddPoint(pPt2); // add the point again since we want a stepped function
    }
  }

//	assert(point_list.NrOfPoints() == 2 * (m_vcRTStrains.size() - 1));
  
  //geo::CValue *val = 0;
  //CWellPointValueMap vlmap("Axial strain", point_list, 0);
  CWellPointList::Iterator it_list = point_list.GetIterator();
  int j = 1;
  while(it_list.current())
  {	
    assert(j != RTagSize());
    
    //val = new geo::CValue(RTAxialStrain(j));
    //vlmap.InsertItem(it_list.current(), val);
    vlmap.InsertItem(it_list.current(), const_cast<geo::CValue*>(&m_vcRTStrains[j]));
    
    ++it_list;
    if(!it_list.current())
      break;
    
    //val = new geo::CValue(RTAxialStrain(j));
    vlmap.InsertItem(it_list.current(), const_cast<geo::CValue*>(&m_vcRTStrains[j]));
    ++it_list;
    j++;
  }
}

void CWellLog::WellPointCompress(CWellPointList &point_list, CWellPointValueMap &vlmap) const
{
  assert(&point_list == &vlmap.List());
  assert(&point_list.WellPath() == m_pWellPath);

  //CWellPointList point_list(const_cast<CWellPathBase&>(*m_pWellPath));
  //std::pair<CWellPointList, CWellPointValueMap> retpr(point_list, CWellPointValueMap("Compressibility", point_list, 0));

  point_list.RemoveAllPoints();
  vlmap.ClearMap();
  for(int i = 0; i < LogPointSize(); i++)
  {
    CWellPoint *pPt = new CWellPoint(const_cast<CWellPathBase&>(*m_pWellPath), Depth(STRETCHED_TMD, i));
    point_list.AddPoint(pPt);
  }

  //geo::CValue *val = 0;
  //CWellPointValueMap vlmap("Compressibility", point_list, 0);
  CWellPointList::Iterator it_list = point_list.GetIterator();
  int j = 0;
  while(it_list.current())
  {	
    assert(j != LogPointSize());
    //val = new geo::CValue(Compressibility(j));
    vlmap.InsertItem(it_list.current(), const_cast<geo::CValue*>(&m_vcCompressibility[j]));
    ++it_list;
    j++;
  }
}
*/

double CWellLog::Modulation(double &TMD) const
{
  std::pair<int, int> indices = NearestIndices(TMD);
  if(indices.first == indices.second)
  {
    return Modulation(indices.first);
  }
  else if(indices.first == -1)
  {
    return 0.0;
  }
  else if(indices.second == -1)
  {
    return 0.0;
  }
  else
  { // calculate a weighted average.
    double mod_begin	= Modulation(indices.first);
    double mod_end		= Modulation(indices.second);
    double tmd_begin	= Depth(STRETCHED_TMD, indices.first);
    double tmd_end		= Depth(STRETCHED_TMD, indices.second);
    double tmd_tot		= tmd_end - tmd_begin;

    return (mod_begin * ((tmd_end - TMD) / tmd_tot)) + (mod_end * ((TMD - tmd_begin) / tmd_tot));
  }
}


// return a pair with double with which the average compressibility can be calculated.
// The average compressibility = pair.first / pair.second
std::pair<double, double> CWellLog::AvgCompComponents(int nStartIndex, int nEndIndex, eDepthType type) const
{ 
  // the average compressibility is calculated in between the two indices. Only comps. inside a sand
  // taken into account and the total TMD in between the two indices is also only calculated for sands.
  assert(nStartIndex >= 0 && nEndIndex < LogPointSize() && nStartIndex < nEndIndex);
  
  if(SandLayerSize() == 0)
    return std::pair<int, int>(0, 1); // no sandlayers at all...

  double depth = Depth(STRETCHED_TMD, nStartIndex);
  std::pair<int, bool> prStart = NearestSandLayer(depth);
  depth = Depth(STRETCHED_TMD, nEndIndex);
  std::pair<int, bool> prEnd = NearestSandLayer(depth);
  std::pair<double, double> RetPair;
  if(prStart.first == prEnd.first && !prStart.second && !prEnd.second)
    return std::pair<int, int>(0, 1); // both indices in same shale layer.

  int first, second;
  double nom, denom, delta;

  nom = 0.0;
  denom = 0.0;

  for(int i = prStart.first; i <= prEnd.first; i++)
  {
    if(prStart.first == prEnd.first)
    {// 1) first and last sandlayer are the same
      if(prStart.second && prEnd.second)
      {
        first = nStartIndex;
        second = nEndIndex;
      }
      else if(!prStart.second && prEnd.second)
      {
        first = m_vcSandLayers[i].first;
        second = nEndIndex;
      }
      else // prStart.second && !prEnd.second)
      {
        first = nStartIndex;
        second = m_vcSandLayers[i].second;
      }
    }
    else if(i == prStart.first)
    {// 2) current sandlayer is the first sandlayer
      if(prStart.second)
      {
        first = nStartIndex;
        second = m_vcSandLayers[i].second;
      }
      else // !prStart.second
      {
        continue; // continue to the next sandlayer
      }
    }
    else if(i == prEnd.first)
    {// 3) current sandlayer is the last sandlayer
      if(prEnd.second)
      {
        first = m_vcSandLayers[i].first;
        second = nEndIndex;
      }
      else // !prEnd.second
      {
        first = m_vcSandLayers[i].first;
        second = m_vcSandLayers[i].second;
      }
    }
    else
    {// 4) current sandlayer is a sandlayer in between the first and last sandlayer
      first = m_vcSandLayers[i].first;
      second = m_vcSandLayers[i].second;
    }
    for(int j = first; j <= second; j++)
    {
      if(j == first)
        delta = fabs((Depth(type, j + 1) - Depth(type, j)) / 2.0);
      else if(j == second)
        delta = fabs((Depth(type, j) - Depth(type, j - 1)) / 2.0);
      else
        delta = fabs((-1.0 * (Depth(type, j - 1)) + Depth(type, j + 1)) / 2.0);

      nom += getCompressibility(j) * delta;
      denom += delta;
    }
  }

  assert(denom > 0.0);

#ifdef DEBUG
  if(type == STRETCHED_TMD)
  {
    double dBegin = Depth(type, nStartIndex);
    double dEnd = Depth(type, nEndIndex);
    assert(fabs(SandDelta(dBegin, dEnd) - denom) < EPS);
  }
#endif

  RetPair.first = nom;
  RetPair.second = denom;
  
  return RetPair;
}


int CWellLog::LogPointSize() const
{
  return m_vcOriginalTMD.size();
}

double CWellLog::Depth(eDepthType type, int nIndex) const
{
  switch(type)
  {
  case STRETCHED_TVD:
    {
      //assert(nIndex >= 0 && nIndex < m_vcStretchedTVD.size());
      assert(nIndex >= 0 && nIndex < m_vcStretchedTMD.size());
      assert(m_pWellPath);
      return m_pWellPath->TMD2TVD(m_vcStretchedTMD[nIndex].Value());
    }
  case STRETCHED_TMD:
    {
      assert(nIndex >= 0 && nIndex < m_vcStretchedTMD.size());
      return m_vcStretchedTMD[nIndex].Value();
    }
  case ORIGINAL_TVD:
    {
      assert(nIndex >= 0 && nIndex < m_vcOriginalTVD.size());
      return m_vcOriginalTVD[nIndex].Value();
    }
  case ORIGINAL_TMD:
    {
      assert(nIndex >= 0 && nIndex < m_vcOriginalTMD.size());
      return m_vcOriginalTMD[nIndex].Value();
    }
  default:
    assert(false);
  }
  
  assert(false);
  return 0.0;
}

double CWellLog::Compressibility(double &TMD) const
{
  std::pair<int, int> indices = NearestIndices(TMD);
  if(indices.first == indices.second)
    return getCompressibility(indices.first);
  if(indices.first == -1 || indices.second == -1)
    return -1.0;

  return Compressibility(TMD, indices);
}

double CWellLog::Compressibility(double &TMD, std::pair<int, int> &pair) const
{
  assert(Depth(STRETCHED_TMD, pair.first) < TMD && Depth(STRETCHED_TMD, pair.second) > TMD);

  double comp1 = getCompressibility(pair.first);
  double comp2 = getCompressibility(pair.second);
  double depth1 = Depth(STRETCHED_TMD, pair.first);
  double depth2 = Depth(STRETCHED_TMD, pair.second);
  double factor1 = (depth2 - TMD) / (depth2 - depth1);
  double factor2 = (TMD - depth1) / (depth2 - depth1);

  return (comp1 * factor1) + (comp2 * factor2);
}

// returns a pair with the two nearest indices. If exactly on an index, first and second are equal to
// this index. If above the first index, first is set to -1. If below last index, second is set to -1.
std::pair<int, int> CWellLog::NearestIndices(const double &TMD) const
{
  std::pair<int, int> RetPair;
  double dDepth;

  for(int i = 0; i < LogPointSize(); i++)
  {
    dDepth = Depth(STRETCHED_TMD, i);

    if(fabs(dDepth - TMD) < EPS)
    {
      RetPair.first = i;
      RetPair.second = i;
      return RetPair;
    }

    if(i == 0 && dDepth > TMD )
    {
      RetPair.first = -1;
      RetPair.second = 0; // just set it to something
      return RetPair;
    }

    if(i == LogPointSize() - 1 && dDepth < TMD)
    {
      RetPair.first = 0; // just set it to something;
      RetPair.second = -1;
      return RetPair;
    }

    assert(i != LogPointSize() - 1);
    
    if(dDepth < TMD && Depth(STRETCHED_TMD, i + 1) > TMD)
    {
      RetPair.first = i;
      RetPair.second = i + 1;
      return RetPair;
    }
  }

  assert(false); // Situation should have been handled by now.
  return RetPair;
}


void CWellLog::OriginalTMD(const double &TMD)
{
  m_vcOriginalTMD.push_back(TMD);
}

const CWellPathBase *CWellLog::WellPath() const
{
  return m_pWellPath;
}
  
void CWellLog::WellPath(const CWellPathBase *pWellPath)
{
#ifdef DEBUG
  if(m_pSection)
    assert(&m_pSection->WellPath() == pWellPath);
#endif
  
  m_pWellPath = pWellPath;
  m_point_avax_rat_list.WellPath(const_cast<CWellPathBase*>(pWellPath));
  m_point_log_tmd_list.WellPath(const_cast<CWellPathBase*>(pWellPath));
}

const IWellSection *CWellLog::WellSection() const
{
  return m_pSection;
}

void CWellLog::SetSectionAndPath(const IWellSection *pSection)
{
  if(!m_pWellPath)
    m_pWellPath = &pSection->WellPath();

  m_pSection = pSection;
}

double CWellLog::SandCutOff() const
{
  return m_dVc;
}

void CWellLog::SandCutOff(const double &dVc)
{
  m_dVc = dVc;
}

void CWellLog::addShalyness(const double &dVsh)
{
  m_vcShalyness.push_back(dVsh);
}

double CWellLog::getShalyness(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcShalyness.size());
  return m_vcShalyness[nIndex].Value();
}

void CWellLog::addCompressibility(const double &dComp)
{
  m_vcCompressibility.push_back(dComp);
}

double CWellLog::getCompressibility(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcCompressibility.size());
  return m_vcCompressibility[nIndex].Value();
}




/*double CWellLog::TVD(const double &TMD) const
{
  std::map<double, double>::const_iterator it;
  it = m_mpTVD_to_TMD.find(TMD);
  assert(it != m_mpTVD_to_TMD.end());

  return (*it).second;
}

bool CWellLog::InsertTVD(const double &TMD, const double &TVD) 
{
  std::pair<std::map<double, double>::iterator, bool> pr;
  pr = m_mpTMD_to_TVD.insert(std::map<double, double>::value_type(TMD, TVD);

  return pr.second;
}

double CWellLog::TMD(const double &TVD) const
{
  std::map<double, double>::const_iterator it;
  it = m_mpTMD_to_TVD.find(TVD);
  assert(it != m_mpTMD_to_TVD.end());

  return (*it).second;
}

bool CWellLog::InsertTMD(const double &TVD, const double &TMD)
{
  std::pair<std::map<double, double>::iterator, bool> pr;
  pr = m_mpTVD_to_TMD.insert(std::map<double, double>::value_type(TVD, TMD);

  return pr.second;
}
  
double CWellLog::Shalyness(const double &TMD) const
{
  std::map<double, double>::const_iterator it;
  it = m_mpTMD_to_Vsh.find(TMD);
  assert(it != m_mpTMD_to_Vsh.end());

  return (*it).second;
}

bool CWellLog::Shalyness(const double &TMD, const double &dVsh)
{
  std::pair<std::map<double, double>::iterator, bool> pr;
  pr = m_mpTMD_to_Vsh.insert(std::map<double, double>::value_type(TMD, dVsh));

  return pr.second;
}

double CWellLog::Compressibility(const double &TMD) const
{
  std::map<double, double>::const_iterator it;
  it = m_mpTMD_to_Comp.find(TMD);
  assert(it != m_mpTMD_to_Comp.end());

  return (*it).second;
}
  
bool CWellLog::Compressibility(const double &TMD, const double &dComp)
{
  std::pair<std::map<double, double>::iterator, bool> pr;
  pr = m_mpTMD_to_Comp.insert(std::map<double, double>::value_type(TMD, dComp));

  return pr.second;
}

  */

} // namespace well