#ifdef SKUA_NEW

#include "GocadImport.h"
#include "SKUAImport.h"
#include "SKUAParseData.h"
#include "SKUAIProgressHandler.h"

#include "IProgressBase.h"
#include "Global.h"
#include "IProgressFactory.h"

#include "GlobalMessage.h"

#include "PointSet.h" // wjrx mantis 2896
#include "HexaModel.h"
#include "HexaSurface.h"
#include "TetraSurface.h"
#include "FvGocadSurface.h"
#include "IMesh3D.h"
#include "IPlatform.h"
#include "ValueTypeFactory.h"
#include "GocadElementSet.h"
#include "ElementValueSet.h"


//#define GOCAD_TIMING
#ifdef GOCAD_TIMING
#include <chrono>
#endif

#include <memory>

namespace
{
  class ProgressWrapper : public gm_skua::IProgressHandler
  {
  IProgressBase& progress;
  public:
  ProgressWrapper(IProgressBase& progress)
      : gm_skua::IProgressHandler()
      , progress(progress)
  {
  }

  virtual bool Step(int number)
  {
      try
      {
    progress.Step(number);
      }
      catch (CProgressCancel *cancelEvent)
      {
    delete cancelEvent;
    return false;
      }

      return true;
  }
  };
}


CGocadImport::CGocadImport(bool bRequireSurfaceDefinitions, bool bAllowLightTSolid)
  : m_bRequireSurfaceDefinitions(bRequireSurfaceDefinitions)
  , m_bAllowLightTSolid(bAllowLightTSolid)
  , m_importer(nullptr)
{
}

CGocadImport::~CGocadImport()
{
  delete m_importer;
}

bool CGocadImport::Import(const std::string& strFileName)
{
  FILE *fp = fopen(strFileName.c_str(), "rb");
  if (!fp)
  return false;

  fseek(fp, 0, SEEK_END);
  long fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::auto_ptr <IProgressBase> progdlg(_g->prog()->create(eProgress::Dual, "Reading Gocad format", true, 1 ));
  progdlg->NextJob("Reading Gocad format");
  progdlg->AddSteps(fileSize);

#ifdef GOCAD_TIMING
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(), end;
#endif

  ProgressWrapper pw(*progdlg.get());

  m_importer = new gm_skua::SKUAImport(fp, pw);

  bool success = m_importer->Import();

  fclose(fp);

#ifdef GOCAD_TIMING
  int lines = m_importer->LineNr();

  end = std::chrono::steady_clock::now();
  long long ms = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
  _m()->msg(QString::asprintf("Elapsed time: %ld.%03ld seconds, %d lines", ms / 1000, ms % 1000, lines));
#endif

  if (success)
  {
  for (int i = 0; i < m_importer->ObjectNr(); ++i)
  {
      const gm_skua::SKUAParseData *object = m_importer->Object(i);

      switch (object->skua_type)
      {
      case gm_skua::SKUAParseData::LightTSolid:
      case gm_skua::SKUAParseData::TSolid:
    m_solids.push_back(object);
    break;
      case gm_skua::SKUAParseData::VSet:
    m_vsets.push_back(object);
    break;
      case gm_skua::SKUAParseData::TSurf:
    m_surfaces.push_back(object);
    break;
      }
  }
  }
  else
  {
  std::string delimiter;
  for (int i = 0; i < m_importer->ErrorNr(); ++i)
  {
      m_strError += m_importer->Error(i) + delimiter;
      delimiter = "\n";
  }
  }

  return success;
}

CElementSet *CGocadImport::CreateElementSet(CFemAppModel& model, int solidIndex) const
{
  const gm_skua::SKUAParseData& solid = *Solid(solidIndex);

  const std::vector<gm_skua::SKUAParseData::Tetra>& tetras = solid.tetras;

  size_t vertex_size = solid.vertices.size();
  size_t tetra_size = tetras.size();

  CElementSet::TPointVec vcPoints;
  vcPoints.reserve(vertex_size);

  CElementSet::TElementVec vcElements;
  vcElements.reserve(tetra_size);

  // in the solid we have vertices and sharedvertices/atoms combined, but we need to filter out the latter
  // ptIndex keeps track of the real vertex indices
  std::vector<int> ptIndex(solid.vertices.size(), -1);

  for (size_t i = 0; i < vertex_size; ++i)
  {
  const gm_skua::SKUAParseData::Vertex& vertex = solid.vertices[i];

  if (vertex.link < 0) // vertex
  {
      ptIndex[i] = vcPoints.size();
      vcPoints.emplace_back(geo::CPoint(vertex.coordinate[1], vertex.coordinate[0], vertex.coordinate[2]));
  }
  else // type indexes vertices
  {
      ptIndex[i] = ptIndex[vertex.link];
  }
  }

  for (size_t i = 0; i < tetra_size; ++i)
  {
  const gm_skua::SKUAParseData::Tetra& tetra = tetras[i];

  vcElements.emplace_back(std::vector<int>{ ptIndex[tetra.node[0]], ptIndex[tetra.node[1]], ptIndex[tetra.node[2]], ptIndex[tetra.node[3]] });
  }

#ifdef SKUA_ALLOW_MERGE
  // Meshing multiple solids should lead to a selection of solids and some merge, but that doesn't seem to work: no dialog
  // Let's rethink and possibly add this later
  // Also, it looks like each gocad elementset would save ALL gocad data (although using shared pointers on import, that is lost on save/load
  const std::vector<const gm_skua::SKUAParseData *>& solids = const_cast<CGocadImport *>(this)->getSolids();
  std::vector<const gm_skua::SKUAParseData *> solid_copies(solids.size(), nullptr);

  for (size_t i = 0; i < solid_copies.size(); ++i)
  {
  gm_skua::SKUAParseData *data = new gm_skua::SKUAParseData(gm_skua::SKUAParseData::Unknown);
  *data = *solids[i];
  solid_copies[i] = data;
  }
#else
  // Just have a single solid in the set
  // TODO: determine if we want to remove the whole vector thing here
  gm_skua::SKUAParseData *solid_copy = new gm_skua::SKUAParseData(gm_skua::SKUAParseData::Unknown);
  *solid_copy = solid;
  std::vector<const gm_skua::SKUAParseData *> solid_copies(1, solid_copy);
#endif
  CGocadElementSet* pElementSet = new CGocadElementSet(solid.name.data, CElementSet::TETRA, vcPoints, vcElements, model, solid_copies);

  // add tetra properties
  const gm_skua::SKUAParseData::PropertyData& tetra_props = solid.tetra_props;

  int prop_size = tetra_props.esizes.size();
  int value_index = 0;

  std::vector<double> value(1);

  for (int i = 0; i < prop_size; ++i)
  {
  for (int j = 0; j < tetra_props.esizes[i]; ++j)
  {
      CElementValueSet& vset = pElementSet->ElementValueSet(pElementSet->AddElementValueSet());

      QString strName = tetra_props.names[i].data;
      if (tetra_props.esizes[i] > 1)
    strName += QString("_%1").arg(j + 1);;
      vset.Name(strName);

      for (int k = 0; k < tetra_size; ++k)
      {
    value[0] = tetra_props.values[value_index][k];
    vset.PushBack(value);
      }
      ++value_index;
  }
  }

  // add nodal properties
  const gm_skua::SKUAParseData::PropertyData& vertex_props = solid.vertex_props;

  prop_size = vertex_props.esizes.size();
  value_index = 0;

  std::vector<double> values(4);

  for (int i = 0; i < prop_size; ++i)
  {
  for (int j = 0; j < vertex_props.esizes[i]; ++j)
  {
      CElementValueSet& vset = pElementSet->ElementValueSet(pElementSet->AddElementValueSet());

      QString strName = vertex_props.names[i].data;
      if (vertex_props.esizes[i] > 1)
    strName += QString("_%1").arg(j + 1);;
      vset.Name(strName);

      for (int k = 0; k < tetra_size; ++k)
      {
    const gm_skua::SKUAParseData::Tetra& tetra = tetras[k];
    for (int l = 0; l < 4; ++l)
    {
          values[l] = vertex_props.values[value_index][tetra.node[l]];
    }
    vset.PushBack(values);
      }
      ++value_index;
  }
  }

  return pElementSet;
}

CPointSet *CGocadImport::CreatePointSet(CFemAppModel& model, int vsetIndex) const
{
  CPointSet *pPointSet = CreatePointSet(model, Vset(vsetIndex));

  CreateFractureIntensityAttribute(pPointSet);

  return pPointSet;
}

void CGocadImport::CreateSurface(CFemAppModel &model, int surfIndex) const
{
  const gm_skua::SKUAParseData& surf = *Surf(surfIndex);

  geo::CMesh3D mesh;
  // tnmx
  // This surface is only used to construct a CHexaSurface or a CTetraSurface later on.
  // I guess this can be any ISurface, still this was the quickest option for me.
  CGoCadSurface gcSurf(mesh);
  gcSurf.Header("header"); // just set a header, else it wil crash later on

  if (surf.surfaces.size() != 1)
  return;

  for (size_t i = 0; i < surf.surfaces[0].size(); ++i)
  {
  int n[3];

  const gm_skua::SKUAParseData::Triangle& triangle = surf.surfaces[0][i];

  for (int j = 0; j < 3; ++j)
  {
      n[j] = triangle.node[j];

      const gm_skua::SKUAParseData::Vertex& vertex = surf.vertices[n[j]];

      gcSurf.InsertPoint(n[j], vertex.coordinate[1], vertex.coordinate[0], vertex.coordinate[2]);
  }

  if (!triangle.collapsed())
      gcSurf.InsertTriangle(n[0], n[1], n[2]);
  else
  {
      IPlatform* platform = IPlatform::instance();
      QString traceString = QString("Trapped collapsed triangle (%1, %2, %3)\n").arg(n[0]).arg(n[1]).arg(n[2]);

      platform->trace(traceString);
  }
  }

  if (dynamic_cast<CHexaModel*>(&model))
  new CHexaSurface(surf.name.data, gcSurf, model);
  else
  new CTetraSurface(surf.name.data, gcSurf, model);

  CreatePointSet(model, &surf);
}

int CGocadImport::SolidSize() const
{
  return static_cast<int>(m_solids.size());
}

int CGocadImport::VsetSize() const
{
  return static_cast<int>(m_vsets.size());
}

int CGocadImport::SurfSize() const
{
  return static_cast<int>(m_surfaces.size());
}

std::vector<const gm_skua::SKUAParseData *>& CGocadImport::getSolids()
{
  return m_solids;
}

const std::string& CGocadImport::Error() const
{
  return m_strError;
}

CPointSet* CGocadImport::CreatePointSet(CFemAppModel& model, const gm_skua::SKUAParseData *gocad) const
{
  size_t size = gocad->vertex_props.values.size();

  CPointSet* pPointSet = new CPointSet(gocad->name.data, model, size, CPointSet::DIM_3D);

  const gm_skua::SKUAParseData::PropertyData *props = &gocad->vertex_props;

  std::vector<geo::CValue> vcRow(3 + size);

  for (size_t i = 0; i < gocad->vertices.size(); ++i)
  {
  const gm_skua::SKUAParseData::Vertex& vertex = gocad->vertices[i];

  vcRow[0] = vertex.coordinate[1];
  vcRow[1] = vertex.coordinate[0];
  vcRow[2] = vertex.coordinate[2];

  for (size_t j = 0; j < size; ++j)
      vcRow[3 + j] = props->values[j][i];

  pPointSet->PushBack(vcRow);
  }

  // Add a suffix _1, _2, etc if some ESIZE > 1.
  // For instance ESIZES  3, 1, 2  tells you that there will
  // be 3+1+2=6 columns of data, accompanied by 3 data-names.
  // If the names are "PROPERTIES f g h"  then you should create
  // the unassigned properties f_1, f_2, f_3, g, h_1, h_2. 
  // If the number of property names does not match the number
  // of ESIZE entries:  abort with error message.  
  // 

  size_t current = 3;
  for (size_t i = 0; i < props->esizes.size(); ++i)
  {
  if (props->esizes[i] > 1)
  {
      for (int j = 1; j <= props->esizes[i]; ++j)
    pPointSet->ValueSet(current++).Name(QString("%1_%2").arg(props->names[i].data).arg(j));
  }
  else
      pPointSet->ValueSet(current++).Name(props->names[i].data);
  }

  return pPointSet;
}

void CGocadImport::CreateFractureIntensityAttribute(CPointSet *pPointSet) const
{
  CValueType *pFractIntens = 0;
  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  for (int i = 0; i < pPointSet->ValueSetSize(); ++i)
  {
  if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Int_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(6).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Azi")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(2).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Incl")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(1).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(0).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Azi")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(5).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Incl")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(4).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(3).LinkTo(pPointSet->ValueSet(i));
  }
  }
}

const gm_skua::SKUAParseData *CGocadImport::Solid(int i) const
{
  return m_solids[i];
}

const gm_skua::SKUAParseData *CGocadImport::Vset(int i) const
{
  return m_vsets[i];
}

const gm_skua::SKUAParseData *CGocadImport::Surf(int i) const
{
  return m_surfaces[i];
}


#else
// GocadImport.cpp: implementation of the CGocadImport class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "GocadImport.h"
#include "ElementValueSet.h"

#include "PointSet.h" // wjrx mantis 2896
#include "HexaModel.h"
#include "HexaSurface.h"
#include "TetraSurface.h"
#include "FvGocadSurface.h"

#include "ValueTypeFactory.h"

#include "IProgressBase.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "IPlatform.h"

#include <cassert>

#include "FieldFactor.h"
#include "Point.h"
#include "GocadElementSet.h"
#include "ITetrahedron.h"
#include "IMesh3D.h"
#include "IPlatform.h"

#include "StreamVersion.h"

#include "Vector.h"
#include "GlobalMessage.h"

//je
#include "GoCadModel.h"
//#include "Printer.h"
#include <fstream>

//#define SKUASCANNER
#ifdef SKUASCANNER
#include "SKUAIProgressHandler.h"
#include "SKUAImport.h"
#include <chrono>
#endif

// handy macros

#define PARSE_ERROR(msg)                                       \
  {                                                            \
  m_strError = QString("Error on line %1: %2").arg(m_nLineNr).arg(msg); \
  return false;                                              \
  }

#define VALIDATION_ERROR(msg)                                                                             \
  {                                                                                                       \
  m_strError = QString("Validation error while processing line %1: %2").arg(m_nLineNr).arg(msg); \
  return false;                                                                                         \
  }

#define ENSURE_HEADER_SEEN(gocadType)                                       \
  {                                                                         \
  if(!m_status.HeaderSeen())                                              \
  {                                                                       \
      QString strError;                                                     \
      strError = QString("'%1' keyword before HEADER definition").          \
    arg(strKeyword);                                                    \
      PARSE_ERROR(strError);                                                \
  }                                                                       \
                                      \
  if (gocadType == 0)                                                     \
  {                                                                       \
      PARSE_ERROR("Unknown GOCAD type (no VSet, TSolid, TSurf)");           \
  }                                                                       \
  }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGocadImport::CGocadImport(bool bRequireSurfaceDefinitions, bool bAllowLightTSolid)
: m_bRequireSurfaceDefinitions(bRequireSurfaceDefinitions),
  m_bAllowLightTSolid(bAllowLightTSolid),
  m_fp(0),
  m_nLineNr(0),
  m_pSolid(0),
  m_pVset(0),
  m_pTSurf(0),
  m_pVolume(0),
  m_pTetra(0),
  m_pSurface(0),
  m_pTFace(0),
  m_currentTri(0)
{
}

CGocadImport::~CGocadImport()
{
  //wjrx mantis 2896
  for(size_t i = 0; i < m_vcVsets.size(); ++i)
  delete m_vcVsets[i];

  for(size_t i = 0; i < m_vcTSurfs.size(); ++i)
  delete m_vcTSurfs[i];
}

bool CGocadImport::Import(const std::string& strFileName)
{
  assert(m_fp == 0);
  m_fp = fopen(strFileName.c_str(), "rb");
  if(!m_fp)
  return false;

  bool bParseOK = ParseMesh();

  fclose(m_fp);
  m_fp = 0;

  return bParseOK;
}

std::vector <QSharedPointer <CGocadData::CTSolid> >& CGocadImport::getSolids()
{
  return m_vcSolids;
}

int CGocadImport::SolidSize() const
{
  return m_vcSolids.size();
}

const CGocadData::CTSolid& CGocadImport::Solid(int i) const
{
  assert(i >= 0 && i < m_vcSolids.size());
  return *m_vcSolids[i];
}

int CGocadImport::VsetSize() const
{
  return m_vcVsets.size();
}

const CGocadData::CVset& CGocadImport::Vset(int i) const
{
  assert(i >= 0 && i < m_vcVsets.size());
  return *m_vcVsets[i];
}

int CGocadImport::SurfSize() const
{
  return m_vcTSurfs.size();
}

const CGocadData::CTSurf& CGocadImport::Surf(int i) const
{
  assert(i >= 0 && i < m_vcTSurfs.size());
  return *m_vcTSurfs[i];
}

std::string CGocadImport::Error() const
{
  return m_strError.toStdString();
}

bool CGocadImport::ParseMesh()
{
  assert(m_fp != 0);

  m_nLineNr = 0;
  QString strLine;

  long lStart = ftell(m_fp);
  fseek(m_fp, 0, SEEK_END);
  long lEnd = ftell(m_fp);
  fseek(m_fp, 0, SEEK_SET);

  long len = 0;

  std::auto_ptr <CProgressBase> progdlg( _g->prog()->create( eProgress::Dual, 1, "Reading Gocad format", true));
  progdlg->NextJob("Reading Gocad format");
  progdlg->AddSteps(lEnd - lStart);

#ifdef SKUASCANNER
  std::vector<size_t> inf;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(), end;

  class ProgressWrapper : public gm_skua::IProgressHandler
  {
  CProgressBase& progress;
  public:
  ProgressWrapper(IProgressBase& progress)
      : gm_skua::IProgressHandler()
      , progress(progress)
  {
  }

  virtual void Step(int number)
  {
      try
      {
    progress.Step(number);
      }
      catch (CProgressCancel *cancelEvent)
      {
    delete cancelEvent;
    throw gm_skua::IProgressCancel();
      }
  }
  };

  ProgressWrapper pw(*progdlg.get());

  gm_skua::SKUAImport importer(m_fp, pw);

  importer.Import();

  int lines = importer.LineNr();

  end = std::chrono::steady_clock::now();
  long long ms = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
  _m()->msg(QString::asprintf("Elapsed time: %ld.%03ld seconds, %d lines", ms / 1000, ms % 1000, lines));

  return false;
#else
  char buf[1024];
  while(fgets(buf, 1024, m_fp))
  {
  // append contents of buf to the current line
  strLine += buf;

  len = strlen(buf);

  // last line might not have '\n'
  //
  if((len > 0 && buf[len-1] == '\n') || feof(m_fp) )
  {
      // update line nr
      ++m_nLineNr;

      // get rid of leading and trailing whitespace characters
      strLine = strLine.trimmed();

      // parse the contents of the line
      if(!ParseLine(strLine))
      {
    return false;
      }


      // reset line
      strLine.clear();
  }

  try
  {
      progdlg->Step(len);
  }
  catch (CProgressCancel* c)
  {
      delete c;
      return false;
  }
  }

  if(m_status.HeaderSeen())
  {
  PARSE_ERROR("Unexpected end of file found (missing END statement?)");
  }

  return true;
#endif
}


void CGocadImport::GetArguments(const QString& strLine, QString& keyword, TArgumentList& lstArgs) const
{
  assert(keyword.isEmpty());
  assert(lstArgs.empty());

  if(strLine.isEmpty())
  return;

  QString s = strLine.trimmed();

  if (s.endsWith("{") && s.length() >= 2 && !s[s.length() - 2].isSpace())
  s.insert(s.length() - 1, ' ');

  int n0 = s.indexOf(' ');
  int n1 = s.indexOf('\t');
  int n = -1;

  if ((n0 != -1) || (n1 != -1))
  {
  if (n0 == -1)
  {
      n = n1;
  }
  else if (n1 == -1)
  {
      n = n0;
  }
  else
  {
      n = std::min(n0, n1);
  }
  }

  assert(n != 0); // should have been trimmed

  if(n == -1)
  keyword = s;
  else
  keyword = s.left(n);

  while(n != -1)
  {
  s = s.mid(n);

  while (!s.isEmpty() && s[0].isSpace())
  {
      s = s.mid(1);
  }

  if(!s.isEmpty())
  {
      if (s.indexOf('"') == 0)
      {
    s = s.mid(1);
    n = s.indexOf('"');

    assert(n > 0); // no way to expose missing quote/empty name errors yet

    lstArgs.push_back(s.left(n));
    ++n;
      }
      else
      {
    n0 = s.indexOf(' ');
    n1 = s.indexOf('\t');
    n = -1;

    if ((n0 != -1) || (n1 != -1))
    {
          if (n0 == -1)
          {
      n = n1;
          }
          else if (n1 == -1)
          {
      n = n0;
          }
          else
          {
      n = std::min(n0, n1);
          }
    }
    if(n == -1)
          lstArgs.push_back(s);
    else
          lstArgs.push_back(s.left(n));
      }
  }
  else
      n = -1;
  }
}

bool CGocadImport::ParseLine(const QString& strLine)
{
  // skip empty lines
  if(strLine.isEmpty())
  return true;

  // skip comment lines, unless it's part of tetra information
  if (strLine[0] == '#')
  {
  if (!m_pTetra)
      return true;

  if (!strLine.startsWith("# CTETRA"))
      return true;
  }

  QString strKeyword;
  TArgumentList lstArgs;

  GetArguments(strLine, strKeyword, lstArgs);

  return ParseArguments(strKeyword, lstArgs.begin(), lstArgs.end());
}

// wjrx mantis 2910
// Remove the number from the property name
// Frac_Intens_150_blablavla -> Frac_Intens_blablavla
//
QString CGocadImport::ParsePropertyName(const QString &propertyName)
{
  std::string s(propertyName.toStdString());
  std::string searchString= "Frac_Intens_";

  // Does property name start with search string?
  //
  if ( s.find(searchString) != 0 ) 
  return propertyName;

  // Go to the last part of the property name, past the number
  //
  size_t pos= s.find_first_not_of("0123456789_",searchString.length());

  // Combine the 2  parts;
  s= searchString + s.substr(pos);
  return QString( s.c_str() );
}

bool CGocadImport::ParseArguments(const QString& strKeyword, TArgIterator itArg, TArgIterator itArgEnd)
{
  CGocadData::IGoCadTypes *pGT = CurrentType();
  
  try
  {
  if (strKeyword == "GOCAD")
  {
      // start of object
      // get type
      if (itArg != itArgEnd)
      {
    std::string tag = (*itArg).toStdString();
    if (*itArg == "TSolid" || *itArg == "LightTSolid")
    {
          bool bLightTSolid = *itArg == "LightTSolid";

          if (!m_bAllowLightTSolid && bLightTSolid)
          {
      PARSE_ERROR("Experimental feature 'GOCAD LightTSolid' encountered outside mesh import; please import as mesh. ");
          }

          if (m_pSolid)
          {
      QString encountered = bLightTSolid ? "LightTSolid" : "TSolid";
      QString reading = m_pSolid->Type() == CGocadData::CTSolid::LightTSolid ? "LightTSolid" : "TSolid";
      QString message = QString("'GOCAD %1' encountered while reading a %2 (missing END marker?)").arg(encountered).arg(reading);
      PARSE_ERROR(message);
          }

          if (bLightTSolid)
          {
      _m()->warn("Warning: import of LightTSolids and double-sided faults is experimental.", MB_ICONEXCLAMATION);
          }

          // create a new TSolid and set a pointer to it
          m_pSolid = new CGocadData::CTSolid(bLightTSolid);
          m_vcSolids.push_back(QSharedPointer <CGocadData::CTSolid>(m_pSolid));
          pGT = m_pSolid;

    }
    else if (*itArg == "VSet") //wjrx mantis 2896
    {
          if (m_pVset)
      PARSE_ERROR("'GOCAD Vset' encountered while reading a Vset (missing END marker?)");

          // create a new Vset and set a pointer to it
          m_pVset = new CGocadData::CVset;
          m_vcVsets.push_back(m_pVset);
          pGT = m_pVset;
    }
    else if (*itArg == "TSurf")
    {
          if (m_pTSurf)
      PARSE_ERROR("'GOCAD TSurf' encountered while reading a TSurf (missing END marker?)");

          //Printer::instance()->error("--> Ini TSurf");

              // a new TSurf is started. Create a TSurf and a CSurface. The CSurface will contain the TFaces
          assert(!m_pSurface);
          // create a new TSurf and set a pointer to it
          m_pTSurf = new CGocadData::CTSurf;
          m_pSurface = QSharedPointer <CGocadData::CSurface>(new CGocadData::CSurface);
          m_pTSurf->m_pSurface = m_pSurface;
          m_vcTSurfs.push_back(m_pTSurf);
          pGT = m_pTSurf;
    }

    ++itArg;
      }
  } //end 'GOCAD' tag

  else if (strKeyword == "GEOLOGICAL_TYPE")
  {
  }
  else if (strKeyword == "HEADER")
  {
      m_status.StartHeader();
  }
  else if (strKeyword == "{")
  {
      m_status.OpenSection();
  }
  else if (strKeyword == "}")
  {
      if (pGT && m_status.ReadingHeader() && pGT->m_strName.isEmpty())
    PARSE_ERROR("No name found in header");

      m_status.CloseSection();
  }
  else if (strKeyword.left(5) == "name:")
  {
      if (pGT && m_status.ReadingHeader() && m_status.SectionOpen())
      {
    if (strKeyword.length() > 5)
    {
          pGT->m_strName = strKeyword.right(strKeyword.length() - 5);

          while (itArg != itArgEnd)
          {
      pGT->m_strName = pGT->m_strName + " " + *itArg;
      ++itArg;
          }
    }
    else if (itArg != itArgEnd)
          pGT->m_strName = *itArg++;

    if (pGT->m_strName.isEmpty())
          PARSE_ERROR("Name not found in header");

    if (m_pTSurf)
    {
          //Printer::instance()->debug("name: %s", pGT->m_strName.toStdString().c_str());
    }
      }
  }
  else if (strKeyword == "AXIS_UNIT")
  {
      ENSURE_HEADER_SEEN(pGT);

      m_status.StartAxisUnit();

      int iArg;
      for (iArg = 0; iArg < 3 && itArg != itArgEnd; ++itArg, ++iArg)
      {
    if (*itArg == "\"ft\"" || *itArg == "'ft'" || *itArg == "ft")
          pGT->m_dUnitFactor[iArg] = 1. / FF_FACTOR_LENGTH;
    else if (*itArg != "\"m\"" && *itArg != "'m'" && *itArg != "m")
    {
          QString msg;
          msg = QString("Unknown AXIS_UNIT: %1").arg(*itArg);
          PARSE_ERROR(msg);
    }
      }
  }
  else if (strKeyword == "ZPOSITIVE")
  {
      ENSURE_HEADER_SEEN(pGT);

      m_status.StartZPositive();
      int iZFactor = 0;

      // either 'Depth' or 'Elevation'
      if (itArg != itArgEnd)
      {
    if (*itArg == "Elevation" || *itArg == "Upward")
          iZFactor = -1;
    else if (*itArg == "Depth")
          iZFactor = 1;
      }

      if (!iZFactor)
    PARSE_ERROR("'Depth', 'Elevation' or 'Upward' expected after 'ZPOSITIVE'");

      {
    pGT->m_dZFactor = iZFactor;
      }
  }
  else if (strKeyword == "PROPERTIES")
  {
      ENSURE_HEADER_SEEN(pGT);

      m_status.StartProperties();

      assert(pGT->m_vcPropertyNames.empty());
      for (; itArg != itArgEnd; ++itArg)
    pGT->m_vcPropertyNames.push_back(ParsePropertyName(*itArg));

      if (pGT->m_vcPropertyNames.empty())
    PARSE_ERROR("No property names listed after 'PROPERTIES'");
  }
  else if (strKeyword == "NO_DATA_VALUES")
  {
      ENSURE_HEADER_SEEN(pGT);

      if (!pGT->m_vcNoDataValues.empty())
    PARSE_ERROR("Second 'NO_DATA_VALUES' keyword encountered");

      for (; itArg != itArgEnd; ++itArg)
      {
    double d;
    if (!StringToDouble(*itArg, d))
          PARSE_ERROR("Expected numeric (floating point) value");
    pGT->m_vcNoDataValues.push_back(d);
      }

      if (pGT->m_vcNoDataValues.empty())
    PARSE_ERROR("No values listed after 'NO_DATA_VALUES'");
  }
  else if (strKeyword == "ESIZES")
  {
      ENSURE_HEADER_SEEN(pGT);

      if (!pGT->m_vcESizes.empty())
    PARSE_ERROR("Second 'ESIZES' keyword encountered");

      for (; itArg != itArgEnd; ++itArg)
      {
    int n;
    if (!StringToInt(*itArg, n))
          PARSE_ERROR("Expected integer value");
    pGT->m_vcESizes.push_back(n);
      }

      if (pGT->m_vcESizes.empty())
    PARSE_ERROR("No values listed after 'ESIZES'");
  }
  else if (strKeyword == "TETRA_PROPERTIES")
  {
      ENSURE_HEADER_SEEN(pGT);

      m_status.StartTetraProperties();

      assert(pGT->m_vcTetraPropertyNames.empty());

      for (; itArg != itArgEnd; ++itArg)
    pGT->m_vcTetraPropertyNames.push_back(*itArg);

      if (pGT->m_vcTetraPropertyNames.empty())
    PARSE_ERROR("No property names listed after 'TETRA_PROPERTIES'");
  }
  else if (strKeyword == "TETRA_NO_DATA_VALUES")
  {
      ENSURE_HEADER_SEEN(pGT);

      if (!pGT->m_vcTetraNoDataValues.empty())
    PARSE_ERROR("Second 'TETRA_NO_DATA_VALUES' keyword encountered");

      for (; itArg != itArgEnd; ++itArg)
      {
    double d;
    if (!StringToDouble(*itArg, d))
          PARSE_ERROR("Expected numeric (floating point) value");
    pGT->m_vcTetraNoDataValues.push_back(d);
      }

      if (pGT->m_vcTetraNoDataValues.empty())
    PARSE_ERROR("No values listed after 'TETRA_NO_DATA_VALUES'");
  }
  else if (strKeyword == "TETRA_ESIZES")
  {
      ENSURE_HEADER_SEEN(pGT);

      if (!pGT->m_vcTetraESizes.empty())
    PARSE_ERROR("Second 'TETRA_ESIZES' keyword encountered");

      for (; itArg != itArgEnd; ++itArg)
      {
    int n;
    if (!StringToInt(*itArg, n))
          PARSE_ERROR("Expected integer value");
    pGT->m_vcTetraESizes.push_back(n);
      }

      if (pGT->m_vcTetraESizes.empty())
    PARSE_ERROR("No values listed after 'TETRA_ESIZES'");
  }
  else if (strKeyword == "TVOLUME")
  {
      ENSURE_HEADER_SEEN(1);

      if (!CheckVolume())
    return false;

      if (m_pSolid)
      {
    // create a new TVolume and set a pointer
    m_pVolume = new CGocadData::CTVolume;
    m_pSolid->m_vcVolumes.push_back(QSharedPointer <CGocadData::CTVolume>(m_pVolume));

    if (itArg == itArgEnd)
    {
          // name not present, make one up
          m_pVolume->m_strName = QString("Volume %1").arg(m_pSolid->m_vcVolumes.size());
    }
    else
    {
          m_pVolume->m_strName = *itArg;
          ++itArg;
    }
      }
  }
  else if (strKeyword == "VRTX")
  {
      ENSURE_HEADER_SEEN(1);

      if (m_status.PropertiesSeen())
    PARSE_ERROR("VRTX used with PROPERTIES (should be PVRTX)");

      if (!ParseVertex(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "ATOM")
  {
      ENSURE_HEADER_SEEN(1);

      if (m_status.PropertiesSeen())
    PARSE_ERROR("ATOM used with PROPERTIES (should be PATOM)");

      if (!ParseAtom(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "PVRTX")
  {
      ENSURE_HEADER_SEEN(1);

      if (!m_status.PropertiesSeen())
    PARSE_ERROR("PVRTX used without PROPERTIES");

      if (!ParseVertex(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "PATOM")
  {
      ENSURE_HEADER_SEEN(1);

      if (!m_status.PropertiesSeen())
    PARSE_ERROR("PATOM used without PROPERTIES");

      if (!ParseAtom(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "SHAREDVRTX")
  {
      ENSURE_HEADER_SEEN(1);

      if (m_status.PropertiesSeen())
    PARSE_ERROR("SHAREDVRTX used with PROPERTIES (should be SHAREDPVRTX)");

      if (!ParseSharedVertex(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "SHAREDPVRTX")
  {
      ENSURE_HEADER_SEEN(1);

      if (!m_status.PropertiesSeen())
    PARSE_ERROR("SHAREDPVRTX used without PROPERTIES");

      if (!ParseSharedVertex(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "TETRA")
  {
      ENSURE_HEADER_SEEN(1);

      if (!ParseTetra(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "#")
  {
      if (!ParseTetraComment(itArg, itArgEnd))
    return false;

      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword == "TETRA_REGION")
  {
      ENSURE_HEADER_SEEN(1);

      if (!ParseTetraRegion(itArg, itArgEnd))
    return false;

      itArg = itArgEnd;
  }
  else if (strKeyword == "TRI")
  {
      ENSURE_HEADER_SEEN(1);

      if (!ParseTri(itArg, itArgEnd))
    return false;

      itArg = itArgEnd;
  }
  else if (strKeyword == "END")
  {
      ENSURE_HEADER_SEEN(1);

      if(m_pSolid)
      {
    if(!CheckSolid())
          return false;

    m_pSolid = 0;
    m_pVolume = 0;
    m_pSurface.clear();
    m_pTFace.clear();
      }

      if( m_pTSurf )
      {
  			if(!CheckSurface())
    		  return false;
  
    m_pTSurf = 0;
    		m_pSurface.clear();
    		m_pTFace.clear();
      }

      if(m_pVset)
      {
    m_pVset = 0;
      }

      m_status = CReadStatus();
  }
  else if(strKeyword == "SURFACE")
  {
      ENSURE_HEADER_SEEN(1);

      if(!CheckSurface())
    return false;

      if(m_pSolid)
      {
    // create a new Surface and set a pointer
    m_pSurface = QSharedPointer <CGocadData::CSurface>(new CGocadData::CSurface);
    m_pSolid->m_vcSurfaces.push_back(m_pSurface);

    if(itArg == itArgEnd)
    {
          // name not present, make one up
          m_pSurface->m_strName = QString("Surface %1").arg(m_pSolid->m_vcSurfaces.size());
    }
    else
    {
          if(!DeriveSurfaceName(*itArg))
      return false;

          ++itArg;
    }
      }
  }
  else if(strKeyword == "TFACE")
  {
      ENSURE_HEADER_SEEN(1);

      if(!CheckTFace())
    return false;

      if(m_pSurface && (!m_pSolid || m_pSolid->m_tetraRegions.empty() || m_pSurface->m_vcTFaces.empty()))
      {
    
    // create a new TFace and set a pointer
    m_pTFace = QSharedPointer <CGocadData::CTFace>(new CGocadData::CTFace);
          m_pSurface->m_vcTFaces.push_back(m_pTFace);
    
    if(itArg == itArgEnd)
    {
          // name not present, make one up
          m_pTFace->m_strName = QString("TFace %1").arg(m_pSurface->m_vcTFaces.size());
    }
    else
    {
          m_pTFace->m_strName = *itArg;
          ++itArg;
    }
      }
  }
  else if(strKeyword == "TRGL")
  {
      ENSURE_HEADER_SEEN(1);

    // We want to save all the wrong points to show it to the user
    /*if( m_pTFace->m_import_failure )
    {
      // no need to parse the line as the surface won't be considered
    }
    else*/
    {
      bool res = ParseTriangle(itArg, itArgEnd);
      if( !res )
      {
        return false;
      }
    }
      // move to the end of the arguments
      itArg = itArgEnd;
  }
  else if (strKeyword.contains("colormap"))
  {
      // move to the end of the arguments
      itArg = itArgEnd;
  }

  // ignore any other keywords

  }
  catch(CReadStatusException& e)
  {
  PARSE_ERROR(e.Error());
  }

  // more to be done?
  if(itArg != itArgEnd)
  {
  TArgIterator itNext = itArg;
  if(!ParseArguments(*itArg, ++itNext, itArgEnd))
      return false;
  }

  return true;
}

void CGocadImport::FindDSFVolumes()
{
  if (m_pSolid->Type() != CGocadData::CTSolid::LightTSolid)
  return;

  for (size_t i = 0; i < m_pSolid->m_vcVolumes.size(); ++i)
  {
  const CGocadData::CTVolume *v = m_pSolid->m_vcVolumes[i].data();

  std::set<QString> surfaces;

  for (size_t j = 0; j < v->m_vcTetras.size(); ++j)
  {
      const CGocadData::CTetra *t = v->m_vcTetras[j].data();

      for (size_t k = 0; k < 4; ++k)
      {
    if (!t->m_strOppositeFace[k].isEmpty())
          surfaces.insert(t->m_strOppositeFace[k]);
      }
  }

  if (surfaces.size() == 2)
  {
      bool bDoubleSidedFaultVolume = true;

      if (!surfaces.begin()->endsWith("fault_minus") && !surfaces.begin()->endsWith("fault_plus"))
    bDoubleSidedFaultVolume = false;

      if (!surfaces.rbegin()->endsWith("fault_minus") && !surfaces.rbegin()->endsWith("fault_plus"))
    bDoubleSidedFaultVolume = false;

      const_cast<CGocadData::CTVolume *>(v)->m_bDoubleSidedFaultVolume = bDoubleSidedFaultVolume;
  }
  }
}

void CGocadImport::FindDSFFaults()
{
  if (m_pSolid->Type() != CGocadData::CTSolid::LightTSolid)
  return;

  for (size_t i = 0; i < m_pSolid->m_vcSurfaces.size(); ++i)
  {
  const CGocadData::CSurface *s = m_pSolid->m_vcSurfaces[i].data();

  if (s->m_strName.endsWith("fault_minus") || s->m_strName.endsWith("fault_plus"))
  {
      const_cast<CGocadData::CSurface *>(s)->m_bDoubleSidedFaultSurface = true;
  }
  }
}

bool CGocadImport::CheckSolid()
{
  assert(m_pSolid);

  if(m_pSolid->m_vcVolumes.empty())
  {
  QString strName(m_pSolid->m_strName.isEmpty() ? "<unnamed>" : m_pSolid->m_strName);
  QString s;
  s = QString("Solid '%1' has no TVolumes").arg(strName);
  VALIDATION_ERROR(s);
  }

  FindDSFVolumes();

  if(m_pSolid->m_vcSurfaces.empty() && m_bRequireSurfaceDefinitions)
  {
  QString strName(m_pSolid->m_strName.isEmpty() ? "<unnamed>" : m_pSolid->m_strName);
  QString s;
  s = QString("Solid '%1' has no Surfaces").arg(strName);
  VALIDATION_ERROR(s);
  }

  FindDSFFaults();

  if(!CheckSolidProperties(m_pSolid->m_vcPropertyNames, m_pSolid->m_vcESizes, m_pSolid->m_vcNoDataValues, ""))
  return false;

  if(!CheckSolidProperties(m_pSolid->m_vcTetraPropertyNames, m_pSolid->m_vcTetraESizes, m_pSolid->m_vcTetraNoDataValues, "TETRA_"))
  return false;

  if(!CheckVolume())
  return false;

  if(!CheckSurface())
  return false;

  return true;
}

bool CGocadImport::CheckSolidProperties(const std::vector<QString>& vcPropertyNames, std::vector<int>& vcESizes, const std::vector<double>& vcNoDataValues, const QString& strPrefix)
{
  // make sure number of properties, esizes and no_data_values match
  if(vcESizes.empty() && !vcPropertyNames.empty())
  vcESizes.resize(vcPropertyNames.size(), 1);

  if(vcPropertyNames.size() != vcESizes.size())
  {
  QString s;
  s = QString("The Solid has %1 %2PROPERTIES defined, and %3 %4ESIZES; these numbers should match").arg(vcPropertyNames.size()).arg(strPrefix).arg(vcESizes.size()).arg(strPrefix);
  VALIDATION_ERROR(s);
  }

  if(vcNoDataValues.size() != vcPropertyNames.size())
  {
  QString s;
  s = QString("The Solid has %1 %2PROPERTIES defined, and %3 %4NO_DATA_VALUES; these numbers should match").arg(vcPropertyNames.size()).arg(strPrefix).arg(vcNoDataValues.size()).arg(strPrefix);
  VALIDATION_ERROR(s);
  }

  return true;
}

bool CGocadImport::CheckVolume()
{
  if(m_pVolume)
  {
  if(m_pVolume->m_vcTetras.empty())
  {
      QString strName(m_pVolume->m_strName.isEmpty() ? "<unnamed>" : m_pVolume->m_strName);
      QString s;
      s = QString("Volume '%1' has no TETRAs").arg(strName);
      VALIDATION_ERROR(s);
  }
  }

  return true;
}

bool CGocadImport::CheckSurface()
{
  if(m_pSurface)
  {
  if(m_pSurface->m_vcTFaces.empty())
  {
      QString strName(m_pSurface->m_strName.isEmpty() ? "<unnamed>" : m_pSurface->m_strName);
      QString s;
      s = QString("Surface '%1' has no TFACEs").arg(strName);
      VALIDATION_ERROR(s);
  }

  if(!CheckTFace())
      return false;
  }

  return true;
}

bool CGocadImport::CheckTFace()
{
  if(m_pTFace)
  {
  if(m_pTFace->m_vcTriangles.empty())
  {
      QString strName(m_pTFace->m_strName.isEmpty() ? "<unnamed>" : m_pTFace->m_strName);
      QString s;
      s = QString("TFace '%1' has no TRGLs").arg(strName);
      VALIDATION_ERROR(s);
  }
  }

  return true;
}

bool CGocadImport::ParseVertex(TArgIterator itArg, TArgIterator itArgEnd)
{
  CGocadData::IGoCadTypes *pGT = CurrentType();
  assert(pGT);

  int nID = -1;
  double dCoord[3];
  double dCoord_orig[3];

  // get the ID
  if(itArg != itArgEnd)
  {
  if (!StringToInt(*itArg, nID))
      PARSE_ERROR("Unable to parse ID");
  if (nID < 0)
      PARSE_ERROR("Vertex ID must be at least equal to zero");
  if (pGT->NodeID(nID))
  {
      PARSE_ERROR("Vertex ID must be unique");
  }

  ++itArg;
  }

  // get the coordinates
  int i;
  for(i = 0; itArg != itArgEnd && i < 3; ++itArg, ++i)
  {
  if(!StringToDouble(*itArg, dCoord[i]))
  {
      QString strName = (i == 0 ? "X" : (i == 1 ? "Y" : "Z"));
      QString s;
      s = QString("Unable to parse VRTX %1-coordinate").arg(strName);
      PARSE_ERROR(s);
  }
  }
  memcpy( dCoord_orig, dCoord, 3*sizeof( double ) );

  if(i < 3)
  PARSE_ERROR("Not enough vertex coordinates provided");

  // apply unit factors
  dCoord[0] *= pGT->m_dUnitFactor[0];
  dCoord[1] *= pGT->m_dUnitFactor[1];
  dCoord[2] *= pGT->m_dUnitFactor[2];

  // swap X and Y
  double d = dCoord[0];
  dCoord[0] = dCoord[1];
  dCoord[1] = d;

  // apply Z factor
  dCoord[2] *= pGT->m_dZFactor;


  std::vector<int> vcESizes;
  std::vector<std::vector<double> > vcProperties;

  // get the property values
  if (!ParseProperties(itArg, itArgEnd, vcESizes, vcProperties))
  return false;

  CGocadData::CVertex *pV = new CGocadData::CVertex;
  pV->m_nID = nID;
  for (size_t j = 0; j < 3; ++j)
  pV->m_dCoord[j] = dCoord[j];
  pV->m_vcProperties.swap(vcProperties);

  pGT->m_vcESizes.swap(vcESizes);

  pGT->AddNode(pV);

  if( m_pVolume )
  m_pVolume->m_vcVertices.push_back(pV);

  return true;
}


bool CGocadImport::ParseAtom(TArgIterator itArg, TArgIterator itArgEnd)
{
  CGocadData::IGoCadTypes *pGT = CurrentType();
  assert(pGT);

  int nID = -1;

  // get the ID
  if(itArg == itArgEnd)
  PARSE_ERROR("Atom ID expected");

  if(!StringToInt(*itArg, nID))
  PARSE_ERROR("Unable to parse ID");
  if(nID < 0)
  PARSE_ERROR("Atom ID must be at least equal to zero");
  if (pGT->NodeID(nID))
  {
  PARSE_ERROR("Atom ID must be unique");
  }

  ++itArg;

  int nVertexID;

  // get the vertex ID
  if(itArg == itArgEnd)
  PARSE_ERROR("Vertex ID expected after Atom ID");

  if(!StringToInt(*itArg, nVertexID))
  PARSE_ERROR("Unable to parse vertex ID");

  const CGocadData::CVertex *pVertex = pGT->VertexID(nVertexID);

  if (!pVertex)
  {
  QString s;
  s = QString("'%1' is not a valid vertex ID").arg(nVertexID);
  PARSE_ERROR(s);
  }
  ++itArg;

  assert(pVertex != 0);

  std::vector<int> vcESizes;
  std::vector<std::vector<double> > vcProperties;

  // get the property values
  if(!ParseProperties(itArg, itArgEnd, vcESizes, vcProperties))
  return false;

  CGocadData::CAtom *pA = new CGocadData::CAtom(pVertex);
  pA->m_nID = nID;

  pA->m_vcProperties.swap(vcProperties);
  pGT->m_vcESizes.swap(vcESizes);

  pGT->AddNode(pA);

  if( m_pVolume )
  m_pVolume->m_vcAtoms.push_back(pA);

  return true;
}

bool CGocadImport::ParseSharedVertex(TArgIterator itArg, TArgIterator itArgEnd)
{
  CGocadData::IGoCadTypes *pGT = CurrentType();
  assert(pGT);

  int nID = -1;

  // get the ID
  if (itArg == itArgEnd)
  PARSE_ERROR("Shared Vertex ID expected");

  if (!StringToInt(*itArg, nID))
  PARSE_ERROR("Unable to parse ID");
  if (nID < 0)
  PARSE_ERROR("Shared Vertex ID must be at least equal to zero");
  if (pGT->NodeID(nID))
  {
  PARSE_ERROR("Shared Vertex ID must be unique");
  }

  ++itArg;

  int nVertexID;

  // get the vertex ID
  if (itArg == itArgEnd)
  PARSE_ERROR("Vertex ID expected after Shared Vertex ID");

  if (!StringToInt(*itArg, nVertexID))
  PARSE_ERROR("Unable to parse vertex ID");

  const CGocadData::CVertex *pVertex = pGT->VertexID(nVertexID);

  if (!pVertex)
  {
  QString s;
  s = QString("'%1' is not a valid vertex ID").arg(nVertexID);
  PARSE_ERROR(s);
  }
  ++itArg;

  assert(pVertex != 0);

  std::vector<int> vcESizes;
  std::vector<std::vector<double> > vcProperties;

  // get the property values
  if (!ParseProperties(itArg, itArgEnd, vcESizes, vcProperties))
  return false;

  CGocadData::CSharedVertex *pS = new CGocadData::CSharedVertex(pVertex);
  pS->m_nID = nID;

  pS->m_vcProperties.swap(vcProperties);
  pGT->m_vcESizes.swap(vcESizes);

  pGT->AddNode(pS);

#if 0
  // not sure yet if LightTSolid supports volumes or how we will handle that
  if (m_pVolume)
  {
  m_pVolume->m_vcSharedVertices.push_back(pS);
  }
#endif

  return true;
}


bool CGocadImport::ParseTetra(TArgIterator itArg, TArgIterator itArgEnd)
{
  bool bLightTSolid = m_pSolid && m_pSolid->Type() == CGocadData::CTSolid::LightTSolid;
  if(m_pVolume || bLightTSolid)
  {
  assert(m_pSolid);

  // get the node IDs
  const CGocadData::CNode *pNodes[4];

  int i;
  for(i = 0; itArg != itArgEnd && i < 4; ++itArg, ++i)
  {
      int nNode;
      if(!StringToInt(*itArg, nNode))
      {
    QString s;
    s = QString("Unable to parse node ID %1 of 4").arg(i + 1);
    PARSE_ERROR(s);
      }

      const CGocadData::CNode *pNode = m_pSolid->NodeID(nNode);

      if(pNode == 0)
      {
    QString s;
    s = QString("'%1' is not a valid vertex or atom ID").arg(nNode);
    PARSE_ERROR(s);
      }

      pNodes[i] = pNode;
  }

  if(i < 4)
      PARSE_ERROR("Not enough TETRA node IDs provided");

  	CGocadData::CTetra *t = new CGocadData::CTetra;

  // get the property values
    std::vector<std::vector<double> > vcProperties;
    if(!ParseProperties(itArg, itArgEnd, m_pSolid->m_vcTetraESizes, vcProperties))
      return false;
    t->m_vcProperties.swap( vcProperties );

  for (i = 0; i < 4; ++i)
      t->m_Nodes[i] = pNodes[i];

  if (bLightTSolid)
      m_pTetra = t;
  else
      m_pVolume->m_vcTetras.push_back(QSharedPointer<CGocadData::CTetra>(t));
  }

  return true;
}

bool CGocadImport::ParseTetraComment(TArgIterator itArg, TArgIterator itArgEnd)
{
  assert(m_pTetra);
  assert(m_pSolid);

  ++itArg;

  QString name = *itArg++;

  if (m_pVolume && m_pVolume->m_strName != name)
  m_pVolume = 0;

  for (size_t i = 0; i < m_pSolid->m_vcVolumes.size(); ++i)
  {
  if (m_pSolid->m_vcVolumes[i]->m_strName == name)
  {
      m_pVolume = m_pSolid->m_vcVolumes[i].data();
      break;
  }
  }

  if (!m_pVolume)
  {
  m_pVolume = new CGocadData::CTVolume;
  m_pVolume->m_strName = name;
  m_pSolid->m_vcVolumes.push_back(QSharedPointer<CGocadData::CTVolume>(m_pVolume));
  }

  if (!m_pSolid->m_tetraRegions.empty())
  {
  m_tetraVolumes.push_back(m_pVolume);
  }

  // handle surface info first, as we may need to change the node types
  for (size_t i = 0; i < 4; ++i)
  {
  QString surf = *itArg++;
  if (QString::compare(surf, "none", Qt::CaseInsensitive))
  {
      assert(surf[0] == '-' || surf[0] == '+');

      bool bQuoted = surf[1] == '"';

      if (bQuoted)
      {
    surf.remove(1, 1);
    while (itArg != itArgEnd && *surf.rbegin() != '"')
          surf += " " + *itArg++;
    surf.remove(surf.length() - 1, 1);
      }
      else
      {
    while (itArg != itArgEnd && QString::compare(*itArg, "none", Qt::CaseInsensitive) && (*itArg)[0] != '+' && (*itArg)[0] != '-')
          surf += " " + *itArg++;
      }
      if (surf[1] == '(') // check for names like "+(boundary)name"
      {
    int len = surf.indexOf(')', 2);
    if (len > 0)
    {
          if (surf.mid(1, len) == "(boundary)")
          {
      // handle single-sided faults; not sure yet if we need to handle double-sided
      // we replace the shared vertex with a new vertex, so that we can have a proper fault with different faces on each side
      if (surf.endsWith("_fault") && m_pTetra->m_Nodes[i]->Type() == CGocadData::IS_SHAREDVERTEX)
      {
              const CGocadData::CSharedVertex *pSharedVertex = static_cast<const CGocadData::CSharedVertex *>(m_pTetra->m_Nodes[i]);
              
              CGocadData::CVertex *pVertex = new CGocadData::CVertex;

              pVertex->m_nID = pSharedVertex->m_nID;
              for (size_t n = 0; n < 3; ++n)
        pVertex->m_dCoord[n] = pSharedVertex->m_pVertex->m_dCoord[n];
              pVertex->m_vcProperties = pSharedVertex->m_pVertex->m_vcProperties;

              m_pSolid->AddNode(pVertex);
              m_pTetra->m_Nodes[i] = pVertex;
      }
          }

          surf.remove(1, len);
    }
      }

      m_pTetra->m_strOppositeFace[i] = surf;
  }
  }

  for (size_t i = 0; i < 4; ++i)
  if (m_pTetra->m_Nodes[i]->Type() == CGocadData::IS_VERTEX)
      m_pVolume->m_vcVertices.push_back(static_cast<const CGocadData::CVertex *>(m_pTetra->m_Nodes[i]));


  m_pVolume->m_vcTetras.push_back(QSharedPointer<CGocadData::CTetra>(m_pTetra));

  m_pTetra = 0;

  return true;
}


bool CGocadImport::ParseTriangle(TArgIterator itArg, TArgIterator itArgEnd)
{
  if(m_pTFace)
  {
    assert(!m_pVset && m_pSurface);
  
    CGocadData::IGoCadTypes *pGT = CurrentType();
    assert(pGT);

    QSharedPointer <CGocadData::CTriangle> t(new CGocadData::CTriangle());

    // get the node IDs
    int i;
    for( i = 0; itArg != itArgEnd && i < 3; ++itArg, ++i)
    {
      int nNode_origin;
      int nNode;
      if(!StringToInt(*itArg, nNode_origin))
      {
        QString s;
        s = QString("Unable to parse node ID %1 of 4").arg(i + 1);
        PARSE_ERROR(s);
      }

      const CGocadData::CNode* pNode = 0;
      nNode = nNode_origin;
  		pNode = pGT->NodeID( nNode );

      if( pNode == 0 )
      {
        QString s;
        s = QString("'%1' is not a valid vertex or atom ID").arg(nNode);
        PARSE_ERROR(s);
      }

      t->m_Nodes[i] = pNode;
    }

    if(i < 3)
      PARSE_ERROR("Not enough TRGL node IDs provided");

    m_pTFace->m_vcTriangles.push_back( t );
  }

  return true;
}

bool CGocadImport::ParseProperties(TArgIterator itArg, TArgIterator itArgEnd, const std::vector<int>& vcESizes, std::vector<std::vector<double> >& vcProperties)
{
  int nProperties = 0;
  int nFound = 0;

  for(size_t i = 0; i < vcESizes.size(); ++i)
  nProperties += vcESizes[i];

  for(size_t i = 0; i < vcESizes.size(); ++i)
  {
  std::vector<double> vcProp;
  int j;
  for(j = 0; j < vcESizes[i]; ++j)
  {
      double d;
      if(itArg == itArgEnd)
      {
    QString s;
    s = QString("Not enough property values provided (should be %1, found %2)").arg(nProperties).arg(nFound);
    PARSE_ERROR(s);
      }

      if(!StringToDouble(*itArg, d))
      {
    QString s;
    s = QString("Unable to parse property value %1 (%2)").arg(nFound + 1).arg(*itArg);
    PARSE_ERROR(s);
      }

      ++itArg;
      ++nFound;

      vcProp.push_back(d);
  }

  vcProperties.push_back(vcProp);
  }

  return true;
}

bool CGocadImport::ParseTetraRegion(TArgIterator itArg, TArgIterator itArgEnd)
{
  if (m_pSolid && itArg != itArgEnd)
  {
  m_pSolid->m_tetraRegions.push_back(*itArg);
  return true;
  }

  return false;
}

bool CGocadImport::ParseTri(TArgIterator itArg, TArgIterator itArgEnd)
{
  int i = 0;
  while (itArg != itArgEnd && *itArg == "0")
  {
  ++i;
  ++itArg;
  }

  if (m_pSolid && i < (int)m_pSolid->m_tetraRegions.size())
  {
  CGocadData::CTVolume *volume = m_tetraVolumes[m_currentTri++];

  m_pSolid->m_tetraRegionMap[volume] = i;
  return true;
  }

  return false;
}


bool CGocadImport::StringToInt(const QString& strVal, int& iVal) const
{
  QString qstr(strVal);
  bool bRet;

  iVal = qstr.toInt(&bRet);

  return bRet;
}

bool CGocadImport::StringToDouble(const QString& strVal, double& dVal) const
{
  QString qstr(strVal);
  bool bRet;

  dVal = qstr.toDouble(&bRet);

  return bRet;
}

bool CGocadImport::DeriveSurfaceName(const QString& strVal)
{
  if(m_pSurface)
  {
  if(strVal.isEmpty())
      PARSE_ERROR("No valid name provided for SURFACE");

  int idx = strVal.lastIndexOf('/');

  if(idx == strVal.length() - 1) // trailing /
      PARSE_ERROR("No valid name provided for SURFACE");

  if(idx >= 0)
      m_pSurface->m_strName = strVal.mid(idx + 1);
  else
      m_pSurface->m_strName = strVal;
  }

  return true;
}

CGocadData::IGoCadTypes *CGocadImport::CurrentType() const
{
  if( m_pSolid )
    return m_pSolid;
  else
  {
  if( m_pVset )
      return m_pVset;
  else
    return m_pTSurf;
  }
}


///// CReadStatusException

CGocadImport::CReadStatusException::CReadStatusException(const QString& strError)
: m_strError(strError)
{
}

CGocadImport::CReadStatusException::CReadStatusException(const CReadStatusException& rhs)
: m_strError(rhs.m_strError)
{
}

const QString& CGocadImport::CReadStatusException::Error() const
{
  return m_strError;
}


///// CReadStatus
CGocadImport::CReadStatus::CReadStatus()
: m_bSectionOpen(false),
  m_bReadingHeader(false),
  m_bHeaderSeen(false),
  m_bZPositiveSeen(false),
  m_bAxisUnitSeen(false),
  m_bPropertiesSeen(false),
  m_bTetraPropertiesSeen(false)
{
}

CGocadImport::CReadStatus::CReadStatus(const CReadStatus& rhs)
: m_bSectionOpen(rhs.m_bSectionOpen)
, m_bReadingHeader(rhs.m_bReadingHeader)
, m_bHeaderSeen(rhs.m_bHeaderSeen)
, m_bZPositiveSeen(rhs.m_bZPositiveSeen)
, m_bAxisUnitSeen(rhs.m_bAxisUnitSeen)
, m_bPropertiesSeen(rhs.m_bPropertiesSeen)
, m_bTetraPropertiesSeen(rhs.m_bTetraPropertiesSeen)
{
}

CGocadImport::CReadStatus& CGocadImport::CReadStatus::operator = (CReadStatus rhs)
{
  rhs.swap(*this);

  return *this;
}

bool CGocadImport::CReadStatus::SectionOpen() const
{
  return m_bSectionOpen;
}

bool CGocadImport::CReadStatus::ReadingHeader() const
{
  return m_bReadingHeader;
}

bool CGocadImport::CReadStatus::HeaderSeen() const
{
  return m_bHeaderSeen;
}

bool CGocadImport::CReadStatus::ZPositiveSeen() const
{
  return m_bZPositiveSeen;
}

bool CGocadImport::CReadStatus::AxisUnitSeen() const
{
  return m_bAxisUnitSeen;
}

bool CGocadImport::CReadStatus::PropertiesSeen() const
{
  return m_bPropertiesSeen;
}

bool CGocadImport::CReadStatus::TetraPropertiesSeen() const
{
  return m_bTetraPropertiesSeen;
}

void CGocadImport::CReadStatus::OpenSection()
{
  m_bSectionOpen = true;
}

void CGocadImport::CReadStatus::CloseSection()
{
  if(!m_bSectionOpen)
  throw CReadStatusException("'}' without corresponding '{'");

  m_bSectionOpen = false;
  if(m_bReadingHeader)
  {
  assert(!m_bHeaderSeen);
  m_bReadingHeader = false;
  m_bHeaderSeen = true;
  }
}

void CGocadImport::CReadStatus::StartHeader()
{
  if(m_bReadingHeader)
  throw CReadStatusException("'HEADER' keyword inside HEADER definition");

  if(m_bHeaderSeen)
  throw CReadStatusException("Second 'HEADER' keyword encountered");

  m_bReadingHeader = true;
}

void CGocadImport::CReadStatus::StartZPositive()
{
  if(m_bZPositiveSeen)
  throw CReadStatusException("Second 'ZPOSITIVE' keyword encountered");

  m_bZPositiveSeen = true;
}

void CGocadImport::CReadStatus::StartAxisUnit()
{
  if(m_bAxisUnitSeen)
  throw CReadStatusException("Second 'AXIS_UNIT' keyword encountered");

  m_bAxisUnitSeen = true;
}

void CGocadImport::CReadStatus::StartProperties()
{
  if(m_bPropertiesSeen)
  throw CReadStatusException("Second 'PROPERTIES' keyword encountered");

  m_bPropertiesSeen = true;
}

void CGocadImport::CReadStatus::StartTetraProperties()
{
  if(m_bTetraPropertiesSeen)
  throw CReadStatusException("Second 'TETRA_PROPERTIES' keyword encountered");

  m_bTetraPropertiesSeen = true;
}

// private

void CGocadImport::CReadStatus::swap(CReadStatus& rhs)
{
  std::swap(m_bSectionOpen, rhs.m_bSectionOpen);
  std::swap(m_bReadingHeader, rhs.m_bReadingHeader);
  std::swap(m_bHeaderSeen, rhs.m_bHeaderSeen);
  std::swap(m_bZPositiveSeen, rhs.m_bZPositiveSeen);
  std::swap(m_bAxisUnitSeen, rhs.m_bAxisUnitSeen);
  std::swap(m_bPropertiesSeen, rhs.m_bPropertiesSeen);
  std::swap(m_bTetraPropertiesSeen, rhs.m_bTetraPropertiesSeen);
}

#if 0
geo::CPoint Node2Point(const CGocadData::CNode & node)
{
  return geo::CPoint(node.X(), node.Y(), node.Z());
}
#endif

CElementSet* CGocadImport::CreateElementSet(CFemAppModel& model, int solidIndex) const
{
  const CGocadData::CTSolid& solid = Solid(solidIndex);

  CElementSet::TPointVec vcPoints;
  CElementSet::TElementVec vcElements;
  std::vector<const CGocadData::CTetra*> vcTetras;
  std::vector<std::vector<std::vector<std::vector<double> > > > vcElementNodeProperties; // nr of elements * nr of properties * ESIZE * nr of nodes

  int nTetraProperties = -1;
  int nNodalProperties = -1;

  typedef std::map<geo::CPoint, size_t> TNodeMap;
  TNodeMap mpNodes; // CPoint -> index in vcPoints

  int iVolume;
  for (iVolume = 0; iVolume < solid.VolumeSize(); ++iVolume)
  {
  const CGocadData::CTVolume& volume = solid.Volume(iVolume);
  CElementSet::TElement vcElement(4);
  int iTetra;
  for (iTetra = 0; iTetra < volume.TetraSize(); ++iTetra)
  {
      const CGocadData::CTetra& tetra = volume.Tetra(iTetra);
      int nProps = tetra.PropertySize();
      std::vector<std::vector<std::vector<double> > > vcNodalValues; // number of properties * ESIZE * number of nodes

      if (nTetraProperties == -1)
    nTetraProperties = nProps;
      else
    assert(nProps == nTetraProperties);

      int iNode;
      for (iNode = 0; iNode < 4; ++iNode)
      {
    const CGocadData::CNode& node = tetra.Node(iNode);

    if (nNodalProperties == -1)
          nNodalProperties = node.PropertySize();
    else
          assert(nNodalProperties == node.PropertySize());

    geo::CPoint pt = Node2Point(node);

    TNodeMap::iterator it = mpNodes.find(pt);
    if (it == mpNodes.end())
    {
          // new node
          vcElement[iNode] = vcPoints.size();
          mpNodes.insert(TNodeMap::value_type(pt, vcPoints.size()));
          vcPoints.push_back(pt);
    }
    else
    {
          // existing node
          vcElement[iNode] = it->second;
    }
      }

      int iProp;
      for (iProp = 0; iProp < nNodalProperties; ++iProp)
      {
    std::vector<std::vector<double> > vcValues; // ESIZE * nr of nodes

                          // The CElementSet constructor checks the orientation of each tetra, and swaps nodes 3 and 4
                          // if the volume is negative.
                          // Make sure the values follow suit.
    bool bOrientOk = geo::ITetrahedron::CheckOrientation(Node2Point(tetra.Node(0)),
          Node2Point(tetra.Node(1)),
          Node2Point(tetra.Node(2)),
          Node2Point(tetra.Node(3)));

    int iESize;
    for (iESize = 0; iESize < solid.ESize(iProp); ++iESize)
    {
          std::vector<double> vcElementValue(4);
          if (bOrientOk)
          {
      for (iNode = 0; iNode < 4; ++iNode)
      {
              const CGocadData::CNode& node = tetra.Node(iNode);
              vcElementValue[iNode] = node.Property(iProp)[iESize];
      }
          }
          else
          {
      vcElementValue[0] = tetra.Node(0).Property(iProp)[iESize];
      vcElementValue[1] = tetra.Node(1).Property(iProp)[iESize];
      vcElementValue[2] = tetra.Node(3).Property(iProp)[iESize];
      vcElementValue[3] = tetra.Node(2).Property(iProp)[iESize];
          }

          vcValues.push_back(vcElementValue);
    }

    vcNodalValues.push_back(vcValues);
      }

      vcElementNodeProperties.push_back(vcNodalValues);
      vcElements.push_back(vcElement);
      vcTetras.push_back(&tetra);
  }
  }

  CGocadElementSet* pElementSet = new CGocadElementSet(solid.Name(),
  CElementSet::TETRA, vcPoints, vcElements, model, const_cast<CGocadImport *>(this)->getSolids());

  int i;

  // add the tetrahedron properties (single values per element)
  for (i = 0; i < nTetraProperties; ++i)
  {
  int j;
  bool bAppend = (solid.TetraESize(i) > 1);
  for (j = 0; j < solid.TetraESize(i); ++j)
  {
      // create a new element value set
      CElementValueSet& vset = pElementSet->ElementValueSet(pElementSet->AddElementValueSet());
      QString strName = solid.TetraPropertyName(i);
      if (bAppend)
      {
    QString strAppend;
    strAppend = QString("(%1)").arg(j + 1);
    strName += strAppend;
      }
      vset.Name(strName);

      // set the values
      for (size_t e = 0; e < vcTetras.size(); ++e)
      {
    const std::vector<double>& vcProps = vcTetras[e]->Property(i);

    // add each ESIZE entry
    vset.PushBack(vcProps[j]);
      }
  }
  }

  // add the nodal properties (single values per element node)
  for (i = 0; i < nNodalProperties; ++i)
  {
  int j;
  bool bAppend = (solid.ESize(i) > 1);
  for (j = 0; j < solid.ESize(i); ++j)
  {
      // create a new element value set
      CElementValueSet& vset = pElementSet->ElementValueSet(pElementSet->AddElementValueSet());
      QString strName = solid.PropertyName(i);
      if (bAppend)
      {
    QString strAppend;
    strAppend = QString("(%1)").arg(j + 1);
    strName += strAppend;
      }
      vset.Name(strName);

      // loop over the elements
      for (size_t iElement = 0; iElement < vcElementNodeProperties.size(); ++iElement)
      {
    // get the vector of properties * ESIZE * values
    const std::vector<std::vector<std::vector<double> > >& vcProps = vcElementNodeProperties[iElement];

    // each vector of properties should be nNodalProperties long (same number of values for each VRTX/ATOM)
    assert(vcProps.size() == nNodalProperties);

    // get the value vector and set it in the value set
    const std::vector<std::vector<double> >& vcPropValues = vcProps[i];

    // add each ESIZE entry
    assert(vcPropValues[j].size() == 4);
    vset.PushBack(vcPropValues[j]);
      }
  }
  }

  return pElementSet;
}

CPointSet *CGocadImport::CreatePointSet(CFemAppModel& model, int vsetIndex) const
{
  CPointSet *pPointSet = CreatePointSet(model, Vset(vsetIndex));

  CreateFractureIntensityAttribute(pPointSet);

  return pPointSet;
}

void CGocadImport::CreateSurface(CFemAppModel &model, int surfIndex) const
{
  const CGocadData::CTSurf& surf = Surf(surfIndex);

  geo::CMesh3D mesh;
  // tnmx
  // This surface is only used to construct a CHexaSurface or a CTetraSurface later on.
  // I guess this can be any ISurface, still this was the quickest option for me.
  CGoCadSurface gcSurf(mesh);
  gcSurf.Header("header"); // just set a header, else it wil crash later on

  for (int i = 0; i < surf.m_pSurface->TFaceSize(); ++i)
  {
  const CGocadData::CTFace &face = surf.m_pSurface->TFace(i);
  for (size_t j = 0; j < face.TriangleSize(); ++j)
  {
      int n0, n1, n2;
      const CGocadData::CTriangle &triangle = face.Triangle(j);

      n0 = triangle.Node(0).ID();
      gcSurf.InsertPoint(n0, triangle.Node(0).X(), triangle.Node(0).Y(), triangle.Node(0).Z());

      n1 = triangle.Node(1).ID();
      gcSurf.InsertPoint(n1, triangle.Node(1).X(), triangle.Node(1).Y(), triangle.Node(1).Z());

      n2 = triangle.Node(2).ID();
      gcSurf.InsertPoint(n2, triangle.Node(2).X(), triangle.Node(2).Y(), triangle.Node(2).Z());

      if (!triangle.Node(0).IsEqualTo(triangle.Node(1)) && !triangle.Node(1).IsEqualTo(triangle.Node(2)) && !triangle.Node(2).IsEqualTo(triangle.Node(0)))
    gcSurf.InsertTriangle(n0, n1, n2);
      else
      {
    IPlatform* platform = IPlatform::instance();
    QString traceString = QString("Trapped collapsed triangle (%1, %2, %3)\n").arg(n0).arg(n1).arg(n2);

    platform->trace(traceString);
      }
  }
  }

  if (dynamic_cast<CHexaModel*>(&model))
  new CHexaSurface(surf.Name(), gcSurf, model);
  else
  new CTetraSurface(surf.Name(), gcSurf, model);

  CreatePointSet(model, surf);
}

CPointSet* CGocadImport::CreatePointSet(CFemAppModel& model, const CGocadData::IGoCadTypes& gocad) const
{
  // determine size: Properties * ESIZE
  int iSize = 0;
  for (size_t jj = 0; (gocad.m_vcNodes.size() > 0) && (jj< gocad.m_vcNodes[0]->m_vcProperties.size()); ++jj)
  iSize += gocad.m_vcNodes[0]->m_vcProperties[jj].size();

  CPointSet* pPointSet = new CPointSet(gocad.Name(), model, iSize, CPointSet::DIM_3D);

  for (size_t ii = 0; ii< gocad.m_vcNodes.size(); ++ii)
  {
  std::vector<geo::CValue> vcRow;
  vcRow.push_back(gocad.m_vcNodes[ii]->X());
  vcRow.push_back(gocad.m_vcNodes[ii]->Y());
  vcRow.push_back(gocad.m_vcNodes[ii]->Z());
  for (size_t jj = 0; jj< gocad.m_vcNodes[ii]->m_vcProperties.size(); ++jj)
      for (size_t kk = 0; kk< gocad.m_vcNodes[ii]->m_vcProperties[jj].size(); ++kk) //ESIZE
      {
    vcRow.push_back(gocad.m_vcNodes[ii]->m_vcProperties[jj][kk]);
      }
  pPointSet->PushBack(vcRow);
  }

  // Add a suffix _1, _2, etc if some ESIZE > 1.
  // For instance ESIZES  3, 1, 2  tells you that there will
  // be 3+1+2=6 columns of data, accompanied by 3 data-names.
  // If the names are "PROPERTIES f g h"  then you should create
  // the unassigned properties f_1, f_2, f_3, g, h_1, h_2. 
  // If the number of property names does not match the number
  // of ESIZE entries:  abort with error message.  
  // 
  int iCurrent = 3;
  for (size_t jj = 0; (gocad.m_vcNodes.size() > 0) && (jj< gocad.m_vcNodes[0]->m_vcProperties.size()); ++jj)
  {
  if (gocad.m_vcNodes[0]->m_vcProperties[jj].size() > 1)
  {
      for (size_t kk = 0; kk< gocad.m_vcNodes[0]->m_vcProperties[jj].size(); ++kk)
      {
    QString s;
    s = QString("%1_%2").arg(gocad.m_vcPropertyNames[jj]).arg(kk + 1);

    pPointSet->ValueSet(iCurrent++).Name(s);
      }
  }
  else
  {
      pPointSet->ValueSet(iCurrent++).Name(gocad.m_vcPropertyNames[jj]);
  }
  }

  return pPointSet;
}

void CGocadImport::CreateFractureIntensityAttribute(CPointSet *pPointSet) const
{
  CValueType *pFractIntens = 0;
  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  for (int i = 0; i < pPointSet->ValueSetSize(); ++i)
  {
  if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Int_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(6).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Azi")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(2).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Incl")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(1).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Max_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(0).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Azi")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(5).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Incl")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(4).LinkTo(pPointSet->ValueSet(i));
  }
  else if (pPointSet->ValueSet(i).Name() == "Frac_Intens_Min_Val")
  {
      if (!pFractIntens)
    pFractIntens = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_FRACT_TENSOR, IDS_VALUENAME_FRACT_TENSOR);
      pFractIntens->Component(3).LinkTo(pPointSet->ValueSet(i));
  }
  }
}

#endif
