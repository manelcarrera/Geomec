 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// GoCadVolume.h: interface for the CGoCadVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADVOLUME_H__E67DE0E8_A657_43D5_9728_53A32876884B__INCLUDED_)
#define AFX_GOCADVOLUME_H__E67DE0E8_A657_43D5_9728_53A32876884B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FvGocadHeader.h"
#include "BodyGroup.h"

#include "FieldValueFileExports.h"

class FIELDVALUEFILE_EXPORT CGoCadVolume : public geo::CBodyGroup
{
  typedef std::map<int, int> TPtIndexMap; // map with two indices: the first index is the index
                                          // of the point read in from the file, the second index is
                                          // the index that is returned after registering the point.
  TPtIndexMap m_mpPtInd;
  
  typedef std::vector<int> TPtIndexVec;
  TPtIndexVec m_nodes;
  
  CGoCadHeader *m_pHeader;
public:
  CGoCadVolume(geo::IMesh& mesh);
  virtual ~CGoCadVolume();

  const CGoCadHeader &Header() const {assert(m_pHeader); return *m_pHeader;}
  CGoCadHeader &Header() {assert(m_pHeader); return *m_pHeader;}
  const QString& Name() const {assert(m_pHeader); return m_pHeader->Name();}

  void Header(const QString &sName);
  bool isPoint(int nIndex) const;
  bool InsertPoint(const int nIndex, const double &dX, const double &dY, const double &dZ);
  int InsertTetrahedron(const int nIndex1, const int nIndex2, const int nIndex3, const int nIndex4);
  int VolumeIndex(const int nFileIndex) const; // returns the index of the point after registering

  virtual int PointSize() const;
  virtual const geo::IPoint &Point(int nIndex) const;
  const geo::IPoint& GetGoCadPoint( int nIndex ) const;

};

#endif // !defined(AFX_GOCADVOLUME_H__E67DE0E8_A657_43D5_9728_53A32876884B__INCLUDED_)
