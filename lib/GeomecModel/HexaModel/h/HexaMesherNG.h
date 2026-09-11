#pragma once

#include "IHexaMesher.h"

#include "HexaMetaMesh.h"
#include "Point.h"

namespace hmm {
class CMetaMesh;
}

namespace geo {
class IVector;
}

class CHexaFormation;
class CRotatedSystem;
class CHexaHorizon;

#include <map>
#include <set>
#include <vector>

class CHexaMesherNG : public IHexaMesher {

  hmm::CHexaMetaMesh m_metaMesh;

  bool m_bCanMesh;

protected:
  void CreateTyings(IProgressBase &progress);

public:
  CHexaMesherNG(CHexaMesh &mesh);
  virtual ~CHexaMesherNG();

  virtual int MesherType() const;

  virtual bool CreateMesh(IProgressBase &progress);

  bool ProblematicRegions();

  virtual bool CanCreateMesh();
  virtual void Invalidate();

  virtual void LoadStream(CArchiveInterface &stream, CStreamVersion &version, IProgressBase &progress);
  virtual void SaveStream(CArchiveInterface &stream, IProgressBase &progress);

  virtual long SavedItems() const;
};