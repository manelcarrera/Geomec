/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Tying.h: interface for the CTying class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITYING_H__2A1D0EF4_6896_4932_91C4_A7CF28EF19EA__INCLUDED_)
#define AFX_ITYING_H__2A1D0EF4_6896_4932_91C4_A7CF28EF19EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vector.h"
#include <string>

#include "GeometryExports.h"

namespace geo {

class INode;
class IMesh;
class CMesh;
class GEOMETRY_EXPORT CTying {
  friend class IMesh;

public:
  typedef enum _Type {
    TR = 0, // translation
    RO      // rotation
  } TType;

  typedef struct _MasterDef {
    int node;
    const IVector *direction;
    TType type;
    double factor;
  } TMasterDef;

private:
  class GEOMETRY_EXPORT CNodeDef {
    int m_nIndex;
    CVector m_vecDirection;
    TType m_nType;

  public:
    CNodeDef(int nIndex, const IVector &vecDirection, TType nType)
        : m_nIndex(nIndex), m_vecDirection(vecDirection), m_nType(nType) {}

    ~CNodeDef() {}

    int Index() const { return m_nIndex; }
    const IVector &Direction() const { return m_vecDirection; }
    TType Type() const { return m_nType; }
    std::string TypeName() const { return Type2TypeName(Type()); }

    static std::string Type2TypeName(TType nType) {
      switch (nType) {
      case TR:
        return std::string("TR");
      case RO:
        return std::string("RO");
      }

      assert(false);
      return std::string();
    }
  };

  IMesh &m_mesh;
  int m_nSlaveNode;
  CVector *m_pSlaveDirection;

  TType m_SlaveType;
  std::vector<std::pair<CNodeDef *, double>> m_vcMaster;

  // single master
  CTying(IMesh &mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType, int nMaster,
         const IVector &vecMasterDir, TType nMasterType, double dFactor);

  // two masters
  CTying(IMesh &mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType, int nMaster0,
         const IVector &vecMaster0Dir, TType nMaster0Type, double dFactor0, int nMaster1, const IVector &vecMaster1Dir,
         TType nMaster1Type, double dFactor1);

  // any number of masters (must be greater than 0 of course...)
  CTying(IMesh &mesh, int nSlave, const IVector &vecSlaveDir, TType nSlaveType,
         const std::vector<TMasterDef> &vcMasters);

public:
  virtual ~CTying();
  const IMesh &Mesh() const;

  // Slave Access
  int SlaveIndex() const;
  const IVector &SlaveDirection() const;

  // Master Access
  int MasterSize() const;
  const INode &Master(int nIndex) const;
  int MasterIndex(int nIndex) const;
  const IVector &MasterDirection(int nIndex) const;
  TType MasterType(int nIndex) const;
  std::string MasterTypeString(int nIndex) const;
  const double &MasterFactor(int nIndex) const;

  // Slave (Equal to Limited node)
  TType SlaveType() const;
  std::string SlaveTypeString() const;
};

} // namespace geo

#endif // !defined(AFX_ITYING_H__2A1D0EF4_6896_4932_91C4_A7CF28EF19EA__INCLUDED_)
