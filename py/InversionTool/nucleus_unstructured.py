import numpy as np
import gauss_abscissa as gsa
import helper
import logging
# gauss_abscissa contains node locations and nodal weight for quadrature integration up to order 7

#Az,Ax,Ay=nu.quadr(trim.trimesh,self.zco,self.avthick,self.xirr,self.yirr,self.zirr,4,2)
def quadr(trim,Z,Hin,xrcv,yrcv,zrcv,ox,oz):
    '''
    Function to integrate nucleus of strain over a prism, using Gaussian quadrature.
    input:
       trim:   triangular object, describing a triangulated top surface of the reservoir
               over which the prism integration should apply
       Z:      vector with Depth-values at each node of the triangulated surface
       Hin:    average thickness of the reservoir, for each triangle in the triangulated surface
       xrcv:   vector with surface x-positions
       yrcv:   vector with surface y-positions
       zrcv:   vector with surface z-positions
       ox:     quadrature order for integration over a triangle
       oz:     quadrature order for integration over the thickness

    Kees Hindriks, 2015
    
    '''
    try:
       #select abscissa's and weights for the desired quadrature order
       wx=gsa.weight[ox-2]
       ux=gsa.abscissa[ox-2]
       wz=gsa.weight[oz-2]
       uz=gsa.abscissa[oz-2]
   
       #initiate displacement vectors
       A=[]    # vertical displacement
       B=[]    # horizontal x displacement
       C=[]    # horizontal y displacement
   
       #iterate over all triangles (nTri)
       for k in range(trim.nTri):
           nodes=trim.pTris[k]         # select node indices of current triangle
           x1=trim.pnodes[nodes[0]-1,0]# get x-coordinates of all three nodes
           x2=trim.pnodes[nodes[1]-1,0]
           x3=trim.pnodes[nodes[2]-1,0]
           y1=trim.pnodes[nodes[0]-1,1]# get y-coordinates of all three nodes
           y2=trim.pnodes[nodes[1]-1,1]
           y3=trim.pnodes[nodes[2]-1,1]
   
           z1=Z[nodes[0]-1]            # get z-coordinates of all three nodes
           z2=Z[nodes[1]-1]
           z3=Z[nodes[2]-1]
   
           H=Hin[k]                    # get average thickness in current triangle
   
           nsubs=0.                    # initiate displacements
           xsubs=0.
           ysubs=0.
           
           # start Gaussian quadrature integration for current triangle
           for m in range(len(wx)):        # iterate over pseudo x-integration nodes
               Juv=(ux[m]+1.)/8.*(x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2))    # get the Jacobian of the coordinate transformation
               for n in range(len(wx)):    # iterate over pseudo y-integration nodes
                   for l in range(len(wz)):# iterate over z-integration nodes
                       x=(1.-ux[m])/2.*x1+(1.+ux[m])/2.*((1.-ux[n])/2.*x2+(1.+ux[n])/2.*x3) # get pseudo x integration node
                       y=(1.-ux[m])/2.*y1+(1.+ux[m])/2.*((1.-ux[n])/2.*y2+(1.+ux[n])/2.*y3) # get pseudo y integration node
                       
                       ## if depths are given on the 3 nodes, we can derive the actual
                       ## depth at (x,y) using barycentric coordinates
                       D0,D1,D2,D3=trim.barycentre(k+1,np.array([x,y]))
                       D=D1*z1+D2*z2+D3*z3
                       z=H/2.*(uz[l]+1.)+D
   
                       xdisp,ydisp,zdisp=forward(x,y,xrcv,yrcv,z,zrcv) # compute nucleus of strain contribution from each integration node
   
                       xsubs=xsubs+H/2.*abs(Juv)*xdisp*wx[m]*wx[n]*wz[l] # update total x-displacement resulting from current triangle 
                       ysubs=ysubs+H/2.*abs(Juv)*ydisp*wx[m]*wx[n]*wz[l] # update total y-displacement resulting from current triangle
                       nsubs=nsubs+H/2.*abs(Juv)*zdisp*wx[m]*wx[n]*wz[l] # update total z-displacement resulting from current triangle
   
           # append the displacement lists
           A.append(nsubs)
           B.append(xsubs)
           C.append(ysubs)
   
       # convert lists into vectors
       A=np.array(A)
       A=A.T
       B=np.array(B)
       B=B.T
       C=np.array(C)
       C=C.T
   
       return A,B,C
    except:
       helper.handle_exception()

def forward(xsrc,ysrc,xrec,yrec,D,zrcv):
    '''
    forward unit impulse nucleus of strain function
    input:
        xsrc x-coordinate of nucleus
        ysrc y-coordinate of nucleus
        xrec x-coordinate of free surface location
        yrec y-coordinate of free surface location
        D    depth of nucleus
        zrcv depth of free surface
    '''
    try:
       subs=xrec*0.
       xdisp=xrec*0.
       ydisp=xrec*0.
   
   
       offset=np.power(xsrc-xrec,2)+np.power(ysrc-yrec,2)
       Z=float(D)-np.array(zrcv)
       #Z=float(D)
       subs=subs+Z/pow(offset+pow(Z,2),3./2.)
   
       xdisp=xdisp+(xsrc-xrec)/pow(offset+pow(Z,2),3./2.)
       ydisp=ydisp+(ysrc-yrec)/pow(offset+pow(Z,2),3./2.)
   
       return xdisp,ydisp,subs
    except:
       helper.handle_exception()
