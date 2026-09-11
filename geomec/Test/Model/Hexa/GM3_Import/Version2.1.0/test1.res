***	Geomec Model Dump	***

Global initial stresses

Vertical Stress Gradient Dry   :  10
Vertical Stress Gradient Wet   :  10
Water Surface Depth            :  0
Water Column Stress Gradient   :  1
Vertical Stress Exponent       :  1


Initial Pore Pressure

Reference Pressure             :  0
Reference Depth                :  0
Pressure Gradient              :  0
Distributed outside convex hull:  1


***	TimeSteps	***
Nr of Timesteps : 2

Name : Initial
Year : 0
Name : Depletion stage 1
Year : 2000
***	Horizons	***

Horizon              : Constant depth at 0 m
    Top              :  0
    Bottom           :  0
    Depth        :  0

Horizon              : Constant depth at 100 m
    Top              :  100
    Bottom           :  100
    Depth        :  100

Horizon              : Constant depth at 200 m
    Top              :  200
    Bottom           :  200
    Depth        :  200
***	Formations	***

Name                   : Constant depth at 0 m
Min Layer Thickness	:  1 
Nr of elements			:  4 

Horizons : 
Constant depth at 0 m
Constant depth at 100 m

Name                   : Constant depth at 100 m
Min Layer Thickness	:  1 
Nr of elements			:  4 

Horizons : 
Constant depth at 100 m
Constant depth at 200 m
***	MeshRegions	***

MeshRegion: Main
    Gridsize X:  25
    Gridsize Y:  25

MeshRegion: Region1
    Gridsize X:  25
    Gridsize Y:  25
