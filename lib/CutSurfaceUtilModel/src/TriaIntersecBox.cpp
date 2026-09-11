// TriaIntersecBox.cpp: implementation of the CTriaIntersecBox class.
//
//////////////////////////////////////////////////////////////////////

#include <cassert>

#include "TriaIntersecBox.h"
#include "TriangleSurface.h"
#include "Plane.h"
#include "math.h"
#include "Line.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




// a helper class for inserting and removing poins with refcount
// see TriaHelper and segment hepler 
class ConnectivityHelper
{
public:


  //owner is owner of the points
  ConnectivityHelper(bool owner):m_owner(owner)
  {
    point_insert_count=0;
  }

  virtual ~ConnectivityHelper()
  {
    if(m_owner)
    {
      PointIndexMap::iterator it = pointmap.begin();
      geo::IPoint* p;
      while(it != pointmap.end())
      {
        p = const_cast<geo::IPoint*>(it->first);
        
        pointmap.erase(it);
        delete p;
        it = pointmap.begin();
      }
    }

  }

  //get the point index or -1 if not exist
  int GetIndex(const geo::IPoint& point) const
  {
    PointIndexMap::const_iterator  it = pointmap.find(&point);
    if(it == pointmap.end())
      return -1;
    return it->second;
  }

  //insert a copy of the point if not exist
  //sets the refcount to zero if created, (does not increase refcount, see AddTriangle or AddSegment in derived class)
  //returns the index (index of new point or existing point)
  int InsertPoint(const geo::IPoint& point)
  {
          
    PointIndexMap::iterator  it = pointmap.find(&point);
    if(it == pointmap.end())
    {
      pointmap.insert(std::make_pair(new geo::CPoint(point),point_insert_count));
      it = pointmap.find(&point);

      indexmap.insert(std::make_pair(point_insert_count,it->first));
      refpointmap.insert(std::make_pair(it->first,0));
      point_insert_count++;
      assert(it != pointmap.end());
    }
    return it->second;
  }


  //transform all points
  void TransformDirection(const geo::IPlane& from, const geo::IPlane& to)
  {
    geo::CVector n1       = from.Normal().UnitVector();			
    geo::CVector n2       = to.Normal().UnitVector();			
    geo::CVector RotVec   = n1.CrossProduct(n2);	//get the rotation vector

    geo::CMatrix TM;

    //get rotation matrix
    if( !(RotVec == geo::CVector::NullVector) )
    {
      double RotAngle = n1.AngleDeg(n2);	//get the rotation angle
      RotVec.GetRotationMatrix(RotAngle,TM);
    }
    else
    {
      TM.CreateIdentityMatrix(4);
    }

    
    geo::CPoint p = from.BasePoint();
    p.Transform(TM);
    

    //get translation matrix if needed
    if(to.Distance(p) > EPS)
    {
      geo::CMatrix tmp;
      tmp.CreateIdentityMatrix(4);
      geo::CPoint pp = to.Project(p);
      geo::CVector trans = geo::CVector(pp-p);

      tmp.Value(0, 3, trans.X());
      tmp.Value(1, 3, trans.Y());
      tmp.Value(2, 3, trans.Z());
      TM = tmp*TM; //mult. trans*rot
    }
    
    Transform(TM);
  }


  //transform all points
  void Transform(const geo::IMatrix &matrix)
  {
    PointIndexMap::const_iterator it = pointmap.begin();
    geo::IPoint* p;
    while(it != pointmap.end())
    {
      p = const_cast<geo::IPoint*>(it->first);
      p->Transform(matrix);
      it++;
    }
  }

  typedef  const geo::IPoint* TConstPoint;

  //for storing in a map
  class IPointLess 
  {
  public:
    bool operator()(  const TConstPoint&  p1 ,  const TConstPoint&  p2) const
    {
      return *p1 < *p2;
    }

  
  };

  
  typedef std::map<TConstPoint, int, IPointLess> PointIndexMap;

  //for iterating points
  const PointIndexMap& PointMap() const {return pointmap;}

protected:

  //am i the owner of the points
  bool m_owner;


  typedef std::map<int, TConstPoint > IndexPointMap;

  void AddRefCount(const geo::IPoint* p)
  {
    //increase refcount for this point
    refpointmap[p]++;
  }


  void ReleaseRefCount(const geo::IPoint* p)
  {
    //if refcount =0
    //this point is not used anymore,remove and delete this point
    int& count = refpointmap[p];
    count--;
    if(count==0)
    {
      refpointmap.erase(p);
      int index = pointmap[p];
      pointmap.erase(p);
      indexmap.erase(index);
      delete p;
    }
  }

  //every time a point is inserted this counter is increased
  //to make sure every point gets a unique index
  int point_insert_count;

  //keep track of refcount for each point
  PointIndexMap refpointmap;

  //to get a index from a given point
  PointIndexMap pointmap;

  //to get an point for given index
  IndexPointMap indexmap;

};




CTriaIntersecBox::~CTriaIntersecBox()
{
  TUserPointMap::iterator itus;
  for(itus=m_UserPointsOnRib.begin();itus != m_UserPointsOnRib.end();itus++)
  {
    std::vector<const geo::IPoint*>& vec = itus->second;
    for(size_t i=0;i<vec.size();i++)
    {
      geo::IPoint* p = const_cast<geo::IPoint*>(vec[i]);
      delete p;
    }
  }

  TPointRibMap::iterator itpr;
  for(itpr=m_PointRibMap.begin();itpr != m_PointRibMap.end();itpr++)
  {
    std::vector<const geo::IPoint*>& vec = itpr->second;
    for(size_t i=0;i<vec.size();i++)
    {
      geo::IPoint* p = const_cast<geo::IPoint*>(vec[i]);
      delete p;
    }
  }


  TSegmentMap::iterator itsm;
  for(itsm=m_SegmentMap.begin();itsm != m_SegmentMap.end();itsm++)
  {
    std::vector<CSegmentOnSide>& vec = itsm->second;
    for(size_t i=0;i<vec.size();i++)
    {
      CSegmentOnSide segment = vec[i];
      delete segment.p1;
      delete segment.p2;
    }
  }

}

const std::vector<SIDE_ID>& CTriaIntersecBox::GetSides() const
{
  return m_Sides;
}


const std::vector<const geo::IPoint*>& CTriaIntersecBox::GetIntersecPointsOnRib(const TRib& rib) const
{
  TPointRibMap::const_iterator it =  m_PointRibMap.find(rib);
  assert(it != m_PointRibMap.end());
  return it->second; //const_cast<CTriaIntersecBox*>(this)->m_PointRibMap[rib];
}


const std::vector<CSegmentOnSide>&  CTriaIntersecBox::GetSegmentsOnSide(SIDE_ID id) const
{
  return const_cast<CTriaIntersecBox*>(this)->m_SegmentMap[id];
}


void CTriaIntersecBox::SetExtraPointsOnAllRibs(int /*nrOfPoints*/ /*=10*/)
{
  const TRibs& ribs = GetRibs();
  assert(ribs.size() ==12);

  for(size_t i=0;i<ribs.size();i++)
    SetExtraPointsOnRib(ribs[i]);

}


/*static*/ void CTriaIntersecBox::GetPointsOnRib(const geo::IBox& box, std::vector<const geo::IPoint*>& vec,  const TRib& rib,int nrOfPoints)
{
  assert(nrOfPoints >1);

  geo::CLine rib_line = GetLine(box, rib);

  //geo::CLine rib_line = plane1.Intersection(plane2);
  assert(!rib_line.Empty());

  double len = rib_line.Length();
  double delta = len/double(nrOfPoints-1);

  int i;
  for(i=0;i<nrOfPoints;i++)
  {
    geo::CPoint* p = new geo::CPoint(rib_line.First() + rib_line.Dir().UnitVector()*(double(i)*delta));
    vec.push_back(p);
  }
}



void CTriaIntersecBox::SetExtraPointsOnRib(const TRib& rib,int nrOfPoints)
{

  std::vector<const geo::IPoint*>& vec = m_UserPointsOnRib[rib];
  assert(vec.size()==0); //call this only once per rib

  GetPointsOnRib(m_box, vec, rib, nrOfPoints);
}


geo::CTriSurface* CTriaIntersecBox::GetSideSurface(SIDE_ID id) const
{
  std::map<SIDE_ID,geo::CTriSurface*>::const_iterator it = m_SideSurfaces.find(id);
  assert(it != m_SideSurfaces.end());
  return it->second;
}

const std::map <SIDE_ID, geo::CTriSurface*>&
  CTriaIntersecBox::getSideSurfaces() const
{
  return m_SideSurfaces;
}

std::vector<const geo::IPoint*> CTriaIntersecBox::GetPointsOnRib(const TRib& rib) const
{
  std::vector<const geo::IPoint*> ret;
  const std::vector<const geo::IPoint*>& vec = GetIntersecPointsOnRib(rib);

  //use helper to avoid duplicates
  ConnectivityHelper help_set(false);

  for(size_t i=0;i<vec.size();i++)
  {
    help_set.InsertPoint(*vec[i]);
  }

  TUserPointMap::const_iterator usrpit =  m_UserPointsOnRib.find(rib);
  assert(usrpit != m_UserPointsOnRib.end());
  

  const std::vector<const geo::IPoint*>& vec_usrp = usrpit->second;
  for(size_t i=0;i<vec_usrp.size();i++)
  {
    help_set.InsertPoint(*vec_usrp[i]);
  }

  ConnectivityHelper::PointIndexMap::const_iterator it= help_set.PointMap().begin();

  const geo::IPoint* p;
  while(it != help_set.PointMap().end())
  {
    p = it->first;
    it++;
    ret.push_back(p);
  }

  return ret;
}

//helper to select all sides
void CTriaIntersecBox::AllSides()
{
  m_Sides.clear();
  for(int i =0;i<6;i++)
    m_Sides.push_back((SIDE_ID)i);

}

//helper to select all sides exept top and bottom
void CTriaIntersecBox::AllSidesExeptTopAndBottom()
{
  m_Sides.clear();
  for(int i =0;i<6;i++)
    if(i != SIDE_ID_TOP && i != SIDE_ID_BOTTOM)
      m_Sides.push_back((SIDE_ID)i);

}

//helper function to get a rib for two sides given
TRib GetRib(SIDE_ID SideID1,SIDE_ID SideID2)
{
  const TRibs& ribs = GetRibs();
  assert(ribs.size()==12);

  for(size_t i=0;i<ribs.size();i++)
  {
    const TRib& rib=ribs[i];

    if((rib.first == SideID1 && rib.second==SideID2) || (rib.first == SideID2 && rib.second==SideID1))
    {
      return rib;
    }
  }

  assert(false);
  return ribs[0];
}



//helper function to get a side plane from a box
geo::CPlane GetPlane(const geo::IBox& box, SIDE_ID SideID)
{
  enum geo::IHexahedron::eFaceName facename;
  switch(SideID)
  {
  case SIDE_ID_TOP:
    {
      facename = geo::IHexahedron::ZETA1;
      break;
    }
  case SIDE_ID_BOTTOM:
    {
      facename = geo::IHexahedron::ZETA2;
      break;
    }
  case SIDE_ID_LEFT:
    {
      facename = geo::IHexahedron::ETA1;
      break;
    }
  case SIDE_ID_RIGHT:
    {
      facename = geo::IHexahedron::ETA2;
      break;
    }
  case SIDE_ID_FRONT:
    {
      facename = geo::IHexahedron::KSI1;
      break;
    }
  case SIDE_ID_BACK:
    {
      facename = geo::IHexahedron::KSI2;
      break;
    }
  default:
    assert(false);
    return geo::CPlane();
    break;
  }

  const geo::IFace& face = box.Face(facename);
  return geo::CPlane(face.Point(0), face.Normal());
}

geo::CLine  GetLine(const geo::IBox& box,TRib rib)
{
  const TRibs& ribs = GetRibs();
  if(rib == ribs[0])
    return geo::CLine(box.Point(0), box.Point(3));
  else if(rib == ribs[1])
    return geo::CLine(box.Point(3), box.Point(7));
  else if(rib == ribs[2])
    return geo::CLine(box.Point(7), box.Point(4));
  else if(rib == ribs[3])
    return geo::CLine(box.Point(4), box.Point(0));
  else if(rib == ribs[4])
    return geo::CLine(box.Point(1), box.Point(2));
  else if(rib == ribs[5])
    return geo::CLine(box.Point(2), box.Point(6));
  else if(rib == ribs[6])
    return geo::CLine(box.Point(6), box.Point(5));
  else if(rib == ribs[7])
    return geo::CLine(box.Point(5), box.Point(1));
  else if(rib == ribs[8])
    return geo::CLine(box.Point(4), box.Point(5));
  else if(rib == ribs[9])
    return geo::CLine(box.Point(7), box.Point(6));
  else if(rib == ribs[10])
    return geo::CLine(box.Point(3), box.Point(2));
  else if(rib == ribs[11])
    return geo::CLine(box.Point(0), box.Point(1));

  assert(false);
  return geo::CLine();
}

//get the ribs of a ortho box
const TRibs& GetRibs()
{
  static TRibs ribs;

  if(!ribs.size())
  {
    ribs.push_back(TRib(SIDE_ID_FRONT,SIDE_ID_TOP));
    ribs.push_back(TRib(SIDE_ID_FRONT,SIDE_ID_RIGHT));
    ribs.push_back(TRib(SIDE_ID_FRONT,SIDE_ID_BOTTOM));
    ribs.push_back(TRib(SIDE_ID_FRONT,SIDE_ID_LEFT));

    ribs.push_back(TRib(SIDE_ID_BACK,SIDE_ID_TOP));
    ribs.push_back(TRib(SIDE_ID_BACK,SIDE_ID_RIGHT));
    ribs.push_back(TRib(SIDE_ID_BACK,SIDE_ID_BOTTOM));
    ribs.push_back(TRib(SIDE_ID_BACK,SIDE_ID_LEFT));

    ribs.push_back(TRib(SIDE_ID_LEFT,SIDE_ID_BOTTOM));
    ribs.push_back(TRib(SIDE_ID_BOTTOM,SIDE_ID_RIGHT));
    ribs.push_back(TRib(SIDE_ID_RIGHT,SIDE_ID_TOP));
    ribs.push_back(TRib(SIDE_ID_TOP,SIDE_ID_LEFT));
  }
  return ribs;
}

TRibs CTriaIntersecBox::GetRibsOnSide(SIDE_ID SideID)
{

  TRibs ret;
  const TRibs& ribs = GetRibs();

  switch(SideID)
  {
  case SIDE_ID_TOP:
    {
    ret.push_back(ribs[0]);
    ret.push_back(ribs[11]);
    ret.push_back(ribs[4]);
    ret.push_back(ribs[10]);
    break;
    }
  case SIDE_ID_BOTTOM:
    {
    ret.push_back(ribs[2]);
    ret.push_back(ribs[8]);
    ret.push_back(ribs[6]);
    ret.push_back(ribs[9]);
    break;
    }
  case SIDE_ID_LEFT:
    {
    ret.push_back(ribs[3]);
    ret.push_back(ribs[8]);
    ret.push_back(ribs[7]);
    ret.push_back(ribs[11]);
    break;

    }
  case SIDE_ID_RIGHT:
    {
    ret.push_back(ribs[1]);
    ret.push_back(ribs[9]);
    ret.push_back(ribs[5]);
    ret.push_back(ribs[10]);
    break;

    }
  case SIDE_ID_FRONT:
    {
    ret.push_back(ribs[0]);
    ret.push_back(ribs[1]);
    ret.push_back(ribs[2]);
    ret.push_back(ribs[3]);
    break;

    }
  case SIDE_ID_BACK:
    {
    ret.push_back(ribs[4]);
    ret.push_back(ribs[5]);
    ret.push_back(ribs[6]);
    ret.push_back(ribs[7]);
    break;
    }
  default:
    assert(false);
  }

  return ret;

}

//helper
static  geo::CPoint GetInsertIntersecPoint(const geo::IPoint* p1, const geo::IPoint* p2, const geo::CPlane* plane)
{
  geo::CLine line(*p1,*p2);
  assert(!plane->IsParallel(line));
  geo::CPoint p = geo::CPoint(plane->Intersection(line));
  return p;
}

// a helper class used by TriaSurfImpl see below
class Tria
{
public:
  Tria(const geo::IPoint*p1,
    const geo::IPoint* p2,
    const geo::IPoint* p3,
    int i1,int i2,int i3)
  {
    assert(i1 !=i2);
    assert(i2 !=i3);
    assert(i3 !=i1);
    
    assert(*p1 !=*p2);
    assert(*p2 !=*p3);
    assert(*p3 !=*p1);

    index[0]=i1,index[1]=i2,index[2]=i3;
    p[0]=p1,p[1]=p2,p[2]=p3;
  }
  const geo::IPoint* p[3];
  int index[3];
};


// a helper class initialized with a CTriSurface.
// this structure is used for for quick acces (adding and removing) of points and triangles.
// call CreateSurface() to create a CTriSurface from this structure 
class TriaSurfImpl : public ConnectivityHelper
{
public:

  //create from element set
  TriaSurfImpl(const geo::ISurface::TElementSet& elementset,bool owner):ConnectivityHelper(owner)
  {
    point_insert_count=0;
    AddElementSet(elementset);
  }

  bool inserted;

  //create from surface
  TriaSurfImpl(const geo::ISurface& surf,bool owner ):ConnectivityHelper(owner)
  {
    point_insert_count=0;

    int i;
    for(i=0 ; i< surf.ElementSize() ;i++)
    {
      const geo::IElement* element=  &surf.Element(i); 

      //triangle's only
      assert(element->NrOfPoints()==3);
      const geo::IPoint *p1 = &element->Point(0);
      const geo::IPoint *p2 = &element->Point(1);
      const geo::IPoint *p3 = &element->Point(2);
      
      int i1 = InsertPoint(*p1);
      int i2 = InsertPoint(*p2);
      int i3 = InsertPoint(*p3);
      InsertTriangle(i1,i2,i3,inserted);

    }
  }

  ~TriaSurfImpl()
  {
    //delete triangles
    if(m_owner)
    {
      TriaSet::const_iterator triit=triaset.begin();
      while(triit != triaset.end())
      {
        Tria* tria = *triit;
        delete tria;
        triaset.erase(*triit);
        triit=triaset.begin();
      }
    }
  }



  //insert elemnt set
  void AddElementSet(const geo::ISurface::TElementSet& elementset)
  {
    bool didInsert;

    for(geo::ISurface::TElementSet::const_iterator it =elementset.begin();it != elementset.end();it++)
    {
      const geo::IElement* element = *it; 

      //triangle's only
      assert(element->NrOfPoints()==3);

      const geo::IPoint *p1 = &element->Point(0);
      const geo::IPoint *p2 = &element->Point(1);
      const geo::IPoint *p3 = &element->Point(2);
      
      int i1 = InsertPoint(*p1);
      int i2 = InsertPoint(*p2);
      int i3 = InsertPoint(*p3);
      
      InsertTriangle(i1,i2,i3, didInsert);

    }
  }

  //create a surface from this structure
  geo::CTriSurface* CreateSurface() const
  {
    std::vector<geo::CPoint> vcPoint;
    std::vector<std::vector<int> > vcTriangle;
    PointIndexMap::const_iterator it = pointmap.begin();
    int count=0;
    std::map<int,int> index_map;

    //fill vector with points
    while(it != pointmap.end())
    {
      int index=it->second;
      index_map.insert(std::make_pair(index,count));
      const geo::IPoint* p = it->first;
      vcPoint.push_back(geo::CPoint(*p));
      count++;
      it++;
    }
    
    //fill triangle vector
    TriaSet::const_iterator triit=triaset.begin();
    while(triit != triaset.end())
    {
      Tria* tria = *triit;
      int i1 = index_map[tria->index[0]];
      int i2 = index_map[tria->index[1]];
      int i3 = index_map[tria->index[2]];
      std::vector<int> vc;
      vc.push_back(i1);
      vc.push_back(i2);
      vc.push_back(i3);
      vcTriangle.push_back(vc);
      triit++;
    }
    return new geo::CTriSurface(vcPoint,vcTriangle);
  }

  void FillRibMap(CTriaIntersecBox& intersec) const
  {

    const TRibs& ribs = GetRibs();

    const geo::IPoint *p;

    assert(ribs.size()==12);

    for(size_t i=0;i<ribs.size();i++)
    {
      const TRib& rib=ribs[i];

      geo::CPlane plane1 = GetPlane(intersec.Box() , rib.first);
      geo::CPlane plane2 = GetPlane(intersec.Box() , rib.second);
      geo::CLine l = plane1.Intersection(plane2);
      
      assert(!l.Empty());

      PointIndexMap::const_iterator it = pointmap.begin();

      std::vector<const geo::IPoint*>& vec = intersec.m_PointRibMap[rib];
      //walk over points
      while(it != pointmap.end())
      {
        p = it->first;
        double dist1 = plane1.Distance(*p);
        double dist2 = plane2.Distance(*p);

        if(fabs(dist1) < EPS && fabs(dist2) < EPS)
        {
          geo::CPoint* np = new geo::CPoint(*p);
//					intersec.AddToTestArray("Intersection Points on Ribs",new geo::CPoint(*p));
          vec.push_back(np);
        }

        it++;
      }

    }

  }


  void FillSegmentMap(CTriaIntersecBox& intersec) const
  {


    int k;
    const geo::IPoint *p, *on1,*on2,*on3;

    //loop over the box sides
    for(size_t i=0;i < intersec.m_Sides.size();i++)
    {
      //a plane representing a box-side
      geo::CPlane plane = GetPlane(intersec.Box() , intersec.m_Sides[i]);

      std::vector<CSegmentOnSide>& vec = intersec.m_SegmentMap[(SIDE_ID)intersec.m_Sides[i]];

      //walk over triangles
      TriaSet::const_iterator triit=triaset.begin();
      while(triit != triaset.end())
      {
        Tria* tria = *triit;

        on1=0,on2=0,on3=0;

        //loop over triangle nodes 
        for( k=0;k<3;k++)
        {
          p = tria->p[k];

          //see on witch side of the plane the point is
          double dist = plane.SignedDistance(*p,true);

          if( fabs(dist) < EPS)
          {
            //point is on the plane
            if(!on1) on1=p;
            else if(!on2) on2=p;
            else if(!on3) on3=p;
          }
        }

        //two points on the plane
        if(on1 && on2)
        {

//					intersec.AddToTestArray("Segments on Side",new geo::CLine(*new geo::CPoint(*on1),*new geo::CPoint(*on2)));

          CSegmentOnSide seg1(new geo::CPoint(*on1),new geo::CPoint(*on2));
          vec.push_back(seg1);
          
          if(on3)
          {
//						intersec.AddToTestArray("Segments on Side and triangle",new geo::CLine(*new geo::CPoint(*on1),*new geo::CPoint(*on3)));
            assert(false); //testing
            CSegmentOnSide seg2(new geo::CPoint(*on1),new geo::CPoint(*on3));
            vec.push_back(seg2);

//						intersec.AddToTestArray("Segments on Side and triangle",new geo::CLine(*new geo::CPoint(*on2),*new geo::CPoint(*on3)));
            CSegmentOnSide seg3(new geo::CPoint(*on2),new geo::CPoint(*on3));
            vec.push_back(seg3);
          }
        }

        triit++;

      }//while(triit != triaset.end())
      
    }//end for(int i=0;i<intersec.m_Sides.size();i++)
  }


  //remove a trinagle
  //and free points if not used
  void RemoveTriangle(Tria* tri)
  {
    assert(tri);
    TriaSet::iterator it = triaset.find(tri);
    assert(it != triaset.end());
    triaset.erase(it);

    ReleaseRefCount(tri->p[0]);
    ReleaseRefCount(tri->p[1]);
    ReleaseRefCount(tri->p[2]);
    delete tri;
  }


  typedef std::set<Tria*> TriaSet;


  //inser a new triangle
  //increase reference count for points
  Tria* InsertTriangle( int i1,int i2,int i3,bool& didInsert )
  {
    assert(i1 >-1);
    assert(i2 >-1);
    assert(i3 >-1);

    const geo::IPoint* p1 = indexmap.find(i1)->second;
    const geo::IPoint* p2 = indexmap.find(i2)->second;
    const geo::IPoint* p3 = indexmap.find(i3)->second;

    assert(p1);
    assert(p2);
    assert(p3);

    Tria* tri = new Tria(p1,p2,p3,i1,i2,i3);
    std::pair<TriaSet::iterator,bool> ins =  triaset.insert(tri);

    if(!ins.second)
    {
      didInsert =false;
      delete tri;
    }

  didInsert = true;
    AddRefCount(p1);
    AddRefCount(p2);
    AddRefCount(p3);
    return *ins.first;
  }
  
  

  //for iterating over triangles
  const TriaSet& GetTriaSet() {return triaset;}


private:

  //all triangles in a set
  TriaSet triaset;

};



// a helper class used by SegmentHelper see below
class Segment
{
public:
  Segment(const geo::IPoint*p1,
    const geo::IPoint* p2,
    int i1,int i2)
  {
    assert(i1 !=i2);
    assert(*p1 !=*p2);

    index[0]=i1,index[1]=i2;
    p[0]=p1,p[1]=p2;
  }

  const geo::IPoint* p[2];
  int index[2];
};


class SegmentLess
{
public:

  SegmentLess(bool use_pointer_compare)
  :m_use_pointer_compare(use_pointer_compare)
  {
  }

  bool operator()(  const Segment*  s1 ,  const Segment*  s2) const
  {

    if(m_use_pointer_compare)
    {
      //check pointers
      return s1 < s2;
    }
    else
    {
      geo::CPoint s1_min = s1->p[0]->Min(*s1->p[1]);
      geo::CPoint s2_min = s2->p[0]->Min(*s2->p[1]);
      
      geo::CPoint s1_max = s1->p[0]->Max(*s1->p[1]);
      geo::CPoint s2_max = s2->p[0]->Max(*s2->p[1]);

      //check real points
      if(s1_min < s2_min)
        return true;

      if(s1_min > s2_min)
        return false;

      if(s1_max < s2_max)
        return true;

      if(s1_max > s2_max)
        return false;
    }
    

    return false;
  }

private:
  bool m_use_pointer_compare;
};




// a helper class for segments
class SegmentHelper : public ConnectivityHelper
{
public:

  SegmentHelper(bool owner,bool use_pointer_compare=true):
    ConnectivityHelper(owner),
    segmentset(SegmentLess(use_pointer_compare))
  {

  }

  //remove
  //and free points if not used
  void RemoveSegment(Segment* seg)
  {
    assert(seg);
    SegmentSet::iterator it = segmentset.find(seg);
    assert(it != segmentset.end());
    segmentset.erase(it);

    ReleaseRefCount(seg->p[0]);
    ReleaseRefCount(seg->p[1]);
    delete seg;
  }

  typedef std::set<Segment*,SegmentLess> SegmentSet;

  bool ExistSegment( int i1,int i2 )
  {
    assert(i1 >-1);
    assert(i2 >-1);

    const geo::IPoint* p1 = indexmap.find(i1)->second;
    const geo::IPoint* p2 = indexmap.find(i2)->second;

    assert(p1);
    assert(p2);

    Segment* seg = new Segment(p1,p2,i1,i2);

    SegmentSet::iterator it =  segmentset.find(seg);

    delete seg;

    return it != segmentset.end();
  }

  //inser segment
  //increase reference count for points
  Segment* InsertSegment( int i1,int i2, bool& inserted )
  {
    assert(i1 >-1);
    assert(i2 >-1);

    const geo::IPoint* p1 = indexmap.find(i1)->second;
    const geo::IPoint* p2 = indexmap.find(i2)->second;

    assert(p1);
    assert(p2);

    Segment* seg = new Segment(p1,p2,i1,i2);

    inserted=true;

    std::pair<SegmentSet::iterator,bool> ins =  segmentset.insert(seg);

    if(!ins.second)
    {
      inserted=false;
      delete seg;
    }

    AddRefCount(p1);
    AddRefCount(p2);
    return *ins.first;
  }
  

  //for iterating over segments
  const SegmentSet& GetSegmentSet() {return segmentset;}

private:
  //all segments in a set
  SegmentSet segmentset;
};



/*static*/ geo::CTriSurface* CreateSurfaceFromElementSet(const geo::ISurface::TElementSet& elementset)
{
  TriaSurfImpl mimic(elementset,true);
  return mimic.CreateSurface();
}



//cut triangle and remove triangles from mimic
void CTriaIntersecBox::GetIntersecSurfaceWithPlane(const geo::CPlane& plane ,  TriaSurfImpl& mimic)
{

  //keep track of triangles to be removed, to remove them later
  std::vector<Tria*> tria_remove;

  bool inserted;
  int k;
  const geo::IPoint *p, *out1,*out2,*out3,*in1,*in2,*on1,*on2,*on3;

  TriaSurfImpl::TriaSet::const_iterator it;


  //walk over triangles of mimic
  for(it = mimic.GetTriaSet().begin() ; it != mimic.GetTriaSet().end() ; it++)
  {
    Tria* tria = *it;

    out1=0;out2=0;out3=0;in1=0;in2=0,on1=0,on2=0,on3=0;

    //loop over triangle nodes 
    for(k=0;k<3;k++)
    {
      p = tria->p[k];

      //see on witch side of the plane the point is
      // plane's normal points outward, so negative distance is inside
      double dist = plane.SignedDistance(*p,true);

      if( fabs(dist) < EPS)
      {
        //point is on the plane
        if(!on1) on1=p;
        else if(!on2) on2=p;
        else if(!on3) on3=p;
      }
      else if(dist < 0)
      {
        //point is inside the plane
        if(!in1) in1 = p;
        else in2 = p;
      }
      else
      {
        //point is outside the plane
        if(!out1) out1 = p;
        else if (!out2) out2 = p;
        else out3=p;
      }
    }

    //see with situation we have, points inside,outside or on the plane etc..
    if(out1 && out2 && out3)
    {
      //3 points out remove triangle
      tria_remove.push_back(tria);
    }
    else if(on1 && out1 && in1 )
    {
      //one point in, one point out and one point on
      geo::CPoint p1 = GetInsertIntersecPoint(in1,out1,&plane);

      assert(!p1.Empty());

      int ia1 = mimic.InsertPoint(p1);

      int i1 = mimic.GetIndex(*in1);
      int ion1 = mimic.GetIndex(*on1);

      assert(i1>-1 && ia1>-1 && ion1>-1);
      
      mimic.InsertTriangle(i1,ia1,ion1,inserted);
      tria_remove.push_back(tria);
    }
    else if(in1 && in2 && out1)
    {
      //two points inside and one outside
      assert(!out2);
      
      assert(*in1 !=*in2);
      assert(*in2 !=*out1);
      assert(*out1 !=*in1);

      //p1 and p2 are the intersection points with the element and the plane
      geo::CPoint p1 = GetInsertIntersecPoint(in1,out1,&plane);
      geo::CPoint p2 = GetInsertIntersecPoint(in2,out1,&plane);

      assert(!p1.Empty() && !p2.Empty());

      //AddToTestArray("points", new geo::CPoint(p1));
      //AddToTestArray("points", new geo::CPoint(p2));
      
      if(p1 !=p2)
      {
        int ia1 = mimic.InsertPoint(p1);
        int ia2 = mimic.InsertPoint(p2);
        int i1 = mimic.GetIndex(*in1);

        mimic.InsertTriangle(i1,ia1,ia2,inserted);

        int i2 = mimic.GetIndex(*in2);
        mimic.InsertTriangle(i1,i2,ia2,inserted);
        tria_remove.push_back(tria);
      }

    }
    else if(out1 && out2 && in1)
    {
      //two points outside and one inside
      assert(!in2);

      //p1 and p2 are the intersection points with the element and the plane
      geo::CPoint p1 = GetInsertIntersecPoint(out1,in1,&plane);
      geo::CPoint p2 = GetInsertIntersecPoint(out2,in1,&plane);

      assert(!p1.Empty() && !p2.Empty());
      //intersec.AddToTestArray("points", new geo::CPoint(p1));
      //intersec.AddToTestArray("points", new geo::CPoint(p2));

      int i1 = mimic.GetIndex(*in1);
      int ia1 = mimic.InsertPoint(p1);
      int ia2 = mimic.InsertPoint(p2);

      mimic.InsertTriangle(i1,ia1,ia2,inserted);
      tria_remove.push_back(tria);
    }
    else if(on1 && on2 && out1)
    {
      tria_remove.push_back(tria);
    }
    else if(on1 && out1 && out2)
    {
      tria_remove.push_back(tria);
    }
    else if(on1 && on2 && on3)
    {
      tria_remove.push_back(tria);
    }
  }

  //remove triangles that are cutting or outside the plane
  for(size_t i=0 ;i<tria_remove.size();i++)
    mimic.RemoveTriangle(tria_remove[i]);

}

void CTriaIntersecBox::FilterSurfaceElements(const geo::ISurface& surf,
                         geo::ISurface::TElementSet& elements_on_the_side,
                         geo::ISurface::TElementSet& /*elements_strict_inside*/,
                         geo::ISurface::TElementSet& elements_strict_outside)
{
  int i;
  for(i = 0; i < surf.ElementSize(); ++i)
  {
    if(m_box.BoundingBoxOutside(surf.Element(i)))
      elements_strict_outside.insert(&surf.Element(i));
    else
      elements_on_the_side.insert(&surf.Element(i));
  }
}




bool CTriaIntersecBox::AddSurfaceToCut(const std::string& name , const geo::ISurface& surf)
{

  //groups of elements
  geo::ISurface::TElementSet elements_on_the_side;
  geo::ISurface::TElementSet elements_strict_inside;
  geo::ISurface::TElementSet elements_strict_outside;

  //first of all, filter all relevant elements ( for speed )
  FilterSurfaceElements(surf, elements_on_the_side, elements_strict_inside, elements_strict_outside);

  //surface is outside box
  if(elements_on_the_side.size()==0 && elements_strict_inside.size()==0)
    return true;

  //just create a mimic from elements that are on the side (on the side of the box intersecting with a side)
  TriaSurfImpl mimic(elements_on_the_side,true);

  //loop over the box sides
  for(size_t i=0;i<m_Sides.size();i++)
  {
    //a plane representing a box-side
    geo::CPlane plane = GetPlane(m_box, m_Sides[i]);

    //cut mimic with plane
    GetIntersecSurfaceWithPlane(plane,mimic);
  }

  //fill segment map ( before adding the inner elements for speed )
  mimic.FillSegmentMap(*this);
  mimic.FillRibMap(*this);

  //now add remaining element inside to the surface
  mimic.AddElementSet(elements_strict_inside);


  //add generated surface
  geo::ISurface* new_surf = mimic.CreateSurface();
  STNamedSurface ns(name,new_surf);
  m_CutSurfaces.push_back(ns);

  return true;
}



int CTriaIntersecBox::CutSurfacesCount() const
{
  return m_CutSurfaces.size();
}

const STNamedSurface& CTriaIntersecBox::GetCutSurfaces(int index ) const
{
  return m_CutSurfaces[index];
}

const std::vector <STNamedSurface>& CTriaIntersecBox::getCutSurfaces() const
{
  return m_CutSurfaces;
}

typedef  const geo::IPoint* TConstPoint;
//for sorting points on a line
class PointLessOnLine 
{
public:

  PointLessOnLine (const geo::CLine* line) {m_line=line;}
  PointLessOnLine (const PointLessOnLine& rhs)
  {
    m_line=rhs.m_line;
    
  }
  PointLessOnLine&	operator=(const PointLessOnLine& rhs)
  {
    m_line=rhs.m_line;
    return *this;
  }


  void SetLine(const geo::CLine* line) {m_line=line;}
  bool operator()(  const TConstPoint&  p1 ,  const TConstPoint&  p2) const
  {
    return (m_line->First().Distance(*p1) + EPS) < m_line->First().Distance(*p2) ;
  }

private:
const geo::CLine* m_line;
};

void CTriaIntersecBox::GenerateSideSurfaces()
{
  //loop over the box sides
  for(size_t j=0;j< GetSides().size();j++)
  {

    std::stringstream strs;
    strs << "segments on side" << j;


    SIDE_ID id = GetSides()[j];
    GenerateSideSurface(id);

  }

}

static void InsertIsolatedEdgeInSet(const geo::ISurface* surf ,std::set<Segment*>& once_inserted_segments,SegmentHelper& segments)
{

  int i,j;
  for(i=0 ; i< surf->ElementSize() ;i++)
  {
    const geo::IElement* element=  &surf->Element(i); 

    assert(element->NrOfPoints()==3);

    for(j=0;j<3;j++)
    {
      const geo::IPoint *p1 = &element->Point(j);
      const geo::IPoint *p2 = &element->Point((j+1)%3);

      
      int i1 = segments.InsertPoint(*p1);
      int i2 = segments.InsertPoint(*p2);

      bool inserted;

      Segment* seg = segments.InsertSegment(i1,i2,inserted);

      if(inserted)
        once_inserted_segments.insert(seg);
      else
        once_inserted_segments.erase(seg);
    }
  }

}

bool CTriaIntersecBox::LookForIsolatedEdges(geo::CPtrArray<geo::IPoint>& points , TEdgeVector& edges ) const
{
  SegmentHelper segments(true,false);

  std::set<Segment*> once_inserted_segments;

  //loop over the box sides
  for(size_t j=0;j< GetSides().size();j++)
  {

    SIDE_ID id = GetSides()[j];
    
    geo::CTriSurface* surf  = GetSideSurface(id);

    InsertIsolatedEdgeInSet(surf,once_inserted_segments,segments);

  }

  int i;
  int count = CutSurfacesCount();

  for(i=0;i<count;i++)
  {
    const STNamedSurface& ns =  GetCutSurfaces(i);
    const geo::ISurface* surf = ns.Surface;
    InsertIsolatedEdgeInSet(surf,once_inserted_segments,segments);
  }

  std::set<const geo::IPoint*> point_set;

  std::set<Segment*>::iterator it = once_inserted_segments.begin();

  while(it != once_inserted_segments.end())
  {
    Segment* seg =*it;
    const geo::IPoint* p1 = new geo::CPoint(*seg->p[0]);
    const geo::IPoint* p2 = new geo::CPoint(*seg->p[1]);

    typedef std::pair< std::set<const geo::IPoint*>::iterator, bool >  TInserter;
    
    TInserter ins1 = point_set.insert(p1);
    TInserter ins2 = point_set.insert(p2);

    if(!ins1.second)
      delete p1;

    if(!ins2.second)
      delete p2;

  
    edges.push_back(std::make_pair(*ins1.first,*ins2.first));
    it++;
  }

  std::set<const geo::IPoint*>::const_iterator itp = point_set.begin();

  while(itp != point_set.end())
  {
    const geo::IPoint* p = *itp;
    points.PushBack((geo::IPoint&)*p);
    itp++;
  }

  return once_inserted_segments.size()!=0;
}

void CTriaIntersecBox::GenerateSideSurface( SIDE_ID id )
{
  //plane representing this side
  geo::CPlane plane = GetPlane(Box(), id);

  //put all segments from this side in this helper segment set
  //to avoid duplicate points
  SegmentHelper segments(true);

  const std::vector<CSegmentOnSide>& vec1 = GetSegmentsOnSide(id);
  
  //get segments on the side of the box
  //and put them in the helper set
  for(size_t k=0 ; k<vec1.size();k++)
  {
    const CSegmentOnSide& seg = vec1[k];
    int i1 = segments.InsertPoint(*seg.p1);
    int i2 = segments.InsertPoint(*seg.p2);
    bool inserted;
    segments.InsertSegment(i1,i2,inserted);
  }

  //get the ribs for this side;
  TRibs ribs = GetRibsOnSide(id);
  assert(ribs.size() ==4);

  //get segments on the ribs of the box
  //sort them on the length of the rib
  //and put them in the helper set
  for(size_t i=0;i<ribs.size();i++)
  {
    const TRib& rib=ribs[i];

    //get the 2 planes for this rib
    geo::CPlane plane1 = GetPlane(Box(), rib.first);
    geo::CPlane plane2 = GetPlane(Box(), rib.second);

    geo::CLine rib_line = plane1.Intersection(plane2);
    assert(!rib_line.Empty());


    PointLessOnLine point_less(&rib_line);
    std::set<TConstPoint, PointLessOnLine > pointset(point_less);
  
    const std::vector<const geo::IPoint*> vec2 = GetPointsOnRib(rib);

    for(size_t l=0;l<vec2.size();l++)
    {
      pointset.insert(vec2[l]);
    }

    typedef std::set<TConstPoint, PointLessOnLine >::const_iterator TPointIt;
    TPointIt t1 = pointset.begin();
    TPointIt t2 = pointset.begin();  t2++;

    //at least two points on a rib
    assert(t1 != pointset.end());
    assert(t2 != pointset.end());

    while(t2 != pointset.end())
    {
      const geo::IPoint* p1 = *t1;
      const geo::IPoint* p2 = *t2;

      int i1 = segments.InsertPoint(*p1);
      int i2 = segments.InsertPoint(*p2);

      bool inserted;
      segments.InsertSegment(i1,i2,inserted);
      t1++;
      t2++;
    }

  }


  //put points and segments in containers for passing to CTriangleSurface later
  std::vector<geo::IPoint*> vcPoint;
  std::vector<std::pair<size_t, size_t> > vcSegments;
  ConnectivityHelper::PointIndexMap::const_iterator it = segments.PointMap().begin();
  int count=0;
  std::map<int,int> index_map;

  //fill vector with points
  while(it != segments.PointMap().end())
  {
    int index=it->second;
    index_map.insert(std::make_pair(index,count));
    geo::IPoint* p = const_cast<geo::IPoint*>(it->first);
//		AddToTestArray("Points for triangle",new geo::CPoint(*p));
    vcPoint.push_back(p);
    count++;
    it++;
  }
  
  
  //fill segment vector
  SegmentHelper::SegmentSet::const_iterator itseg = segments.GetSegmentSet().begin();
  while(itseg != segments.GetSegmentSet().end())
  {

    Segment* seg = *itseg;
    int i1 = index_map[seg->index[0]];
    int i2 = index_map[seg->index[1]];
    assert(i1 >= 0 && i1 < vcPoint.size());
    assert(i2 >= 0 && i2 < vcPoint.size());
    assert(i1 != i2);

//		AddToTestArray("Edges for triangle",new geo::CLine(*seg->p[0],*seg->p[1]));

    vcSegments.push_back(std::make_pair(i1,i2));
    itseg++;
  }

  //transform surface to XY plane for using CTriangleSurface
  segments.TransformDirection(plane,geo::CPlane::PlaneXY);	

  //create a triangulated surface
  CTriangleSurface ts;
  ts.AllowSteinerPointsOnBoundary(false);
  ts.Points(vcPoint);
  ts.Segments(vcSegments);
  //ts.CreateMeshPLSGNoQuality();
  ts.CreateMesh(20);
  //AddToTestArray("TEST SURFACE",ts);

  //create a surface back from triangulated surface
  TriaSurfImpl surf(ts,true);

  //transform surface back to orginal plane
  surf.TransformDirection(geo::CPlane::PlaneXY ,plane);

  //eventually put a created side surface in vector
  m_SideSurfaces.insert(std::make_pair(id,surf.CreateSurface()));

}

const geo::CBox& CTriaIntersecBox::Box() const
{
  return m_box;
}
