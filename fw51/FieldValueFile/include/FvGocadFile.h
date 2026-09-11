 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// GoCadFile.h: interface for the CGoCadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADFILE_H__A0154F7C_CBEB_4058_AA55_53FA1D1F99D6__INCLUDED_)
#define AFX_GOCADFILE_H__A0154F7C_CBEB_4058_AA55_53FA1D1F99D6__INCLUDED_

#include "FvTextFile.h"
#include "FvGocadSurface.h"
#include "FvGocadVolume.h"

#include "FieldValueFileExports.h"



class FIELDVALUEFILE_EXPORT CGoCadFile : public CTextFile  
{
  // State vector
  enum GoCadType { GOCAD_IDLE,
                   GOCAD_SURFACE,
                   GOCAD_VOLUME };

  enum GoCadState { GOCAD_CLOSED,
          PARSE_TYPE,
          PARSE_HEADER,
          PARSE_SURFACE };

  GoCadType  m_type;
  GoCadState  m_state;
  std::vector<CGoCadSurface*> m_vcSurface;
  std::vector<CGoCadVolume*> m_vcVolume;
  typedef CTokenTemplate<CGoCadFile> TGocadToken;
  bool WriteSurface(TOutputStream& stream, const CGoCadSurface &surface);
  bool WriteVolume(TOutputStream& stream, const CGoCadVolume &volume);
  CGoCadHeader& CurrentHeader();
  geo::IMesh& m_mesh;
public:
  CGoCadFile(geo::IMesh& mesh);
  virtual ~CGoCadFile();

  void RemoveAll();
  void AppendSurface(const geo::ISurface& surface, const QString &sName);
  size_t SurfaceSize() const;
  size_t VolumeSize() const;
  const CGoCadSurface& Surface(size_t nIndex) const;
  const CGoCadVolume& Volume(size_t nIndex) const;
  enum GoCadType Type() const {return m_type;}
  virtual long SavedItems() const;
  virtual bool OnWrite(TOutputStream& stream);
protected:
  virtual bool OnParseFail(TInputStream& stream, const QString& sToken);
  bool OpenGocad(TInputStream& stream, const QString& sToken);    // Called when the "GOCAD" tag is read
  bool TypeSurface(TInputStream& stream, const QString& sToken);  // Called when the "TSurf 1" tag is read
  bool TypeVolume(TInputStream& stream, const QString& sToken);
  bool CloseGocad(TInputStream& stream, const QString& sToken);
  bool ReadHeader(TInputStream& stream, const QString& sToken);
  bool ReadGeoType(TInputStream& stream, const QString& sToken);
  bool ReadPropClass(TInputStream& stream, const QString& sToken);
  bool ReadStatPos(TInputStream& stream, const QString& sToken);
  bool ReadVertex(TInputStream& stream, const QString& sToken);
  bool ReadTFace(TInputStream& stream, const QString& sToken);
  bool ReadTriangle(TInputStream& stream, const QString& sToken);
  bool ReadBStone(TInputStream& stream, const QString& sToken);
  bool ReadBorder(TInputStream& stream, const QString& sToken);
  bool ReadProperties(TInputStream& stream, const QString& sToken);
  bool ReadPropertiesSize(TInputStream& stream, const QString& sToken);
  bool ReadPropertyVertex(TInputStream& stream, const QString& sToken);
  bool ReadDefaultPropertyValues(TInputStream& stream, const QString& sToken);
  bool ReadPropertyUnits(TInputStream& stream, const QString& sToken);
  bool ReadPropertyClasses(TInputStream& stream, const QString& sToken);
  virtual bool ReadTetrahedron(TInputStream& stream, const QString& sToken);
  void NewSurface(); // Adds a new surface to the m_vcSurface vector.
  CGoCadSurface *LatestSurf();
  CGoCadVolume *LatestVol();

  // inserts a tetrahedron in the last volume of the m_vcVolume vector.
  int InsertTetInVol(const int nIndex1, const int nIndex2, const int nIndex3, const int nIndex4);
  // inserts a triangel in the last surface of the m_vcSurface vector.
  void InsertTriInSurf(const int nIndex1, const int nIndex2, const int nIndex3);
};

#endif // !defined(AFX_GOCADFILE_H__A0154F7C_CBEB_4058_AA55_53FA1D1F99D6__INCLUDED_)
