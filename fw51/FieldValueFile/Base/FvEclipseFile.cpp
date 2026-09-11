 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include <cstdlib>  // for size_t
#include "FvEclipseFile.h"
#include "FvEclipseReservoir.h"

CEclipseFile::CEclipseFile()
{
  // Comment
  DefineComment("--");

  // Keys
  DefineToken(*(new TEclipseToken(*this, "SPECGRID", &CEclipseFile::ReadSpecGrid)));
  DefineToken(*(new TEclipseToken(*this, "DIMENS", &CEclipseFile::ReadSpecGrid)));
  DefineToken(*(new TEclipseToken(*this, "COORD", &CEclipseFile::ReadCoordinates)));
  DefineToken(*(new TEclipseToken(*this, "COORDSYS", &CEclipseFile::ReadCoordSys)));
  DefineToken(*(new TEclipseToken(*this, "ZCORN", &CEclipseFile::ReadZCoord)));
  DefineToken(*(new TEclipseToken(*this, "ACTNUM", &CEclipseFile::ReadCellEnable)));
}

CEclipseFile::~CEclipseFile()
{
  // Destroy
  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
    delete m_vcReservoir[nReservoir];

  m_vcReservoir.clear();
}

bool CEclipseFile::ReadSpecGrid(TInputStream& stream, const QString& sToken)
{
  assert(m_vcReservoir.size() == 0);

  // Read Array Size
  int x, y, nReservoir;

  stream >> x;
  stream >> y;
  stream >> m_nDepth;

  // Read Number Of reservoirs
  stream.eatwhite();
  if( stream.peek() == '/' ) 
    nReservoir = 1;
  else
    stream >> nReservoir;

  // Create Reservoirs
  m_vcReservoir.resize(nReservoir);

  for(size_t i = 0; i < m_vcReservoir.size(); i++)
    m_vcReservoir[i] = &CreateReservoir(x, y);

  // Read Coordinates Format
  stream.eatwhite();

  // Coordinates Type (Cartesian or Cylindrical)
  switch(stream.get())
  {
  case 'F':
    m_bCartesian = true;
    break;
  case 'T':
    m_bCartesian = false;
    break;
  case '/':
    m_bCartesian = true;
    return true;
    break;
  default:
    throw CReadException("Illigal definition after keyword " + sToken);
  }

  // Do we have a delimiter
  stream.checkDelimiter('/');

  return true;
}

bool CEclipseFile::ReadCoordinates(TInputStream& stream, const QString& /*sToken*/)
{
  assert(m_vcReservoir.size() != 0);

  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
  {
    for(int y = 0; y < (m_vcReservoir[nReservoir]->SizeY() + 1); y++)
    {
      for(int x = 0; x < (m_vcReservoir[nReservoir]->SizeX() + 1); x++)
      {
        CEclipseReservoir* pRes = m_vcReservoir[nReservoir];

        // Read Lower Node
        double dx, dy, dz;

        stream >> dx;
        stream >> dy;
        stream >> dz;

        pRes->LowerGrid(x, y, geo::CPoint(dx, dy, dz));

        // Read Upper node
        stream >> dx;
        stream >> dy;
        stream >> dz;

        pRes->UpperGrid(x, y, geo::CPoint(dx, dy, dz));

        assert(pRes->LowerGrid(x, y).Z() <= pRes->UpperGrid(x, y).Z());

        ItemRead();

      } // X Loop
    } // Y Loop
  } // Res Loop

  stream.checkDelimiter('/');

  return true;
}


void CEclipseFile::Destroy()
{
  for(size_t i = 0; i < m_vcReservoir.size(); i++)
    delete m_vcReservoir[i];

  m_vcReservoir.clear();
}

bool CEclipseFile::ReadCoordSys(TInputStream& stream, const QString& sToken)
{
  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
  {
    // Read the reservoir boundaries
    int nLowerBound, nUpperBound;

    stream >> nLowerBound;
    stream >> nUpperBound;

    m_vcReservoir[nReservoir]->Boundary(nLowerBound - 1, nUpperBound - 1);


    // Read the Comp/Incomp
    stream.eatwhite();
    if(stream.peek() != '/') {
      QString sString;
      stream >> sString;

      if(sString.toUpper().contains("INCOMP"))
        m_vcReservoir[nReservoir]->CircleCompleted(false);
      else if(sString.toUpper().contains("COMP"))
        m_vcReservoir[nReservoir]->CircleCompleted(true);
      else
        throw CReadException("Illigal definition after keyword " + sToken);
    } else {
      m_vcReservoir[nReservoir]->CircleCompleted(false);
    }

    stream.eatwhite();
    if(stream.peek() == '\'') stream.get();
    stream.eatwhite();

    // Read the Seperate / Join
    if(stream.peek() != '/'){
      QString sString;
      stream >> sString;

      if(sString.toUpper().contains("JOIN"))
        m_vcReservoir[nReservoir]->Join(true);
      else if(sString.toUpper().contains("SEPARATE"))
        m_vcReservoir[nReservoir]->Join(false);
      else
        throw CReadException("Illigal definition after keyword " + sToken);
    } else {
      m_vcReservoir[nReservoir]->Join(false);
    }

    stream.eatwhite();
    if(stream.peek() != '/')
    {
      int nLowerIndex, nUpperIndex;

      stream >> nLowerIndex;
      stream >> nUpperIndex;

      m_vcReservoir[nReservoir]->ReservoirBoundary(*m_vcReservoir[nLowerIndex],
                             *m_vcReservoir[nUpperIndex]);
    }
    else
      m_vcReservoir[nReservoir]->ReservoirBoundary(*m_vcReservoir[nReservoir],
                             *m_vcReservoir[nReservoir]);

    stream.checkDelimiter('/');
  }

  return true;
}



bool CEclipseFile::WriteSpecGrid(TOutputStream& stream)
{
  assert(m_vcReservoir.size() != 0);

  // Write Tag
  stream << "SPECGRID\n";
  // Write Grid Size
  stream << m_vcReservoir[0]->SizeX() << " ";
  stream << m_vcReservoir[0]->SizeY() << " ";
  stream << m_nDepth << " ";

  // Write Number Of reservoirs
  stream << ((int)(m_vcReservoir.size())) << " ";

  // Write Coordinate system
  if(m_bCartesian)
    stream << "F" << " ";
  else
    stream << "T" << " ";

  // Write Delimiter
  stream << "/\n";

  return true;
}

bool CEclipseFile::ReadZCoord(TInputStream& stream, const QString& /*sToken*/)
{
  assert(m_vcReservoir.size() != 0);
  double dZ;

  for(int z = 0; z < (m_nDepth * 2); z++)
  {
    for(int y = 0; y < (m_vcReservoir[0]->SizeY() * 2); y++)
    {
      for(int x = 0; x < (m_vcReservoir[0]->SizeX() * 2); x++)
      {
        CEclipseReservoir *pRes = &Reservoir((z-(z%2))/2);

        // Read the z coordinate
        stream >> dZ;

        pRes->Depth(x, y, z, dZ);

        ItemRead();
      }
    }
  }

  stream.checkDelimiter('/');
  return true;
}

const CEclipseReservoir& CEclipseFile::Reservoir(int nDepth) const
{
  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
    if((m_vcReservoir[nReservoir]->LowerBoundary() <= nDepth) &&
      (nDepth <= m_vcReservoir[nReservoir]->UpperBoundary()))
    return *m_vcReservoir[nReservoir];

  assert(false);

  return *m_vcReservoir[0];
}

CEclipseReservoir& CEclipseFile::Reservoir(int nDepth)
{
  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
    if((m_vcReservoir[nReservoir]->LowerBoundary() <= nDepth) &&
      (nDepth <= m_vcReservoir[nReservoir]->UpperBoundary()))
    return *m_vcReservoir[nReservoir];

  assert(false);

  return *m_vcReservoir[0];
}

int CEclipseFile::SizeX() const
{
  assert(m_vcReservoir.size() != 0);
  return m_vcReservoir[0]->SizeX();
}

int CEclipseFile::SizeY() const
{
  assert(m_vcReservoir.size() != 0);
  return m_vcReservoir[0]->SizeY();
}

int CEclipseFile::SizeZ() const
{
  assert(m_vcReservoir.size() != 0);
  return m_nDepth;
}

bool CEclipseFile::WriteCoord(TOutputStream& stream)
{
  assert(m_vcReservoir.size() != 0);

  stream << "COORD\n";

  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
  {
    for(int y = 0; y < (m_vcReservoir[nReservoir]->SizeY() + 1); y++)
    {
      for(int x = 0; x < (m_vcReservoir[nReservoir]->SizeX() + 1); x++)
      {
        CEclipseReservoir* pRes = m_vcReservoir[nReservoir];

        stream << pRes->LowerGrid(x, y).X() << " ";
        stream << pRes->LowerGrid(x, y).Y() << " ";
        stream << pRes->LowerGrid(x, y).Z() << "\t";

        stream << pRes->UpperGrid(x, y).X() << " ";
        stream << pRes->UpperGrid(x, y).Y() << " ";
        stream << pRes->UpperGrid(x, y).Z() << "\n";

        assert(pRes->LowerGrid(x,y).Z() <= pRes->UpperGrid(x, y).Z());

        ItemSaved();

      } // X Loop
    } // Y Loop
  } // Res Loop

  stream << "/\n";

  return true;
}

bool CEclipseFile::WriteZCoord(TOutputStream& stream)
{
  int nRep = 0;

  stream << "ZCORN\n";

  for(int z = 0; z < (m_nDepth * 2); z++)
  {
    for(int y = 0; y < (m_vcReservoir[0]->SizeY() * 2); y++)
    {
      for(int x = 0; x < (m_vcReservoir[0]->SizeX() * 2); x++)
      {
        CEclipseReservoir *pRes = &Reservoir((z-(z%2))/2);

        // Read the z coordinate
        stream << pRes->Depth(x, y, z) << " ";

        if(nRep == 5)
        {
          nRep = 0;
          stream << "\n";
        }
        else
          nRep++;

        ItemSaved();
      }// X Loop
    }// Y Loop
  }// Z Loop

  stream << "/\n";

  return false;
}

bool CEclipseFile::OnParseFail(TInputStream& stream, const QString& sToken)
{
  // We must have reservoirs otherwise we have failure
  if(m_vcReservoir.size() == 0)
    return CTextFile::OnParseFail( stream, sToken );

  // When the parser fails it could be a property or a failing
  // keyword (a wrong file)
  // The current token could be a property so create one
  int nIndex = CreateProperty(sToken);

  // Start reading doubles
  for(int z = 0; z < SizeZ(); z++)
  {
    for(int y = 0; y < SizeY(); y++)
    {
      for(int x = 0; x < SizeX(); x++)
      {
        // Read a double
        double dValue;
        stream >> dValue;
        PropertyValue(nIndex, x, y, z, dValue);

        ItemRead();
      }
    }
  }

  // When delimiter is available everthing is ok
  stream.checkDelimiter('/');

  return true;
}

bool CEclipseFile::OnEndParseSucceed()
{
  // For the voxel indices yields
  // SizeXv = SizeX() = the total amount of voxels in the x direction
  // SizeYv = SizeY() = the total amount of voxels in the y direction
  // SizeZv = SizeZ() = the total amount of voxels in the z direction

  // For the point indices yields
  // SizeXp = SizeXv + 1 = the total amount of voxels in the x direction
  // SizeYp = SizeYv + 1 = the total amount of voxels in the x direction
  // SizeZp = SizeZv + 1 = the total amount of voxels in the x direction

  // Allocate SizeZp potential horizons and allocate a grid of SizeXp x SizeYp points for each horizon.
  m_vcEclipseHorizon.resize(SizeZ() + 1, TEclipseHorizon(SizeX() + 1, TRow(SizeY() + 1)));

  int Zp, Xp, Yp;
  // Insert points for each horizon
  //for(Zp = 0; Zp < m_vcHorizon.size(); Zp++)
  for(Zp = 0; Zp < m_vcEclipseHorizon.size(); Zp++)
  {
    // Insert mean points in the current horizon
    TEclipseHorizon& horizon = m_vcEclipseHorizon[Zp];
    for(Xp = 0; Xp < horizon.size(); Xp++)
    {
      for(Yp = 0; Yp < horizon[Xp].size(); Yp++)
      {
        horizon[Xp][Yp] = MeanPoint(Xp, Yp, Zp);
      }
    }
  }


  return true;
}

geo::CPoint CEclipseFile::MeanPoint(int Xp, int Yp, int Zp) const
{
  // Calculate mean point
  std::vector<geo::CPoint> vcPoint = AdjacentPoints(Xp, Yp, Zp);
  if(vcPoint.size() == 0)
    return geo::CPoint();

  geo::CPoint ret = vcPoint[0];
  for(size_t i = 1; i < vcPoint.size(); i++)
    ret = ret + vcPoint[i];

  return ret / vcPoint.size();
}

std::vector<geo::CPoint> CEclipseFile::AdjacentPoints(int Xp, int Yp, int Zp) const
{
  std::vector<geo::CPoint> vcRet;
  // Determ the 8 adjacent points
  AddPoint(vcRet, Xp - 1, Yp - 1, Zp - 1, 1, 1, 1);
  AddPoint(vcRet, Xp - 1, Yp - 1, Zp    , 1, 1, 0);
  AddPoint(vcRet, Xp - 1, Yp    , Zp - 1, 1, 0, 1);
  AddPoint(vcRet, Xp - 1, Yp    , Zp    , 1, 0, 0);
  AddPoint(vcRet, Xp    , Yp - 1, Zp - 1, 0, 1, 1);
  AddPoint(vcRet, Xp    , Yp - 1, Zp,     0, 1, 0);
  AddPoint(vcRet, Xp    , Yp    , Zp - 1, 0, 0, 1);
  AddPoint(vcRet, Xp    , Yp    , Zp,     0, 0, 0);
  return vcRet;
}

void CEclipseFile::AddPoint(std::vector<geo::CPoint> &vcPoint, int Xv, int Yv, int Zv, int px, int py, int pz) const
{
  if((Xv >= 0) && (Yv >= 0) && (Zv >= 0) && (Xv < SizeX()) && (Yv < SizeY()) && (Zv < SizeZ()))
  {
    // Voxel exists so return the requested point
//    if(Reservoir(Zv).CellEnable(Xv, Yv, Zv))
    vcPoint.push_back(geo::CPoint(Cell(Xv, Yv, Zv).GetAt(px, py, pz)));
  }
}

int CEclipseFile::HorizonSize() const
{
  return (int)m_vcEclipseHorizon.size();
}

CEclipseFile::TEclipseHorizon CEclipseFile::Horizon(size_t nHorizonIndex)
{
  assert(nHorizonIndex < m_vcEclipseHorizon.size());
  return m_vcEclipseHorizon[nHorizonIndex];
}

const CEclipseFile::TEclipseHorizon CEclipseFile::Horizon(size_t nHorizonIndex) const
{
  assert(nHorizonIndex < m_vcEclipseHorizon.size());
  return m_vcEclipseHorizon[nHorizonIndex];
}

bool CEclipseFile::ReadCellEnable(TInputStream& stream, const QString& /*sToken*/)
{
  assert(m_vcReservoir.size() != 0);

  bool bEnable;

  for(int z = 0; z < SizeZ(); z++)
  {
    for(int y = 0; y < SizeY(); y++)
    {
      for(int x = 0; x < SizeX(); x++)
      {
        CEclipseReservoir *pRes = &Reservoir(z);

        // Read Enable
        stream >> bEnable;

        pRes->CellEnable(x, y, z, bEnable);

        ItemRead();
      }
    }
  }

  stream.checkDelimiter('/');

  return true;
}

bool CEclipseFile::WriteCellEnable(TOutputStream& stream)
{
  assert(m_vcReservoir.size() != 0);

  stream << "ACTNUM\n";

  for(int z = 0; z < SizeZ(); z++)
  {
    for(int y = 0; y < SizeY(); y++)
    {
      for(int x = 0; x < SizeX(); x++)
      {
        CEclipseReservoir *pRes = &Reservoir(z);

        // Write Enable
        stream << pRes->CellEnable(x, y, z) << " ";

        if(x == (pRes->SizeX() - 1))
          stream << "\n";

        if((x == (pRes->SizeX() - 1)) && (y == (pRes->SizeY() - 1)))
          stream << "\n";

//        if(stream.fail())
//          return false;

        ItemSaved();
      }
    }
  }

  stream << "/\n";

  return true;
}

bool CEclipseFile::WriteProperties(TOutputStream& stream)
{
  for(size_t i = 0; i < PropertySize(); i++)
  {
    assert(m_vcReservoir.size() != 0);

    stream << PropertyName(i) << '\n';

    for(int z = 0; z < SizeZ(); z++)
    {
      for(int y = 0; y < SizeY(); y++)
      {
        for(int x = 0; x < SizeX(); x++)
        {
          CEclipseReservoir *pRes = &Reservoir(z);

          // Write Enable
          stream << PropertyValue(i, x, y, z) << " ";

          if(x == (pRes->SizeX() - 1))
            stream << "\n";

          if((x == (pRes->SizeX() - 1)) && (y == (pRes->SizeY() - 1)))
            stream << "\n";

//          if(stream.fail())
//            return false;

          ItemSaved();
        }
      }
    }

    stream << "/\n";
  }

  return true;
}


bool CEclipseFile::WriteCoordSys(TOutputStream& stream)
{
  assert(m_vcReservoir.size() != 0);

  stream << "COORDSYS\n";

  for(size_t nReservoir = 0; nReservoir < m_vcReservoir.size(); nReservoir++)
  {
    // Write boundary
    stream << m_vcReservoir[nReservoir]->LowerBoundary() + 1 << " ";
    stream << m_vcReservoir[nReservoir]->UpperBoundary() + 1 << " ";

    // Write circle completed
    if(m_vcReservoir[nReservoir]->CircleCompleted())
      stream << "'COMP' ";
    else
      stream << "'INCOMP' ";

    // Write join
    if(m_vcReservoir[nReservoir]->Join())
      stream << "'JOIN' ";
    else
      stream << "'SEPARATE' ";

    // Write Lower boundary
    for(size_t nTestRes = 0; nTestRes < m_vcReservoir.size(); nTestRes++)
      if(m_vcReservoir[nTestRes] == &m_vcReservoir[nReservoir]->LowerReservoirBoundary())
        stream << nTestRes + 1 << " ";

    // Write Upper boundary
    for(size_t nTestRes = 0; nTestRes < m_vcReservoir.size(); nTestRes++)
      if(m_vcReservoir[nTestRes] == &m_vcReservoir[nReservoir]->UpperReservoirBoundary())
        stream << nTestRes + 1 << " ";

    // Write Delimiter
    stream << "/\n";

//    if(stream.fail())
//      return false;
  }

  stream << "\n";

  return true;
}

CEclipseReservoir& CEclipseFile::CreateReservoir(int x, int y)
{
  // Creation of the standard reservoir
  return *(new CEclipseReservoir(x, y));
}


CEclipseCell CEclipseFile::Cell(int x, int y, int z) const
{
  return Reservoir(z).Cell(x, y, z);
}

size_t CEclipseFile::PropertySize() const
{
  return m_vcProperty.size();
}

const QString& CEclipseFile::PropertyName(size_t nPropertyIndex) const
{
  assert(nPropertyIndex < m_vcProperty.size());
  return m_vcProperty[nPropertyIndex].first;
}

const double& CEclipseFile::PropertyValue(size_t nPropertyIndex, int x, int y, int z) const
{
  assert(nPropertyIndex < m_vcProperty.size());
  return m_vcProperty[nPropertyIndex].second.get_at(x, y, z);
}

void CEclipseFile::PropertyValue(size_t nPropertyIndex, int x, int y, int z, const double& dValue)
{
  assert(nPropertyIndex < m_vcProperty.size());
  m_vcProperty[nPropertyIndex].second.set_at(x, y, z, dValue);
}

int CEclipseFile::CreateProperty(const QString& sName)
{
  int nIndex = (int)m_vcProperty.size();
  m_vcProperty.push_back(TProperty(sName, array_temp<double>(SizeX(), SizeY(), SizeZ())));
  return nIndex;
}

long CEclipseFile::SavedItems() const
{
  return (long)((SizeX() + 1) * (SizeY() + 1) * 2 * m_vcReservoir.size() + SizeX() * SizeY() * SizeZ() * (8 + PropertySize()));
}

bool CEclipseFile::OnWrite(TOutputStream& stream)
{
  WriteSpecGrid(stream);
  WriteCoordSys(stream);
  WriteCoord(stream);
  WriteZCoord(stream);
  WriteCellEnable(stream);
  WriteProperties(stream);
  return true;
}
