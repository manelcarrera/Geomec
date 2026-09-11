#include "WellTetraMesher.h"
#include "ModelBase.h"
#include "IWellModel.h"
#include "NewWellPath.h"
#include "TriangleSurface.h"
#include "TriSurface.h"
#include "Rectangle.h"
#include "BoundingBox.h"
#include "Triangle.h"

#ifdef _WIN32
#include "TetMeshCM2.h"
#else
#include "TetMeshBase.h"
#endif

#include "Global.h"
#include "GlobalMessage.h"
#include "IProgressFactory.h"
#include "IProgressBase.h"
#include "Progress_.h"
#include "MeshProgress.h"
#include "Environment.h"
#include "GeomecUtils.h"
#include "GeomecStringTable.h"
#include "QuestionThreadWrapper.h"

#include "HorizonBase.h"
#include "BaseEntryTypes.h"
#include "GeoSurface.h"
#include "BoundaryBase.h"

#include <cmath>
#include <iterator>
#include <algorithm>

#include "Interface.h" // IMesherDlg


CWellTetraMesher::CWellTetraMesher(const CModelBase& model, const std::vector<geo::CPoint>& well, double radius, double idealElementLength)
  : m_model(model)
  , m_radius(radius)
  , m_idealElementLength(idealElementLength)
{
  CreatePolyLine(well);
}

CWellTetraMesher::~CWellTetraMesher()
{
}

const CModelBase& CWellTetraMesher::Model() const
{
  return m_model;
}

int CWellTetraMesher::NrOfJobs() const
{
  return 4;
}

void CWellTetraMesher::CreatePolyLine(const std::vector<geo::CPoint>& well, double refineFactor)
{
  m_originalWell.reserve(well.size());
  for (size_t i = 0; i < well.size(); ++i)
  m_originalWell.push_back(well[i]);

  int wellLength = OriginalPolyLineLength();

  m_well.reserve(wellLength);
  m_well.push_back(m_originalWell[0]);

  double refineLength = refineFactor * IdealElementLength();
  double minimumLength = IdealElementLength() / refineFactor;

  for (int i = 1; i < wellLength; ++i)
  {
  geo::CVector dir(m_well.back(), m_originalWell[i]);
  double dist = dir.Length();
  if (dist > refineLength)
  {
      int cutSize = static_cast<int>(std::ceil(dist / IdealElementLength()));
      double cutDistance = dist / cutSize;

      dir = dir.UnitVector();

      for (int j = 0; j < cutSize - 1; ++j)
      {
    geo::CPoint p = m_well.back() + dir * cutDistance;
    m_well.push_back(p);
      }
  }
  if (dist >= minimumLength)
  {
      m_well.push_back(m_originalWell[i]);
  }
  }
}

void CWellTetraMesher::CreateAdjustmentData()
{
  int wellLength = PolyLineLength() - 1;

  m_adjustmentLine.reserve(wellLength);

  for (int i = 0; i < wellLength; ++i)
  {
  AdjustmentData data;
  data.line = geo::CVector(m_well[i], m_well[i + 1]);
  data.length = data.line.Length();
  data.length2 = data.line.SquareLength();
  data.length3 = 1.3 * data.length2;
  m_adjustmentLine.push_back(data);
  }
}

double CWellTetraMesher::Radius() const
{
  return m_radius;
}

double CWellTetraMesher::IdealElementLength() const
{
  return m_idealElementLength;
}

int CWellTetraMesher::PolyLineSize() const
{
  return static_cast<int>(std::round(2 * M_PI * Radius() / IdealElementLength()));
}

int CWellTetraMesher::PolyLineLength() const
{
  return static_cast<int>(m_well.size());
}

int CWellTetraMesher::OriginalPolyLineLength() const
{
  return static_cast<int>(m_originalWell.size());
}


geo::CTetMeshBase *CWellTetraMesher::CreateMesh(IProgressBase& prog)
{
  geo::CTetMeshBase *mesh = nullptr;

  prog.NextJob("Create grids");
  prog.AddSteps(4);

  Create2DGrid();
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  Create3DGrid(m_originalWell, m_original3DGrid);
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  Create3DGrid(m_well, m_3DGrid);
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  CreateAHDGrid();
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  prog.NextJob("Collect intersections with parent surfaces");
  prog.AddSteps(1);

  CollectParentSurfaces();
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WritePointset("C:\\Local\\wtm\\collect.dat", m_surfaces);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  prog.NextJob("Create surfaces");
  prog.AddSteps(11);

  Map3DGridTo2DSideSurface();
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WritePointset("C:\\Local\\wtm\\2dgrid.dat", m_2DGrid);
  Debug_WritePointset("C:\\Local\\wtm\\3dgrid.dat", m_3DGrid);
  Debug_WritePointset("C:\\Local\\wtm\\2dsidesurface.dat", m_2DSideSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }


  CTriangleSurface inputSideSurface;
  Map2DSideSurfaceToSurface(inputSideSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WritePointset("C:\\Local\\wtm\\inputsidesurface.dat", inputSideSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  MeshSurface(inputSideSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\inputsidesurface.ts", inputSideSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  std::vector<geo::CPoint> sidePoints;
  std::vector<std::vector<int> > sideTriangles;

  Convert2DSurfaceToVectors(inputSideSurface, sidePoints, sideTriangles);
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  Check2DSideSurfaceArea(sidePoints, sideTriangles);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\clean_inputsidesurface.ts", inputSideSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  std::vector<geo::CTriSurface *> outputSurfaces(3, nullptr);
  for (int i = 0; i < 3; ++i)
  outputSurfaces[i] = new geo::CTriSurface;

  Map2DSideSurfaceTo3DSideSurface(sidePoints, sideTriangles, *outputSurfaces[0]);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WritePointset("C:\\Local\\wtm\\outputsidesurface.dat", *outputSurfaces[0]);
  Debug_WriteSurface("C:\\Local\\wtm\\outputsidesurface.ts", *outputSurfaces[0]);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  CTriangleSurface topGridSurface;
  Create2DGridSurface(topGridSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\topGridSurface.ts", topGridSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  Map2DGridSurfaceToWellPoint(0, topGridSurface, *outputSurfaces[1]);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\topsurface.ts", *outputSurfaces[1]);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  CTriangleSurface bottomGridSurface;
  Create2DGridSurface(bottomGridSurface, true);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\bottomGridSurface.ts", bottomGridSurface);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  Map2DGridSurfaceToWellPoint(PolyLineLength() - 1, bottomGridSurface, *outputSurfaces[2]);
#ifdef DEBUG_WELL_TETRA_MESHER
  Debug_WriteSurface("C:\\Local\\wtm\\bottomsurface.ts", *outputSurfaces[2]);
#endif
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  CreateIntersectionSurfaces(outputSurfaces);
  try
  {
  prog.Step();
  }
  catch (CProgressCancel* c)
  {
  delete c;
  return mesh;
  }

  prog.NextJob("Create volume mesh");
  mesh = MeshVolume(outputSurfaces, prog);

  return mesh;
}



geo::CMatrix CWellTetraMesher::Create3DGridTransformationMatrix(const geo::IPoint& point, const geo::IVector& vecNormal) const
{
  geo::CMatrix matTransform(3, 4);

  assert(vecNormal == vecNormal.UnitVector());
  geo::CVector vecRotate(vecNormal.CrossProduct(geo::CVector::Zaxis));
  if (!vecRotate.isNullVector()) // do we need to rotate?
  {
  double dAngleRad = acos(vecNormal.DotProduct(geo::CVector::Zaxis));
  geo::CMatrix matRotate;
  matRotate.CreateRotationMatrixRad(vecRotate, -dAngleRad);
  assert(matRotate.RowSize() == 3 && matRotate.ColumnSize() == 3);

  int iRow, iCol;
  for (iRow = 0; iRow < 3; ++iRow)
  {
      for (iCol = 0; iCol < 3; ++iCol)
      {
    matTransform.Value(iRow, iCol, matRotate.Value(iRow, iCol));
      }
  }
  }
  else
  {
  // unity rotation
  matTransform.Value(0, 0, 1.);
  matTransform.Value(1, 1, 1.);
  matTransform.Value(2, 2, 1.);
  }

  // translation components
  matTransform.Value(0, 3, point.X());
  matTransform.Value(1, 3, point.Y());
  matTransform.Value(2, 3, point.Z());

  return matTransform;
}

void CWellTetraMesher::Create2DGrid()
{
  int size = PolyLineSize();

  m_2DGrid.resize(size);

  double factor = 2 * M_PI / size;

  for (int i = 0; i < size; ++i)
  {
  m_2DGrid[i].Set(Radius() * cos(i * factor), Radius() * sin(i * factor), 0);
  }
}

void CWellTetraMesher::Create3DGrid(TPolyline& well, T3DGrid& grid)
{
  int wellSize = PolyLineSize();
  int wellLength = static_cast<int>(well.size());

  grid.resize(m_2DGrid.size());
  for (int i = 0; i < wellSize; ++i)
  grid[i].resize(wellLength);

  const geo::CPoint *prevPoint = nullptr;
  geo::CVector lastNormal;

  for (int i = 0; i < wellLength; ++i)
  {
  geo::CPoint wellPoint = well[i];

  const geo::CPoint *nextPoint = i < wellLength - 1 ? &well[i + 1] : nullptr;
  geo::CVector normal = geo::CVector::NullVector;
  if (prevPoint)
      normal += geo::CVector(*prevPoint, wellPoint).UnitVector();
  if (nextPoint)
      normal += geo::CVector(wellPoint, *nextPoint).UnitVector();
  normal = normal.UnitVector();

  geo::CMatrix rotation = Create3DGridTransformationMatrix(wellPoint, normal);

  for (int j = 0; j < wellSize; ++j)
  {
      grid[j][i] = m_2DGrid[j];
      grid[j][i].Transform(rotation);
  }

  lastNormal = normal;

  prevPoint = &well[i];
  }
}

void CWellTetraMesher::CreateAHDGrid()
{
  int wellSize = PolyLineSize();
  int wellLength = PolyLineLength();

  double factor = Radius() * 2 * M_PI / wellSize;

  m_AHDGrid.resize(m_2DGrid.size());
  for (int i = 0; i < wellSize; ++i)
  {
  m_AHDGrid[i].resize(wellLength);
  m_AHDGrid[i][0] = 0;
  }

  for (int i = 1; i < wellLength; ++i)
  {
  for (int j = 0; j < wellSize; ++j)
  {
      geo::CPoint prevPoint = m_3DGrid[j][i - 1];
      geo::CPoint wellPoint = m_3DGrid[j][i];
      geo::CVector dist = wellPoint - prevPoint;

      m_AHDGrid[j][i] = m_AHDGrid[j][i - 1] + dist.Length();
  }
  }
}

void CWellTetraMesher::CollectParentSurfaces()
{
  const IWellModel *model = static_cast<const IWellModel *>(&Model());
  const CNewWellPath *wellPath = model->NewWellPath();
  if (!wellPath)
  return;

  const CModelBase *parentModel = static_cast<const CModelBase *>(&wellPath->Model());
  const THorizonBaseEntry *entry = static_cast<const THorizonBaseEntry *>(parentModel->GraphEntry(MD_BASE_HORIZON));
  const THorizonBaseEntry::TNodeSet horizons = entry->EntryNodes();

  geo::CPoint min(DBL_MAX, DBL_MAX, DBL_MAX);
  geo::CPoint max(-DBL_MAX, -DBL_MAX, -DBL_MAX);

  for (int i = 0; i < OriginalPolyLineLength(); ++i)
  {
  geo::CPoint wmin = m_originalWell[i].Min();
  geo::CPoint wmax = m_originalWell[i].Max();

  min.X(std::min(min.X(), wmin.X()));
  min.Y(std::min(min.Y(), wmin.Y()));
  min.Z(std::min(min.Z(), wmin.Z()));
  max.X(std::max(max.X(), wmax.X()));
  max.Y(std::max(max.Y(), wmax.Y()));
  max.Z(std::max(max.Z(), wmax.Z()));
  }

  min.X(min.X() - Radius());
  min.Y(min.Y() - Radius());
  min.Z(min.Z() - Radius());
  max.X(max.X() + Radius());
  max.Y(max.Y() + Radius());
  max.Z(max.Z() + Radius());

  for (THorizonBaseEntry::TNodeSet::const_iterator horizon = horizons.begin(); horizon != horizons.end(); ++horizon)
  {
  const std::set<CSurfaceBase *> surfaces = (*horizon)->Links<CSurfaceBase>();

  for (std::set<CSurfaceBase *>::const_iterator surf = surfaces.begin(); surf != surfaces.end(); ++surf)
  {
      SurfaceInfo surfInfo(*horizon, *surf, min, max, PolyLineSize());

      if (std::abs(surfInfo.max.Z() - m_well.back().Z()) < 1E-6)
    continue;

      if (std::abs(surfInfo.min.Z() - m_well.front().Z()) < 1E-6)
    continue;

      if (!surfInfo.indices.empty())
      {
    CollectSurface(surfInfo);

    if (!surfInfo.empty)
    {
          surfInfo.surfaceIndex = m_surfaces.size();
          m_surfaces.push_back(surfInfo);
    }
      }
  }
  }
}

void CWellTetraMesher::CollectSurface(SurfaceInfo& surfInfo)
{
  int wellSize = PolyLineSize();
  int wellLength = OriginalPolyLineLength();

  geo::CPoint line_min;
  geo::CPoint line_max;

  std::set<geo::CPoint, PointComparer> done;

  const geo::ISurface& surface = surfInfo.surface->Surface();

  bool written = false;

  for (int i = 0; i < wellSize; ++i)
  {
  for (int j = 0; j < wellLength - 1; ++j)
  {
      const geo::CPoint& point1 = m_original3DGrid[i][j];
      const geo::CPoint& point2 = m_original3DGrid[i][j + 1];

      geo::BBox::setBoundingBox(point1, point2, 0, line_min, line_max);

      if (geo::BBox::Intersects(line_min, line_max, surfInfo.min, surfInfo.max, true))
      {
    int faceSize = static_cast<int>(surfInfo.indices.size());

    for (int k = 0; k < faceSize; ++k)
    {
          const geo::IFace& face = surface.Face(surfInfo.indices[k]);

          if (geo::BBox::Intersects(line_min, line_max, face.Min(), face.Max(), true))
          {
      geo::CVector dir(point1, point2);

      double signedDist1 = face.SignedDistance(point1, true);
      double signedDist2 = face.SignedDistance(point2, true);

      if (signedDist1 * signedDist2 <= 0)
      {
              double factor = signedDist1 / (signedDist1 - signedDist2);
              geo::CPoint contact = point1 + dir * factor;

              double newSignedDist = face.SignedDistance(contact, true);

              contact = contact - face.Normal() * newSignedDist;

              newSignedDist = face.SignedDistance(contact, true);

              if (done.find(contact) == done.end() && face.Contains(contact, true))
              {
        surfInfo.usedIndices.insert(k);
        surfInfo.points[i].push_back(contact);
        surfInfo.empty = false;
        done.insert(contact);
              }
      }
          }
    }
      }
  }
  }
}

void CWellTetraMesher::Map3DGridTo2DSideSurface()
{
  int wellSize = PolyLineSize();
  int wellLength = PolyLineLength();

  double factor = Radius() * 2 * M_PI / wellSize;

  m_2DSideSurface.resize(m_2DGrid.size());
  for (int i = 0; i < wellSize; ++i)
  m_2DSideSurface[i].resize(wellLength);

  for (int i = 0; i < wellSize; ++i)
  {
  for (int j = 0; j < wellLength; ++j)
  {
      m_2DSideSurface[i][j].point.Set(i * factor, m_AHDGrid[i][j], 0);
  }
  }

  double epsilon = IdealElementLength() / 3;

  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
  for (size_t p = 0; p < m_surfaces[s].points[0].size(); ++p)
  {
      const geo::CPoint point = m_surfaces[s].points[0][p];

      double ahd = 0;

      for (int i = 1; i < wellLength; ++i)
      {
    geo::CPoint p1 = m_3DGrid[0][i - 1];
    geo::CPoint p2 = m_3DGrid[0][i];

    double l1 = geo::CVector(p1, point).Length();
    double l2 = geo::CVector(p1, p2).Length();

    double diff_l2_l1 = l2 - l1;

    if (l1 >= 0 && l1 < epsilon)
    {
          m_2DSideSurface[0][i - 1].point.Y(ahd + l1);
          m_2DSideSurface[0][i - 1].surface = s;
          break;
    }
    else if (diff_l2_l1 > 0 && diff_l2_l1 < epsilon)
    {
          m_2DSideSurface[0][i].point.Y(ahd + l1);
          m_2DSideSurface[0][i].surface = s;
          break;
    } else if (l1 < l2)
    {
          ahd += l1;
          m_2DSideSurface[0].push_back(GridPoint(geo::CPoint(0, ahd, 0), s));
          break;
    }
    else
    {
          ahd += l2;
    }
      }
  }
  }

  std::sort(m_2DSideSurface[0].begin(), m_2DSideSurface[0].end(), PointComparer());
}

double CWellTetraMesher::CalculateAHD(int column, const geo::IPoint & point) const
{
  int wellLength = static_cast<int>(m_3DGrid[column].size());

  double ahd = 0;
  for (int i = 1; i < wellLength; ++i)
  {
  geo::CPoint p1 = m_3DGrid[column][i - 1];
  geo::CPoint p2 = m_3DGrid[column][i];

  double l1 = geo::CVector(p1, point).Length();
  double l2 = geo::CVector(p1, p2).Length();

  if (l1 < l2)
  {
      ahd += l1;
      break;
  }
  else
  {
      ahd += l2;
  }
  }

  return ahd;
}

void CWellTetraMesher::Map2DSideSurfaceToSurface(CTriangleSurface& surface)
{
  int wellSize = PolyLineSize();
  int wellLength = PolyLineLength();
  int firstWellLength = static_cast<int>(m_2DSideSurface[0].size());

  double last = Radius() * 2 * M_PI;

  std::vector<geo::CPoint> points;
  points.reserve(wellLength * 2 + wellSize * 2);

  for (int i = 0; i < firstWellLength; ++i)
  {
  m_2DSideSurface[0][i].index = points.size();
  geo::CPoint p = m_2DSideSurface[0][i].point;
  p.X(last);
  points.push_back(p);
  }

  int P0 = points.size();

  for (int i = 0; i < firstWellLength; ++i)
  {
  m_2DSideSurface[0][i].index = points.size();
  geo::CPoint p = m_2DSideSurface[0][i].point;
  points.push_back(p);
  }

  int P1 = points.size();

  for (int j = 1; j < wellSize; ++j)
  {
  m_2DSideSurface[j][wellLength - 1].index = points.size();
  geo::CPoint p = m_2DSideSurface[j][wellLength - 1].point;
  points.push_back(p);
  }

  int P2 = points.size();

  for (int j = 1; j < wellSize; ++j)
  {
  m_2DSideSurface[j][0].index = points.size();
  geo::CPoint p = m_2DSideSurface[j][0].point;
  points.push_back(p);
  }

  int P3 = points.size();

  double factor = Radius() * 2 * M_PI / wellSize;

  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
  m_surfaces[s].surfacePoints.reserve(m_surfaces[s].points.size());

  geo::CPoint prevPoint;
  geo::CPoint nextPoint;

  size_t surfPointSize = m_surfaces[s].points.size();

  bool forceNextInterpolation = false;

  for (size_t w = 0; w < surfPointSize; ++w)
  {
      if (m_surfaces[s].points[w].empty())
      {
    prevPoint = geo::CPoint();
    nextPoint = geo::CPoint();
      }

      for (size_t p = 0; p < m_surfaces[s].points[w].size(); ++p)
      {
    const geo::CPoint point = m_surfaces[s].points[w][p];

    double ahd = CalculateAHD(w, point);

    geo::CPoint newPoint(w * factor, ahd, 0);

    if (w == 0)
    {
          prevPoint.Set(0, ahd, 0);
          nextPoint.Set(2 * M_PI * Radius(), ahd, 0);
          break;
    }
    else if (prevPoint.Empty())
    {
          geo::CVector dir(geo::CPoint((w + 1) * factor, CalculateAHD(w + 1, m_surfaces[s].points[w + 1][p])), newPoint);

          double angle = DBL_MAX;
          int index = -1;

          for (int x = P1; x < P2; ++x)
          {
      geo::CVector tryDir(newPoint, points[x]);
      double tryAngle = tryDir.AngleDeg(dir);
      if (tryAngle < angle)
      {
              angle = tryAngle;
              index = x;
      }
          }
          if (index >= 0)
          {
      m_surfaces[s].bottomIntersections.push_back(index - P1 + 1);
      prevPoint = points[index];
          }
    }
    else if (nextPoint.Empty() && (w == wellSize - 1 || m_surfaces[s].points[w + 1].empty()))
    {
          geo::CVector dir(prevPoint, newPoint);

          double angle = DBL_MAX;
          int index = -1;

          for (int x = P1; x < P2; ++x)
          {
      geo::CVector tryDir(newPoint, points[x]);
      double tryAngle = tryDir.AngleDeg(dir);
      if (tryAngle < angle)
      {
              angle = tryAngle;
              index = x;
      }
          }
          if (index >= 0)
          {
      m_surfaces[s].bottomIntersections.push_back(index - P1 + 1);
      nextPoint = points[index];
      forceNextInterpolation = true;
          }
    }

    // interpolate points
    if (!prevPoint.Empty())
    {
          geo::CVector dir(prevPoint, newPoint);
          double dist = dir.Length();

          if (dist > 1.3 * factor)
          {
      int interpolSize = std::ceil(dist / factor);
      double interfac = 1.0 / (double)interpolSize;

      for (int i = 0; i < interpolSize - 1; ++i)
      {
              geo::CPoint interPoint = prevPoint + dir * interfac;
              m_surfaces[s].surfacePoints.push_back(interPoint);
              m_surfaces[s].surfacePoints.back().index = points.size();
              points.push_back(interPoint);
              prevPoint = interPoint;
      }
          }
    }

    m_surfaces[s].surfacePoints.push_back(newPoint);
    m_surfaces[s].surfacePoints.back().index = points.size();
    m_surfaces[s].surfacePoints.back().pointIndex = w;
    points.push_back(newPoint);

    prevPoint = newPoint;

    if ((w == surfPointSize - 1 || forceNextInterpolation) && !nextPoint.Empty())
    {
          geo::CVector dir(newPoint, nextPoint);
          double dist = dir.Length();

          if (dist > 1.3 * factor)
          {
      int interpolSize = std::ceil(dist / factor);
      double interfac = 1.0 / (double)interpolSize;

      for (int i = 0; i < interpolSize - 1; ++i)
      {
              geo::CPoint interPoint = prevPoint + dir * interfac;
              m_surfaces[s].surfacePoints.push_back(interPoint);
              m_surfaces[s].surfacePoints.back().index = points.size();
              points.push_back(interPoint);
              prevPoint = interPoint;
      }
          }
    }
      }
  }
  }

  surface.Points(points);

  std::vector<std::pair<size_t, size_t> > segments;
  segments.reserve(points.size());

  for (int i = 0; i < P0 - 1; ++i)
  segments.push_back(std::make_pair(i, i + 1));
  segments.push_back(std::make_pair(P0 - 1, P2 - 1));
  for (int i = P2 - 1; i > P0; --i)
  segments.push_back(std::make_pair(i, i - 1));
  segments.push_back(std::make_pair(P0, P2));
  for (int i = P2; i < P3 - 1; ++i)
  segments.push_back(std::make_pair(i, i + 1));
  segments.push_back(std::make_pair(P3 - 1, 0));

  size_t  currentSegmentSize = segments.size();

  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
  SurfaceInfo& surfInfo = m_surfaces[s];

  int firstIndex = -1;
  int lastIndex = -1;

  if (surfInfo.bottomIntersectionPoints.empty())
  {
      for (int i = 0; i < firstWellLength; ++i)
    if (m_2DSideSurface[0][i].surface == surfInfo.surfaceIndex)
    {
          firstIndex = i + firstWellLength;
          lastIndex = i;
          break;
    }
  }
  else
  {
      firstIndex = m_2DSideSurface[surfInfo.bottomIntersections[0]].back().index;
      lastIndex = m_2DSideSurface[surfInfo.bottomIntersections[1]].back().index;
  }

  if (firstIndex >= 0)
  {
      segments.push_back(std::make_pair(firstIndex, surfInfo.surfacePoints.front().index));
  }

  for (int w = 0; w < (int)surfInfo.surfacePoints.size() - 1; ++w)
  {
      segments.push_back(std::make_pair(surfInfo.surfacePoints[w].index, surfInfo.surfacePoints[w + 1].index));
  }

  if (lastIndex >= 0)
  {
      segments.push_back(std::make_pair(surfInfo.surfacePoints.back().index, lastIndex));
  }
  }

  std::vector<int> segment_indices(currentSegmentSize);

  for (int i = 0; i < segment_indices.size(); ++i)
  segment_indices[i] = i;

  surface.Segments(segments);
  surface.NewRegion(segment_indices);
}

void CWellTetraMesher::MeshSurface(CTriangleSurface& surface)
{
  double angle = 20;
  double area = 0.7 * IdealElementLength() * IdealElementLength();

  surface.AllowSteinerPointsOnBoundary(false);
  surface.CreateMesh(angle, area);
}

int CWellTetraMesher::AHDGridIndex(int sizeIndex, double length) const
{
  const TAHD& ahd = m_AHDGrid[sizeIndex];

  if (length >= ahd.back())
  return static_cast<int>(ahd.size()) - 1;

  for (TAHD::const_iterator it = ahd.begin(); it != ahd.end(); ++it)
  if (*it > length)
      return static_cast<int>(std::distance(ahd.begin(), it)) - 1;

  return -1;
}

void CWellTetraMesher::MapAngleAHDTo3DPoint(const geo::IPoint& inputPoint, geo::CPoint& outputPoint) const
{
  int wellSize = PolyLineSize();
  int wellLength = PolyLineLength();

  double factor = 2 * M_PI / wellSize;
  double rfactor = Radius() * factor;

  double circ = inputPoint.X();
  double ahd = inputPoint.Y();

  int size_index_0 = static_cast<int>(std::floor(inputPoint.X() / rfactor));
  int size_index_1 = size_index_0 + 1;

  int length_index_0 = AHDGridIndex(size_index_0, ahd);
  int length_index_1 = AHDGridIndex(size_index_1, ahd);

  if (length_index_0 == wellLength - 1)
  --length_index_0;

  if (length_index_1 == wellLength - 1)
  --length_index_1;

  double x_factor = (inputPoint.X() - m_2DSideSurface[size_index_0][length_index_0].point.X()) / (m_2DSideSurface[size_index_1][length_index_1].point.X() - m_2DSideSurface[size_index_0][length_index_0].point.X());
  double z_factor = 0;

  if (x_factor < 0.5)
  z_factor = (inputPoint.Y() - m_AHDGrid[size_index_0][length_index_0]) / (m_AHDGrid[size_index_0][length_index_0 + 1] - m_AHDGrid[size_index_0][length_index_0]);
  else
  z_factor = (inputPoint.Y() - m_AHDGrid[size_index_1][length_index_1]) / (m_AHDGrid[size_index_1][length_index_1 + 1] - m_AHDGrid[size_index_1][length_index_1]);

  size_index_1 = size_index_1 % wellSize;
  geo::CPoint x_top = m_3DGrid[size_index_0][length_index_0] + geo::CVector(m_3DGrid[size_index_0][length_index_0], m_3DGrid[size_index_1][length_index_1]) * x_factor;
  geo::CPoint x_bot = m_3DGrid[size_index_0][length_index_0 + 1] + geo::CVector(m_3DGrid[size_index_0][length_index_0 + 1], m_3DGrid[size_index_1][length_index_1 + 1]) * x_factor;

  outputPoint = x_top + geo::CVector(x_top, x_bot) * z_factor;
}

void CWellTetraMesher::Convert2DSurfaceToVectors(const CTriangleSurface& inputSurface, std::vector<geo::CPoint>& points, std::vector<std::vector<int>>& triangles)
{
  int pointSize = inputSurface.PointSize();
  int triangleSize = inputSurface.FaceSize();

  points.reserve(pointSize);
  triangles.reserve(triangleSize);

  for (int i = 0; i < pointSize; ++i)
  points.push_back(inputSurface.Point(i));

  for (int i = 0; i < triangleSize; ++i)
  {
  const geo::IFace& face = inputSurface.Face(i);
  std::vector<int> indices { face.PointIndex(0), face.PointIndex(1), face.PointIndex(2) };
  triangles.push_back(indices);
  }
}

void CWellTetraMesher::Check2DSideSurfaceArea(std::vector<geo::CPoint>& points, std::vector<std::vector<int>>& triangles)
{
  int triangleSize = static_cast<int>(triangles.size());

  std::vector<int> bigTriangles;
  bigTriangles.reserve(20);

  double bigArea = 1.3 * IdealElementLength() * IdealElementLength();

  for (int i = 0; i < triangleSize; ++i)
  {
  const std::vector<int>& indices = triangles[i];
  double area = geo::CTriangle(points[indices[0]], points[indices[1]], points[indices[2]]).Area();
  if (area > bigArea)
      bigTriangles.push_back(i);
  }

  int bigTriangleSize = static_cast<int>(bigTriangles.size());

  std::vector<int> newIndices(3);

  for (int i = 0; i < bigTriangleSize; ++i)
  {
  const std::vector<int> indices = triangles[bigTriangles[i]];

  geo::CPoint midPoint(0, 0, 0);
  for (int j = 0; j < 3; ++j)
      midPoint = midPoint + points[indices[j]];
  midPoint = midPoint / 3;

  int newIndex = static_cast<int>(points.size());

  points.push_back(midPoint);

  triangles[bigTriangles[i]][2] = newIndex;

  newIndices[0] = indices[0];
  newIndices[1] = newIndex;
  newIndices[2] = indices[2];
  triangles.push_back(newIndices);

  newIndices[0] = indices[1];
  newIndices[1] = indices[2];
  newIndices[2] = newIndex;
  triangles.push_back(newIndices);
  }
}

void CWellTetraMesher::Map2DSideSurfaceTo3DSideSurface(const std::vector<geo::CPoint>& points, const std::vector<std::vector<int>>& triangles, geo::CTriSurface& outputSurface)
{
  int wellSize = PolyLineSize();
  int wellLength = PolyLineLength();
  int wellLengthFirst = static_cast<int>(m_2DSideSurface[0].size());

  int pointSize = static_cast<int>(points.size());

  double factor = 2 * M_PI / wellSize;
  double rfactor = Radius() * factor;
  double last = 2 * M_PI * Radius();

  // add sentinel
  m_2DSideSurface.push_back(m_2DSideSurface[0]);
  for (int i = 0; i < wellLengthFirst; ++i)
  m_2DSideSurface[wellSize][i].point.X(last);
  m_AHDGrid.push_back(m_AHDGrid[0]);

  m_2DTo3DMap.resize(wellLengthFirst);

  for (int i = wellLengthFirst; i < pointSize; ++i)
  {
  geo::CPoint p_prime(0, 0, 0);

  MapAngleAHDTo3DPoint(points[i], p_prime);

  outputSurface.AddPoint(p_prime);
  m_2DTo3DMap.push_back(p_prime);
  }

  int triangleSize = static_cast<int>(triangles.size());

  for (int i = 0; i < triangleSize; ++i)
  {
  std::vector<int> indices = triangles[i];

  for (int j = 0; j < 3; ++j)
  {
      if (indices[j] >= wellLengthFirst)
    indices[j] -= wellLengthFirst;
  }

  outputSurface.AddTriangle(indices[0], indices[1], indices[2]);
  }
}

void CWellTetraMesher::Create2DGridSurface(CTriangleSurface& inputSurface, bool addIntersections)
{
  std::vector<geo::CPoint> points(m_2DGrid);

  size_t size = points.size();

  std::vector<std::pair<size_t, size_t> > segments;
  segments.reserve(size);

  for (size_t i = 0; i < size; ++i)
  segments.push_back(std::make_pair(i, (i + 1) % size));

  std::vector<int> segment_indices(segments.size());

  for (int i = 0; i < segment_indices.size(); ++i)
  segment_indices[i] = i;

  if (addIntersections)
  {
  double factor = IdealElementLength();

  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
      if (!m_surfaces[s].bottomIntersections.empty())
      {
    for (size_t w = 0; w < m_surfaces[s].bottomIntersections.size(); w += 2)
    {
          geo::CPoint start = points[m_surfaces[s].bottomIntersections[w]];
          geo::CPoint end = points[m_surfaces[s].bottomIntersections[w + 1]];

          geo::CVector dir(start, end);
          double dist = dir.Length();

          if (dist > 1.3 * factor)
          {
      size_t segment_start = points.size();

      int interpolSize = std::ceil(dist / factor);
      double interfac = 1.0 / (double)interpolSize;

      for (int i = 0; i < interpolSize - 1; ++i)
      {
              geo::CPoint interPoint = start + dir * interfac;
              points.push_back(interPoint);
              start = interPoint;
      }

      m_surfaces[s].bottomIntersectionPoints.reserve(interpolSize - 1);
      int pSize = (int)points.size() - 1;
      for (int i = 0; i < interpolSize - 1; ++i)
      {
              m_surfaces[s].bottomIntersectionPoints.push_back(points[pSize--]);
      }

      for (int i = 0; i < interpolSize - 2; ++i)
      {
              segments.push_back(std::make_pair(segment_start, segment_start + 1));
              ++segment_start;
      }
          }
    }
      }
  }
  }

  inputSurface.Points(points);
  inputSurface.Segments(segments);
  inputSurface.NewRegion(segment_indices);

  MeshSurface(inputSurface);
}

void CWellTetraMesher::Map2DGridSurfaceToWellPoint(int wellPointIndex, const CTriangleSurface& inputSurface, geo::CTriSurface& outputSurface)
{
  int wellLength = PolyLineLength();

  bool bottom = wellPointIndex == wellLength - 1;

  const geo::CPoint *prevPoint = wellPointIndex > 0 ? &m_well[wellPointIndex - 1] : nullptr;

  geo::CPoint wellPoint = m_well[wellPointIndex];

  const geo::CPoint *nextPoint = wellPointIndex < wellLength - wellPointIndex ? &m_well[wellPointIndex + 1] : nullptr;
  geo::CVector normal = geo::CVector::NullVector;
  if (prevPoint)
  normal += geo::CVector(*prevPoint, wellPoint).UnitVector();
  if (nextPoint)
  normal += geo::CVector(wellPoint, *nextPoint).UnitVector();
  normal = normal.UnitVector();

  geo::CMatrix rotation = Create3DGridTransformationMatrix(wellPoint, normal);

  int pointSize = inputSurface.PointSize();
  int triangleSize = inputSurface.FaceSize();

  for (int i = 0; i < pointSize; ++i)
  {
  geo::CPoint p = inputSurface.Point(i);
  p.Transform(rotation);
  outputSurface.AddPoint(p);
  }

  for (int i = 0; i < triangleSize; ++i)
  {
  const geo::ITriangle& triangle = inputSurface.Triangle(i);
  outputSurface.AddTriangle(triangle.PointIndex(0), triangle.PointIndex(1), triangle.PointIndex(2));
  }

  if (bottom)
  {
  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
      SurfaceInfo& surfInfo = m_surfaces[s];
      for (size_t i = 0; i < surfInfo.bottomIntersectionPoints.size(); ++i)
      {
    geo::CPoint p = surfInfo.bottomIntersectionPoints[i];
    p.Transform(rotation);

    double dist = DBL_MAX;
    int index = -1;
    for (int j = 0; j < pointSize; ++j)
    {
          geo::CPoint p_surf = outputSurface.Point(j);
          double len = geo::CVector(p, p_surf).Length();
          if (len < dist)
          {
      dist = len;
      index = j;
          }
    }

    if (index >= 0)
          p = outputSurface.Point(index);

    surfInfo.bottomIntersectionPoints[i] = p;
      }
  }
  }
}

void CWellTetraMesher::Map2DIntersectionSurfaceBoundaryTo3DSurface(const SurfaceInfo& surfInfo, TPolyline& outputBoundary)
{
  const std::vector<GridPoint>& firstWell = m_2DSideSurface[0];
  int firstWellLength = static_cast<int>(firstWell.size());
  int radius = Radius();

  int surfacePointsSize = static_cast<int>(surfInfo.surfacePoints.size());

  int firstIndex = -1;

  int skipLast = 0;

  if (surfInfo.bottomIntersections.empty())
  {
  outputBoundary.reserve(surfacePointsSize + 1);

  for (size_t i = 0; i < firstWellLength; ++i)
  {
      if (firstWell[i].surface == surfInfo.surfaceIndex)
      {
    double angle = firstWell[i].point.X() / radius;
    firstIndex = firstWell[i].index;
    break;
      }
  }
  
  outputBoundary.push_back(m_2DTo3DMap[firstIndex]);

  for (int i = 0; i < surfacePointsSize; ++i)
  {
      int index = surfInfo.surfacePoints[i].index;
      outputBoundary.push_back(m_2DTo3DMap[index]);
  }
  }
  else
  {
  outputBoundary.reserve(surfacePointsSize + 2 + surfInfo.bottomIntersectionPoints.size());

  int intIndex_1 = surfInfo.bottomIntersections[0];
  int intIndex_11 = m_2DSideSurface[surfInfo.bottomIntersections[0]].back().index;

  outputBoundary.push_back(m_2DTo3DMap[m_2DSideSurface[surfInfo.bottomIntersections[0]].back().index]);
  for (int i = 0; i < (int)surfInfo.surfacePoints.size(); ++i)
  {
      int index = surfInfo.surfacePoints[i].index;
      outputBoundary.push_back(m_2DTo3DMap[index]);
  }
  int intIndex_2 = surfInfo.bottomIntersections[1];
  int intIndex_21 = m_2DSideSurface[surfInfo.bottomIntersections[1]].back().index;

  outputBoundary.push_back(m_2DTo3DMap[m_2DSideSurface[surfInfo.bottomIntersections[1]].back().index]);

  for (int i = 0; i < (int)surfInfo.bottomIntersectionPoints.size(); ++i)
  {
      geo::CPoint inPoint = surfInfo.bottomIntersectionPoints[i];
      outputBoundary.push_back(inPoint);
  }
  }
}

void CWellTetraMesher::Map3DIntersectionSurfaceBoundaryTo2DSurface(const SurfaceInfo& surfInfo, const TPolyline& inputBoundary, CTriangleSurface& outputSurface)
{
  size_t inputSize = inputBoundary.size();

  geo::CPoint midPoint(0, 0, 0);

  for (size_t i = 0; i < inputSize; ++i)
  {
  midPoint = midPoint + inputBoundary[i];
  }

  midPoint = midPoint / -(int)inputBoundary.size();

  geo::CMatrix matrix = Create3DGridTransformationMatrix(midPoint, geo::CVector::Zaxis);


  TPolyline output;
  output.reserve(inputSize);

  for (size_t i = 0; i < inputSize; ++i)
  {
  output.push_back(inputBoundary[i]);
  output.back().Transform(matrix);
  }

  std::vector<std::pair<size_t, size_t>> segments;
  segments.reserve(inputSize);

  for (size_t i = 0; i < inputSize; ++i)
  segments.push_back(std::make_pair(i, (i + 1) % inputSize));

  std::vector<int> segment_indices(inputSize);

  for (int i = 0; i < inputSize; ++i)
  segment_indices[i] = i;

  outputSurface.Points(output);
  outputSurface.Segments(segments);
  outputSurface.NewRegion(segment_indices);

  MeshSurface(outputSurface);
}

void CWellTetraMesher::Map2DIntersectionSurfaceTo3DData(const CTriangleSurface& inputSurface, const TPolyline& inputBoundary, std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles)
{
  int inputSize = inputSurface.PointSize();
  int mapSize = static_cast<int>(inputBoundary.size());
  int triangleSize = inputSurface.FaceSize();

  points.reserve(inputSize);
  triangles.reserve(triangleSize);

  for (int i = 0; i < mapSize; ++i)
  points.push_back(inputBoundary[i]);

  geo::CPoint midPoint(0, 0, 0);

  for (int i = 0; i < mapSize; ++i)
  midPoint = midPoint + points[i];

  midPoint = midPoint / points.size();

  geo::CVector dir = geo::CVector::NullVector;
  int totalNormal = 0;

  for (int i = 0; i < mapSize; ++i)
  {
  geo::CVector n1(midPoint, points[i]);
  geo::CVector n2(midPoint, points[(i + 1) % mapSize]);
  geo::CVector n = n1.CrossProduct(n2).UnitVector();
  dir = dir + n;
  ++totalNormal;
  }

  dir = dir / totalNormal;
  dir = dir.UnitVector();

  geo::CMatrix matrix = Create3DGridTransformationMatrix(midPoint, dir);

  for (int i = mapSize; i < inputSize; ++i)
  {
  geo::CPoint point = inputSurface.Point(i);
  point.Transform(matrix);
  AdjustPointDistance(point);
  points.push_back(point);
  }

  for (int i = 0; i < triangleSize; ++i)
  {
  const geo::ITriangle& triangle = inputSurface.Triangle(i);
  triangles.push_back(std::vector<int>{ triangle.PointIndex(0), triangle.PointIndex(1), triangle.PointIndex(2) });
  }
}

void CWellTetraMesher::AdjustPointDistance(geo::CPoint & point)
{
  double ideal = 1.3 * IdealElementLength() * IdealElementLength();
  double smallRadius = Radius() - 1;
  double smallRadius2 = smallRadius * smallRadius;

  int y = (int)std::floor(point.Y());
  int z = (int)std::floor(point.Z());



  int size = PolyLineLength() - 1;

  for (int i = 0; i < size; ++i)
  {
  geo::CVector toPoint(m_well[i], point);

  double factor = toPoint.DotProduct(m_adjustmentLine[i].line) / m_adjustmentLine[i].length2;

  if (factor >= 0 && factor <= 1)
  {
      geo::CPoint proj = m_well[i] + m_adjustmentLine[i].line * factor;
      geo::CVector dir(proj, point);

      if (dir.SquareLength() >= smallRadius2)
      {
    point = proj + dir.UnitVector() * smallRadius;
      }
      break;
  }
  }
}

void CWellTetraMesher::CreateSideSurfacePointMap(const geo::CTriSurface& sideSurface, TTrianglePointMap& pointMap)
{
  int faceSize = static_cast<int>(sideSurface.FaceSize());

  for (int i = 0; i < faceSize; ++i)
  {
  const geo::IFace& face = sideSurface.Face(i);
  for (int j = 0; j < 3; ++j)
  {
      geo::CPoint point(face.Point(j).X(), face.Point(j).Y(), face.Point(j).Z());
      std::pair<TTrianglePointMap::iterator, bool> retval = pointMap.insert(std::make_pair(point, std::set<int>()));
      retval.first->second.insert(i);
  }
  }
}

bool CWellTetraMesher::TriangleInPointMap(const TTrianglePointMap& pointMap, const geo::IPoint& point1, const geo::IPoint& point2, const geo::IPoint& point3)
{
  TTrianglePointMap::const_iterator it = pointMap.find(point1);
  if (it != pointMap.end())
  {
  const std::set<int>& found = it->second;

  it = pointMap.find(point2);
  if (it != pointMap.end())
  {
      std::vector<int> intersection;
      intersection.reserve(10);

      std::set_intersection(found.begin(), found.end(), it->second.begin(), it->second.end(), std::back_inserter(intersection));

      if (!intersection.empty())
      {
    it = pointMap.find(point3);

    if (it != pointMap.end())
    {
          for (std::set<int>::iterator check = it->second.begin(); check != it->second.end(); ++check)
          {
      if (std::find(intersection.begin(), intersection.end(), *check) != intersection.end())
              return true;
          };
    }
      }
  }
  }

  return false;
}

void CWellTetraMesher::FilterCoincidingTriangles(const TTrianglePointMap& pointMap, const std::vector<geo::CPoint>& inputPoints, const std::vector<std::vector<int> >& inputTriangles, std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles)
{
  int inputPointSize = static_cast<int>(inputPoints.size());
  int inputTriangleSize = static_cast<int>(inputTriangles.size());

  points.reserve(inputPointSize);
  triangles.reserve(inputTriangleSize);

  bool rejected = false;

  for (int i = 0; i < inputTriangleSize; ++i)
  {
  const std::vector<int>& triangle = inputTriangles[i];

  geo::CPoint point1 = inputPoints[triangle[0]];
  geo::CPoint point2 = inputPoints[triangle[1]];
  geo::CPoint point3 = inputPoints[triangle[2]];

  if (TriangleInPointMap(pointMap, point1, point2, point3))
      rejected = true;
  else
      triangles.push_back(triangle);
  }

  if (!rejected)
  {
  for (int i = 0; i < inputPointSize; ++i)
      points.push_back(inputPoints[i]);
  }
  else
  {
  inputTriangleSize = static_cast<int>(triangles.size());

  typedef std::map<geo::CPoint, int, PointComparer> TTransformPoints;
  TTransformPoints newPoints;

  for (int i = 0; i < inputTriangleSize; ++i)
  {
      std::vector<int>& triangle = triangles[i];

      for (int j = 0; j < 3; ++j)
      {
    int oldIndex = triangle[j];

    std::pair<TTransformPoints::iterator, bool> retval = newPoints.insert(std::make_pair(inputPoints[oldIndex], 0));
    if (retval.second)
    {
          retval.first->second = static_cast<int>(points.size());
          points.push_back(inputPoints[oldIndex]);
    }

    triangle[j] = retval.first->second;
      }
  }
  }
}

void CWellTetraMesher::CreateSurface(const std::vector<geo::CPoint>& points, const std::vector<std::vector<int> >& triangles, geo::CTriSurface& outputSurface)
{
  for (std::vector<geo::CPoint>::const_iterator it = points.begin(); it != points.end(); ++it)
  outputSurface.AddPoint(*it);

  for (std::vector<std::vector<int> >::const_iterator it = triangles.begin(); it != triangles.end(); ++it)
  outputSurface.AddTriangle((*it)[0], (*it)[1], (*it)[2]);
}

void CWellTetraMesher::CreateIntersectionSurfaces(std::vector<geo::CTriSurface *>& outputSurfaces)
{
  TTrianglePointMap trianglePointMap;
  CreateSideSurfacePointMap(*outputSurfaces.front(), trianglePointMap);
  CreateAdjustmentData();

  for (size_t s = 0; s < m_surfaces.size(); ++s)
  {
  const SurfaceInfo& surfInfo = m_surfaces[s];

  TPolyline boundary3DSurface;
  CTriangleSurface boundary2DSurface;
  geo::CTriSurface *outputSurface = new geo::CTriSurface;

  Map2DIntersectionSurfaceBoundaryTo3DSurface(surfInfo, boundary3DSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  QString name = QString("C:\\Local\\wtm\\3dboundary_%1_%2.dat").arg(s).arg(surfInfo.horizon->Name().left(6));
  Debug_WritePointset(name.toStdString(), boundary3DSurface);
#endif

  Map3DIntersectionSurfaceBoundaryTo2DSurface(surfInfo, boundary3DSurface, boundary2DSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  name = QString("C:\\Local\\wtm\\2dboundary_%1_%2.dat").arg(s).arg(surfInfo.horizon->Name().left(6));
  Debug_WritePointset(name.toStdString(), boundary2DSurface);
  Debug_WriteSurface(name.replace(".dat", ".ts").toStdString(), boundary2DSurface);
#endif

  std::vector<geo::CPoint> inputPoints, points;
  std::vector<std::vector<int> > inputTriangles, triangles;

  Map2DIntersectionSurfaceTo3DData(boundary2DSurface, boundary3DSurface, inputPoints, inputTriangles);
  FilterCoincidingTriangles(trianglePointMap, inputPoints, inputTriangles, points, triangles);
  CreateSurface(points, triangles, *outputSurface);
#ifdef DEBUG_WELL_TETRA_MESHER
  name = QString("C:\\Local\\wtm\\3dsurface_%1_%2.dat").arg(s).arg(surfInfo.horizon->Name().left(6));
  Debug_WritePointset(name.toStdString(), *outputSurface);
  Debug_WriteSurface(name.replace(".dat", ".ts").toStdString(), *outputSurface);
#endif

  TPolyline boundary;
  FindBoundary(*outputSurface, boundary);

  if (boundary.empty())
  {
      // log dropped surface
  }
  else
  {
#ifdef DEBUG_WELL_TETRA_MESHER
      name = QString("C:\\Local\\wtm\\edgeboundary_%1_%2.dat").arg(s).arg(surfInfo.horizon->Name().left(6));
      Debug_WritePointset(name.toStdString(), boundary);
#endif
      outputSurfaces.push_back(outputSurface);
  }
  }
}


void CWellTetraMesher::FindBoundary(const geo::ISurface& surface, TPolyline& boundary)
{
  boundary.reserve(100);
  TEdgeTriangles edgeMap;

  for (int i = 0; i < surface.FaceSize(); ++i)
  {
  const geo::IFace& face = surface.Face(i);
  for (int j = 0; j < 3; ++j)
  {
      TEdge edge = std::make_pair(face.PointIndex(j), face.PointIndex((j + 1) % 3));
      if (edge.first > edge.second) std::swap(edge.first, edge.second);
      std::pair<TEdgeTriangles::iterator, bool> retval = edgeMap.insert(std::make_pair(edge, TTriangles()));
      if (retval.second)
    retval.first->second.reserve(2);
      retval.first->second.push_back(i);
  }
  }

  TEdges edges, wrong;
  edges.reserve(100);
  wrong.reserve(10);

  for (TEdgeTriangles::iterator it = edgeMap.begin(); it != edgeMap.end(); ++it)
  {
  if (it->second.size() == 1)
      edges.push_back(it->first);
  else if (it->second.size() != 2)
      wrong.push_back(it->first);
  }

  std::list<TEdge> sorted;

  sorted.push_front(edges.front());

  bool found = false;

  TEdge first = sorted.front();

  do
  {
  TEdge edge = sorted.front();

  found = false;

  TEdge item;

  for (TEdges::iterator it = edges.begin(); it != edges.end(); ++it)
  {
      if (it->first == edge.second && it->second != edge.first)
      {
    item = *it;
    found = true;
    break;
      }
      else if (it->second == edge.second && it->first != edge.first)
      {
    item = *it;
    std::swap(item.first, item.second);
    found = true;
    break;
      }
  }
  if (found)
  {
      if ((item.first == first.first && item.second == first.second) || (item.first == first.second && item.second == first.first))
    found = false;
      else
    sorted.push_front(item);
  }
  if (sorted.size() > edges.size())
      found = false;
  } while (found);

  if (sorted.size() == edges.size() && sorted.front().second == sorted.back().first)
  {
  for (std::list<TEdge>::iterator it = sorted.begin(); it != sorted.end(); ++it)
  {
      boundary.push_back(surface.Point(it->first));
  }
  }
}

geo::CTetMeshBase *CWellTetraMesher::MeshVolume(const std::vector<geo::CTriSurface *>& surfaces, IProgressBase& prog)
{
#ifdef _WIN32
  // create temp directory for mesh creation procedure and change to it
  QString strTempDir = CreateTempDirExt(CTempPath::TEMP_GENERAL).toStdString().c_str();
  QString strOldDir = ChangeDirectory(strTempDir.toStdString().c_str());

  CEnvironment* env = CEnvironment::instance();
  std::string sFgvExeName = env->get(CEnvironment::FgvExeName).toStdString();
  if (sFgvExeName.empty())
  {
  _m()->msg("The mesher program is not installed");
  ChangeDirectory(strOldDir.toStdString().c_str());
  return nullptr;
  }

  geo::CTetMeshCM2 *mesh = new geo::CTetMeshCM2;

  int surfSize = static_cast<int>(surfaces.size());
  for (int i = 0; i < surfSize; ++i)
  {
  std::string name = "Surface " + std::to_string(i);
  mesh->AddSurface(*surfaces[i], name, false);
  }

  //std::auto_ptr <IProgressBase> dlg(_g->prog()->create(eProgress::Geo, getStringTableEntry(1), false));

  CMeshProgress progress(prog);
  geo::CTetMeshBase::TRet ret = mesh->CreateMesh(
  env->get(CEnvironment::DIAPATH).toStdString(),
  env->get(CEnvironment::DIASHARE).toStdString(),
  env->get(CEnvironment::DIALIB).toStdString(),
  env->get(CEnvironment::DIASLIB).toStdString(),
  sFgvExeName,
  progress);

  ChangeDirectory(strOldDir.toStdString().c_str());
  //	VERIFY(RemoveDir(strTempDir));

  if (!ret.second || !mesh->ElementSize())
  {
  if (!mesh->ElementSize())
      ret.first.push_back(std::string("ERROR: No elements generated by the mesher"));

  std::vector<std::string> errors;
  errors.reserve(20);
  bool haveErrors = false;

  for (size_t i = 0; i < ret.first.size(); ++i)
  {
      std::string& line = ret.first[i];
      if (!haveErrors && line.length() > 5 && line.substr(0, 5) == "ERROR")
    haveErrors = true;
      if (haveErrors)
    errors.push_back(line);
  }

  //globalTetMesherMessage(ret.first);
  auto show_error_dlg=[=]( const std::vector< std::string >& data )
  {
    if( _g->is_gui_thread() )
    {
      IMesherDlg* dlg = ((CModelBase*)&m_model)->mesher_dlg();
      if( dlg )
      {
        dlg->data( data );
        dlg->DoModal_(); // 'OnExport' and 'OnExportPoints' happens inside
      }
    }
    //
    // in principle this case never happens as almost everything happens in the mani gui thread
    //
    else
    {
      if( _e->registered( Question ) )
      {	
        CQuestionThreadWrapper th( CQuestionThreadWrapper::MesherDlg, new std::vector<std::string>{ data } );
        th.join();
      }
      else
      {
        _m()->msg( data );
      }
    }
  };
  show_error_dlg(ret.first);

  delete mesh;
  return nullptr;
  }

  return mesh;

#else
  return nullptr;
#endif
}



#ifdef DEBUG_WELL_TETRA_MESHER
void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const std::vector<geo::CPoint>& points)
{
  FILE *fp = fopen(fileName.c_str(), "w");
  if (fp)
  {
  fprintf(fp, "POINTSET\n\nUNIT\tSI\n\nDEPL.STAGES\nD0\t1 2000\nD1\t1 2001\n\n");
  fprintf(fp, "Northing\tEasting\tDepth\n");

  int pointSize = static_cast<int>(points.size());

  for (int i = 0; i < pointSize; ++i)
      fprintf(fp, "%0.06f\t%0.06f\t%0.06f\n", points[i].X(), points[i].Y(), points[i].Z());

  fprintf(fp, "\nEND\n\n");
  fclose(fp);
  }
}

void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const std::vector<GridPoint>& points)
{
  std::vector<geo::CPoint> combined;
  combined.reserve(points.size());

  for (size_t i = 0; i < points.size(); ++i)
  combined.push_back(points[i].point);
  Debug_WritePointset(fileName, combined);
}


void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const std::vector<std::vector<geo::CPoint> >& points)
{
  std::vector<geo::CPoint> combined;
  combined.reserve(1000);

  for (size_t i = 0; i < points.size(); ++i)
  for (size_t j = 0; j < points[i].size(); ++j)
      combined.push_back(points[i][j]);
  Debug_WritePointset(fileName, combined);
}

void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const std::vector<std::vector<GridPoint> >& points)
{
  std::vector<geo::CPoint> combined;
  combined.reserve(1000);

  for (size_t i = 0; i < points.size(); ++i)
  for (size_t j = 0; j < points[i].size(); ++j)
      combined.push_back(points[i][j].point);
  Debug_WritePointset(fileName, combined);
}

void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const CTriangleSurface& surface)
{
  int pointSize = surface.InternalInputPointSize();

  std::vector<geo::CPoint> points;
  points.reserve(1000);

  for (int i = 0; i < pointSize; ++i)
  {
  const double *pp = surface.InternalInputPoint(i);
  const geo::CPoint p(pp[0], pp[1], 0);
  points.push_back(p);
  }
  Debug_WritePointset(fileName, points);
}

void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const geo::CTriSurface& surface)
{
  int pointSize = surface.PointSize();

  std::vector<geo::CPoint> points;
  points.reserve(1000);

  for (int i = 0; i < pointSize; ++i)
  {
  geo::CPoint p = surface.Point(i);
  points.push_back(p);
  }
  Debug_WritePointset(fileName, points);
}

void CWellTetraMesher::Debug_WritePointset(const std::string& fileName, const TSurfaces& surfaces)
{
  std::vector<geo::CPoint> points;
  points.reserve(1000);

  for (size_t i = 0; i < surfaces.size(); ++i)
  for (size_t j = 0; j < surfaces[i].points.size(); ++j)
      for (size_t k = 0; k < surfaces[i].points[j].size(); ++k)
    points.push_back(surfaces[i].points[j][k]);
  Debug_WritePointset(fileName, points);
}


void CWellTetraMesher::Debug_WriteSurface(const std::string& fileName, const geo::ISurface& surface)
{
  FILE *fp = fopen(fileName.c_str(), "w");
  if (fp)
  {
  fprintf(fp, "GOCAD TSurf\nHEADER {\nname:surf\n}\nAXIS_UNIT\t\"m\"\t\"m\"\t\"m\"\nTFACE\n");

  int pointSize = surface.PointSize();
  int triangleSize = surface.FaceSize();

  for (int i = 0; i < pointSize; ++i)
  {
      const geo::CPoint p = surface.Point(i);
      fprintf(fp, "VRTX %d %0.06f %0.06f %0.06f\n", i, p.Y(), p.X(), p.Z());
  }
  for (int i = 0; i < triangleSize; ++i)
  {
      const geo::IFace& t = surface.Face(i);
      fprintf(fp, "TRGL %d %d %d\n", t.PointIndex(0), t.PointIndex(1), t.PointIndex(2));
  }
  fprintf(fp, "END\n");
  fclose(fp);
  }
}

void CWellTetraMesher::Debug_WriteSurface(const std::string& fileName, const std::vector<geo::CPoint>& points, const std::vector<std::vector<int> >& triangles)
{
  FILE *fp = fopen(fileName.c_str(), "w");
  if (fp)
  {
  fprintf(fp, "GOCAD TSurf\nHEADER {\nname:surf\n}\nAXIS_UNIT\t\"m\"\t\"m\"\t\"m\"\nTFACE\n");

  int pointSize = static_cast<int>(points.size());
  int triangleSize = static_cast<int>(triangles.size());

  for (int i = 0; i < pointSize; ++i)
  {
      const geo::CPoint p = points[i];
      fprintf(fp, "VRTX %d %0.06f %0.06f %0.06f\n", i, p.Y(), p.X(), p.Z());
  }
  for (int i = 0; i < triangleSize; ++i)
  {
      std::vector<int> indices = triangles[i];
      fprintf(fp, "TRGL %d %d %d\n", indices[0], indices[1], indices[2]);
  }
  fprintf(fp, "END\n");
  fclose(fp);
  }
}
#endif

CWellTetraMesher::GridPoint::GridPoint()
  : surface(-1)
  , index(-1)
  , pointIndex(-1)
{
}

CWellTetraMesher::GridPoint::GridPoint(const geo::CPoint& point, int surface)
  : point(point)
  , surface(surface)
  , index(-1)
  , pointIndex(-1)
{
}

CWellTetraMesher::SurfaceInfo::SurfaceInfo(const CHorizonBase * horizon, const CSurfaceBase * surface, const geo::CPoint& restrict_min, const geo::CPoint& restrict_max, int wellSize)
  : surfaceIndex(0)
  , horizon(horizon)
  , surface(surface)
  , min(DBL_MAX, DBL_MAX, DBL_MAX)
  , max(-DBL_MAX, -DBL_MAX, -DBL_MAX)
  , empty(true)
{
  if (geo::BBox::Intersects(restrict_min, restrict_max, horizon->Min(), horizon->Max(), true))
  {
  indices.reserve(20);

  const geo::ISurface& srcSurface = surface->Surface();

  int size = srcSurface.FaceSize();

  for (int i = 0; i < size; ++i)
  {
      const geo::IFace& face = srcSurface.Face(i);
      if (geo::BBox::Intersects(restrict_min, restrict_max,face.Min(), face.Max(), true))
      {
    indices.push_back(i);

    geo::CPoint face_min = face.Min();
    geo::CPoint face_max = face.Max();

    min.X(std::min(min.X(), face_min.X()));
    min.Y(std::min(min.Y(), face_min.Y()));
    min.Z(std::min(min.Z(), face_min.Z()));
    max.X(std::max(max.X(), face_max.X()));
    max.Y(std::max(max.Y(), face_max.Y()));
    max.Z(std::max(max.Z(), face_max.Z()));
      }
  }

  if (!indices.empty())
  {
      points.resize(wellSize);
      for (int i = 0; i < wellSize; ++i)
    points[i].reserve(2);
  }
  }
}

bool CWellTetraMesher::SurfaceInfo::continuous() const
{
  for (size_t i = 0; i < points.size(); ++i)
  if (points[i].empty())
      return false;
  return true;
}


CWellTetraMesher::PointComparer::PointComparer()
{
}

bool CWellTetraMesher::PointComparer::operator()(const geo::IPoint& p1, const geo::IPoint& p2) const
{
  double m_dEps = 0.0001;

  if (m_dEps > 0)
  {
  TLongLong p1X = TLongLong(p1.X() / m_dEps);
  TLongLong p1Y = TLongLong(p1.Y() / m_dEps);
  TLongLong p2X = TLongLong(p2.X() / m_dEps);
  TLongLong p2Y = TLongLong(p2.Y() / m_dEps);

  if (p1X < p2X)
      return true;

  if (p1X > p2X)
      return false;

  return p1Y < p2Y;
  }
  else
  {
  if (p1.X() < p2.X())
      return true;

  if (p1.X() > p2.X())
      return false;
  }

  return p1.Y() < p2.Y();
}

bool CWellTetraMesher::PointComparer::operator()(const CWellTetraMesher::GridPoint& p1, const CWellTetraMesher::GridPoint& p2) const
{
  return operator()(p1.point, p2.point);
}


