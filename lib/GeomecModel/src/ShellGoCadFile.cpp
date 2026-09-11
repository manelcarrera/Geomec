// ShellGoCadFile.cpp: implementation of the CShellGoCadFile class.
//
//////////////////////////////////////////////////////////////////////
#include "ShellGoCadFile.h"
#include "BodyTriangle.h"
#include "ITetrahedron.h"
#include "TetSurface.h"
#include "TetraFormation.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSurface.h"
#include "Tetrahedron.h"

//////////////////////////////////////////////////////////////////////
// Implementation of shell surface descriptor
//////////////////////////////////////////////////////////////////////

CShellGoCadFile::CShellSurfaceDesc::CShellSurfaceDesc(geo::CTetMeshBase &mesh, const geo::ISurface &surface,
                                                      const std::string &sName, bool bSlip)
    : CSurfaceDesc(mesh, surface, sName, bSlip) {}

void CShellGoCadFile::CShellSurfaceDesc::SetInputSurface(const geo::ISurface & /*surface*/) {
  assert(!Slip());
  //	m_pSurface = &surface;
}
/*
void CShellGoCadFile::CShellSurfaceDesc::SetInputFault(const geo::ISurface &front, const geo::ISurface &back)
{
  m_pSurface = &front;
  m_pBack = &back;
//	assert(m_pContactSurface);
}

void CShellGoCadFile::CShellSurfaceDesc::ToggleSlip()
{
  // Toggle from fault to horizon and vice versa
  if(Slip())
  {
    m_pBack = 0;
  }
  else
  {
    SetContactSurface();
    m_pBack = m_pSurface;
  }
}
*/
void CShellGoCadFile::CShellSurfaceDesc::AddTetIndexPair(const geo::CTetrahedron *Tet, int nIndex,
                                                         eTetOrientation orientation) {
  TIndexOrientPr prIndOr(nIndex, orientation);
  TTetIndexPr prTetIndex(Tet, prIndOr);
  m_vcTotalTets.push_back(prTetIndex);
}

void CShellGoCadFile::CShellSurfaceDesc::SetContactSurface() {
  for (int i = 0; i < m_vcTotalTets.size(); i++) {
    TTetIndexPr prTI_1 = m_vcTotalTets[i];
    switch (prTI_1.second.second) {
    case MINUS: {
      //				new geo::CContactElement(*m_pContactSurface, *prTI_1.first, prTI_1.second.first, true);
      break;
    }
    case PLUS: {
      //				new geo::CContactElement(*m_pContactSurface, *prTI_1.first, prTI_1.second.first, false);
      break;
    }
    default:
      assert(false);
    }
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShellGoCadFile::CShellGoCadFile(CTetraMesh &tetmesh)
    : CGoCadFile(*tetmesh.m_pMesh), m_tetmesh(tetmesh), m_bOrderSwitched(false) {
  // the # comment tag must be undefined, we need the contents for the Shell specific data
  VERIFY(UndefineComment("#"));
}

CShellGoCadFile::~CShellGoCadFile() {}

CTetraSurface *CShellGoCadFile::CreateSurface(const std::string &name, CShellSurfaceDesc &desc) {
  CTetraSurface *pSurface = new CTetraSurface(name.c_str(), InputSurface(desc), m_tetmesh.Model());
  desc.SetInputSurface(pSurface->Surface());
  return pSurface;
}

CTetraSubHorizon *CShellGoCadFile::CreateSubHorizon(const std::string &name, CShellSurfaceDesc &desc, bool bFlt) {
  CTetraSurface *pSurface = CreateSurface(name, desc);
  CTetraSubHorizon *pHorizon = new CTetraSubHorizon(*pSurface, pSurface->Model(), bFlt);
  m_tetmesh.m_vcSurfaceSource[SurfaceDescriptorIndex(desc)] = pSurface;
  return pHorizon;
}

std::vector<const geo::IFace *> CShellGoCadFile::InputSurface(const CShellSurfaceDesc &desc) const {
  std::vector<const geo::IFace *> vcRet;
  TFaceSet stTriangle;
  for (int nSurface = 0; nSurface < desc.TetSurfaceSize(); nSurface++) {
    const geo::CTetSurface &surface = desc.TetSurface(nSurface);
    for (int nFace = 0; nFace < surface.FaceSize(); nFace++) {
      const geo::CBodyTriangle &face = dynamic_cast<const geo::CBodyTriangle &>(surface.Face(nFace));
      assert(face.Parent()->IndexingElementSet() == m_tetmesh.m_pMesh);
      TTriangle triangle;
      for (int i = 0; i < face.NrOfPoints(); i++) {
        triangle.insert(face.Parent()->PointIndex(face.BodyNode(i)));
      }

      assert(triangle.size() == 3);
      if ((stTriangle.insert(triangle)).second)
        vcRet.push_back(&face);
    }
  }

  return vcRet;
}

bool CShellGoCadFile::SideSurface(const CShellSurfaceDesc &desc) const {
  TFaceSet stTriangle;
  for (int nSurface = 0; nSurface < desc.TetSurfaceSize(); nSurface++) {
    const geo::CTetSurface &surface = desc.TetSurface(nSurface);
    for (int nFace = 0; nFace < surface.FaceSize(); nFace++) {
      const geo::CBodyTriangle &face = dynamic_cast<const geo::CBodyTriangle &>(surface.Face(nFace));
      assert(face.Parent()->IndexingElementSet() == m_tetmesh.m_pMesh);
      TTriangle triangle;
      for (int i = 0; i < face.NrOfPoints(); i++) {
        triangle.insert(face.PointIndex(i));
      }

      assert(triangle.size() == 3);
      if (!(stTriangle.insert(triangle)).second)
        return false;
    }
  }

  return true;
}

bool CShellGoCadFile::OnRead(TInputStream &stream) {
  // First we lock the mesh
  m_tetmesh.Lock();

  if (!CGoCadFile::OnRead(stream))
    return false;

  assert(m_tetmesh.m_pMesh->NrOfSurfaces() == m_mpSurfaces.size());
  // Create horizons and the boundary now
  typedef std::vector<std::pair<std::string, CShellSurfaceDesc *>> TSurfaceVec;
  TSurfaceVec vcSideSurface, vcSubHorizon;
  m_tetmesh.m_vcSurfaceSource.resize(m_tetmesh.m_pMesh->NrOfSurfaces());
  for (TSurfaceMap::iterator it = m_mpSurfaces.begin(); it != m_mpSurfaces.end(); it++) {
    // Push side surfaces on the vector
    if (SideSurface(*it->second))
      vcSideSurface.push_back(*it);
    else
      vcSubHorizon.push_back(*it);
  }

  if (vcSideSurface.size() > 1) {
    CTetraModel &model = dynamic_cast<CTetraModel &>(m_tetmesh.Model());

    // Search for upper and lower horizon in side surfaces
    int nLowerHorizon = 0;
    int nUpperHorizon = 1;
    int i;
    for (i = 0; i < vcSideSurface.size(); i++) {
      if (vcSideSurface[i].second->TetSurface(0).Max().Z() <
          vcSideSurface[nUpperHorizon].second->TetSurface(0).Max().Z())
        nUpperHorizon = i;
      if (vcSideSurface[i].second->TetSurface(0).Min().Z() >
          vcSideSurface[nLowerHorizon].second->TetSurface(0).Min().Z())
        nLowerHorizon = i;
    }

    // Create side surfaces first
    for (i = 0; i < vcSideSurface.size(); i++) {
      if (!((nLowerHorizon == i) || (nUpperHorizon == i))) {
        CTetraSurface *pSurface = CreateSurface(vcSideSurface[i].first, *vcSideSurface[i].second);
        model.SubBoundary().ConnectItem(*pSurface);
        m_tetmesh.m_vcSurfaceSource[SurfaceDescriptorIndex(*vcSideSurface[i].second)] = &model.SubBoundary();
      }
    }

    // Create upper horizon and link to sub boundary
    CTetraSubHorizon *pUpperHorizon =
        CreateSubHorizon(vcSideSurface[nUpperHorizon].first, *vcSideSurface[nUpperHorizon].second,
                         vcSideSurface[nUpperHorizon].second->Slip());
    model.SubBoundary().TopHorizon().LinkTo(*pUpperHorizon);
    assert(m_tetmesh.m_pMesh->NrOfSurfaces() > 0);

    // Create lower horizon and link to sub boudnary
    CTetraSubHorizon *pLowerHorizon =
        CreateSubHorizon(vcSideSurface[nLowerHorizon].first, *vcSideSurface[nLowerHorizon].second,
                         vcSideSurface[nUpperHorizon].second->Slip());
    model.SubBoundary().BottomHorizon().LinkTo(*pLowerHorizon);
  }

  // Create other sub horizon
  for (int i = 0; i < vcSubHorizon.size(); i++) {
    // Create surfaces and horizons
    CreateSubHorizon(vcSubHorizon[i].first, *vcSubHorizon[i].second, vcSubHorizon[i].second->Slip());
  }

  m_tetmesh.Unlock();

  return true;
}

bool CShellGoCadFile::ReadTetrahedron(TInputStream &stream, const QString & /*sToken*/) {
  m_vcPtIndices.clear();
  int nIndex1, nIndex2, nIndex3, nIndex4;

  stream >> nIndex1;
  m_vcPtIndices.push_back(nIndex1);
  stream >> nIndex2;
  m_vcPtIndices.push_back(nIndex2);
  stream >> nIndex3;
  m_vcPtIndices.push_back(nIndex3);
  stream >> nIndex4;
  m_vcPtIndices.push_back(nIndex4);

  CGoCadVolume *pCurrentVol = LatestVol();
  const geo::IPoint &p1 = pCurrentVol->GetGoCadPoint(nIndex1);
  const geo::IPoint &p2 = pCurrentVol->GetGoCadPoint(nIndex2);
  const geo::IPoint &p3 = pCurrentVol->GetGoCadPoint(nIndex3);
  const geo::IPoint &p4 = pCurrentVol->GetGoCadPoint(nIndex4);
  if (geo::ITetrahedron::CheckOrientation(p1, p2, p3, p4)) {
    m_bOrderSwitched = false;
    m_nCurrentTetIndex = InsertTetInVol(nIndex1, nIndex2, nIndex3, nIndex4);
  } else {
    m_bOrderSwitched = true;
    m_nCurrentTetIndex = InsertTetInVol(nIndex1, nIndex2, nIndex4, nIndex3);
  }

  stream.eatwhite();
  if (stream.peek() == '#') {
    // Expecting extra information
    stream.get();
    QString sKeyword;
    stream >> sKeyword;
    if (sKeyword.toUpper() != "CTETRA")
      throw CReadException("Keyword CTETRA expected.");
    stream >> sKeyword;
    geo::CBodyGroup &body_group = AddBodyToRegion(sKeyword.toStdString());
    // Look for the 4 sides
    for (int i = 0; i < 4; i++) {
      QString sSideName;
      stream >> sSideName;
      if (sSideName.toUpper() == "NONE") {

      } else if (sSideName.at(0) == '-' || sSideName.at(0) == '+') {
        CShellGoCadFile::CShellSurfaceDesc::eTetOrientation orientation(CShellGoCadFile::CShellSurfaceDesc::MINUS);
        if (sSideName.at(0) == '+')
          orientation = CShellGoCadFile::CShellSurfaceDesc::PLUS;
        sSideName = sSideName.mid(1);

        if (sSideName.toUpper().indexOf("(BOUNDARY)") != -1)
          sSideName = sSideName.mid(10); // Cut off boundary

        int nFaceIndex = i;
        if (m_bOrderSwitched && i == 2)
          nFaceIndex = 3;
        if (m_bOrderSwitched && i == 3)
          nFaceIndex = 2;

        AddFaceToSurface(sSideName.toStdString(), body_group, nFaceIndex, orientation);

      } else {
        throw CReadException("Side name or NONE expected.");
      }
    }
  }

  return true;
}

// This function tries to add a region and puts the index of the current tet in its TRegion vector
// If the region already exists, the index is just added to the region's vector.
geo::CBodyGroup &CShellGoCadFile::AddBodyToRegion(const std::string &sName) {
  geo::CBodyGroup *pGroup = 0;
  TRegionMap::iterator it = m_mpRegions.find(sName);
  if (it == m_mpRegions.end()) { // entry does not exist yet. Create a tetvolume
    std::vector<int> vcTetIndex;
    int nGroupIndex = m_tetmesh.m_pMesh->RegisterTetVolume(vcTetIndex);
    pGroup = const_cast<geo::CBodyGroup *>(&m_tetmesh.m_pMesh->Volume(nGroupIndex));
    // Create a formation and a formation volume
    CModelBase &model = dynamic_cast<CModelBase &>(m_tetmesh.Model());
    CTetraFormation *pFormation = new CTetraFormation(sName.c_str(), model);

    /*CTetraFormationVolume *pVolume =*/new CTetraFormationVolume(*pFormation, *pGroup);

    /* From IFormationElementSet constructor */
    /*		pVolume->reParent(pFormation); */
    //		pVolume->LinkTo(model.Mesh());
    /* ************************************* */

    m_mpRegions.insert(TRegionMap::value_type(sName, pGroup));
  } else { // entry is present, just add.
    pGroup = it->second;
  }

  const geo::CTetrahedron &tet =
      dynamic_cast<const geo::CTetrahedron &>(m_tetmesh.m_pMesh->Element(m_nCurrentTetIndex));
  pGroup->AddBody(tet);
  return *pGroup;
}

int CShellGoCadFile::SurfaceDescriptorIndex(const CShellSurfaceDesc &desc) const {
  assert(m_tetmesh.m_pMesh->NrOfSurfaces() > 0);
  for (int i = 0; i < m_tetmesh.m_pMesh->NrOfSurfaces(); i++) {
    if (&desc == &m_tetmesh.m_pMesh->SurfaceDesc(i))
      return i;
  }
  assert(false);
  return -1;
}

void CShellGoCadFile::AddFaceToSurface(const std::string &sName, geo::CBodyGroup &group, int nFaceIndex,
                                       CShellSurfaceDesc::eTetOrientation orientation) {
  // nFaceIndex	|	Face index in geo::ITetrahedron (m_bOrderSwitched = false)
  //		0		|				1
  //		1		|				3
  //		2		|				2
  //		3		|				0

  assert(nFaceIndex < 4);

  // Search input surface
  CShellSurfaceDesc *pInputSurface = 0;
  TSurfaceMap::iterator it = m_mpSurfaces.find(sName);
  if (it == m_mpSurfaces.end()) {
    // entry does not exist yet. Create a surface for input in Geomec

    bool bFault = false;
    // If "fault" is a part of sName, the surface is a fault
    if (sName.size() >= 5) {
      // Try to find the word "fault" in the name of the surface (sName)
      std::string fault = "fault";
      int steps = sName.size() - 4;
      for (int i = 0; i < steps; i++) {
        std::string sub = sName.substr(i, 5);
        if (!QString::compare(sub.c_str(), fault.c_str(), Qt::CaseInsensitive)) // case insensitive comparison
        {
          bFault = true;
          break;
        }
      }
    }

    geo::ISurface *pSurfaceTemp = 0;
    int nSurfaceIndex = m_tetmesh.m_pMesh->NrOfSurfaces();
    pInputSurface = new CShellSurfaceDesc(*m_tetmesh.m_pMesh, *pSurfaceTemp, sName, bFault);

    assert(nSurfaceIndex < m_tetmesh.m_pMesh->NrOfSurfaces());
    assert(pInputSurface->TetSurfaceSize() == 0);
    m_mpSurfaces.insert(TSurfaceMap::value_type(sName, pInputSurface));
  } else {
    pInputSurface = it->second;
  }

  // Try to find the right tet surface
  geo::CBodyGroup::CSideSurface *pSideSurface = 0;
  for (int i = 0; i < pInputSurface->TetSurfaceSize(); i++) {
    if (&pInputSurface->TetSurface(i).BodyGroup() == &group)
      pSideSurface = const_cast<geo::CTetSurface *>(&pInputSurface->TetSurface(i));
  }
  if (pSideSurface == 0) {
    // Create new tet surface
    pSideSurface = new geo::CTetSurface(*pInputSurface, group);
  }

  // Get the tetrahedron
  const geo::CTetrahedron &tet =
      dynamic_cast<const geo::CTetrahedron &>(m_tetmesh.m_pMesh->Element(m_nCurrentTetIndex));

  // Get the current body face
  const geo::IFace *pFace = 0;
  switch (nFaceIndex) {
  case 0: {
    pFace = &tet.Face(1);
    pInputSurface->AddTetIndexPair(&tet, 1, orientation);
    break;
  }
  case 1: {
    pFace = &tet.Face(3);
    pInputSurface->AddTetIndexPair(&tet, 3, orientation);
    break;
  }
  case 2: {
    pFace = &tet.Face(2);
    pInputSurface->AddTetIndexPair(&tet, 2, orientation);
    break;
  }
  case 3: {
    pFace = &tet.Face(0);
    pInputSurface->AddTetIndexPair(&tet, 0, orientation);
    break;
  }
  default:
    assert(false);
  }

  pSideSurface->AddFace(*pFace);
}
