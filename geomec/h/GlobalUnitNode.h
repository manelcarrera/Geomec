#ifndef _GlobalUnitNode_h_
#define _GlobalUnitNode_h_

class CUnitNode;

class CGlobalUnitNode
{
  public:
  CGlobalUnitNode();

  const CUnitNode& unitNode() const;

  private:
  CGlobalUnitNode(const CGlobalUnitNode& rhs);
  CGlobalUnitNode& operator = (const CGlobalUnitNode& rhs);

  static const CUnitNode& retrieveGlobalUnitNode();

  const CUnitNode& m_unitNode;
};

#endif  // _GlobalUnitNode_h_
