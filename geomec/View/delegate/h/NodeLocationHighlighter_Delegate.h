#ifndef _NodeLocationHighlighter_Delegate_h_
#define _NodeLocationHighlighter_Delegate_h_

class CPointHighlightNode;

#include "OpenGLNode_Delegate.h"
#include "NodeLocationHighlighter.h"

class CNodeLocationHighlighter_Delegate
{
public:
  class CPointHighlightNode_Delegate : public COpenGLNode_Delegate
  {
  public:
  CPointHighlightNode_Delegate(
      CNodeLocationHighlighter::CPointHighlightNode* pointHighlightNode);

  private:
  CPointHighlightNode_Delegate(const CPointHighlightNode_Delegate& rhs);
  CPointHighlightNode_Delegate& operator =
      (const CPointHighlightNode_Delegate& rhs);

  CNodeLocationHighlighter::CPointHighlightNode* m_pointHighlightNode;

  REGISTER_DELEGATE(CNodeLocationHighlighter::CPointHighlightNode,
      CNodeLocationHighlighter_Delegate::CPointHighlightNode_Delegate);
  };

public:
  CNodeLocationHighlighter_Delegate(
  CNodeLocationHighlighter* nodeLocationHighlighter);

private:
  CNodeLocationHighlighter_Delegate(
  const CNodeLocationHighlighter_Delegate& rhs);
  CNodeLocationHighlighter_Delegate& operator =
  (const CNodeLocationHighlighter_Delegate& rhs);

  CNodeLocationHighlighter* m_nodeLocationHighlighter;
};

#endif  // _NodeLocationHighlighter_Delegate_h_
