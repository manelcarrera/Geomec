// colornode.h: interface for the CColorNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORNODE_H__D3F47E88_C6C3_4E3B_BAC2_83B9D3DC314D__INCLUDED_)
#define AFX_COLORNODE_H__D3F47E88_C6C3_4E3B_BAC2_83B9D3DC314D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "openglnode.h"

#ifndef _WINDEF_
typedef unsigned long COLORREF;
#endif  // _WINDEF_

class CColorNode : public COpenGLNode  
{
public:
  // Using the color factory ...
  CColorNode(CFemAppModel& model);
  CColorNode(const QString &strName, CFemAppModel& model, COLORREF nColor);
  CColorNode(const unsigned int uName, CFemAppModel& model, COLORREF nColor);
  CColorNode(const CColorNode &rhs);

  // Not using the color factory ...
  CColorNode(const QString &strName, CFemAppModel& model);
  CColorNode(const unsigned int uName, CFemAppModel& model);

  // Destruction ...
  virtual ~CColorNode();
  
  // Color access ...
  virtual TColor Color() const;
  void Color(TColor nColor);

  // Save and load
  virtual bool Empty() const;
  virtual long SavedItems() const;

  // Streaming
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  // Assignment
  CColorNode& operator=(const CColorNode &rhs);
  bool operator==(const CColorNode &rhs) const;

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitColorNode(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitColorNode(*this); }

private:
  COLORREF m_nColor;
};

#endif // !defined(AFX_COLORNODE_H__D3F47E88_C6C3_4E3B_BAC2_83B9D3DC314D__INCLUDED_)
