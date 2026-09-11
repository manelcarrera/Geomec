import numpy as np


class sparse():
    def __init__(self,A,thresh=1e-15):
        
        A=np.array(A)
        shp=A.shape

        if len(shp)==2: # it is a matrix
            self.nr=shp[0]
            self.nc=shp[1]


            self.I={}
            self.S={}
            self.nn=0
            for m in range(self.nr):
                val=[]
                indx=[]
                for n in range(self.nc):
                    if abs(A[m,n])>thresh:
                        val.append(A[m,n])
                        indx.append(n)
                        self.nn=self.nn+1
                self.I[m]=indx
                self.S[m]=val

    def T(self):

        row=self.I.keys()

        col={}
        val={}
        nc=self.nc
        nr=self.nr
        for m in range(self.nc):
            col[m]=[]
            val[m]=[]

        for m in row:
            indx=self.I[m]
            values=self.S[m]

            for n in range(len(indx)):
                col[indx[n]].append(m)
                val[indx[n]].append(values[n])

        self.I=col
        self.S=val

        self.nr=nc
        self.nc=nr

    def mult(self,vec):

        if len(vec)!=self.nc:
            print "dimensions do not agree"
            result=-1
        else:
            result=np.zeros((self.nr,))
            for m in self.I.keys():
                indx=self.I[m]
                val=self.S[m]
                for n in range(len(indx)):
                    result[m]=result[m]+val[n]*vec[indx[n]]

        return result

    def Tmult(self,vec):

        if len(vec)!=self.nr:
            print "dimensions do not agree"
            result=-1
        else:
            result=np.zeros((self.nc,))
            for m in self.I.keys():
                indx=self.I[m]
                val=self.S[m]
                for n in range(len(indx)):
                    result[indx[n]]=result[indx[n]]+val[n]*vec[m]

        return result
            
        

    def full(self):
        A=np.zeros((self.nr,self.nc))
        for m in self.I.keys():
            indx=self.I[m]
            val=self.S[m]
            for n in range(len(indx)):
                A[m,indx[n]]=val[n]

        return A

class Csparse():
    def __init__(self,A,thresh=1e-15):
        
        A=np.array(A)
        shp=A.shape

        if len(shp)==2: # it is a matrix
            self.nr=shp[0]
            self.nc=shp[1]


            self.I={}
            self.S={}
            for m in range(self.nr):
                val=[]
                indx=[]
                for n in range(self.nc):
                    if abs(A[m,n])>thresh:
                        val.append(A[m,n])
                        indx.append(n)
                self.I[m]=indx
                self.S[m]=val

    def T(self):

        row=self.I.keys()

        col={}
        val={}
        nc=self.nc
        nr=self.nr
        for m in range(self.nc):
            col[m]=[]
            val[m]=[]

        for m in row:
            indx=self.I[m]
            values=self.S[m]

            for n in range(len(indx)):
                col[indx[n]].append(m)
                val[indx[n]].append(values[n].real-1j*values[n].imag)

        self.I=col
        self.S=val

        self.nr=nc
        self.nc=nr

    def mult(self,vec):

        if len(vec)!=self.nc:
            print "dimensions do not agree"
            result=-1
        else:
            result=np.zeros((self.nr,))*(1.+1j)
            for m in self.I.keys():
                indx=self.I[m]
                val=self.S[m]
                for n in range(len(indx)):
                    result[m]=result[m]+vec[indx[n]]*val[n]

        return result

    def Tmult(self,vec):

        if len(vec)!=self.nr:
            print "dimensions do not agree"
            result=-1
        else:
            result=np.zeros((self.nc,))*(1.+1j)
            for m in self.I.keys():
                indx=self.I[m]
                val=self.S[m]
                for n in range(len(indx)):
                    result[indx[n]]=result[indx[n]]+(val[n].real-1j*val[n].imag)*vec[m]

        return result

    def full(self):
        A=np.zeros((self.nr,self.nc))*(1.+1j)
        for m in self.I.keys():
            indx=self.I[m]
            val=self.S[m]
            for n in range(len(indx)):
                A[m,indx[n]]=val[n]

        return A


def Tdiag(A,vec):
        if len(vec)!=A.nr:
            print "dimensions do not agree"
            result=-1
        else:
            B=sparse(np.ones((2,2)))
            B.I=A.I.copy()
            B.S=A.S.copy()
            B.nc=A.nc
            B.nr=A.nr
            B.nn=A.nn

            for m in B.I.keys():
                indx=B.I[m]
                for n in range(len(indx)):
                    B.S[m][n]=B.S[m][n]*vec[m]

        return B

def diag(A,vec):
        if len(vec)!=A.nc:
            print "dimensions do not agree"
            result=-1
        else:
            B=sparse(np.ones((2,2)))
            B.I=A.I.copy()
            B.S=A.S.copy()
            B.nc=A.nc
            B.nr=A.nr
            B.nn=A.nn

            for m in B.I.keys():
                indx=B.I[m]
                val=B.S[m]
                for n in range(len(indx)):
                    B.S[m][n]=B.S[m][n]*vec[indx[n]]

        return B
            
        
        
