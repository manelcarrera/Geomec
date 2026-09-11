#include "SKUAParser.h"

#include "SKUAIProgressHandler.h"
#include "SKUAParseData.h"
#include "SKUACoordinatePostProcessor.h"
#include "SKUASolidPostProcessor.h"

#include <sstream>


namespace gm_skua
{

namespace internal
{

enum TLexState : int
{
  LEX_STATE_NEW = 0,
  LEX_STATE_PM,
  LEX_STATE_NOINT,
  LEX_STATE_INTEGER,
  LEX_STATE_DOUBLE,
  LEX_STATE_IDENTIFIER,
  LEX_STATE_IDENTIFIER_PLUS_NUMBER,
  LEX_STATE_QUOTED_IDENTIFIER_START,
  LEX_STATE_QUOTED_IDENTIFIER_CONTINUE,
  LEX_STATE_PARENTHESIS_IDENTIFIER_START,
  LEX_STATE_PARENTHESIS_IDENTIFIER_CONTINUE,
  LEX_STATE_IDENTIFIER_TO_EOL_START,
  LEX_STATE_IDENTIFIER_TO_EOL_CONTINUE,
  LEX_STATE_OTHER,
  LEX_STATE_TOKEN,
  LEX_STATE_DONE,
  LEX_STATE_ERROR,
};

enum TToken : int
{
  TOK_EOF = 0,
  TOK_UNKNOWN,
  TOK_CHAR,
  TOK_INTEGER,
  TOK_DOUBLE,
  TOK_IDENTIFIER,
  TOK_GOCAD,
  TOK_OBJECT,
  TOK_HEADER,
  TOK_ATTRIB,
  TOK_ATTRIB_VALUE,
  TOK_NONE_VALUE,
  TOK_OCS_OPEN,
  TOK_OCS_CLOSE,
  TOK_NAME,
  TOK_PROJECTION,
  TOK_DATUM,
  TOK_AXIS_NAME,
  TOK_AXIS_UNIT,
  TOK_ZPOSITIVE,
  TOK_PROPERTIES,
  TOK_LEGAL_RANGES,
  TOK_NO_DATA_VALUES,
  TOK_PROP_CLASSES,
  TOK_PROP_KINDS,
  TOK_PROP_SUBCLASSES,
  TOK_PROP_ESIZES,
  TOK_UNITS,
  TOK_PROP_HEADER,
  TOK_TETRA_REGION,
  TOK_TVOLUME,
  TOK_VRTX,
  TOK_SHAREDVRTX,
  TOK_TETRA,
  TOK_CTETRA,
  TOK_GEO_TYPE,
  TOK_GEO_TYPE_VALUE,
  TOK_TRI_OPEN,
  TOK_TRI_CLOSE,
  TOK_TRI,
  TOK_MODEL,
  TOK_SURFACE,
  TOK_TFACE,
  TOK_KEYVERTICES,
  TOK_TRGL,
  TOK_MODEL_REGION,
  TOK_END
};

enum TTokObject : int
{
  TOK_OBJECT_LIGTHTSOLID = 0,
  TOK_OBJECT_TSOLID,
  TOK_OBJECT_TSURF,
  TOK_OBJECT_VSET
};

enum TTokAttrib : int
{
  TOK_ATTR_NAME = 0,
  TOK_ATTR_UNIT,
  TOK_ATTR_LOW_CLIP,
  TOK_ATTR_HIGH_CLIP,
  TOK_ATTR_QUANTITY
};

enum TTokAttribValue : int
{
  TOK_AV_X = 0,
  TOK_AV_Y,
  TOK_AV_Z,
  TOK_AV_U,
  TOK_AV_V,
  TOK_AV_W,
  TOK_AV_FLOAT,
  TOK_AV_DEPTH,
  TOK_AV_ELEVATION,
  TOK_AV_M,
  TOK_AV_FT
};

enum TTokNoneValue : int
{
  TOK_NV_NONE = 0,
  TOK_NV_DEFAULT,
  TOK_NV_UNKNOWN_U,
  TOK_NV_UNKNOWN_L,
  TOK_NV_NONE_STAR,
  TOK_NV_UNITLESS
};

enum TTokPropType : int
{
  TOK_PT_NODE = 0,
  TOK_PT_TETRA 
};

enum TTokVertexType : int
{
  TOK_VT_VRTX = 0,
  TOK_VT_PVRTX = 1,
  TOK_VT_ATOM = 2
};

enum TTokGeoTypeValue : int
{
  TOK_GTV_DEFAULT = 0,
  TOK_GTV_INTRUSIVE,
  TOK_GTV_UNCONFORMITY,
  TOK_GTV_BOUNDARY,
  TOK_GTV_TOP,
  TOK_GTV_BOTTOM,
  TOK_GTV_FAULT
};

__inline int is_node(int value)
{
  return (value & 1) == 0;
}
__inline int is_tetra(int value)
{
  return (value & 1) == 1;
}
__inline int is_vrtx(int value)
{
  return (value & 1) == 0;
}
__inline int is_pvrtx(int value)
{
  return (value & 1) == 1;
}
__inline int is_sharedvrtx(int value)
{
  return (value & 2) == 0;
}
__inline int is_atom(int value)
{
  return (value & 2) == 2;
}



Symbol::Symbol()
  : token(TOK_IDENTIFIER)
  , attribute(0)
{
}

Symbol::Symbol(int token, int attribute)
  : token(token)
  , attribute(attribute)
{
}

Symbol::Symbol(int token, int attribute, char *token_data)
  : token(token)
  , attribute(attribute)
{
  size_t len = strlen(token_data) + 1;
  if (len >= MAX_IDENTIFIER_SIZE)
    len = MAX_IDENTIFIER_SIZE;
  std::memcpy( data, token_data, len);
}

Symbol::Symbol(const Symbol& rhs)
  : token(rhs.token)
  , attribute(rhs.attribute)
{
  std::memcpy( data, rhs.data, strlen(rhs.data) + 1);
}

SKUAParseData::String Symbol2String(const Symbol & symbol)
{
  return std::move(SKUAParseData::String(symbol.attribute, symbol.data)); \
}

SymbolTable::SymbolTable()
  : m_symbolStorage(DEFAULT_SYMBOLTABLE_SIZE)
  , m_identifiers(0)
  , m_size(0)
  , m_have_reserve(false)
{
  m_symbolLookup.reserve(DEFAULT_SYMBOLTABLE_SIZE);
}

SymbolTable::~SymbolTable()
{
}

void SymbolTable::resize()
{
  m_symbolLookup.clear();

  size_t old_size = m_symbolStorage.size();
  m_symbolStorage.resize(old_size + DEFAULT_SYMBOLTABLE_SIZE);

  for (m_size = 1; m_size <= old_size; ++m_size)
    insert();
}

void SymbolTable::start_identifiers_at(int size)
{
  m_identifiers = size;
}

int SymbolTable::insert(const Symbol& symbol)
{
  m_symbolStorage[m_size++] = symbol;
  return insert();
}

int SymbolTable::insert()
{
  std::pair<SymbolLookup::iterator, bool> retval = m_symbolLookup.insert(SymbolLookup::value_type((char *)m_symbolStorage[m_size - 1].data, m_size - 1));
  if (retval.second)
    m_have_reserve = false;

  return retval.first->second;
}

int SymbolTable::reserve(int token)
{
  if (!m_have_reserve)
  {
    if (m_size == m_symbolStorage.size())
      resize();
    m_symbolStorage[m_size].attribute = m_size - m_identifiers;
    ++m_size;
    m_have_reserve = true;
  }
  return m_size - 1;
}

void SymbolTable::reserve(bool toggle)
{
  if (toggle != m_have_reserve)
  {
    if (toggle)
      reserve(TOK_IDENTIFIER);
    else
    {
      m_have_reserve = false;
      --m_size;
    }
  }
}

bool SymbolTable::have_reserve() const
{
  return m_have_reserve;
}

void SymbolTable::invalidate_reserve()
{
  if (m_have_reserve)
  {
    m_symbolStorage[m_size - 1].data[0] = 0;
  }
}

int SymbolTable::size() const
{
  return m_have_reserve ? m_size - 1 : m_size;
}

Symbol& SymbolTable::operator[](int index)
{
  return m_symbolStorage[index];
}

const Symbol& SymbolTable::operator[](int index) const
{
  return m_symbolStorage[index];
}


#define CC_EOF      0
#define CC_NEWLINE  1
#define CC_WS       2
#define CC_DIGIT    3
#define CC_DOT      4
#define CC_LETTER   5
#define CC_QUOTE    6
#define CC_OTHER    7
#define CC_ILLEGAL  8

int SKUAParser::m_char_class[256] = {
  /*   0 -   7, NUL - BEL */
  CC_EOF, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  /*   8 -  15, BS \t \n \v \f \r SO SI */
  CC_ILLEGAL, CC_WS, CC_NEWLINE, CC_WS, CC_WS, CC_WS, CC_ILLEGAL, CC_ILLEGAL,
  /*  16 -  23, DLE - ETB */
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  /*  24 -  31, CAN - US */
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  /*  32 -  39, SPACE ! " # $ % & ' */
  CC_WS, CC_OTHER, CC_QUOTE, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER, CC_QUOTE,
  /*  40 -  47, ( ) * + ' - . / */
  CC_OTHER, CC_OTHER, CC_LETTER, CC_OTHER, CC_OTHER, CC_OTHER, CC_DOT, CC_OTHER,
  /*  48 -  55, 0 1 2 3 4 5 6 7 */
  CC_DIGIT, CC_DIGIT, CC_DIGIT, CC_DIGIT, CC_DIGIT, CC_DIGIT, CC_DIGIT, CC_DIGIT,
  /*  56 -  63, 8 9 : ; < = > ? */
  CC_DIGIT, CC_DIGIT, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER,
  /*  64 -  71, @ A B C D E F G */
  CC_OTHER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /*  72 -  79, H I J K L M N O */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /*  80 -  87, P Q R S T U V W */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /*  88 -  95, X Y Z [ \ ] ^ _ */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER,
  /*  96 - 103, ` a b c d e f g */
  CC_OTHER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /* 104 - 111, h i j k l m n o */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /* 112 - 119, p q r s t u v w */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER, CC_LETTER,
  /* 120 - 127, x y z { | } ~ DEL */
  CC_LETTER, CC_LETTER, CC_LETTER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER, CC_OTHER,
  /* 128 - 255, Extended ASCII */
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
  CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL, CC_ILLEGAL,
};


SKUAParser::SKUAParser(FILE *fp, IProgressHandler& progress, SKUAIPostProcessor *postProcessor)
  : m_fp(fp)
  , m_progress(progress)
  , m_symbols()
  , m_identifier_start(0)
  , m_postProcessor(postProcessor)
  , m_lineNr(0)
  , m_count(0)
  , m_data(nullptr)
{
  m_buffer = (char *)malloc(2 * (m_buffer_size + m_sentinel_size));

  m_buffer_0_end = m_buffer + m_buffer_size;
  m_buffer_1_end = m_buffer + 2 * m_buffer_size + m_sentinel_size;

  *m_buffer_0_end = m_sentinel;
  *m_buffer_1_end = m_sentinel;

  m_forward = m_buffer;
  m_lexeme = m_buffer;
}

SKUAParser::~SKUAParser()
{
  delete m_data;

  for (std::vector<const char *>::iterator it = m_errors.begin(); it != m_errors.end(); ++it)
    delete *it;

  for (std::vector<const SKUAParseData *>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
    delete *it;
}


int SKUAParser::LineNr() const
{
  return m_lineNr;
}

int SKUAParser::ErrorNr() const
{
  return static_cast<int>(m_errors.size());
}

const char *SKUAParser::Error(int index) const
{
  return m_errors[index];
}

int SKUAParser::ObjectNr() const
{
  return static_cast<int>(m_objects.size());
}

const SKUAParseData *SKUAParser::Object(int index) const
{
  return m_objects[index];
}

bool SKUAParser::Parse()
{
  if (!scan_init())
    return ParseError("Failed to load file");

  do
  {
    scan();

    switch (TOKEN)
    {
    case TOK_GOCAD:
      if (!ParseGOCAD())
        return false;

      if (m_data)
      {
        m_objects.emplace_back(m_data);
        m_data = nullptr;
      }
      break;
    case TOK_CHAR:
      if (CVAL == '#')
        scan_skip_to_eol();
      break;
    case TOK_EOF:
      break;
    default:
      return ParseError("Unknown or unexpected token");
    }
  } while (TOKEN != TOK_EOF);

  return true;
}


/// PARSER INTERNAL routines

bool SKUAParser::ParseError(const char *error)
{
  std::stringstream msg;

  msg << "Parse error (line " << m_lineNr << "): " << error;

// Preprocessor definitions: _CRT_NONSTDC_NO_DEPRECATE ... missed
#ifdef WIN32
  m_errors.emplace_back(_strdup(msg.str().c_str()));
#else
  m_errors.emplace_back(strdup(msg.str().c_str()));
#endif

  return false;
}


namespace debug
{
void ParseOutput(const SKUAParseData& data, char *filename)
{
  std::vector<int> has_shared;
  has_shared.reserve(1000);

  for (int i = 0; i < data.vertices.size(); ++i)
  {
    int t = data.vertices[i].link;
    if (t >= 0) has_shared.push_back(t);
  }

  FILE *fp = fopen(filename, "w");

  fprintf(fp, "GOCAD Vset 1\n");
  fprintf(fp, "HEADER {\n");
  fprintf(fp, "name: Split nodes\n");
  fprintf(fp, "}\n");
  fprintf(fp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM");
  fprintf(fp, "NAME Default\n");
  fprintf(fp, "PROJECTION Unknown\n");
  fprintf(fp, "DATUM Unknown\n");
  fprintf(fp, "AXIS_NAME X Y Z\n");
  fprintf(fp, "AXIS_UNIT m m m\n");
  fprintf(fp, "ZPOSITIVE Depth\n");
  fprintf(fp, "END_ORIGINAL_COORDINATE_SYSTEM\n");

  for (int i = 0; i < has_shared.size(); ++i)
  {
    const SKUAParseData::Vertex& v = data.vertices[has_shared[i]];

    fprintf(fp, "VRTX %d %0.06f %0.06f %0.06f\n", i + 1, v.coordinate[0], v.coordinate[1], v.coordinate[2]);
  }

  fprintf(fp, "END\n");

  fclose(fp);
}
}


bool SKUAParser::ParseGOCAD()
{
  scan();
  if (TOKEN != TOK_OBJECT)
    return ParseError("Expected object type");

  int object = ATTRIB;

  scan_skip_to_eol();
  // ignore rest of line

  bool retval = true;

  switch (object)
  {
  case TOK_OBJECT_LIGTHTSOLID:
    retval = ParseLightTSolid();
    break;
  case TOK_OBJECT_TSOLID:
    retval = ParseTSolid();
    break;
  case TOK_OBJECT_TSURF:
    retval = ParseTSurf();
    break;
  case TOK_OBJECT_VSET:
    retval = ParseVSet();
    break;
  }

  if (retval)
  {
    if (m_postProcessor)
      (*m_postProcessor)(m_data)->PostProcess();
    else
    {
      SKUACoordinatePostProcessor postProcessor(m_data);

      SKUASolidPostProcessor solidPostProcessor;

      if (m_data->skua_type == SKUAParseData::LightTSolid || m_data->skua_type == SKUAParseData::TSolid)
      {
        postProcessor.SetNextPostProcessor(&solidPostProcessor);
      }

      postProcessor.PostProcess();
    }
  }

    // Shows split nodes; this would be a nice feature to have as a generated pointset (without saving to file and then import)
    //debug::ParseOutput(*m_data, "C:\\Local\\vset.so");
 
  return retval;
}

bool SKUAParser::ParseLightTSolid()
{
  m_data = new SKUAParseData(SKUAParseData::LightTSolid);

  scan();

  while (TOKEN != TOK_EOF)
  {
    bool retval = true;

    switch (TOKEN)
    {
    case TOK_HEADER:
      retval = ParseHeader();
      break;
    case TOK_OCS_OPEN:
      retval = ParseOriginalCoordinateSystem();
      break;
    case TOK_PROPERTIES:
      retval = ParseProps();
      break;
    case TOK_TETRA_REGION:
      retval = ParseTetraRegions();
      break;
    case TOK_VRTX:
      retval = ParseVertices();
      break;
    case TOK_SHAREDVRTX:
      retval = ParseSharedVertices();
      break;
    case TOK_TETRA:
      retval = ParseTetras();
      break;
    case TOK_TRI_OPEN:
      retval = ParseTetraRegionIndicators();
      break;
    case TOK_MODEL:
      retval = ParseSurfaces();
      break;
    case TOK_MODEL_REGION:
      retval = ParseModelRegions();
      break;
    case TOK_END:
      return true;
    default:
      scan();
    }

    if (!retval)
      return false;
  }

  return false;
}

bool SKUAParser::ParseTSolid()
{
  m_data = new SKUAParseData(SKUAParseData::TSolid);

  scan();

  while (TOKEN != TOK_EOF)
  {
    bool retval = true;

    switch (TOKEN)
    {
    case TOK_HEADER:
      retval = ParseHeader();
      break;
    case TOK_OCS_OPEN:
      retval = ParseOriginalCoordinateSystem();
      break;
    case TOK_PROPERTIES:
      retval = ParseProps();
      break;
    case TOK_TETRA_REGION:
      retval = ParseTetraRegions();
      break;
    case TOK_TVOLUME:
      retval = ParseTVolume();
      break;
    case TOK_VRTX:
      retval = ParseVertices();
      break;
    case TOK_SHAREDVRTX:
      retval = ParseSharedVertices();
      break;
    case TOK_TETRA:
      retval = ParseTetras();
      break;
    case TOK_TRI_OPEN:
      retval = ParseTetraRegionIndicators();
      break;
    case TOK_MODEL:
      retval = ParseSurfaces();
      break;
    case TOK_MODEL_REGION:
      retval = ParseModelRegions();
      break;
    case TOK_END:
      return true;
    default:
      scan();
    }

    if (!retval)
      return false;
  }

  return false;
}

bool SKUAParser::ParseTSurf()
{
  m_data = new SKUAParseData(SKUAParseData::TSurf);

  scan();

  while (TOKEN != TOK_EOF)
  {
    bool retval = true;

    switch (TOKEN)
    {
    case TOK_HEADER:
      retval = ParseHeader();
      break;
    case TOK_OCS_OPEN:
      retval = ParseOriginalCoordinateSystem();
      break;
    case TOK_AXIS_UNIT:
      retval = ParseAxisUnit();
      break;
    case TOK_VRTX:
      retval = ParseVertices();
      break;
    case TOK_TRGL:
      retval = ParseTriangles();
      break;
    case TOK_TFACE:
      retval = ParseTFace();
      break;
    case TOK_END:
      return true;
    default:
      scan();
    }

    if (!retval)
      return false;
  }

  return false;
}

bool SKUAParser::ParseVSet()
{
  m_data = new SKUAParseData(SKUAParseData::VSet);

  scan();

  while (TOKEN != TOK_EOF)
  {
    bool retval = true;

    switch (TOKEN)
    {
    case TOK_HEADER:
      retval = ParseHeader();
      break;
    case TOK_OCS_OPEN:
      retval = ParseOriginalCoordinateSystem();
    case TOK_VRTX:
      retval = ParseVertices();
      break;
    case TOK_SHAREDVRTX:
      retval = ParseSharedVertices();
      break;
    case TOK_END:
      return true;
    default:
      scan();
    }

    if (!retval)
      return false;
  }

  return false;
}

bool SKUAParser::ParseHeader()
{
  scan();
  if (TOKEN != TOK_CHAR && CVAL != '{')
    return ParseError("Expected '{'");

  scan();
  while (TOKEN != TOK_CHAR && CVAL != '}')
  {
    if (TOKEN == TOK_ATTRIB)
    {
      int attrib = ATTRIB;

      scan();
      if (TOKEN != TOK_CHAR && CVAL != ':')
        return ParseError("Expected ':'");

      scan_skip_whitespace();

      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);

      if (attrib == TOK_ATTR_NAME)
      {
        m_data->name = Symbol2String(m_symbols[m_identifier_start + ATTRIB]);
      }
    }
    scan();
  }

  return true;
}

bool SKUAParser::ParseOriginalCoordinateSystem()
{
  do
  {
    scan();

    switch (TOKEN)
    {
    case TOK_DATUM:
    case TOK_PROJECTION:
      scan_skip_to_eol();
      break;
    case TOK_AXIS_NAME:
      scan_skip_to_eol();
      break;
    case TOK_AXIS_UNIT:
      if (!ParseAxisUnit())
        return false;
      break;
    case TOK_ZPOSITIVE:
      scan();

      if (TOKEN != TOK_ATTRIB_VALUE)
        return ParseError("Expected 'Depth' or 'Elevation'");

      if (ATTRIB != TOK_AV_DEPTH)
      {
        m_data->coord_unit[2] = std::abs(m_data->coord_unit[2] - 1) < 1E-12 ? -1 : -FIELD_LENGTH;
      }
      break;
    case TOK_EOF:
      return ParseError("Unexpected end of file");
    }

  } while (TOKEN != TOK_OCS_CLOSE);
  scan();

  return true;
}


bool SKUAParser::ParseAxisUnit()
{
  scan();

  if (TOKEN != TOK_ATTRIB_VALUE)
    return ParseError("Expected 'X' or 'U'");

  if (ATTRIB == TOK_AV_FT)
    m_data->coord_unit[0] = FIELD_LENGTH;

  scan();

  if (TOKEN != TOK_ATTRIB_VALUE)
    return ParseError("Expected 'Y' or 'V'");

  if (ATTRIB == TOK_AV_FT)
    m_data->coord_unit[1] = FIELD_LENGTH;

  scan();

  if (TOKEN != TOK_ATTRIB_VALUE)
    return ParseError("Expected 'Z' or 'W'");

  if (ATTRIB == TOK_AV_FT)
    m_data->coord_unit[2] = FIELD_LENGTH;

  return true;
}


bool SKUAParser::ParseProps()
{
  size_t *cap;
  size_t *size;
  std::vector<SKUAParseData::String> *names;
  std::vector<int> *esizes;
  std::vector<double> *no_data;
  std::vector<SKUAParseData::String> *units;
  std::vector<std::vector<double> > *props;

  bool nodal = ATTRIB == TOK_PT_NODE;

  if (nodal)
  {
    cap = &m_data->vertex_props.capacity;
    size = &m_data->vertex_props.size;
    names = &m_data->vertex_props.names;
    esizes = &m_data->vertex_props.esizes;
    no_data = &m_data->vertex_props.no_data_values;
    units = &m_data->vertex_props.units;
    props = &m_data->vertex_props.values;
  }
  else
  {
    cap = &m_data->tetra_props.capacity;
    size = &m_data->tetra_props.size;
    names = &m_data->tetra_props.names;
    esizes = &m_data->tetra_props.esizes;
    no_data = &m_data->tetra_props.no_data_values;
    units = &m_data->tetra_props.units;
    props = &m_data->tetra_props.values;
  }

  int props_size = 0;

  scan();
  while (TOKEN == TOK_IDENTIFIER)
  {
    names->emplace_back(Symbol2String(m_symbols[m_identifier_start + ATTRIB]));

    ++props_size;

    scan();
  }

  bool parseMeta = true;

  while (parseMeta)
  {
    switch (TOKEN)
    {
    case TOK_LEGAL_RANGES:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    case TOK_NO_DATA_VALUES:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    case TOK_PROP_CLASSES:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    case TOK_PROP_KINDS:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    case TOK_PROP_SUBCLASSES:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    case TOK_PROP_ESIZES:
      for (int i = 0; i < props_size; ++i)
      {
        scan();
        if (TOKEN != TOK_INTEGER)
          return ParseError("Expected integer");

        *size += IVAL;

        esizes->emplace_back(IVAL);
      }
      break;
    case TOK_UNITS:
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      break;
    default:
      parseMeta = false;
      break;
    }
    if (parseMeta)
      scan();
  }

  while (TOKEN == TOK_PROP_HEADER)
  {
    bool adjust_cap = false;

    scan();
    if (TOKEN != TOK_IDENTIFIER)
      return ParseError("Expected the name of a property");

    const char *identifier = m_symbols[m_identifier_start + ATTRIB].data;

    if (!strcmp(identifier, "NodeId") || !strcmp(identifier, "ElementId"))
      adjust_cap = true;

    scan();
    if (TOKEN != TOK_CHAR && CVAL != '{')
      return ParseError("Expected '{'");

    scan();

    int low = 0;
    int hig = 0;

    while (TOKEN != TOK_CHAR && CVAL != '}')
    {
      if (TOKEN == TOK_ATTRIB)
      {
        int attrib = ATTRIB;

        scan();
        if (TOKEN != TOK_CHAR && CVAL != ':')
          return ParseError("Expected ':'");

        if (attrib == TOK_ATTR_LOW_CLIP)
        {
          scan();
          if (TOKEN == TOK_INTEGER)
            low = IVAL;
        }
        else if (attrib == TOK_ATTR_HIGH_CLIP)
        {
          scan();
          if (TOKEN == TOK_INTEGER)
            hig = IVAL;
        }
        else
        {
          scan_skip_whitespace();

          scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
        }
      }
      scan();
    }

    if (adjust_cap && hig != 0)
    {
      *cap = hig - low;
      if (*cap < 0)
        *cap = 10000;

      if (nodal)
        m_data->vertices.reserve(*cap);
      else
        m_data->tetras.reserve(*cap);
    }

    scan();
  }

  props->resize(*size);
  for (size_t i = 0; i < *size; ++i)
    (*props)[i].reserve(*cap);

  return true;
}

bool SKUAParser::ParseTetraRegions()
{
  do
  {
    scan();
    if (TOKEN != TOK_IDENTIFIER)
      return ParseError("Expected the name of a tetra region");

    m_data->tetra_region_names.emplace_back(Symbol2String(m_symbols[m_identifier_start + ATTRIB]));

    scan();
    if (TOKEN != TOK_INTEGER)
      return ParseError("Expected an integer");

    m_data->tetra_region_names.back().id = IVAL;

    scan();
  } while (TOKEN == TOK_TETRA_REGION);

  return true;
}

bool SKUAParser::ParseTVolume()
{
  ++m_data->volume;
  scan();
  if (TOKEN == TOK_IDENTIFIER)
  {
    // We don't add the volume name here; expect MODEL_REGIONs to be defined
    scan();
  }

  return true;
}

bool SKUAParser::ParseVertices()
{
  size_t prop_size = m_data->vertex_props.size;

  do
  {
    scan();

    if (TOKEN != TOK_INTEGER)
      return ParseError("Expected an integer");

    if (m_data->vertex_range.empty() || IVAL - m_data->vertex_range.back().offset > m_data->vertex_range.back().size)
    {
      int start = static_cast<int>(m_data->vertices.size());
      m_data->vertex_range.emplace_back(SKUAParseData::VertexRange(start, 1, IVAL - start));
      m_data->last_range = m_data->vertex_range.rbegin();
    }
    else
    {
      ++m_data->vertex_range.back().size;
    }

    m_data->vertices.emplace_back(SKUAParseData::Vertex(IVAL));

    for (int i = 0; i < 3; ++i)
    {
      scan();

      switch (TOKEN)
      {
      case TOK_INTEGER:
        m_data->vertices.back().coordinate[i] = IVAL;
        break;
      case TOK_DOUBLE:
        m_data->vertices.back().coordinate[i] = DVAL;
        break;
      default:
        return ParseError("Expected a number");
      }
    }

    for (int i = 0; i < prop_size; ++i)
    {
      scan();
      switch (TOKEN)
      {
      case TOK_INTEGER:
        m_data->vertex_props.values[i].emplace_back(IVAL);
        break;
      case TOK_DOUBLE:
        m_data->vertex_props.values[i].emplace_back(DVAL);
        break;
      default:
        return ParseError("Expected a number");
      }
    }

    scan();

  } while (TOKEN == TOK_VRTX);

  return true;
}

bool SKUAParser::ParseSharedVertices()
{
  size_t prop_size = m_data->vertex_props.size;

  do
  {
    scan();

    if (TOKEN != TOK_INTEGER)
      return ParseError("Expected an integer");

    if (m_data->vertex_range.empty() || IVAL - m_data->vertex_range.back().offset > m_data->vertex_range.back().size)
    {
      int start = static_cast<int>(m_data->vertices.size());
      m_data->vertex_range.emplace_back(SKUAParseData::VertexRange(start, 1, IVAL - start));
      m_data->last_range = m_data->vertex_range.rbegin();
    }
    else
    {
      ++m_data->vertex_range.back().size;
    }

    m_data->vertices.emplace_back(SKUAParseData::Vertex(IVAL));

    scan();

    if (TOKEN != TOK_INTEGER)
      return ParseError("Expected an integer");

    m_data->vertices.back().link = m_data->find_node(IVAL);
    
    for (int i = 0; i < prop_size; ++i)
    {
      scan();
      switch (TOKEN)
      {
      case TOK_INTEGER:
        m_data->vertex_props.values[i].emplace_back(IVAL);
        break;
      case TOK_DOUBLE:
        m_data->vertex_props.values[i].emplace_back(DVAL);
        break;
      default:
        return ParseError("Expected a number");
      }
    }

    scan();

  } while (TOKEN == TOK_SHAREDVRTX);

  return true;
}

bool SKUAParser::ParseTetras()
{
  size_t prop_size = m_data->tetra_props.size;
  do
  {
    m_data->tetras.emplace_back(SKUAParseData::Tetra(m_data->volume));

    int node = 0;

    for (int i = 0; i < 4; ++i)
    {
      scan();
      if (TOKEN != TOK_INTEGER)
        return ParseError("Expected an integer");
      m_data->tetras.back().node[i] = node = m_data->find_node(IVAL);
      m_data->tetras.back().opposite_signs[i] = 1;
      m_data->tetras.back().opposite_types[i] = 0;
      m_data->tetras.back().opposite_faces[i] = -1;
      ++m_data->vertices[node].elements;
    }

    bool flip = !m_data->check_orientation(m_data->tetras.back());

    if (flip)
    {
      std::swap(m_data->tetras.back().node[2], m_data->tetras.back().node[3]);
    }

    for (int i = 0; i < prop_size; ++i)
    {
      scan();
      switch (TOKEN)
      {
      case TOK_INTEGER:
        m_data->tetra_props.values[i].emplace_back(IVAL);
        break;
      case TOK_DOUBLE:
        m_data->tetra_props.values[i].emplace_back(DVAL);
        break;
      default:
        return ParseError("Expected a number");
      }
    }

    scan();

    if (TOKEN == TOK_CHAR && CVAL == '#')
    {
      scan();
      if (TOKEN == TOK_CTETRA)
      {
        scan(LEX_STATE_NOINT);
        m_data->tetras.back().model_region = ATTRIB;

        for (int i = 0; i < 4; ++i)
        {
          scan(LEX_STATE_NOINT);
          if (TOKEN == TOK_GEO_TYPE_VALUE)
          {
            m_data->tetras.back().opposite_signs[i] = SIGN;
            m_data->tetras.back().opposite_types[i] = ATTRIB;
            scan(LEX_STATE_NOINT);
            if (TOKEN == TOK_IDENTIFIER)
              m_data->tetras.back().opposite_faces[i] = ATTRIB;
          }
          else if (TOKEN == TOK_IDENTIFIER)
          {
            m_data->tetras.back().opposite_signs[i] = SIGN;
            m_data->tetras.back().opposite_faces[i] = ATTRIB;
          }
        }

        if (flip)
        {
          std::swap(m_data->tetras.back().opposite_signs[2], m_data->tetras.back().opposite_signs[3]);
          std::swap(m_data->tetras.back().opposite_faces[2], m_data->tetras.back().opposite_faces[3]);
        }
      }
      else
      {
        scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
      }
      scan();
    }
  } while (TOKEN == TOK_TETRA);

  return true;
}

bool SKUAParser::ParseTetraRegionIndicators()
{
  scan();

  size_t tetra = 0;

  while (TOKEN == TOK_TRI)
  {
    int tetra_region = 0;

    scan();
    while (TOKEN == TOK_INTEGER)
    {
      if (IVAL == 1)
        m_data->tetras[tetra].tetra_region = tetra_region;
      scan();

      ++tetra_region;
    }
    ++tetra;
  }
  if (TOKEN != TOK_TRI_CLOSE)
    return ParseError("Expected END_TETRA_REGION_INDICATORS");

  scan();

  return true;
}

bool SKUAParser::ParseTriangles()
{
  bool parseTriangles = true;

  while (parseTriangles)
  {
    switch (TOKEN)
    {
    case TOK_TFACE:
      if (!m_data->surface_faces.back().empty())
        m_data->surface_faces.back().back().size = (int)m_data->surfaces.back().size() - m_data->surface_faces.back().back().triangle;

      m_data->surface_faces.back().emplace_back(SKUAParseData::Face());

      scan_skip_whitespace();
      if (m_char_class[*m_forward] == CC_QUOTE)
        scan();
      else
        scan(LEX_STATE_IDENTIFIER_TO_EOL_START);

      m_data->surface_faces.back().back().id = (int)m_data->face_names.size();
      m_data->surface_faces.back().back().triangle = (int)m_data->surfaces.back().size();

      if (TOKEN == TOK_IDENTIFIER)
        m_data->face_names.emplace_back(SKUAParseData::String(m_data->surface_faces.back().back().id, m_symbols[m_identifier_start + ATTRIB].data));
      else
        m_data->face_names.emplace_back(SKUAParseData::String(m_data->surface_faces.back().back().id, ""));
      break;
    case TOK_KEYVERTICES:
      if (m_data->surface_faces.empty())
        scan_skip_to_eol();
      else
      {
        for (int i = 0; i < 3; ++i)
        {
          scan();
          if (TOKEN != TOK_INTEGER)
            return ParseError("Expected an integer");
          m_data->surface_faces.back().back().key_vertices[i] = m_data->find_node(IVAL);
        }
      }
      break;
    case TOK_TRGL:
      m_data->surfaces.back().emplace_back(SKUAParseData::Triangle());
      for (int i = 0; i < 3; ++i)
      {
        scan();
        if (TOKEN != TOK_INTEGER)
          return ParseError("Expected an integer");
        m_data->surfaces.back().back().node[i] = m_data->find_node(IVAL);
      }
      break;
    default:
      parseTriangles = false;
      if (!m_data->surface_faces.back().empty())
        m_data->surface_faces.back().back().size = (int)m_data->surfaces.back().size() - m_data->surface_faces.back().back().triangle;
    }
    if (parseTriangles)
      scan();
  }

  return true;
}

bool SKUAParser::ParseSurfaces()
{
  scan();
  while (TOKEN == TOK_SURFACE)
  {
    scan_skip_whitespace();

    if (m_char_class[*m_forward] == CC_QUOTE)
      scan();
    else
      scan(LEX_STATE_IDENTIFIER_TO_EOL_START);
    if (TOKEN != TOK_IDENTIFIER)
      return ParseError("Expected a surface name");

    m_data->surface_names.emplace_back(Symbol2String(m_symbols[m_identifier_start + ATTRIB]));

    m_data->surfaces.emplace_back(std::vector<SKUAParseData::Triangle>());
    m_data->surfaces.back().reserve(1024);

    m_data->surface_types.emplace_back(SKUAParseData::SurfaceType::SurfUnknown);

    m_data->surface_faces.emplace_back(std::vector<SKUAParseData::Face>());
    m_data->surface_faces.back().reserve(10);

    scan();
    if (!ParseTriangles())
      return false;
  }

  return true;
}

bool SKUAParser::ParseTFace()
{
  m_data->surfaces.emplace_back(std::vector<SKUAParseData::Triangle>());
  m_data->surfaces.back().reserve(1024);

  m_data->surface_types.emplace_back(SKUAParseData::SurfaceType::SurfUnknown);

  m_data->surface_faces.emplace_back(std::vector<SKUAParseData::Face>());
  m_data->surface_faces.back().reserve(10);
  m_data->surface_faces.back().emplace_back(SKUAParseData::Face());

  scan_skip_whitespace();
  if (m_char_class[*m_forward] == CC_QUOTE)
    scan();
  else
    scan(LEX_STATE_IDENTIFIER_TO_EOL_START);

  if (TOKEN == TOK_IDENTIFIER)
    m_data->surface_faces.back().back().id = ATTRIB;

  scan();
  return ParseTriangles();
}

bool SKUAParser::ParseModelRegions()
{
  do
  {
    scan(LEX_STATE_NOINT);
    if (TOKEN != TOK_IDENTIFIER)
      return ParseError("Expected a model region name");

    m_data->model_region_names.emplace_back(Symbol2String(m_symbols[m_identifier_start + ATTRIB]));

    scan();
    if (TOKEN != TOK_INTEGER)
      return ParseError("Expected an integer");

    //m_data->model_region_names.back().id = IVAL;

  } while (TOKEN == TOK_MODEL_REGION);

  return true;
}



/// BUFFER INTERFACE routines

bool SKUAParser::load()
{
  m_count = fread(m_forward, 1, m_buffer_size, m_fp);
  *(m_forward + m_count) = m_sentinel;

  return m_progress.Step((int)m_count);
}

bool SKUAParser::handle_forward_is_null()
{
  if (m_forward == m_buffer_0_end)
    m_forward += m_sentinel_size;
  else if (m_forward == m_buffer_1_end)
    m_forward = m_buffer;
  else
    return false;

  return load();
}


/// SCANNER INTERFACE routines

bool SKUAParser::scan_init()
{
  if (!load())
    return false;

  ++m_lineNr;

  m_symbols.insert(Symbol(TOK_GOCAD, 0, "GOCAD"));
  m_symbols.insert(Symbol(TOK_OBJECT, TOK_OBJECT_LIGTHTSOLID, "LightTSolid"));
  m_symbols.insert(Symbol(TOK_OBJECT, TOK_OBJECT_TSOLID, "TSolid"));
  m_symbols.insert(Symbol(TOK_OBJECT, TOK_OBJECT_TSURF, "TSurf"));
  m_symbols.insert(Symbol(TOK_OBJECT, TOK_OBJECT_VSET, "Vset"));
  m_symbols.insert(Symbol(TOK_HEADER, 0, "HEADER"));
  m_symbols.insert(Symbol(TOK_ATTRIB, TOK_ATTR_NAME, "name"));
  m_symbols.insert(Symbol(TOK_ATTRIB, TOK_ATTR_UNIT, "unit"));
  m_symbols.insert(Symbol(TOK_ATTRIB, TOK_ATTR_LOW_CLIP, "low_clip"));
  m_symbols.insert(Symbol(TOK_ATTRIB, TOK_ATTR_HIGH_CLIP, "high_clip"));
  m_symbols.insert(Symbol(TOK_ATTRIB, TOK_ATTR_QUANTITY, "QUANTITY"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_X, "X"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_Y, "Y"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_Z, "Z"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_U, "U"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_V, "V"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_W, "W"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_FLOAT, "Float"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_DEPTH, "Depth"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_ELEVATION, "Elevation"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_M, "m"));
  m_symbols.insert(Symbol(TOK_ATTRIB_VALUE, TOK_AV_FT, "ft"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_NONE, "none"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_DEFAULT, "Default"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_UNKNOWN_U, "Unknown"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_UNKNOWN_L, "unknown"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_NONE_STAR, "**none**"));
  m_symbols.insert(Symbol(TOK_NONE_VALUE, TOK_NV_UNITLESS, "unitless"));
  m_symbols.insert(Symbol(TOK_OCS_OPEN, 0, "GOCAD_ORIGINAL_COORDINATE_SYSTEM"));
  m_symbols.insert(Symbol(TOK_OCS_CLOSE, 0, "END_ORIGINAL_COORDINATE_SYSTEM"));
  m_symbols.insert(Symbol(TOK_NAME, 0, "NAME"));
  m_symbols.insert(Symbol(TOK_PROJECTION, 0, "PROJECTION"));
  m_symbols.insert(Symbol(TOK_DATUM, 0, "DATUM"));
  m_symbols.insert(Symbol(TOK_AXIS_NAME, 0, "AXIS_NAME"));
  m_symbols.insert(Symbol(TOK_AXIS_UNIT, 0, "AXIS_UNIT"));
  m_symbols.insert(Symbol(TOK_ZPOSITIVE, 0, "ZPOSITIVE"));
  m_symbols.insert(Symbol(TOK_PROPERTIES, TOK_PT_NODE, "PROPERTIES"));
  m_symbols.insert(Symbol(TOK_LEGAL_RANGES, TOK_PT_NODE, "PROP_LEGAL_RANGES"));
  m_symbols.insert(Symbol(TOK_NO_DATA_VALUES, TOK_PT_NODE, "NO_DATA_VALUES"));
  m_symbols.insert(Symbol(TOK_PROP_CLASSES, TOK_PT_NODE, "PROPERTY_CLASSES"));
  m_symbols.insert(Symbol(TOK_PROP_KINDS, TOK_PT_NODE, "PROPERTY_KINDS"));
  m_symbols.insert(Symbol(TOK_PROP_SUBCLASSES, TOK_PT_NODE, "PROPERTY_SUBCLASSES"));
  m_symbols.insert(Symbol(TOK_PROP_ESIZES, TOK_PT_NODE, "ESIZES"));
  m_symbols.insert(Symbol(TOK_UNITS, TOK_PT_NODE, "UNITS"));
  m_symbols.insert(Symbol(TOK_PROP_HEADER, TOK_PT_NODE, "PROPERTY_CLASS_HEADER"));
  m_symbols.insert(Symbol(TOK_PROPERTIES, TOK_PT_TETRA, "TETRA_PROPERTIES"));
  m_symbols.insert(Symbol(TOK_LEGAL_RANGES, TOK_PT_TETRA, "TETRA_PROP_LEGAL_RANGES"));
  m_symbols.insert(Symbol(TOK_NO_DATA_VALUES, TOK_PT_TETRA, "TETRA_NO_DATA_VALUES"));
  m_symbols.insert(Symbol(TOK_PROP_CLASSES, TOK_PT_TETRA, "TETRA_PROPERTY_CLASSES"));
  m_symbols.insert(Symbol(TOK_PROP_KINDS, TOK_PT_TETRA, "TETRA_PROPERTY_KINDS"));
  m_symbols.insert(Symbol(TOK_PROP_SUBCLASSES, TOK_PT_TETRA, "TETRA_PROPERTY_SUBCLASSES"));
  m_symbols.insert(Symbol(TOK_PROP_ESIZES, TOK_PT_TETRA, "TETRA_ESIZES"));
  m_symbols.insert(Symbol(TOK_UNITS, TOK_PT_TETRA, "TETRA_UNITS"));
  m_symbols.insert(Symbol(TOK_PROP_HEADER, TOK_PT_TETRA, "TETRA_PROPERTY_CLASS_HEADER"));
  m_symbols.insert(Symbol(TOK_TETRA_REGION, 0, "TETRA_REGION"));
  m_symbols.insert(Symbol(TOK_TVOLUME, 0, "TVOLUME"));
  m_symbols.insert(Symbol(TOK_VRTX, TOK_VT_VRTX, "VRTX"));
  m_symbols.insert(Symbol(TOK_VRTX, TOK_VT_PVRTX, "PVRTX"));
  m_symbols.insert(Symbol(TOK_SHAREDVRTX, TOK_VT_VRTX, "SHAREDVRTX"));
  m_symbols.insert(Symbol(TOK_SHAREDVRTX, TOK_VT_PVRTX, "SHAREDPVRTX"));
  m_symbols.insert(Symbol(TOK_SHAREDVRTX, TOK_VT_ATOM + TOK_VT_VRTX, "ATOM"));
  m_symbols.insert(Symbol(TOK_SHAREDVRTX, TOK_VT_ATOM + TOK_VT_PVRTX, "PATOM"));
  m_symbols.insert(Symbol(TOK_TETRA, 0, "TETRA"));
  m_symbols.insert(Symbol(TOK_CTETRA, 0, "CTETRA"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE, 0, "GEOLOGICAL_TYPE"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE_VALUE, TOK_GTV_INTRUSIVE, "intrusive"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE_VALUE, TOK_GTV_UNCONFORMITY, "unconformity"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE_VALUE, TOK_GTV_BOUNDARY, "boundary"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE_VALUE, TOK_GTV_TOP, "top"));
  m_symbols.insert(Symbol(TOK_GEO_TYPE_VALUE, TOK_GTV_FAULT, "fault"));
  m_symbols.insert(Symbol(TOK_TRI_OPEN, 0, "BEGIN_TETRA_REGION_INDICATORS"));
  m_symbols.insert(Symbol(TOK_TRI_CLOSE, 0, "END_TETRA_REGION_INDICATORS"));
  m_symbols.insert(Symbol(TOK_TRI, 0, "TRI"));
  m_symbols.insert(Symbol(TOK_MODEL, 0, "MODEL"));
  m_symbols.insert(Symbol(TOK_SURFACE, 0, "SURFACE"));
  m_symbols.insert(Symbol(TOK_TFACE, 0, "TFACE"));
  m_symbols.insert(Symbol(TOK_KEYVERTICES, 0, "KEYVERTICES"));
  m_symbols.insert(Symbol(TOK_TRGL, 0, "TRGL"));
  m_symbols.insert(Symbol(TOK_MODEL_REGION, 0, "MODEL_REGION"));
  m_symbols.insert(Symbol(TOK_END, 0, "END"));

  m_symbols.start_identifiers_at(m_identifier_start = m_symbols.size());

  return true;
}

void SKUAParser::scan(int next_lexstate)
{
  TOKEN = TOK_UNKNOWN;
  m_lexeme = m_forward;
  m_lexstate = next_lexstate;

  do
  {
    while ((m_char_read = *m_forward) != m_sentinel)
    {
      m_char_class_read = m_char_class[m_char_read];

      switch (m_lexstate)
      {
      case LEX_STATE_NEW:
        handle_lexstate_new();
        break;
      case LEX_STATE_PM:
        handle_lexstate_pm();
        break;
      case LEX_STATE_NOINT:
        handle_lexstate_noint();
        break;
      case LEX_STATE_INTEGER:
        handle_lexstate_integer();
        break;
      case LEX_STATE_DOUBLE:
        handle_lexstate_double();
        break;
      case LEX_STATE_IDENTIFIER:
        handle_lexstate_identifier();
        break;
      case LEX_STATE_IDENTIFIER_PLUS_NUMBER:
        handle_lexstate_identifier_plus_number();
        break;
      case LEX_STATE_QUOTED_IDENTIFIER_START:
        handle_lexstate_quoted_identifier_start();
        break;
      case LEX_STATE_QUOTED_IDENTIFIER_CONTINUE:
        handle_lexstate_quoted_identifier();
        break;
      case LEX_STATE_PARENTHESIS_IDENTIFIER_START:
        handle_lexstate_parenthesis_identifier_start();
        break;
      case LEX_STATE_PARENTHESIS_IDENTIFIER_CONTINUE:
        handle_lexstate_parenthesis_identifier();
        break;
      case LEX_STATE_IDENTIFIER_TO_EOL_START:
        handle_lexstate_identifier_to_eol_start();
        break;
      case LEX_STATE_IDENTIFIER_TO_EOL_CONTINUE:
        handle_lexstate_identifier_to_eol();
        break;
      case LEX_STATE_ERROR:
        return;
      }

      if (m_lexstate == LEX_STATE_TOKEN)
      {
        m_symbols.invalidate_reserve();
        return;
      }
    }
  } while (handle_forward_is_null());

  if (m_symbols.have_reserve())
  {
    if (strncmp(m_symbols[m_symbols.size()].data, "END", 3) == 0)
    {
      end_identifier();
      m_symbols.reserve(false);
      if (m_lexstate == LEX_STATE_TOKEN)
        return;
    }
  }

  m_lexstate = LEX_STATE_DONE;
  TOKEN = TOK_EOF;
}

void SKUAParser::scan_skip_whitespace()
{
  do
  {
    while ((m_char_read = *m_forward) != m_sentinel)
    {
      m_char_class_read = m_char_class[m_char_read];
      if (m_char_class_read != CC_WS)
      {
        m_lexeme = m_forward;
        return;
      }
      ++m_forward;
    }
  } while (handle_forward_is_null());
}

void SKUAParser::scan_skip_to_eol()
{
  do
  {
    while ((m_char_read = *m_forward) != m_sentinel)
    {
      ++m_forward;
      if (m_char_read == '\n')
      {
        m_lexeme = m_forward;
        ++m_lineNr;
        return;
      }
    }
  } while (handle_forward_is_null());
}



/// SCANNER INTERNAL routines

void SKUAParser::start_identifier()
{
  m_identifier_data = m_symbols[m_symbols.reserve(TOK_IDENTIFIER)].data;
  *m_identifier_data++ = m_char_read;
}

void SKUAParser::end_identifier()
{
  *m_identifier_data = 0;
  int id = m_symbols.insert();
  TOKEN = m_symbols[id].token;
  ATTRIB = m_symbols[id].attribute;
  m_lexstate = LEX_STATE_TOKEN;
}

void SKUAParser::end_identifier_forced()
{
  *m_identifier_data = 0;
  int id = m_symbols.insert();

  if (id < m_identifier_start)
  {
    TOKEN = TToken::TOK_IDENTIFIER;
    ATTRIB = id - m_identifier_start;
  }
  else
  {
    TOKEN = m_symbols[id].token;
    ATTRIB = m_symbols[id].attribute;
  }
  m_lexstate = LEX_STATE_TOKEN;
}

void SKUAParser::handle_lexstate_new()
{
  TOKEN = TOK_UNKNOWN;
  ATTRIB = 0;
  IVAL = 0;
  DVAL = 0;

  SIGN = 1;

  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    /* Fall through */
  case CC_WS:
    ++m_forward;
    m_lexeme = m_forward;
    break;
  case CC_DIGIT:
    ++m_forward;
    IVAL = m_char_read - '0';
    SIGN = 1;
    m_lexstate = LEX_STATE_INTEGER;
    break;
  case CC_LETTER:
    ++m_forward;
    m_lexstate = LEX_STATE_IDENTIFIER;
    start_identifier();
    break;
  case CC_OTHER:
    switch (m_char_read)
    {
    case '+':
      ++m_forward;
      SIGN = 1;
      m_lexstate = LEX_STATE_PM;
      break;
    case '-':
      ++m_forward;
      SIGN = -1;
      m_lexstate = LEX_STATE_PM;
      break;
    case '{':
    case '}':
    case ':':
    case '#':
      ++m_forward;
      TOKEN = TOK_CHAR;
      CVAL = m_char_read;
      m_lexstate = LEX_STATE_TOKEN;
      break;
    default:
      ++m_forward;
    }
    break;
  case CC_QUOTE:
    ++m_forward;
    m_scan_quote_char = m_char_read;
    m_lexstate = LEX_STATE_QUOTED_IDENTIFIER_START;
    break;
  case CC_ILLEGAL:
    m_lexstate = LEX_STATE_ERROR;
    break;
  default:
    break;
  }
}

void SKUAParser::handle_lexstate_pm()
{
  switch (m_char_class_read)
  {
  case CC_DIGIT:
    IVAL = m_char_read - '0';
    m_lexstate = LEX_STATE_INTEGER;
    break;
  case CC_NEWLINE:
    ++m_lineNr;
    m_lexstate = LEX_STATE_ERROR;
    break;
  default:
    switch (m_char_read)
    {
    case '(':
      m_lexstate = LEX_STATE_PARENTHESIS_IDENTIFIER_START;
      break;
    case '"':
      /* Fall-through */
    case '\'':
      m_scan_quote_char = m_char_read;
      m_lexstate = LEX_STATE_QUOTED_IDENTIFIER_START;
      break;
    default:
      m_lexstate = LEX_STATE_IDENTIFIER;
      start_identifier();
    }
  }
  ++m_forward;
}

void SKUAParser::handle_lexstate_noint()
{
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_forward;
    ++m_lineNr;
    m_lexstate = LEX_STATE_ERROR;
    break;
  case CC_WS:
    ++m_forward;
    m_lexeme = m_forward;
    break;
  default:
    switch (m_char_read)
    {
    case '-':
      ++m_forward;
      m_lexeme = m_forward;
      SIGN = -1;
      break;
    case '+':
      ++m_forward;
      m_lexeme = m_forward;
      SIGN = 1;
      break;
    case '(':
      ++m_forward;
      m_lexstate = LEX_STATE_PARENTHESIS_IDENTIFIER_START;
      break;
    case '"':
      /* Fall-through */
    case '\'':
      m_scan_quote_char = m_char_read;
      ++m_forward;
      m_lexstate = LEX_STATE_QUOTED_IDENTIFIER_START;
      break;
    default:
      m_lexstate = LEX_STATE_IDENTIFIER_PLUS_NUMBER;
      start_identifier();
      ++m_forward;
    }
  }
}

void SKUAParser::handle_lexstate_integer()
{
  ++m_forward;
  switch (m_char_class_read)
  {
  case CC_DIGIT:
    IVAL = 10 * IVAL + m_char_read - '0';
    break;
  case CC_DOT:
    m_lexstate = LEX_STATE_DOUBLE;
    break;
  case CC_NEWLINE:
    ++m_lineNr;
    /* Fall through */
  default:
    TOKEN = TOK_INTEGER;
    IVAL = SIGN * IVAL;
    SIGN = 1;
    m_lexstate = LEX_STATE_TOKEN;
  }
}

void SKUAParser::handle_lexstate_double()
{
  char *buf;
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    /* Fall through */
  case CC_WS:
    buf = m_tmp_buffer_for_double;
    while (m_lexeme != m_forward)
    {
      if (!*m_lexeme)
      {
        if (m_lexeme == m_buffer_0_end)
          m_lexeme += m_sentinel_size;
        else if (m_lexeme == m_buffer_1_end)
          m_lexeme = m_buffer;
        else
          m_lexstate = LEX_STATE_ERROR;
      }
      else
        *buf++ = *m_lexeme++;
    }
    *buf = 0;

    DVAL = std::strtod(m_tmp_buffer_for_double, &buf);
    if (*buf == 0)
    {
      TOKEN = TOK_DOUBLE;
      SIGN = 1;
      m_lexstate = LEX_STATE_TOKEN;
    }
    else
    {
      m_lexstate = LEX_STATE_ERROR;
    }
    break;
  default:
    break;
  }
  ++m_forward;
}

void SKUAParser::handle_lexstate_identifier()
{
  ++m_forward;
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    /* Fall through */
  case CC_OTHER:
    if (m_char_read != '{' && m_char_read != ':')
    {
      *m_identifier_data++ = m_char_read;
      break;
    }
    else
    {
      --m_forward;
    }
  case CC_WS:
    end_identifier();
    break;
  default:
    *m_identifier_data++ = m_char_read;
  }
}

void SKUAParser::handle_lexstate_identifier_plus_number()
{
  ++m_forward;
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    /* Fall through */
  case CC_OTHER:
    if (m_char_read != '{' && m_char_read != ':')
    {
      *m_identifier_data++ = m_char_read;
      break;
    }
    else
    {
      --m_forward;
    }
    end_identifier();
    break;
  case CC_WS:
    if (!*m_forward)
      handle_forward_is_null();
    if (m_char_class[*m_forward] == CC_DIGIT)
      *m_identifier_data++ = m_char_read;
    else
      end_identifier();
    break;
  default:
    *m_identifier_data++ = m_char_read;
  }
}

void SKUAParser::handle_lexstate_quoted_identifier_start()
{
  ++m_forward;
  if (m_char_read == '\n')
  {
    m_lexstate = LEX_STATE_ERROR;
  }
  else
  {
    m_lexstate = LEX_STATE_QUOTED_IDENTIFIER_CONTINUE;
    start_identifier();
  }
}

void SKUAParser::handle_lexstate_quoted_identifier()
{
  ++m_forward;
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    m_lexstate = LEX_STATE_ERROR;
    break;
  case CC_QUOTE:
    if (m_char_read == m_scan_quote_char)
    {
      end_identifier();
      break;
    }
  default:
    *m_identifier_data++ = m_char_read;
  }
}

void SKUAParser::handle_lexstate_parenthesis_identifier_start()
{
  ++m_forward;
  if (m_char_read == '\n')
  {
    m_lexstate = LEX_STATE_ERROR;
  }
  else
  {
    m_lexstate = LEX_STATE_PARENTHESIS_IDENTIFIER_CONTINUE;
    start_identifier();
  }
}

void SKUAParser::handle_lexstate_parenthesis_identifier()
{
  ++m_forward;
  switch (m_char_read)
  {
  case '\n':
    ++m_lineNr;
    m_lexstate = LEX_STATE_ERROR;
    break;
  case ')':
    end_identifier();
    break;
  default:
    *m_identifier_data++ = m_char_read;
  }
}

void SKUAParser::handle_lexstate_identifier_to_eol_start()
{
  scan_skip_whitespace();
  ++m_forward;
  if (m_char_read == '\n')
  {
    m_lexstate = LEX_STATE_TOKEN;
  }
  else
  {
    m_lexstate = LEX_STATE_IDENTIFIER_TO_EOL_CONTINUE;
    start_identifier();
  }
}

void SKUAParser::handle_lexstate_identifier_to_eol()
{
  ++m_forward;
  switch (m_char_class_read)
  {
  case CC_NEWLINE:
    ++m_lineNr;
    end_identifier_forced();
    break;
  default:
    if (m_char_read != '\r')
      *m_identifier_data++ = m_char_read;
  }
}


void SKUAParser::debug_print_token(const char *filename) const
{
#if 1
  FILE *fp = fopen(filename, "a");
  switch (TOKEN)
  {
  case TOK_CHAR:
    fprintf(fp, "TOKEN: CHAR '%c'\n", CVAL);
    break;
  case TOK_INTEGER:
    fprintf(fp, "TOKEN: INTEGER %d\n", IVAL);
    break;
  case TOK_DOUBLE:
    fprintf(fp, "TOKEN: DOUBLE %f\n", DVAL);
    break;
  case TOK_IDENTIFIER:
    fprintf(fp, "TOKEN: IDENTIFIER %d --%s--\n", ATTRIB, ATTRIB >= 0 && ATTRIB < m_symbols.size() ? m_symbols[ATTRIB + m_identifier_start].data : "");
    break;
  default:
    fprintf(fp, "TOKEN: %d %d\n", TOKEN, ATTRIB);
  }
  fclose(fp);
#endif
}

}

}
