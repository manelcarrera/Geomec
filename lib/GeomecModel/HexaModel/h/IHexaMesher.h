#pragma once

#include "StorageNode.h"

namespace geo
{
class IPoint;
}

class IProgressBase;
class CHexaMesh;
class CModelBase;



class IHexaMesher
{
  CHexaMesh& m_hexamesh;

protected:

  CHexaMesh& Mesh();
  CModelBase& Model();

  class CXYLess
  {
  public:
  bool operator()(const geo::IPoint &P1, const geo::IPoint &P2) const;
  };

  virtual void CreateTyings(IProgressBase& progress) = 0;

public:

  IHexaMesher(CHexaMesh& mesh);
  virtual ~IHexaMesher();

  typedef enum { OLD_MESHER = 0, NEXT_GEN } TMesherType;
  virtual int MesherType() const = 0;

  virtual bool CreateMesh(IProgressBase& progress) = 0;

  bool ValidRegions();
  void CreateTyings();

  virtual bool CanCreateMesh() = 0;
  virtual void Invalidate() = 0;

  virtual void LoadStream(CArchiveInterface& stream, CStreamVersion& version, IProgressBase& progress) = 0;
  virtual void SaveStream(CArchiveInterface& stream, IProgressBase& progress) = 0;

  virtual long SavedItems() const = 0;
};


