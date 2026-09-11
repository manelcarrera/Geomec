#ifndef _GEOMECSPLITTERWND_H_
#define _GEOMECSPLITTERWND_H_

class CGeomecSplitterWnd : public CSplitterWnd
{
public:
  CGeomecSplitterWnd();

  CWnd* GetPaneFromPoint(const CPoint& pt);

  afx_msg BOOL OnMouseWheel(unsigned int nFlags, short zDelta, CPoint pt);
  afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

  DECLARE_MESSAGE_MAP()

private:
  bool m_bRecursionLock;
};

#endif // _GEOMECSPLITTERWND_H_
