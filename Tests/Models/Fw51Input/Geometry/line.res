

     ************************* LINE TEST ************************
     * Geometry test file                                       *
     * testing of the line functionality of the geometry lib    *
     ************************************************************


p1 := 0,0,0

p2 := 1,1,0

p3 := 1,0,0

p4 := 0,1,0

l1(p1,p2) := 
First  = 0,0,0
Second = 1,1,0
Length = 1.41421

l2(p3,p4) := 
First  = 1,0,0
Second = 0,1,0
Length = 1.41421

l1.Dir().AngleDeg(l2.Dir()) = 90
(l1==l2) = 0
(l1.IsParallel(l2)) = 0
l1.Intersection(l2) := 0.5,0.5,0

l3 := 
First  = 0,0,0
Second = 1,1,0
Length = 1.41421

(l3==l1) = 1
(l3.IsParallel(l1)) = 1
l3.Intersection(l1) = Empty

l3(p1,p3) := 
First  = 0,0,0
Second = 1,0,0
Length = 1

(l1.IsConnected(l3,Index1,Index2)) = 1
Index1 = 0
Index2 = 0
l3(p3,p1) := 
First  = 1,0,0
Second = 0,0,0
Length = 1

(l1.IsConnected(l3,Index1,Index2)) = 1
Index1 = 0
Index2 = 1
l2.Distance(geo::CPoint::NullPoint) = 0.707107
l1.Contains(p2,true) = 1
l1.Contains(p2,false) = 0
l2.Project(geo::CPoint::NullPoint) = 0.5,0.5,0

l1.SquareLength() = 2


     ********************** END LINE TEST *********************
