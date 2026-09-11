// ShellGoCadFile.h: interface for the CShellGoCadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLGOCADFILE_H__8A7938A6_3067_409F_9FFE_AFB87E6DC822__INCLUDED_)
#define AFX_SHELLGOCADFILE_H__8A7938A6_3067_409F_9FFE_AFB87E6DC822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FvGocadFile.h"
#include "SurfaceDesc.h"

class CTetraMesh;
class CTetraSurface;
class CTetraSubHorizon;

namespace geo {
class CTetrahedron;
}

class CShellGoCadFile : public CGoCadFile {
public:
  friend class CShellParser;
  class CShellSurfaceDesc : public geo::CSurfaceDesc {
  public:
    enum eTetOrientation { MINUS = 0, PLUS };

  private:
    typedef std::pair<int, eTetOrientation> TIndexOrientPr;
    typedef std::pair<const geo::CTetrahedron *, TIndexOrientPr>
        TTetIndexPr; // int is an index refering to one of the faces of the tetrahedron (either 0, 1, 2)
    std::vector<TTetIndexPr> m_vcTotalTets;

  public:
    CShellSurfaceDesc(geo::CTetMeshBase &mesh, const geo::ISurface &surface, const std::string &sName, bool bFault);
    void SetInputSurface(const geo::ISurface &surface);
    //		void SetInputFault(const geo::ISurface &front, const geo::ISurface &back);
    //		void ToggleSlip();
    void AddTetIndexPair(const geo::CTetrahedron *Tet, int nIndex, eTetOrientation orientation);
    void SetContactSurface();
  };

private:
  typedef std::map<const std::string, CShellSurfaceDesc *> TSurfaceMap;
  typedef std::map<const std::string, geo::CBodyGroup *> TRegionMap;
  TSurfaceMap m_mpSurfaces;
  TRegionMap m_mpRegions;
  int m_nCurrentTetIndex;
  std::vector<int> m_vcPtIndices;
  CTetraMesh &m_tetmesh;
  int SurfaceDescriptorIndex(const CShellSurfaceDesc &desc) const;
  CTetraSurface *CreateSurface(const std::string &name, CShellSurfaceDesc &desc);
  CTetraSubHorizon *CreateSubHorizon(const std::string &name, CShellSurfaceDesc &desc, bool bFlt);
  std::vector<const geo::IFace *> InputSurface(const CShellSurfaceDesc &desc) const;
  bool SideSurface(const CShellSurfaceDesc &desc) const;
  // Functions to calculate side and other side
  typedef std::set<int> TTriangle;
  typedef std::set<int> TPointSet;
  typedef std::set<TTriangle> TFaceSet;

  bool m_bOrderSwitched;

protected:
  virtual bool OnRead(TInputStream &stream);

public:
  CShellGoCadFile(CTetraMesh &tetmesh);
  virtual ~CShellGoCadFile();

private:
  virtual bool ReadTetrahedron(TInputStream &stream, const QString &sToken);
  geo::CBodyGroup &AddBodyToRegion(const std::string &sName);
  void AddFaceToSurface(const std::string &sName, geo::CBodyGroup &group, int nFaceIndex,
                        CShellSurfaceDesc::eTetOrientation orientation);
};

#endif // !defined(AFX_SHELLGOCADFILE_H__8A7938A6_3067_409F_9FFE_AFB87E6DC822__INCLUDED_)
