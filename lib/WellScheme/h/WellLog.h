// WellLog.h: interface for the CWellLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLLOG_H__12909A2A_2E02_46AC_B4FE_7FC8AEDE241A__INCLUDED_)
#define AFX_WELLLOG_H__12909A2A_2E02_46AC_B4FE_7FC8AEDE241A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WellSchemeInclude.h"
#include "WellPointList.h"
#include "WellPointMapper.h"

namespace well {
class CWellPathBase;
class IWellSection;
class CWellPoint;
class CWellPointValueMap;

class CWellLog  
{
public:
  enum eDepthType {ORIGINAL_TMD, STRETCHED_TMD, ORIGINAL_TVD, STRETCHED_TVD};

private:
  typedef std::pair<int, int> TSandLayer;
  std::vector<TSandLayer> m_vcSandLayers;
  std::vector<TSandLayer> m_vcNonSandLayers;
  std::vector<geo::CValue> m_vcOriginalTMD; // Unstretched data
  std::vector<geo::CValue> m_vcStretchedTMD; // Stretched data
  std::vector<geo::CValue> m_vcOriginalTVD; // Unstretched data

  const CWellPathBase *m_pWellPath;
  const IWellSection *m_pSection;
  double m_dVc; // sand cut-off value;
  std::vector<geo::CValue> m_vcShalyness;
  std::vector<geo::CValue> m_vcCompressibility;
  std::vector<geo::CValue> m_vcModulation;

  // Radioactive tag data
  std::vector<geo::CValue> m_vcRTTMD; // true measured depth of the radioactive tags.
  std::vector<geo::CValue> m_vcRTTVD; // true vertical depth of the radioactive tags.
  std::vector<geo::CValue> m_vcRTdeltaUv; // relative displacements of the radioactive tags.
  std::vector<geo::CValue> m_vcRTStrains; // strains calculated from the relative vertical displacements in
                     // CalculateVertStrains()
  std::vector<geo::CValue> m_vcRTAvgCompInterval; // average comressibility per RT interval. size == m_vcRTTMD.size() - 1


  // The following point lists and value maps are used for displaying purposes only.
  // *******************************************************************************
  // point list created from the tmd entries from read in log data.
  CWellPointList m_point_log_tmd_list; 
  // point list created for displaying the average axial strain calculated from radioactive tags.
  CWellPointList m_point_avax_rat_list; 
  // valuemap used for displaying the compressibility data
  CWellPointValueMap m_compress_map;
  // valuemap used for displaying the modulation
  CWellPointValueMap m_modul_map;
  // valuemap used for displaying the average axial strain
  CWellPointValueMap m_avax_map;
  // *******************************************************************************
  
public:
  CWellLog(CWellPathBase *pWellPath, IWellSection *pSection);
  CWellLog(CWellPathBase *pWellPath);
  CWellLog();
  CWellLog(const CWellLog &welllog);
  virtual ~CWellLog();

  void InitLog(const std::vector<double> &vcTMD, const std::vector<double> &vcTVD, 
         const std::vector<double> &vcShalyness, const std::vector<double> &vcCompressibility, 
         const double dSandCutOff);
  void InitRadioactiveTags(const std::vector<double> &vcTMD, const std::vector<double> &vcTVD,
               const std::vector<double> &vcRelDispl);
private:
  // Init functions, for the copy constructor
  void InitLog(const std::vector<geo::CValue> &vcTMD, const std::vector<geo::CValue> &vcTVD,
         const std::vector<geo::CValue> &vcShalyness, const std::vector<geo::CValue> &vcCompressibility,
         const double dSandCutOff);
  void InitRadioactiveTags(const std::vector<geo::CValue> &vcTMD, const std::vector<geo::CValue> &vcTVD,
               const std::vector<geo::CValue> &vcRelDispl);
public:
  

  
  void OriginalTMD(const double &TMD);
  void OriginalTVD(const double &TVD);

  double MaxDeltaTVD() const;

  int LogPointSize() const;
  int RTagSize() const;
  int SandLayerSize() const;
  size_t NonSandLayerSize() const;
  
  const CWellPathBase *WellPath() const;
  void WellPath(const CWellPathBase *pWellPath);
  const IWellSection *WellSection() const;
  void SetSectionAndPath(const IWellSection *pSection);
  double SandCutOff() const;
  void SandCutOff(const double &dVc);

  void addShalyness(const double &dVsh);
  double getShalyness(int nIndex) const;
  void addCompressibility(const double &dComp);
  double getCompressibility(int nIndex) const;

  TSandLayer SandLayer(int nIndex) const;
  TSandLayer NonSandLayer(size_t nIndex) const;

  double Depth(eDepthType type, int nIndex) const;
  double TotalAverageCompressibility() const; // uses TVD
  double AverageCompressibility(const double &dStart, const double &dEnd) const;

  // return a pair with doubles with which the average compressibility can be calculated.
  // The average compressibility = pair.first / pair.second
  std::pair<double, double> AvgCompComponents(int nStartIndex, int nEndIndex, eDepthType type) const;

  // calculates a compressibility that is a weighted average of two compressibilities. Returns -1.0
  // if the TMD is above or beloe all indices.
  double Compressibility(double &TMD) const;
  // same as previous but now you already have to know in between which two indices the TMD is situated.
  // use NearestIndices to get them.
  double Compressibility(double &TMD, std::pair<int, int> &pair) const;
  
  // returns a pair with the two nearest indices. If exactly on an index, first and second are equal to
  // this index. If a above the first index, first is set to -1. If below last index, second is set to -1.
  std::pair<int, int> NearestIndices(const double &TMD) const;

  // function returns an index to the sandlayer closest to (above) the TMD and a bool indicating of the TMD is
  // inside the sandlayer. If the index is -1 the TMD is above the top sand layer.
  std::pair<int, bool> NearestSandLayer(const double &TMD) const;

  double Modulation(int nIndex) const;
  double Modulation(double &TMD) const;
  double RTTMD(int nIndex) const;
  double RTTVD(int nIndex) const;
  double RTRelDispl(int nIndex) const;
  double RTAvgComp(int nInterval) const;
  double RTAxialStrain(int nInterval) const; 
  double AxialStrain(double &TMD) const;
  
//	void AxialStrainIntervals(CWellPointList &point_list, CWellPointValueMap &vlmap) const;
//	void WellPointCompress(CWellPointList &point_list, CWellPointValueMap &vlmap) const;
//	void WellPointModul(CWellPointList &point_list, CWellPointValueMap &vlmap) const;
  const CWellPointList	 &LogPoints()     const;
  const CWellPointList	 &RatAvAxPoints() const;
  const CWellPointValueMap &AvAxMap()		  const;
  const CWellPointValueMap &ModulMap()	  const;
  const CWellPointValueMap &CompressMap()	  const;
private:
  void StretchLogData();
  void CalculateAxialStrains();
  void AvgCompRTIntervals(); // calculates the average compressibility for each radioactive tag interval.
  void CalculateModulation(); // calculates the modulation for each log point.

  // function returns the distance between dStartTMD and dEndTMD that actuallly passes through sand;
  double SandDelta(const double &dStartTMD, const double &dEndTMD) const;

  void ExtractSandLayers();
  void ExtractNonSandLayers();

  bool InsideSand(int nSandIndex, const double &TMD) const;
  double TopSand(int nSandIndex, eDepthType type) const;
  double BottomSand(int nSandIndex, eDepthType type) const;

  // returns the radioactive tag interval in which the TMD is situated.
  // returns -1 if outside all tags.
  int RTInterval(double &TMD) const; 
  
  void FillLogPointList();
  void FillAvAxPointList();
  void FillAvAxValueMap();
  void FillCompressMap();
  void FillModulMap();


};	

} // namespace well

#endif // !defined(AFX_WELLLOG_H__12909A2A_2E02_46AC_B4FE_7FC8AEDE241A__INCLUDED_)
