// IconImageList.h: interface for the CIconImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONIMAGELIST_H__F0D316E8_99FB_41A6_BCEB_4A8A01522F39__INCLUDED_)
#define AFX_ICONIMAGELIST_H__F0D316E8_99FB_41A6_BCEB_4A8A01522F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcmn.h>
#include <map>

class CIconImageList : public CImageList {
  std::map<unsigned int, int> m_mpResToImage;

public:
  CIconImageList();
  virtual ~CIconImageList();
  int IconToImage(const unsigned int uIconID);
};

#endif // !defined(AFX_ICONIMAGELIST_H__F0D316E8_99FB_41A6_BCEB_4A8A01522F39__INCLUDED_)
