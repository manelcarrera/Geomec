// DCPointMoment.h: interface for the CPointMoment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOINTMOMENT_H__D67FA136_CBAA_414B_A168_EA4CCA3DB940__INCLUDED_)
#define AFX_IPOINTMOMENT_H__D67FA136_CBAA_414B_A168_EA4CCA3DB940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
class INode;
}

namespace dia {

class CLoadManager;

class CPointMoment : public IDirectedLoad  
{
  const geo::INode &m_Node;

public:
  CPointMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::INode &node);
  virtual ~CPointMoment();

  virtual bool WriteFilos() const;
  const geo::INode &Node() const;
};

}

#endif // !defined(AFX_IPOINTMOMENT_H__D67FA136_CBAA_414B_A168_EA4CCA3DB940__INCLUDED_)
