// ControlManager.h: interface for the CControlManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLMANAGER_H__2F0347E4_D896_4C33_93E0_B38A54B463A5__INCLUDED_)
#define AFX_CONTROLMANAGER_H__2F0347E4_D896_4C33_93E0_B38A54B463A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CControlManager {
public:
  typedef enum {
    LEFT = 0,  // maintain offset from left
    RIGHT = 1, // maintain offset from right
    HBOTH = 2  // maintain both offsets (resize)
  } THorizontal;

  typedef enum {
    TOP = 0,    // maintain offset from top
    BOTTOM = 1, // maintain offset from bottom
    VBOTH = 2   // maintain both offsets (resize)
  } TVertical;

private:
  CWnd &m_control;
  CWnd &m_parent;
  THorizontal m_horbehaviour;
  TVertical m_verbehaviour;

  // original offset from borders
  // 0: top
  // 1: bottom
  // 2: left
  // 3: right
  int m_offset[4];

  // original size
  // 0: width
  // 1: height
  int m_size[2];

  void GetOffsets();
  void ResizeParams(int beh, int newsiz, int left, int right, int size, int &x, int &width);

public:
  CControlManager(CWnd &control, CWnd &parent, THorizontal horbehaviour, TVertical verbehaviour);
  ~CControlManager();

  // call when parent is resized
  void OnResize();
};

#endif // !defined(AFX_CONTROLMANAGER_H__2F0347E4_D896_4C33_93E0_B38A54B463A5__INCLUDED_)
