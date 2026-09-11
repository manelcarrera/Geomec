// EclipseModel.cpp: implementation of the CEclipseModel class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "BaseEntryTypes.h"
#include "EclipseModel.h"
#include "ElementSet.h"
#include "FvEclipseFile.h"
#include "FvEclipseReservoir.h"
#include "GeoSurface.h"
#include "Global.h"
#include "GlobalMessage.h"
#include "IProgressFactory.h"
#include "TetraEntryTypes.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSurface.h"
#include "Triangle.h"
#include "resourceIDI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEclipseModel::CEclipseModel(CAnalysisLogger &logger, const CVersionManager &versionManager)
    : CTetraModel(logger, versionManager), m_nUpperHorizon(0), m_nLowerHorizon(0), m_pFrontSurface(0),
      m_pBackSurface(0), m_pLeftSurface(0), m_pRightSurface(0) {}

void CEclipseModel::InitFile(const CEclipseFile &file) {
  // For the voxel indices yields
  // SizeXv = file.SizeX() = the total amount of voxels in the x direction
  // SizeYv = file.SizeY() = the total amount of voxels in the y direction
  // SizeZv = file.SizeZ() = the total amount of voxels in the z direction

  // For the point indices yields
  // SizeXp = SizeXv + 1 = the total amount of voxels in the x direction
  // SizeYp = SizeYv + 1 = the total amount of voxels in the x direction
  // SizeZp = SizeZv + 1 = the total amount of voxels in the x direction

  // Allocate SizeZp potential horizons and allocate a grid of SizeXp x SizeYp points for each horizon.
  // m_vcHorizon.resize(file.SizeZ() + 1, THorizon(TEclipseHorizon(file.SizeX() + 1, TRow(file.SizeY() + 1)), 0));
  m_vcHorizon.resize(file.HorizonSize(),
                     THorizon(TEclipseHorizon(file.SizeX() + 1, TRow(file.SizeY() + 1)), (CTetraSubHorizon *)0));

  size_t Zp; //, Xp, Yp;

  for (Zp = 0; Zp < m_vcHorizon.size(); Zp++) {
    m_vcHorizon[Zp].first = file.Horizon(Zp);
  }

  // Determine reservoirs by walking over the voxels
  m_vcReservoir.resize(0);

  const CEclipseReservoir *pReservoir = 0;

  // Vz is the voxel index in the z direction
  for (int Vz = 0; Vz < file.SizeZ(); Vz++) {
    if (pReservoir != &file.Reservoir(Vz)) {
      // Allocate new reservoir with the corresponding horizons and points
      pReservoir = &file.Reservoir(Vz);
      m_vcReservoir.push_back(TEclipseReservoir(pReservoir->LowerBoundary(), pReservoir->UpperBoundary() + 1));
    }
  }

  // Store all the original data (unmerged)
  for (int z = 0; z < file.SizeZ(); z++) {
    CEclipseReservoir *pRes = new CEclipseReservoir(file.Reservoir(z));
    m_vcOriginalData.push_back(pRes);
  }

  // Initially enable all horizons
  for (int i = 0; i < HorizonSize(); i++)
    HorizonEnable(i, true);

  // As soon as an appropriate element is available to represent the eclipse cells, uncomment the follwing line:
  new CElementSet(file, *this);
}

CEclipseModel::~CEclipseModel() {
  for (size_t nOrigData = 0; nOrigData < m_vcOriginalData.size(); nOrigData++)
    delete m_vcOriginalData[nOrigData];
}

int CEclipseModel::ReservoirSize() const { return m_vcReservoir.size(); }

std::pair<int, int> CEclipseModel::Reservoir(int nReservoirIndex) const {
  assert(nReservoirIndex >= 0);
  assert(nReservoirIndex < m_vcReservoir.size());
  return m_vcReservoir[nReservoirIndex];
}

long CEclipseModel::SavedItems() const {
  long lRet = CTetraModel::SavedItems();

  //
  for (size_t nHorizon = 0; nHorizon < m_vcHorizon.size(); nHorizon++) {
    for (size_t nRow = 0; nRow < m_vcHorizon[nHorizon].first.size(); nRow++) {
      lRet += m_vcHorizon[nHorizon].first[nRow].size();
    }
  }

  int nX, nY;
  for (size_t nRes = 0; nRes < m_vcOriginalData.size(); nRes++) {
    CEclipseReservoir *pRes = m_vcOriginalData[nRes];

    for (nX = 0; nX < pRes->DepthSizeX(); nX++) {
      for (nY = 0; nY < pRes->DepthSizeY(); nY++) {
        lRet += pRes->DepthSizeZ();
      }
    }

    for (nX = 0; nX < pRes->EnableSizeX(); nX++) {
      for (nY = 0; nY < pRes->EnableSizeY(); nY++) {
        lRet += pRes->EnableSizeZ();
      }
    }

    for (nX = 0; nX < pRes->GridSizeX(); nX++) {
      lRet += pRes->GridSizeY();
    }
  }
  return lRet;
}

int CEclipseModel::HorizonSize() const { return m_vcHorizon.size(); }

const CTetraSubHorizon *CEclipseModel::Horizon(int nHorizonIndex) const {
  assert(nHorizonIndex >= 0);
  assert(nHorizonIndex < m_vcHorizon.size());
  return m_vcHorizon[nHorizonIndex].second;
}

CTetraSubHorizon *CEclipseModel::Horizon(int nHorizonIndex) {
  assert(nHorizonIndex >= 0);
  assert(nHorizonIndex < m_vcHorizon.size());
  return m_vcHorizon[nHorizonIndex].second;
}

int CEclipseModel::PointSizeX() const {
  assert(m_vcHorizon.size() > 0);
  return m_vcHorizon[0].first.size();
}

int CEclipseModel::PointSizeY() const {
  assert(PointSizeX() > 0);
  return m_vcHorizon[0].first[0].size();
}

void CEclipseModel::HorizonEnable(int nHorizonIndex, bool bHorizonEnable) {
  if ((Horizon(nHorizonIndex) != 0) != bHorizonEnable) {
    if (Horizon(nHorizonIndex)) {
      // Destroy horizon and surface
      assert(Horizon(nHorizonIndex)->SurfaceSize() == 1);
      CSurfaceBase *pSurface = &Horizon(nHorizonIndex)->Surface(0);
      delete Horizon(nHorizonIndex);
      delete pSurface;
      m_vcHorizon[nHorizonIndex].second = 0;
    } else {
      // Create a horizon and a surface
      std::vector<const geo::IFace *> vcFaces;
      for (int Xp = 0; Xp < PointSizeX() - 1; Xp++) {
        for (int Yp = 0; Yp < PointSizeY() - 1; Yp++) {
          geo::IPoint &p1 = (geo::IPoint &)HorizonPoint(nHorizonIndex, Xp, Yp);
          geo::IPoint &p2 = (geo::IPoint &)HorizonPoint(nHorizonIndex, Xp + 1, Yp);
          geo::IPoint &p3 = (geo::IPoint &)HorizonPoint(nHorizonIndex, Xp, Yp + 1);
          geo::IPoint &p4 = (geo::IPoint &)HorizonPoint(nHorizonIndex, Xp + 1, Yp + 1);

          if (!p1.Empty() && !p2.Empty() && !p4.Empty()) {
            geo::CTriangle *pFirstTriangle = new geo::CTriangle(p1, p2, p4);
            vcFaces.push_back(pFirstTriangle);
          }
          if (!p1.Empty() && !p3.Empty() && !p4.Empty()) {
            geo::CTriangle *pSecondTriangle = new geo::CTriangle(p1, p3, p4);
            vcFaces.push_back(pSecondTriangle);
          }
        }
      }

      QString sName;
      sName = QString("Horizon %1").arg(nHorizonIndex + 1);

      // Create a horizon
      CTetraSurface *pSurface = new CTetraSurface(sName.toStdString().c_str(), vcFaces, *this);
      CTetraSubHorizon *pHorizon = new CTetraSubHorizon(*pSurface, pSurface->Model(), FALSE);
      m_vcHorizon[nHorizonIndex].second = pHorizon;
    }
  }
}

const geo::IPoint &CEclipseModel::HorizonPoint(int nHorizonIndex, int Xp, int Yp) const {
  assert(nHorizonIndex >= 0);
  assert(nHorizonIndex < m_vcHorizon.size());
  assert(Xp >= 0);
  assert(Xp < m_vcHorizon[nHorizonIndex].first.size());
  assert(Yp >= 0);
  assert(Yp < m_vcHorizon[nHorizonIndex].first[Xp].size());
  return m_vcHorizon[nHorizonIndex].first[Xp][Yp];
}

void CEclipseModel::ExtractCells(const CEclipseReservoir *pReservoir) {

  int i, j, k;
  for (i = pReservoir->LowerBoundary(); i < pReservoir->UpperBoundary(); i++) { // Loop over layers in rervoir

    for (j = 0; j < pReservoir->SizeX(); j++) { // Loop over columns in X direction in layer i

      for (k = 0; k < pReservoir->SizeY(); k++) { // Loop over cells in column j
        /*CEclipseCell* pCell =*/new CEclipseCell(pReservoir->Cell(j, k, i));
        //				m_vcCells.set_at(j, k, i, pCell);
      }
    }
  }
}

/*void CEclipseModel::MergePoints()
{
//	int i, j, k;

//	CEclipseCell* pCurrent;
//	geo::CPoint* pPt0;
//	geo::CPoint* pBuddy0;
//	geo::CPoint* pPt1;
//	geo::CPoint* pBuddy1;
//	geo::CPoint* pPt2;
//	geo::CPoint* pBuddy2;
//	geo::CPoint* pPt3;
//	geo::CPoint* pBuddy3;
//	geo::CPoint* pPt4;
//	geo::CPoint* pBuddy4;
//	geo::CPoint* pPt5;
//	geo::CPoint* pBuddy5;
//	geo::CPoint* pPt6;
//	geo::CPoint* pBuddy6;
//	geo::CPoint* pPt7;
//	geo::CPoint* pBuddy7;

  for(i = 0; i < m_vcCells.size_z(); i++)
  {
    for(j = 0; j < m_vcCells.size_y(); j++)
    {
      for(k = 0; k < m_vcCells.size_x(); k++)
      {
        pCurrent = m_vcCells.get_at(k, j, i);
        pPt0 = (geo::CPoint*)&(pCurrent->Point(0));
        pPt1 = (geo::CPoint*)&(pCurrent->Point(1));
        pPt2 = (geo::CPoint*)&(pCurrent->Point(2));
        pPt3 = (geo::CPoint*)&(pCurrent->Point(3));
        pPt4 = (geo::CPoint*)&(pCurrent->Point(4));
        pPt5 = (geo::CPoint*)&(pCurrent->Point(5));
        pPt6 = (geo::CPoint*)&(pCurrent->Point(6));
        pPt7 = (geo::CPoint*)&(pCurrent->Point(7));
      }
    }

  }
*/
//}

// ##ModelId=3C74E3F10188
bool CEclipseModel::NewModel(bool bAttachToDocument) {
  if (!m_fileName.isEmpty()) {
    // Try to read the eclipse file in ....
    CEclipseFile file;
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, ""));
      if (file.Open(m_fileName, *prog)) {
        CTetraModel::NewModel(bAttachToDocument);
        InitFile(file);
      } else {
        _m()->msg(file.ErrorMessage());
        return false;
      }

    } catch (CProgressCancel *e) {
      delete e;
      file.Close();
      return false;
    }

  } else
    return false; // File dialog is cancled

  assert(isOpen());

  return true;
}
// ##ModelId=3C74E3F101C5
void CEclipseModel::OnCloseModel() {
  // First we delete the geo objects

  // Delete the base model
  CTetraModel::OnCloseModel();
}

void CEclipseModel::createContainers() {
  CTetraModel::createContainers();
  new CEclipseSubModelEntry(*this);
}

unsigned int CEclipseModel::ModelIconId() const { return staticModelIconId(); }

QString CEclipseModel::documentType() const { return staticDocumentType(); }

CEclipseModel::ModelType CEclipseModel::modelType() const { return eEclipseModel; }

unsigned int CEclipseModel::staticModelIconId() { return IDI_ECLIPSE_MODEL; }

QString CEclipseModel::staticDocumentType() { return "Eclipse"; }

void CEclipseModel::setFileName(const QString &fileName) { m_fileName = fileName; }

bool CEclipseModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  if (!CTetraModel::OnLoad(stream, version, prog))
    return false;

  return LoadEclipse(stream, version, prog);
}

bool CEclipseModel::LoadEclipse(CStorageNode::TSTREAM &stream, CStreamVersion & /*version*/,
                                CStorageNode::TPROGRESS &prog) {
  int nHorizonSize;
  stream >> nHorizonSize;
  m_vcHorizon.resize(nHorizonSize);
  for (int nHorizon = 0; nHorizon < nHorizonSize; nHorizon++) {
    int nRowSize;
    stream >> nRowSize;
    m_vcHorizon[nHorizon].first.resize(nRowSize);
    for (int nRow = 0; nRow < nRowSize; nRow++) {
      int nPointSize;
      stream >> nPointSize;
      m_vcHorizon[nHorizon].first[nRow].resize(nPointSize);
      for (int nPoint = 0; nPoint < nPointSize; nPoint++) {
        double X, Y, Z;
        stream >> X;
        stream >> Y;
        stream >> Z;
        m_vcHorizon[nHorizon].first[nRow][nPoint] = geo::CPoint(X, Y, Z);
        prog.Step();
      }
    }
    int nHorizonIndex;
    stream >> nHorizonIndex;
    if (nHorizonIndex == -1)
      m_vcHorizon[nHorizon].second = 0;
    else {
      CTetraSubHorizonEntry *pEntry = dynamic_cast<CTetraSubHorizonEntry *>(GraphEntry(MD_TETRA_SUB_HORIZON));
      assert(pEntry);
      CTetraSubHorizonEntry::TNodeSet stNode = pEntry->EntryNodes();
      for (CTetraSubHorizonEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
        if ((*it)->Index() == nHorizonIndex)
          m_vcHorizon[nHorizon].second = *it;
      }
    }
  }

  int nSurfaceIndex;
  stream >> nSurfaceIndex;
  m_pFrontSurface = Surface(nSurfaceIndex);
  stream >> nSurfaceIndex;
  m_pBackSurface = Surface(nSurfaceIndex);
  stream >> nSurfaceIndex;
  m_pLeftSurface = Surface(nSurfaceIndex);
  stream >> nSurfaceIndex;
  m_pRightSurface = Surface(nSurfaceIndex);
  stream >> m_nUpperHorizon;
  stream >> m_nLowerHorizon;

  int nResSize, nRes;
  stream >> nResSize;
  m_vcReservoir.resize(nResSize);
  for (nRes = 0; nRes < nResSize; nRes++) {
    stream >> m_vcReservoir[nRes].first;
    stream >> m_vcReservoir[nRes].second;
  }

  int nOrigResSize;
  stream >> nOrigResSize;
  int nX, nY, nZ;
  int nXtot, nYtot, nZtot;
  int nUpBound, nLowBound, nbool;
  double depth, dX, dY, dZ;

  for (nRes = 0; nRes < nOrigResSize; nRes++) {
    stream >> nX;
    stream >> nY;
    CEclipseReservoir *pRes = new CEclipseReservoir(nX, nY);
    stream >> nLowBound;
    stream >> nUpBound;
    pRes->Boundary(nLowBound, nUpBound);

    stream >> nXtot;
    stream >> nYtot;
    stream >> nZtot;
    for (nX = 0; nX < nXtot; nX++) {
      for (nY = 0; nY < nYtot; nY++) {
        for (nZ = 0; nZ < nZtot; nZ++) {
          stream >> depth;
          pRes->Depth(nX, nY, nZ, depth);
          prog.Step();
        }
      }
    }

    stream >> nXtot;
    stream >> nYtot;
    stream >> nZtot;
    for (nX = 0; nX < nXtot; nX++) {
      for (nY = 0; nY < nYtot; nY++) {
        for (nZ = 0; nZ < nZtot; nZ++) {
          stream >> nbool;
          if (nbool)
            pRes->CellEnable(nX, nY, nZ, true);
          else
            pRes->CellEnable(nX, nY, nZ, false);
          prog.Step();
        }
      }
    }

    stream >> nXtot;
    stream >> nYtot;
    for (nX = 0; nX < nXtot; nX++) {
      for (nY = 0; nY < nYtot; nY++) {
        stream >> dX;
        stream >> dY;
        stream >> dZ;
        pRes->LowerGrid(nX, nY, geo::CPoint(dX, dY, dZ));

        stream >> dX;
        stream >> dY;
        stream >> dZ;
        pRes->UpperGrid(nX, nY, geo::CPoint(dX, dY, dZ));
        prog.Step();
      }
    }

    stream >> nbool;
    if (nbool)
      pRes->CircleCompleted(true);
    else
      pRes->CircleCompleted(false);

    stream >> nbool;
    if (nbool)
      pRes->Join(true);
    else
      pRes->Join(false);

    m_vcOriginalData.push_back(pRes);
  }

  return true;
}

CTetraSurface *CEclipseModel::Surface(int nIndex) {
  CSurfaceEntry *pEntry = dynamic_cast<CSurfaceEntry *>(GraphEntry(MD_BASE_SURFACE));
  assert(pEntry);
  CSurfaceEntry::TNodeSet stNode = pEntry->EntryNodes();
  for (CSurfaceEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    if ((*it)->Index() == nIndex) {
      CTetraSurface *pSurface = dynamic_cast<CTetraSurface *>(*it);
      if (pSurface)
        return pSurface;
    }
  }

  assert(false);
  return 0;
}

bool CEclipseModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog) {
  if (!CTetraModel::OnSave(stream, prog))
    return false;

  int nHorizonSize = m_vcHorizon.size();
  stream << nHorizonSize;

  for (int nHorizon = 0; nHorizon < nHorizonSize; nHorizon++) {
    stream << (int)m_vcHorizon[nHorizon].first.size();
    for (size_t nRow = 0; nRow < m_vcHorizon[nHorizon].first.size(); nRow++) {
      stream << (int)m_vcHorizon[nHorizon].first[nRow].size();
      for (size_t nPoint = 0; nPoint < m_vcHorizon[nHorizon].first[nRow].size(); nPoint++) {
        stream << m_vcHorizon[nHorizon].first[nRow][nPoint].X();
        stream << m_vcHorizon[nHorizon].first[nRow][nPoint].Y();
        stream << m_vcHorizon[nHorizon].first[nRow][nPoint].Z();
        prog.Step();
      }
    }

    // here we save the horizon pointer; second of the pair
    if (m_vcHorizon[nHorizon].second)
      stream << m_vcHorizon[nHorizon].second->Index();
    else
      stream << -1;
  }

  stream << m_pFrontSurface->Index();
  stream << m_pBackSurface->Index();
  stream << m_pLeftSurface->Index();
  stream << m_pRightSurface->Index();
  stream << m_nUpperHorizon;
  stream << m_nLowerHorizon;

  int nResSize;
  nResSize = m_vcReservoir.size();
  stream << nResSize;
  for (int nRes = 0; nRes < nResSize; nRes++) {
    stream << m_vcReservoir[nRes].first;
    stream << m_vcReservoir[nRes].second;
  }

  int nOrigResSize;
  nOrigResSize = m_vcOriginalData.size();
  stream << nOrigResSize;
  CEclipseReservoir *pRes;
  int nX, nY, nZ;
  for (int nOrigRes = 0; nOrigRes < nOrigResSize; nOrigRes++) {
    pRes = m_vcOriginalData[nOrigRes];
    stream << pRes->SizeX(); // X grid size
    stream << pRes->SizeY(); // Y grid size
    stream << pRes->LowerBoundary();
    stream << pRes->UpperBoundary();

    stream << pRes->DepthSizeX();
    stream << pRes->DepthSizeY();
    stream << pRes->DepthSizeZ();
    for (nX = 0; nX < pRes->DepthSizeX(); nX++) {
      for (nY = 0; nY < pRes->DepthSizeY(); nY++) {
        for (nZ = 0; nZ < pRes->DepthSizeZ(); nZ++) {
          stream << pRes->Depth(nX, nY, nZ);
          prog.Step();
        }
      }
    }

    stream << pRes->EnableSizeX();
    stream << pRes->EnableSizeY();
    stream << pRes->EnableSizeZ();
    for (nX = 0; nX < pRes->EnableSizeX(); nX++) {
      for (nY = 0; nY < pRes->EnableSizeY(); nY++) {
        for (nZ = 0; nZ < pRes->EnableSizeZ(); nZ++) {
          if (pRes->CellEnable(nX, nY, nZ))
            stream << 1;
          else
            stream << 0;
          prog.Step();
        }
      }
    }

    stream << pRes->GridSizeX();
    stream << pRes->GridSizeY();
    for (nX = 0; nX < pRes->GridSizeX(); nX++) {
      for (nY = 0; nY < pRes->GridSizeY(); nY++) {
        stream << pRes->LowerGrid(nX, nY).X();
        stream << pRes->LowerGrid(nX, nY).Y();
        stream << pRes->LowerGrid(nX, nY).Z();

        stream << pRes->UpperGrid(nX, nY).X();
        stream << pRes->UpperGrid(nX, nY).Y();
        stream << pRes->UpperGrid(nX, nY).Z();
        prog.Step();
      }
    }
    if (pRes->CircleCompleted())
      stream << 1;
    else
      stream << 0;
    if (pRes->Join())
      stream << 1;
    else
      stream << 0;
  }

  return true;
}

void CEclipseModel::CreateSubBoundary() {
  if (m_pFrontSurface) {
    m_pFrontSurface->UnLink(SubBoundary());
    delete m_pFrontSurface;
  }
  if (m_pBackSurface) {
    m_pBackSurface->UnLink(SubBoundary());
    delete m_pBackSurface;
  }
  if (m_pLeftSurface) {
    m_pLeftSurface->UnLink(SubBoundary());
    delete m_pLeftSurface;
  }
  if (m_pRightSurface) {
    m_pRightSurface->UnLink(SubBoundary());
    delete m_pRightSurface;
  }

  bool bFirst = false;
  for (int i = 0; i < HorizonSize(); i++) {
    if (Horizon(i) && !bFirst) {
      m_nUpperHorizon = i;
      bFirst = true;
      continue;
    }
    if (Horizon(i))
      m_nLowerHorizon = i;
  }

  // Create the side surfaces of the sub-boundary....
  int Zp, Xp, Yp;
  std::vector<const geo::IFace *> vcFacesFront;
  std::vector<const geo::IFace *> vcFacesBack;
  std::vector<const geo::IFace *> vcFacesRight;
  std::vector<const geo::IFace *> vcFacesLeft;
  for (Zp = m_nUpperHorizon; Zp < m_nLowerHorizon; Zp++) {
    int Yend = PointSizeY() - 1;
    for (Xp = 0; Xp < PointSizeX() - 1; Xp++) {
      geo::IPoint &p1 = (geo::IPoint &)HorizonPoint(Zp, Xp, 0);
      geo::IPoint &p2 = (geo::IPoint &)HorizonPoint(Zp + 1, Xp, 0);
      geo::IPoint &p3 = (geo::IPoint &)HorizonPoint(Zp + 1, Xp + 1, 0);
      geo::IPoint &p4 = (geo::IPoint &)HorizonPoint(Zp, Xp + 1, 0);

      if (!p1.Empty() && !p2.Empty() && !p3.Empty()) {
        if (!(p1 == p2 || p2 == p3 || p1 == p3)) {
          geo::CTriangle *pFirstTriangle = new geo::CTriangle(p1, p2, p3);
          vcFacesFront.push_back(pFirstTriangle);
        }
      }
      if (!p1.Empty() && !p4.Empty() && !p3.Empty()) {
        if (!(p1 == p4 || p4 == p3 || p1 == p3)) {
          geo::CTriangle *pSecondTriangle = new geo::CTriangle(p1, p3, p4);
          vcFacesFront.push_back(pSecondTriangle);
        }
      }

      geo::IPoint &p5 = (geo::IPoint &)HorizonPoint(Zp, Xp, Yend);
      geo::IPoint &p6 = (geo::IPoint &)HorizonPoint(Zp + 1, Xp, Yend);
      geo::IPoint &p7 = (geo::IPoint &)HorizonPoint(Zp + 1, Xp + 1, Yend);
      geo::IPoint &p8 = (geo::IPoint &)HorizonPoint(Zp, Xp + 1, Yend);

      if (!p5.Empty() && !p6.Empty() && !p7.Empty()) {
        if (!(p5 == p6 || p6 == p7 || p5 == p7)) {
          geo::CTriangle *pFirstTriangle = new geo::CTriangle(p5, p6, p7);
          vcFacesBack.push_back(pFirstTriangle);
        }
      }
      if (!p5.Empty() && !p8.Empty() && !p7.Empty()) {
        if (!(p5 == p8 || p8 == p7 || p5 == p7)) {
          geo::CTriangle *pSecondTriangle = new geo::CTriangle(p5, p7, p8);
          vcFacesBack.push_back(pSecondTriangle);
        }
      }
    }
  }
  for (Zp = m_nUpperHorizon; Zp < m_nLowerHorizon; Zp++) {
    int Xend = PointSizeX() - 1;
    for (Yp = 0; Yp < PointSizeY() - 1; Yp++) {
      geo::IPoint &p1 = (geo::IPoint &)HorizonPoint(Zp, Xend, Yp);
      geo::IPoint &p2 = (geo::IPoint &)HorizonPoint(Zp + 1, Xend, Yp);
      geo::IPoint &p3 = (geo::IPoint &)HorizonPoint(Zp + 1, Xend, Yp + 1);
      geo::IPoint &p4 = (geo::IPoint &)HorizonPoint(Zp, Xend, Yp + 1);

      if (!p1.Empty() && !p2.Empty() && !p3.Empty()) {
        if (!(p1 == p2 || p2 == p3 || p1 == p3)) {
          geo::CTriangle *pFirstTriangle = new geo::CTriangle(p1, p2, p3);
          vcFacesRight.push_back(pFirstTriangle);
        }
      }
      if (!p1.Empty() && !p4.Empty() && !p3.Empty()) {
        if (!(p1 == p4 || p4 == p3 || p1 == p3)) {
          geo::CTriangle *pSecondTriangle = new geo::CTriangle(p1, p3, p4);
          vcFacesRight.push_back(pSecondTriangle);
        }
      }

      geo::IPoint &p5 = (geo::IPoint &)HorizonPoint(Zp, 0, Yp);
      geo::IPoint &p6 = (geo::IPoint &)HorizonPoint(Zp + 1, 0, Yp);
      geo::IPoint &p7 = (geo::IPoint &)HorizonPoint(Zp + 1, 0, Yp + 1);
      geo::IPoint &p8 = (geo::IPoint &)HorizonPoint(Zp, 0, Yp + 1);

      if (!p5.Empty() && !p6.Empty() && !p7.Empty()) {
        if (!(p5 == p6 || p6 == p7 || p5 == p7)) {
          geo::CTriangle *pFirstTriangle = new geo::CTriangle(p5, p6, p7);
          vcFacesLeft.push_back(pFirstTriangle);
        }
      }
      if (!p5.Empty() && !p8.Empty() && !p7.Empty()) {
        if (!(p5 == p8 || p8 == p7 || p5 == p7)) {
          geo::CTriangle *pSecondTriangle = new geo::CTriangle(p5, p7, p8);
          vcFacesLeft.push_back(pSecondTriangle);
        }
      }
    }
  }

  m_pFrontSurface = new CTetraSurface("Front", vcFacesFront, *this);
  m_pBackSurface = new CTetraSurface("Back", vcFacesBack, *this);
  m_pLeftSurface = new CTetraSurface("Left", vcFacesLeft, *this);
  m_pRightSurface = new CTetraSurface("Right", vcFacesRight, *this);

  SubBoundary().ConnectItem(*m_pFrontSurface);
  SubBoundary().ConnectItem(*m_pBackSurface);
  SubBoundary().ConnectItem(*m_pLeftSurface);
  SubBoundary().ConnectItem(*m_pRightSurface);

  if (!SubBoundary().TopHorizon().IsLinkedTo(*Horizon(m_nUpperHorizon)))
    SubBoundary().TopHorizon().LinkTo(*Horizon(m_nUpperHorizon));
  if (!SubBoundary().BottomHorizon().IsLinkedTo(*Horizon(m_nLowerHorizon)))
    SubBoundary().BottomHorizon().LinkTo(*Horizon(m_nLowerHorizon));
}

CEclipseSubModelEntry::CEclipseSubModelEntry(CEclipseModel &model)
    : TSubModelEntry(MD_TETRA_SUB_ALL, IDI_ECLIPSE_MODEL, "Eclipse Sub Model", model) {}
