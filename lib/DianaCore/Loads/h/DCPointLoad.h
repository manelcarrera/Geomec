// DCPointLoad.h: interface for the CPointLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOINTLOAD_H__581A489C_888C_4563_AA9E_03A9D1864AB9__INCLUDED_)
#define AFX_IPOINTLOAD_H__581A489C_888C_4563_AA9E_03A9D1864AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
class INode;
}

namespace dia {

class CLoadManager;

class CPointLoad : public IDirectedLoad {
  const geo::INode &m_Node;

public:
  CPointLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::INode &node);
  virtual ~CPointLoad();

  virtual bool WriteFilos() const;
  const geo::INode &Node() const;
};

} // namespace dia

#endif // !defined(AFX_IPOINTLOAD_H__581A489C_888C_4563_AA9E_03A9D1864AB9__INCLUDED_)
