#ifdef SKUA_NEW

#pragma once

#include <vector>
#include <string>

class CFemAppModel;
class CElementSet;
class CPointSet;

class QString;

namespace gm_skua
{
class SKUAImport;
struct SKUAParseData;
}



class CGocadImport
{
public:
  CGocadImport(bool bRequireSurfaceDefinitions, bool bAllowLightTSolid = false);
  virtual ~CGocadImport();

  bool Import(const std::string& strFileName);

  CElementSet *CreateElementSet(CFemAppModel& model, int solidIndex) const;
  CPointSet *CreatePointSet(CFemAppModel& model, int vsetIndex) const;
  void CreateSurface(CFemAppModel &model, int surfIndex) const;

  int SolidSize() const;
  int VsetSize() const;
  int SurfSize() const;

  std::vector<const gm_skua::SKUAParseData *>& getSolids();

  const std::string& Error() const;

private:
  CPointSet *CreatePointSet(CFemAppModel& model, const gm_skua::SKUAParseData *gocad) const;
  void CreateFractureIntensityAttribute(CPointSet *pPointSet) const;

  const gm_skua::SKUAParseData *Solid(int i) const;
  const gm_skua::SKUAParseData *Vset(int i) const;
  const gm_skua::SKUAParseData *Surf(int i) const;

  bool m_bRequireSurfaceDefinitions;
  bool m_bAllowLightTSolid;

  gm_skua::SKUAImport *m_importer;

  std::vector<const gm_skua::SKUAParseData *> m_solids;
  std::vector<const gm_skua::SKUAParseData *> m_vsets;
  std::vector<const gm_skua::SKUAParseData *> m_surfaces;

  std::string m_strError;
};




#else
// GocadImport.h: interface for the CGocadImport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADIMPORT_H__C6DFED81_A91B_4D7D_B1F1_CBE211725A9C__INCLUDED_)
#define AFX_GOCADIMPORT_H__C6DFED81_A91B_4D7D_B1F1_CBE211725A9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
#include <map>

class CElementSet;
class CPointSet;

namespace geo {
  class CPoint;
  class CVector;
}

#include <qstring.h>
#include <string>


#include <QSharedPointer>

#include "StorageNode.h"

#include "GocadData.h"

geo::CPoint Node2Point(const CGocadData::CNode& node);


class CGocadImport  
{
public:
  CGocadImport(bool bRequireSurfaceDefinitions, bool bAllowLightTSolid = false);
  virtual ~CGocadImport();

  bool Import(const std::string& strFileName);

  CElementSet* CreateElementSet(CFemAppModel& model, int solidIndex) const;
  CPointSet* CreatePointSet(CFemAppModel& model, int vsetIndex) const;
  void CreateSurface(CFemAppModel &model, int surfIndex) const;

  int SolidSize() const;
  int VsetSize() const;
  int SurfSize() const;

  std::vector < QSharedPointer <CGocadData::CTSolid> >& getSolids();

  std::string Error() const;

private:
  CPointSet* CreatePointSet(CFemAppModel& model, const CGocadData::IGoCadTypes& gocad) const;
  void CreateFractureIntensityAttribute(CPointSet *pPointSet) const;

  const CGocadData::CTSolid &Solid(int i) const;
  const CGocadData::CVset &Vset(int i) const;
  const CGocadData::CTSurf &Surf(int i) const;

private:
  QString ParsePropertyName(const QString &propertyName);
  typedef std::list<QString> TArgumentList;
  typedef std::list<QString>::const_iterator TArgIterator;

  bool ParseMesh();
  void GetArguments(const QString& strLine, QString& keyword, TArgumentList& lstArgs) const;
  bool ParseLine(const QString& strLine);
  bool ParseArguments(const QString& strKeyword, TArgIterator itArg, TArgIterator itArgEnd);
  void FindDSFVolumes();
  void FindDSFFaults();
  bool CheckSolid();
  bool CheckSolidProperties(const std::vector<QString>& vcPropertyNames, std::vector<int>& vcESizes, const std::vector<double>& vcNoDataValues, const QString& strPrefix);
  bool CheckVolume();
  bool CheckSurface();
  bool CheckTFace();
  bool ParseVertex(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseAtom(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseSharedVertex(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseTetra(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseTetraComment(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseTriangle(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseProperties(TArgIterator itArg, TArgIterator itArgEnd, const std::vector<int>& vcESizes, std::vector<std::vector<double> >& vcProperties);
  bool ParseTetraRegion(TArgIterator itArg, TArgIterator itArgEnd);
  bool ParseTri(TArgIterator itArg, TArgIterator itArgEnd);
  bool StringToInt(const QString& strVal, int& iVal) const;
  bool StringToDouble(const QString& strVal, double& dVal) const;
  bool DeriveSurfaceName(const QString& strVal);
  CGocadData::IGoCadTypes *CurrentType() const;

private:
  bool m_bRequireSurfaceDefinitions;
  bool m_bAllowLightTSolid;
  FILE* m_fp;
  int m_nLineNr;
  QString m_strError;

  std::vector <QSharedPointer <CGocadData::CTSolid> > m_vcSolids;
  std::vector<CGocadData::CVset*> m_vcVsets;
  std::vector<CGocadData::CTSurf*> m_vcTSurfs;
  CGocadData::CTSolid* m_pSolid;
  CGocadData::CVset* m_pVset;
  CGocadData::CTSurf* m_pTSurf;
  CGocadData::CTVolume* m_pVolume;
  CGocadData::CTetra   *m_pTetra;
  QSharedPointer <CGocadData::CSurface> m_pSurface;
  QSharedPointer <CGocadData::CTFace> m_pTFace;
  std::vector<CGocadData::CTVolume *> m_tetraVolumes;
  int m_currentTri;


  class CReadStatusException
  {
  public:
  CReadStatusException(const QString& strError);
  CReadStatusException(const CReadStatusException& rhs);
  const QString& Error() const;

  private:
  CReadStatusException& operator = (CReadStatusException rhs);

  QString m_strError;
  };

  class CReadStatus
  {
  public:
  CReadStatus();
  CReadStatus(const CReadStatus& rhs);
  CReadStatus& operator = (CReadStatus rhs);

  bool SectionOpen() const;
  bool ReadingHeader() const;
  bool HeaderSeen() const;
  bool ZPositiveSeen() const;
  bool AxisUnitSeen() const;
  bool PropertiesSeen() const;
  bool TetraPropertiesSeen() const;

  void OpenSection();
  void CloseSection();
  void StartHeader();
  void StartZPositive();
  void StartAxisUnit();
  void StartProperties();
  void StartTetraProperties();

  private:
  void swap(CReadStatus& rhs);

  bool m_bSectionOpen; // reading between { and }
  bool m_bReadingHeader; // reading section HEADER
  bool m_bHeaderSeen;
  bool m_bZPositiveSeen;
  bool m_bAxisUnitSeen;
  bool m_bPropertiesSeen;
  bool m_bTetraPropertiesSeen;
  };

  CReadStatus m_status;

//  typedef std::map<int, int> TNodeIndexMap; // map from VRTX/ATOM ids to mesh node indices
};

#endif // !defined(AFX_GOCADIMPORT_H__C6DFED81_A91B_4D7D_B1F1_CBE211725A9C__INCLUDED_)
#endif