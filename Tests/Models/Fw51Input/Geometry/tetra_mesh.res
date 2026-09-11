     ********************** TETRA MESH TEST *********************
     * Mesh test file                                           *
     * Testing the functionnality of the tetra mesh             *
     ************************************************************

Testing the contains function of the tetrahedron.
Point(69.208,-10.1999,115.471) is in tetra : 
P0 (100,0,100)
P1 (36.5693,-21.0115,131.87)
P2 (25,-100,-100)
P3 (87.5,-100,-100)

Point(92.4334,-60.5329,-21.0657) is in tetra : 
P0 (100,0,100)
P1 (36.5693,-21.0115,131.87)
P2 (25,-100,-100)
P3 (87.5,-100,-100)

Point(92.4334,-60.5329,-21.0657) is in tetra : 
P0 (100,0,100)
P1 (36.5693,-21.0115,131.87)
P2 (25,-100,-100)
P3 (87.5,-100,-100)

Point(69.208,-10.1999,115.471) is in tetra : 
P0 (100,0,100)
P1 (36.5693,-21.0115,131.87)
P2 (25,-100,-100)
P3 (87.5,-100,-100)

Point(48.6254,34.4056,332.933) is not in tetra : 
P0 (100,100,100)
P1 (22.8006,37.0027,239.576)
P2 (45.8227,30.8272,345.64)
P3 (26.9347,92.215,364.815)

Point(36.9096,59.7957,354.689) is not in tetra : 
P0 (100,100,100)
P1 (22.8006,37.0027,239.576)
P2 (45.8227,30.8272,345.64)
P3 (26.9347,92.215,364.815)

Point(48.6254,34.4056,332.933) is not in tetra : 
P0 (100,100,100)
P1 (22.8006,37.0027,239.576)
P2 (45.8227,30.8272,345.64)
P3 (26.9347,92.215,364.815)

Point(69.208,-10.1999,115.471) is in tetra : 
P0 (0,0,100)
P1 (36.5693,-21.0115,131.87)
P2 (25,-100,-100)
P3 (100,0,100)

Point(49.383,32.7639,350.916) is not in tetra : 
P0 (45.8227,30.8272,345.64)
P1 (97.1655,59.679,358.511)
P2 (150,87.5,500)
P3 (150,25,500)

Point(49.383,32.7639,350.916) is not in tetra : 
P0 (45.8227,30.8272,345.64)
P1 (97.1655,59.679,358.511)
P2 (150,87.5,500)
P3 (150,25,500)

Insert point(100,100,0) at index 0
Insert point(100,100,100) at index 1
Insert point(0,100,100) at index 2
Insert point(0,100,0) at index 3
Insert point(0,0,100) at index 4
Insert point(0,0,0) at index 5
Insert point(100,0,100) at index 6
Insert point(100,0,0) at index 7
Insert point(50,50,50) at index 8

Insert TE12L (0,2,8,1) at index 0
Insert TE12L (5,7,0,6) at index 1
Insert TE12L (4,5,8,6) at index 2
Insert TE12L (3,5,0,2) at index 3
Insert TE12L (5,0,8,6) at index 4
Insert TE12L (4,2,8,5) at index 5
Insert TE12L (2,0,8,5) at index 6
Insert TE12L (6,8,4,1) at index 7
Insert TE12L (1,2,8,4) at index 8
Insert TE12L (0,1,8,6) at index 9

ElementSize = 10
PointSize = 9
Min = (0,0,0)
Max = (100,100,100)

Point 0 connected to element 0, 1, 3, 4, 6, 9
Point 1 connected to element 0, 7, 8, 9
Point 2 connected to element 0, 3, 5, 6, 8
Point 3 connected to element 3
Point 4 connected to element 2, 5, 7, 8
Point 5 connected to element 1, 2, 3, 4, 5, 6
Point 6 connected to element 1, 2, 4, 7, 9
Point 7 connected to element 1
Point 8 connected to element 0, 2, 4, 5, 6, 7, 8, 9

Point in mesh testing including edge

Point (0,0,0) in mesh
Point (50,50,50) in mesh
Point (150,0,200) not in mesh
Point (0.1,0.1,0.1) in mesh
Point (50,25,10) in mesh
Point (100,100,100) in mesh

Point in body testing

Point (0,0,0) in body 1, 2, 3, 4, 5, 6
Point (50,50,50) in body 0, 2, 4, 5, 6, 7, 8, 9
Point (150,0,200) not in mesh
Point (0.1,0.1,0.1) in body 2, 4, 5, 6
Point (50,25,10) in body 1
Point (100,100,100) in body 0, 7, 8, 9

Testing bodygroup 

Body 0 added to body group.
Body 1 added to body group.
Body 2 added to body group.
Body 3 added to body group.
Body 4 added to body group.
Body 5 added to body group.
Body 6 added to body group.
Body 7 added to body group.
Body 8 added to body group.
Body 9 added to body group.

Point in body group testing including edge

Point (0,0,0) in mesh
Point (50,50,50) in mesh
Point (150,0,200) not in mesh
Point (0.1,0.1,0.1) in mesh
Point (50,25,10) in mesh
Point (100,100,100) in mesh

Point in body group testing excluding edge

Point (0,0,0) not in mesh
Point (50,50,50) in mesh
Point (150,0,200) not in mesh
Point (0.1,0.1,0.1) in mesh
Point (50,25,10) in mesh
Point (100,100,100) not in mesh

Point in bodygroup testing

Point (0,0,0) in body 1, 2, 3, 4, 5, 6
Point (50,50,50) in body 0, 2, 4, 5, 6, 7, 8, 9
Point (150,0,200) not in body group
Point (0.1,0.1,0.1) in body 2, 4, 5, 6
Point (50,25,10) in body 1
Point (100,100,100) in body 0, 7, 8, 9

Line intersection with bodygroup

Line (25,-50,25) Line (25,125,25) intersects
P0 (25,0,25) P1 (25,100,25) 

Line (50,-50,75) Line (50,125,75) intersects
P0 (50,0,75) P1 (50,100,75) 

Line (5,5,-25) Line (5,5,125) intersects
P0 (5,5,0) P1 (5,5,100) 

Line (-50,0,25) Line (150,100,25) intersects
P0 (0,25,25) P1 (100,75,25) 

