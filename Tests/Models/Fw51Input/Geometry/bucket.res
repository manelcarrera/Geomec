     ************************ BUCKET TEST ***********************
     * Bucket engine test file                                  *
     * Testing the functionality of the bucket method           *
     ************************************************************


Kernel is in box (0.5,0.5,-1) (3.5,2.5,1)
Edgelength is 1

Point(2, 1, 0.5) in box(1, 0, 1)
Point(3, 1, -0.25) in box(2, 0, 0)
Point(3, 2, 0.25) in box(2, 1, 1)
Point(-10, -10, -10) in box(0, 0, 0)
Point(10, 10, 10) in box(2, 1, 1)
Point(2, -10, 10) in box(1, 0, 1)

Point(1, 1, -0.5) added
Point(1, 1, 0.5) added
Point(1, 2, -0.5) added
Point(1, 2, 0.5) added
Point(2, 1, -0.5) added
Point(2, 1, 0.5) added
Point(2, 2, -0.5) added
Point(2, 2, 0.5) added
Point(10, 10, 0.25) added
Point(10, -10, -0.25) added
Point(1, 1, 0.5) not added

Bucket(0, 0, 0) : ( (1, 1, -0.5) )
Bucket(0, 0, 1) : ( (1, 1, 0.5) )
Bucket(0, 1, 0) : ( (1, 2, -0.5) )
Bucket(0, 1, 1) : ( (1, 2, 0.5) )
Bucket(1, 0, 0) : ( (2, 1, -0.5) )
Bucket(1, 0, 1) : ( (2, 1, 0.5) )
Bucket(1, 1, 0) : ( (2, 2, -0.5) )
Bucket(1, 1, 1) : ( (2, 2, 0.5) )
Bucket(2, 0, 0) : ( (10, -10, -0.25) )
Bucket(2, 0, 1) : -
Bucket(2, 1, 0) : -
Bucket(2, 1, 1) : ( (10, 10, 0.25) )

Expand = 0 on (5, -5,  -2)
Points: ( (10, -10, -0.25) )
Expand = 1 on (5, -5,  -2)
Points: ( (2, 1, -0.5) (2, 1, 0.5) (2, 2, -0.5) (2, 2, 0.5) (10, -10, -0.25) (10, 10, 0.25) )
Expand = 0 on XY (0, 0, 10)
Points: ( (1, 1, -0.5) (1, 1, 0.5) )

Bounding box (0, 0, 0) (1.4, 10, 1)
Points: ( (1, 1, 0.5) (1, 2, 0.5) )
Bounding box (0, 0, 0) (6, 1, 100)
Points: ( (1, 1, 0.5) (2, 1, 0.5) )

Point (1, 1, 0.5) removed.
Points at (1, 1, 0.5) : -
Point (1, 1, 0.5) not removed.
Points at (1, 1, 0.5) : -
Point (2, 1, 0.5) removed from box (0, 0, 0)-(2, 2, 2).
Points at (2, 1, 0.5) : -
Point (2, 1, -0.5) not removed from box (0, 0, 0)-(2, 2, 2).
Points at (2, 1, -0.5) : ( (2, 1, -0.5) )

Requested for box (10,20,30) (110,70,50) for 1000 buckets
Size X = 22, Size Y = 11, Size Z = 5
Requested for box (0,0,0) (10,10,10) for 1000 buckets
Size X = 10, Size Y = 10, Size Z = 10
Requested for box (0,0,0) (10,10,0) for 100 buckets
Size X = 10, Size Y = 10, Size Z = 1
Requested for box (0,0,0) (70,70,0) for 4 buckets
Size X = 2, Size Y = 2, Size Z = 1
