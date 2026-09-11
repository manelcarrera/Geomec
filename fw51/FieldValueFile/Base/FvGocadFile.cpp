 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "INode.h"
#include "IMesh.h"
#include "FvGocadFile.h"
#include "ITetrahedron.h"
#include <iostream>
#include <iomanip>

CGoCadFile::CGoCadFile(geo::IMesh& mesh)
: m_type(GOCAD_IDLE), 
  m_state(GOCAD_CLOSED), 
  m_mesh(mesh)
{
  // Comment ...
  DefineComment("#");
  DefineComment("*");

  // Tokens / Keywords
  DefineToken(*(new TGocadToken(*this, "GOCAD", &CGoCadFile::OpenGocad)));
  DefineToken(*(new TGocadToken(*this, "TSurf", &CGoCadFile::TypeSurface)));
  DefineToken(*(new TGocadToken(*this, "TSolid", &CGoCadFile::TypeVolume)));
  DefineToken(*(new TGocadToken(*this, "TETRA", &CGoCadFile::ReadTetrahedron)));
  DefineToken(*(new TGocadToken(*this, "END", &CGoCadFile::CloseGocad)));
  DefineToken(*(new TGocadToken(*this, "HEADER", &CGoCadFile::ReadHeader)));
  DefineToken(*(new TGocadToken(*this, "GEOLOGICAL_FEATURE", &CGoCadFile::ReadGeoType)));
  DefineToken(*(new TGocadToken(*this, "GEOLOGICAL_TYPE", &CGoCadFile::ReadGeoType)));
  DefineToken(*(new TGocadToken(*this, "PROPERTY_CLASS", &CGoCadFile::ReadPropClass)));
  DefineToken(*(new TGocadToken(*this, "PROPERTY_CLASS_HEADER", &CGoCadFile::ReadPropClass)));
  DefineToken(*(new TGocadToken(*this, "PROPERTIES", &CGoCadFile::ReadProperties)));
  DefineToken(*(new TGocadToken(*this, "ESIZES", &CGoCadFile::ReadPropertiesSize)));
  DefineToken(*(new TGocadToken(*this, "NO_DATA_VALUES", &CGoCadFile::ReadDefaultPropertyValues)));
  DefineToken(*(new TGocadToken(*this, "UNITS", &CGoCadFile::ReadPropertyUnits)));
  DefineToken(*(new TGocadToken(*this, "PROPERTY_CLASSES", &CGoCadFile::ReadPropertyClasses)));
  DefineToken(*(new TGocadToken(*this, "STRATIGRAPHIC_POSITION", &CGoCadFile::ReadStatPos)));
  DefineToken(*(new TGocadToken(*this, "TFACE", &CGoCadFile::ReadTFace)));
  DefineToken(*(new TGocadToken(*this, "VRTX", &CGoCadFile::ReadVertex)));
  DefineToken(*(new TGocadToken(*this, "PVRTX", &CGoCadFile::ReadPropertyVertex)));
  DefineToken(*(new TGocadToken(*this, "TRGL", &CGoCadFile::ReadTriangle)));
  DefineToken(*(new TGocadToken(*this, "BSTONE", &CGoCadFile::ReadBStone)));
  DefineToken(*(new TGocadToken(*this, "BORDER", &CGoCadFile::ReadBorder)));
}

CGoCadFile::~CGoCadFile()
{
  RemoveAll();
}

void CGoCadFile::RemoveAll()
{
  for(size_t i = 0; i < m_vcSurface.size(); i++)
  delete m_vcSurface[i];
//  for(i = 0; i < m_vcVolume.size(); i++)
//    delete m_vcVolume[i];

  m_vcSurface.clear();
}

void CGoCadFile::AppendSurface(const geo::ISurface& surface, const QString &sName)
{
  // wtf
  m_vcSurface.resize(m_vcSurface.size() + 1);
  m_vcSurface[m_vcSurface.size() - 1] = new CGoCadSurface(m_mesh, surface, sName);
}

bool CGoCadFile::OnParseFail(TInputStream& stream, const QString &sToken)
{
  // Just deny the words we don't know ..
  if(m_state == GOCAD_CLOSED)
  return CTextFile::OnParseFail(stream, sToken);

  return true;
}

bool CGoCadFile::OpenGocad(TInputStream& /*stream*/, const QString &/*sToken*/)
{
  m_state = PARSE_TYPE;
  return true;
}

bool CGoCadFile::CloseGocad(TInputStream& /*stream*/, const QString &/*sToken*/)
{
  // Check the surfaces
  std::vector<CGoCadSurface*>::iterator iter;
  iter = m_vcSurface.begin();
  while (iter != m_vcSurface.end())
  {
  if ((*iter)->PointSize() == 0)
  {
      m_vcSurface.erase(iter);
      iter = m_vcSurface.begin();
  }
  else
      iter++;
  }

  m_state = GOCAD_CLOSED;
  m_type = GOCAD_IDLE;

  return true;
}

bool CGoCadFile::TypeSurface(TInputStream& stream, const QString &/*sToken*/)
{
  m_type = GOCAD_SURFACE;

  // Eat version number
  while(stream.peek() != '\n' && stream.peek() != 0)
  stream.get();

  NewSurface();


  return true;
}

void CGoCadFile::NewSurface()
{
  // Add new surface
  // wtf
  m_vcSurface.resize(m_vcSurface.size() + 1);
  m_vcSurface[m_vcSurface.size() - 1] = new CGoCadSurface(m_mesh);
}

CGoCadSurface *CGoCadFile::LatestSurf()
{
  return m_vcSurface[m_vcSurface.size() - 1];
}

CGoCadVolume *CGoCadFile::LatestVol()
{
  return m_vcVolume[m_vcVolume.size() - 1];
}

bool CGoCadFile::TypeVolume(TInputStream& stream, const QString &/*sToken*/)
{

  m_type = GOCAD_VOLUME;

  while(stream.peek() != '\n' && stream.peek() != 0)
  stream.get();

  // wtf
  m_vcVolume.resize(m_vcVolume.size() + 1);
  m_vcVolume[m_vcVolume.size() - 1] = new CGoCadVolume(m_mesh);

  return true;
}

bool CGoCadFile::ReadHeader(TInputStream& stream, const QString &/*sToken*/)
{
  QString sName;
  // Mini parser ....
  while(stream.get() != '{') {
  if( stream.eof() ) {
      throw CReadException( "{ Expected." );
  }
  };

  QString sLine;
  bool bName = false;
  while( !stream.eof() ) {
  char ch = stream.get();
  switch( ch ) {
  case '\n':
      sLine = "";
      bName = false;
      break;
  case '}':
      // Stop ...
      if(m_type == GOCAD_SURFACE ) {
    LatestSurf()->Header(sName);
      } else {
    assert(m_type == GOCAD_VOLUME);
    LatestVol()->Header(sName);
      }

      ItemRead();
      return true;
  default:
      sLine += ch;
      if( bName ) sName += ch;
      break;
  }

  if(sLine == "name:") bName = true;
  };

  throw CReadException("} expected for header end.");

  return true;
}

bool CGoCadFile::ReadGeoType(TInputStream& stream, const QString &/*sToken*/)
{
  // Eat version number
  while(stream.peek() != '\n' && stream.peek() != 0)
  stream.get();

  return true;
}

bool CGoCadFile::ReadPropClass(TInputStream& stream, const QString &/*sToken*/)
{
   while(stream.peek() != '}' && !stream.eof()) stream.get();
   return true;
}

bool CGoCadFile::ReadStatPos(TInputStream& stream, const QString &/*sToken*/)
{
  while(stream.peek() != '\n' &&stream.peek() != 0)
  stream.get();

  return true;
}

bool CGoCadFile::ReadVertex(TInputStream& stream, const QString &/*sToken*/)
{
  int nIndex;
  double x, y, z;


  stream >> nIndex;
  stream >> x; // ATTENTION: x == Easting!!
  stream >> y; // ATTENTION: y == Northing!!
  stream >> z;

  // jbj
  // Mantis #2289 patch
  if ( m_unitType == FIELD_UNIT ) {
    // unitType has been TEMPORARILY set to FIELD in CTextFile::Parse
    // We are storing SI values.
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    x *= 0.3048;
    y *= 0.3048;
    z *= 0.3048;
  }

  switch(m_type)
  {
  case GOCAD_SURFACE:
  assert(m_vcSurface.size() > 0);
  if(!LatestSurf()->InsertPoint(nIndex, y, x, z)) {
      throw CReadException(QString("Duplicate vertex %1").arg(nIndex));
  }
  break;
  case GOCAD_VOLUME:
  assert(m_vcVolume.size() > 0);
  if(!LatestVol()->InsertPoint(nIndex, y, x, z)) {
      throw CReadException(QString("Duplicate vertex %1").arg(nIndex));
  }
  break;
  default:
  throw CReadException("Define GoCad type before reading a vertex.");
  break;
  }

  ItemRead();

  return true;
}

bool CGoCadFile::ReadTriangle(TInputStream& stream, const QString &/*sToken*/)
{
  assert(m_vcSurface.size() > 0);
  std::vector<int> vcNode(3);

  for(size_t i = 0; i < vcNode.size(); i++) {
   stream >> vcNode[i];
   if(!LatestSurf()->isPoint(vcNode[i])) {
     throw CReadException(QString("Node %1 does not exist.").arg( vcNode[i] ));
   }
  } 
 
  if(!LatestSurf()->InsertTriangle(vcNode[0], vcNode[1], vcNode[2]))  {
     //! \todo Degenerated triangles
  }

  ItemRead();

  return true;
}

bool CGoCadFile::ReadBorder(TInputStream& stream, const QString &/*sToken*/)
{
  while(stream.peek() != '\n' &&stream.peek() != 0)
  stream.get();

  return true;
}

bool CGoCadFile::ReadBStone(TInputStream& stream, const QString &/*sToken*/)
{
  while(stream.peek() != '\n' &&stream.peek() != 0)
  stream.get();

  return true;
}

bool CGoCadFile::ReadTFace(TInputStream& /*stream*/, const QString &/*sToken*/)
{
  return true;
}

size_t CGoCadFile::SurfaceSize() const
{
  return m_vcSurface.size();
}

size_t CGoCadFile::VolumeSize() const
{
  return m_vcVolume.size();
}

const CGoCadSurface& CGoCadFile::Surface(size_t nIndex) const
{
  assert(nIndex < m_vcSurface.size());
  return *m_vcSurface[nIndex];
}

const CGoCadVolume& CGoCadFile::Volume(size_t nIndex) const
{
  assert(nIndex < m_vcVolume.size());
  return *m_vcVolume[nIndex];
}
long CGoCadFile::SavedItems() const
{
  long lRet = 0;

  // Count surfaces ....
  size_t i;
  for(i = 0; i < SurfaceSize(); i++)
  lRet += Surface(i).PointSize() + Surface(i).FaceSize();

  for(i = 0; i < VolumeSize(); i++)
  lRet += Volume(i).PointSize() + Volume(i).ElementSize();

  return lRet;
}

bool CGoCadFile::OnWrite(TOutputStream& stream)
{
  size_t i ;
  for(i = 0; i < SurfaceSize(); i++) {
  if(!WriteSurface(stream, Surface(i)))
      return false;
  }
  for(i = 0; i < VolumeSize(); i++) {
  if(!WriteVolume(stream, Volume(i)))
      return false;
  }

  return true;
}

bool CGoCadFile::WriteSurface( TOutputStream&      stream, 
                 const CGoCadSurface &surface )
{
  stream << "GOCAD TSurf" << '\n';

  // Write header
  stream << "HEADER {" << '\n';
  stream << "name:" << surface.Header().Name() << '\n';  
  stream << "}" << '\n';

  stream << "AXIS_UNIT    ";
  if ( m_unitType == FIELD_UNIT )
    stream << "\"ft\"    \"ft\"    \"ft\"" << '\n';
  else
    stream << "\"m\"     \"m\"     \"m\"" << '\n';

  // Write points
  stream << "TFACE" << '\n';

  // Set stream double output to precision 12
  stream.setRealNumberPrecision(12);

  double unitFactor = 1.0;
  if ( m_unitType == FIELD_UNIT )
  	  // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    unitFactor = 0.3048;

  for ( int n = 0; n < surface.PointSize(); n++ ) {
  const geo::INode& node = dynamic_cast<const geo::INode&>(surface.Point(n));

  // Write point to stream ...
  stream << "VRTX ";
  stream << node.Index() << " ";
  // In a output we have left handed axis-system
  stream << node.Y() / unitFactor << " ";
  stream << node.X() / unitFactor << " ";
  stream << node.Z() / unitFactor << '\n';

  // Update save ...
  ItemSaved();
  }

  // Write faces ...
  for ( int i = 0; i < surface.FaceSize(); i++ ) {
  const geo::IFace& face = surface.Face(i);
  assert(face.IndexingElementSet() == &m_mesh);

  // Write triangle
  stream << "TRGL";

  for(int j = 0; j < face.NrOfPoints(); j++)
      stream << " " << face.PointIndex(j);

  stream << '\n';

  }

  // Write end tag ...
  stream << "END" << '\n';

  return true;
}

bool CGoCadFile::WriteVolume(TOutputStream& stream, const CGoCadVolume &volume)
{
  stream << "GOCAD TSolid" << '\n';

  // Write header
  stream << "HEADER {" << '\n';
  stream << "name:" << volume.Header().Name() << '\n';
  stream << "}" << '\n';

  // Write points
  stream << "TVOLUME" << '\n';

  for(int n = 0; n < volume.PointSize(); n++)
  {
  const geo::INode& node = dynamic_cast<const geo::INode&>(volume.Point(n));

  // Write point to stream ...
  stream << "VRTX ";
  stream << node.Index() << " ";
  // In a output we have left handed axis-system
  stream << node.Y() << " ";
  stream << node.X() << " ";
  stream << node.Z() << '\n';

  // Update save ...
  ItemSaved();
  }

  // Write faces ...
  for(int i = 0; i < volume.ElementSize(); i++)
  {
      const geo::ITetrahedron* pTetra = dynamic_cast<const geo::ITetrahedron*>(&volume.Element(i));
      if(pTetra) {
      assert(pTetra->IndexingElementSet() == &m_mesh);

      // Write triangle
      stream << "TETRA";

      for(int j = 0; j < pTetra->NrOfPoints(); j++)
    stream << " " << pTetra->PointIndex(j);

      stream << '\n';
  }

  }

  // Write end tag ...
  stream << "END" << '\n';

  return true;
}

bool CGoCadFile::ReadProperties(TInputStream& stream, const QString &/*sToken*/)
{
  // Read the properties ...
  QString sLine;

  char ch;
  do
  {
  ch = stream.get();
  switch(ch)
  {
  case ' ':
  case '\t':
  case '\n':
      if(sLine.length() > 0)
      {
    // Create new header item ...
    switch(m_type)
    {
    case GOCAD_SURFACE:
          LatestSurf()->Header().AddProperty(sLine);
          break;
    case GOCAD_VOLUME:
          LatestVol()->Header().AddProperty(sLine);
          break;
    default:
          assert(false);
    }
    sLine = "";
      }
      break;
  default:
      sLine += ch;
  }
  }
  while(ch != '\n');



  return true;
}

bool CGoCadFile::ReadPropertiesSize(TInputStream& stream, const QString &/*sToken*/)
{
  for(int i = 0; i < CurrentHeader().PropertySize(); i++)
  {
  int nSize;
  stream >> nSize;
  CurrentHeader().Property(i).Size(nSize);
  }



  return true;
}

bool CGoCadFile::ReadPropertyVertex(TInputStream& stream, const QString &/*sToken*/)
{
  int nIndex;
  double x, y, z;

  // Read vertex
  stream >> nIndex;
  stream >> x; // ATTENTION: x == Easting!!
  stream >> y; // ATTENTION: y == Northing!!
  stream >> z;

  const geo::IPoint *pPoint = new geo::CPoint(y, x, z);

  switch(m_type)
  {
  case GOCAD_SURFACE:
  if(!LatestSurf()->InsertPoint(nIndex, y, x, z))
      throw CReadException(QString("Duplicate vertex %1").arg(nIndex));
  break;
  case GOCAD_VOLUME:
  if(!LatestVol()->InsertPoint(nIndex, y, x, z))
      throw CReadException(QString("Duplicate vertex %1").arg(nIndex));
  break;
  default:
  // Error handling ...
  break;

  }

  // Read properties ....
  for(int i = 0; i < CurrentHeader().PropertySize(); i++)
  {
  std::vector<double> vcProperty(CurrentHeader().Property(i).Size());
  for(size_t j = 0; j < vcProperty.size(); j++)
      stream >> vcProperty[j];
  CurrentHeader().Property(i).AddValue(*pPoint, vcProperty);
  }

  CurrentHeader().AddPoint(pPoint);

  //ItemRead();

  while(stream.peek() != '\n')
  stream.get();



  ItemRead();

  return true;
}

bool CGoCadFile::ReadDefaultPropertyValues(TInputStream& stream, const QString &/*sToken*/)
{
  while(stream.peek() != '\n' && stream.peek() != 0)
  stream.get();

  return true;
}

bool CGoCadFile::ReadPropertyUnits(TInputStream& stream, const QString &/*sToken*/)
{
  for(int i = 0; i < CurrentHeader().PropertySize(); i++)
  {
  QString sUnit;
  stream >> sUnit;
  CurrentHeader().Property(i).UnitName(sUnit);
  }



  return true;
}

bool CGoCadFile::ReadPropertyClasses(TInputStream& stream, const QString &/*sToken*/)
{
  for(int i = 0; i < CurrentHeader().PropertySize(); i++)
  {
  QString sClass;
  stream >> sClass;
  CurrentHeader().Property(i).ClassName(sClass);
  }

  return true;
}


CGoCadHeader& CGoCadFile::CurrentHeader()
{
  CGoCadHeader *pHeader = 0;
  // Determ type
  switch(m_type)
  {
  case GOCAD_SURFACE:
  pHeader = &LatestSurf()->Header();
  break;
  case GOCAD_VOLUME:
  pHeader = &LatestVol()->Header();
  break;
  default:
  // Error handling ..
  break;
  }

  return *pHeader;
}

bool CGoCadFile::ReadTetrahedron(TInputStream& stream, const QString &/*sToken*/)
{
  assert(m_type == GOCAD_VOLUME);

  std::vector<int> vcNode(4);

  for(size_t i = 0; i < vcNode.size(); i++) {
   stream >> vcNode[i];
   if(!LatestVol()->isPoint(vcNode[i])) {
     throw CReadException(QString("Node %1 does not exist.").arg( vcNode[i] ));
   }
  }
  
  InsertTetInVol(vcNode[0], vcNode[1], vcNode[2], vcNode[3]);

  ItemRead();
  return true;
}

int CGoCadFile::InsertTetInVol(const int nIndex1, const int nIndex2, const int nIndex3, const int nIndex4)
{
  return LatestVol()->InsertTetrahedron(nIndex1, nIndex2, nIndex3, nIndex4);
}

void CGoCadFile::InsertTriInSurf(const int nIndex1, const int nIndex2, const int nIndex3)
{
  LatestSurf()->InsertTriangle(nIndex1, nIndex2, nIndex3);
}
