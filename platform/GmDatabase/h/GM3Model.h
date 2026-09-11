// 3DModel.h: interface for the C3DModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DMODEL_H__01F55A00_0BA5_40BF_9436_440608ED0EA2__INCLUDED_)
#define AFX_3DMODEL_H__01F55A00_0BA5_40BF_9436_440608ED0EA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GM3Timestep.h"
#include "GM3Horizon.h"
#include "GM3MeshZone.h"
#include "GM3CrossSection.h"
#include "GM3Formation.h"
#include "GM3WellPath.h"
#include "GMModel.h"

namespace gm
{
class C3DDatabase;
//##ModelId=3BA8A1C8036F
class C3DModel : public CModel
{
  geo::CPoint m_min;
  geo::CPoint m_max;
  double m_dRectangleSize;
  BOOL	m_bEclipseFile;
  std::vector<C3DTimestep>		m_vcTimestep;		// Timesteps of the model ...
  std::vector<C3DHorizon>			m_vcHorizon;
  std::vector<C3DFormation>		m_vcFormation;
  std::vector<C3DMeshZone>		m_vcMeshZone;
  std::vector<C3DCrossSection>	m_vcCrossSection;
  std::vector<C3DWellPath>		m_vcWellPath;
  BOOL m_bTimeAnalysis;
public:


  //##ModelId=3BA8A1C90013
#ifndef _WIN64 // not available on Win64
  void LoadModel(const CString &strPath, const BOOL bComplete, IProgressBase& callback);
#endif

  //##ModelId=3BA8A1C90017
  const std::vector<C3DHorizon>& Horizons() const;
  const std::vector<C3DFormation>& Formations() const;
  const std::vector<C3DCrossSection>& CrossSections() const;
  const std::vector<C3DWellPath>& WellPaths() const;

  virtual int TimestepSize() const;
  virtual const CTimestep& Timestep(int nIndex) const;

  // Boundaries
  const geo::IPoint& Min() const;
  const geo::IPoint& Max() const;

  // Mesh Features
  const double& RectangleSize() const;
  const std::vector<C3DMeshZone>& MeshZones() const;

  //##ModelId=3BA8A1C90025
  C3DModel();
  //##ModelId=3BA8A1C90026
  virtual ~C3DModel();
private:
  // Read functions ...
  //##ModelId=3BA8A1C90028
  void ReadGlobalSettings(C3DDatabase &db, IProgressBase& callback);
  //##ModelId=3BA8A1C90034
  void ReadTimeSteps(C3DDatabase &db, IProgressBase& callback);
  void ReadMesh(C3DDatabase &db, IProgressBase& callback);

};

} // End of namespace gm3

#endif // !defined(AFX_3DMODEL_H__01F55A00_0BA5_40BF_9436_440608ED0EA2__INCLUDED_)
