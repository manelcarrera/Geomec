 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */

//*******************************************************************
//              
//  FILE      : GeoStream.h
//  AUTHOR    : htg
//  PROJECT   : Geometry
//  DATE      : 15.10.2002
//  COMMENTS  : -
//              
//*******************************************************************


#if defined(USE_GEO_STREAM_)

#if !defined(GEO_STREAM_HEAD__INCLUDED_)
#define GEO_STREAM_HEAD__INCLUDED_


#include <set>

#include "ICoordinate.h"
#include "ILine.h"
#include "IMatrix.h"
#include "IPolyLine.h"
#include "IPlane.h"
#include "Point.h"
#include "IElement.h"
#include "ISurface.h"

#include "GeometryExports.h"

template< class T> T& operator <<(T& os , const geo::ICoordinate& coor)
{
  if(coor.Empty())
  {
    os << "Empty";
  }
  else
  {
    os <<  coor.X() << "," << coor.Y() << "," << coor.Z();
  }
  
  return os;
}


template< class T> T& operator <<( T& os , const geo::ILine& line )
{
  if(line.Empty())
  {
    os << "Empty\n";
  }
  else
  {
    os << "First  = "  << line.Point(0) << "\n";
    os << "Second = "  << line.Point(1) << "\n";
    os << "Length = "  << line.Length() << "\n";
  }
  return os;
}

template< class T> T& operator <<( T& os , const geo::IMatrix& mat )
{

  if(mat.Empty())
  {
    os << "Empty\n";
  }
  else
  {
    for(int row=0;row<mat.RowSize();row++)
    {
      for(int col=0;col<mat.ColumnSize();col++)
      {
        os << mat.Value(row,col) << "\t";
      }
      os << "\n";
    }
  }
  return os;
}


template< class T> T& operator <<( T& os , const geo::IPolyLine& polyline )
{
  if(polyline.Empty())
  {
    os << "Empty\n";
  }
  else
  {
    for(int i=0;i<polyline.LineSize();i++)
    {
      os << "Line(" << i << ") \n" ;
      os << polyline.Line(i) << "\n";
    }
  }
  
  return os;
}


template< class T> T& operator <<( T& os , const geo::IPlane& plane )
{
  if(plane.Empty())
  {
    os << "Empty\n";
  }
  else
  {
    os << "BasePoint  = "  << plane.BasePoint()  << "\n";
    os << "Normal     = "  << plane.Normal()     << "\n";
  }
  return os;
}



template< class T> T& operator <<( T& os, const std::set<geo::CPoint>& PointSet)
{
  for(std::set<geo::CPoint>::const_iterator it =PointSet.begin();it != PointSet.end() ;it++)
  {
    os << (const geo::IPoint&)(*it) << "\n";
  }

  return os;
}



template< class T> T& operator <<( T& os, const std::set<geo::CPoint*>& PointSet)
{
  for(std::set<geo::CPoint*>::const_iterator it =PointSet.begin();it != PointSet.end() ;it++)
  {
    os << (const geo::IPoint&)(**it) << "\n";
  }

  return os;
}


template< class T> T& operator <<( T& os, const geo::IElement& elem)
{
  for(int i=0; i< elem.NrOfPoints() ;i++)
  {
    os << elem.Point(i) << "\n";
  }
  return os;
}

template< class T> T& operator <<( T& os, const geo::ISurface& surface)
{
  int i;
  for(i = 0; i < surface.PointSize(); i++)
    os << "p" << i << " (" << surface.Point(i) << ")\n";

  for(i = 0; i < surface.ElementSize(); i++)
  {
    const geo::IElement& element = surface.Element(i);
    os << "ELEMENT ";
    for(int n = 0; n < element.NrOfPoints(); n++)
      os << element.PointIndex(n) << " ";
    os << "\n";
  }
  return os;
}



#endif // !defined(GEO_STREAM_HEAD__INCLUDED_)

#endif //USE_GEO_STREAM_
