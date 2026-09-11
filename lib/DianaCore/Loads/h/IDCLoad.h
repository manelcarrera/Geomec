// ILoad.h: interface for the ILoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILOAD_H__3708D829_62D0_41D2_A66E_B1223B108599__INCLUDED_)
#define AFX_ILOAD_H__3708D829_62D0_41D2_A66E_B1223B108599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IElementSet.h"
#include "MeshNodeGroup.h"
#include "dimple.h"

namespace dia {

class CLoadManager;
class ICase;

class ILoad {
  CLoadManager &m_Manager;
  int m_nValueSize;
  ftn_double_t *m_pValue;

protected:
  void WriteGroupName(const geo::IElementSet &group, int nIndex) const;
  void WriteGroupName(const geo::CMeshNodeGroup &group) const;
  ftn_double_t *Values() const;

public:
  ILoad(ICase &icase, double size);
  ILoad(ICase &icase, const std::vector<double> &vcValue);
  virtual ~ILoad();

  virtual bool WriteFilos() const = 0;
  ftn_int_t ValueSize() const;

  CLoadManager &Manager() const;
  virtual const ICase &Case() const = 0;
};

} // namespace dia

#endif // !defined(AFX_ILOAD_H__3708D829_62D0_41D2_A66E_B1223B108599__INCLUDED_)
