     ******************* POINT TEST *****************************
     * Geometry test file                                       *
     * testing of the point functionality of the geometry lib   *
     ************************************************************
p1 := 1,1,1

p2 := 2,2,2

(p1==p2) = 0
(p1<p2) = 1
(p1>p2) = 0
p1.Distance(p2) = 1.73205
p1.SquareDistance(p2) = 3
p1 + p2 = 3,3,3

p2 - p1 = 1,1,1

p1*3.0 = 3,3,3

p1/2.0 = 0.5,0.5,0.5

p1.Rotate(geo::Zaxis,90.0)
p1 = -1,1,1

p1.Rotate(geo::CVector::Zaxis,-90.0)
p1 = 1,1,1

p1.Rotate(geo::CVector(3,8,6),33.6)
p1 = 1.01711,1.2004,0.724244

p1.Rotate(geo::CVector(3,8,6),-33.6);)
p1 = 1,1,1

mat := 
1.000000	0.000000	0.000000	1.000000	
0.000000	1.000000	0.000000	2.000000	
0.000000	0.000000	1.000000	3.000000	

p1.Transform(mat)
p1 = 2,3,4

p1.Move(geo::CVector(1,1,1))
p1 = 3,4,5

p3 = p1
p3 = 3,4,5



     ********************** END POINT TEST *********************
