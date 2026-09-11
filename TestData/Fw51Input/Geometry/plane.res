     ******************* PLANE TEST *****************************
     * Geometry test file                                       *
     * testing of the plane functionality of the geometry lib   *
     ************************************************************


p1 = 1,0,0

p2 = 0,1,0

p3 = 0,0,1

plane1(p1,p2,p3) := 
BasePoint  = 1,0,0
Normal     = 0.57735,0.57735,0.57735

l1 := 
First  = 0,0,0
Second = 10,10,10
Length = 17.3205

plane1.Intersection(l1) = 0.333333,0.333333,0.333333

plane1.Project(geo::CPoint::NullPoint) = 0.333333,0.333333,0.333333

plane1.Distance(geo::CPoint::NullPoint) = 0.57735
plane1.SquareDistance(geo::CPoint::NullPoint) = 0.333333
plane2(geo::CPlane::PlaneXY) := 
BasePoint  = 0,0,0
Normal     = 0,0,1

plane1.Intersection(plane2) = 
First  = 1,0,0
Second = 1.57735,-0.57735,0
Length = 0.816497

plane2.Rotate(geo::CVector::Xaxis,90)
plane2.IsParallel(geo::CPlane::PlaneZX) = 1
geo::CPlane::PlaneZX.Intersection(geo::CPlane::PlaneZX) = 
Empty



     ********************** END PLANE TEST *********************
