     ******************* VECTOR TEST ****************************
     * Geometry test file                                       *
     * testing of the vector functionality of the geometry lib  *
     ************************************************************
v1 := 1,0,0

v2 := 0,1,0

v1.CrossProduct(v2) = 0,0,1

v2.CrossProduct(v1) = 0,0,-1

v2.DotProduct(v1) = 0
v1.AngleDeg(v2) = 90
v2.AngleDeg(v1) = 90
v1.Length() = 1
v2.Length() = 1
(v1 == v2) = 0
v1.UnitVector() = 1,0,0

v1 + v2 = 1,1,0

v1 - v2 = 1,-1,0

v1*3.0 = 3,0,0

v1/2.0 = 0.5,0,0

v3 := 1,0,0

v3.Flip()
v3 = -1,0,0

v3 := v1
v3 := 1,0,0

v3.Rotate(v1.CrossProduct(v2),v1.AngleDeg(v2))
v3 = 0.00000E+00,1,0

v3 := v2
v3 := 0,1,0

v3.Rotate(v2.CrossProduct(v1),v2.AngleDeg(v1))
v3 = 1,0.00000E+00,0

mat = v1.GetRotationMatrix(90)
mat = 
1.000000	0.000000	0.000000	0.000000	
0.000000	0.000000	-1.000000	0.000000	
0.000000	1.000000	0.000000	0.000000	
0.000000	0.000000	0.000000	1.000000	

v3 := v2
v3 := 0,1,0

v3.Transform(mat)
v3 = 0,0.00000E+00,1

v4 := 2,8,9

v3=v4*2.0
v3 = 4,16,18

v3.IsParallel(v4) = 1
v3.Flip()
v3 = 4,16,18

v3.IsParallel(v4) = 1


     ********************** END VECTOR TEST ********************
