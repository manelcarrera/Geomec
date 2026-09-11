# CG schemes
import numpy as np
import sparse as sp
import math as mt
import copy

import helper

'''
This module contains a bunch of inversion schemes:
matchedfilter --> least squares inverse in the spatial frequency domain
CG****          --> conjugate gradient schemes
lsqr            --> gradient method from from Paige and Saunders (see also Golub and Van Loan)

Kees Hindriks, 2015
'''

def masked(x,mask):
    mean=sum(x*mask)/sum(mask)
    stdv=mt.sqrt(sum(np.power(mask*(x-mean),2)/sum(mask)))

    return mean,stdv

def outliers(err,extra,frac):

    nn=int(frac*float(len(err)))

    masker=np.ones((len(err),))
    vals=np.zeros((len(err),))

    for m in range(nn):
        mean,stdv=masked(err,masker)
        test=(abs(err-mean)+abs(extra))*masker/stdv
        outl=np.argmax(test)

        masker[outl]=0.
        vals[outl]=test[outl]
        vals[outl]=1.

##    masker=masker.reshape((32,32))
##    masker2=copy.deepcopy(masker)
##    for m in range(32):
##        for n in range(32):
##            if masker[m,n]<1.:
##                masker2[m-1:m+2,n-1:n+2]=0.

    masker=1.-vals/max(vals)
    return masker
    
def matched_filt(x0,A,y,nit,nrw,aan):
    fy=np.fft.fft(np.fft.fftshift(y))
    fA=np.fft.fft(np.fft.fftshift(A))

    mf=fA.conj()/(fA*fA.conj()+1.e-15*max(abs(fA)))
    fx0=fy*mf
    #x0=np.fft.fftshift(np.fft.ifft(fx0))

    xit=[np.zeros((len(x0),)),]
    xtot=[]
    for m in range(nit):
        xx=2.*fx0.copy()
        xx[0]=xx[0]/2.
        xx[m+1:len(xx)]=0.
        xit.append(xx)

    for m in range(nrw):
        xtot.append(xit)

    wt=np.ones((len(xx),))


    return xtot,wt

    
def CGNRC(x0,A,y,nit,nrw,aan):

    wt=np.ones((len(y),))
    xtot=[]

    for m in range(nrw):

        
        r0=y-A*x0
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=A.conj()*r0
            beta=np.dot(hulp1.T.conj(),hulp1)/np.dot(old.T.conj(),old)
            p1=(hulp1+beta*p0)
            
            hulp2=A*p1
            alpha=np.dot(hulp1.T.conj(),hulp1)/np.dot(hulp2.T.conj(),hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            #check the error
            rr=mt.sqrt(np.dot(r1.T.conj(),r1)/np.dot(y.T.conj(),y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xx=np.fft.ifft(x0).real
            xx[xx<0.]=0.
            xx=np.fft.fft(xx)

            #xx=x0.copy()
            xx[len(xx)/2:len(xx)]=0.
            xx=2.*xx
            xx[0]=xx[0]/2.
            
            xit.append(xx)

        xtot.append(xit)
        x0=xx
        
            
            


    return xtot,wt

def CGNRC2(x0,A,yin,nit,nrw,aan):

    y=yin.copy()
    ytrue=np.fft.ifft(yin).real
    wt=np.diag(np.ones((len(y),)))
    wt0=np.ones((len(y),))
    xtot=[]

    ax4_2.plot(ytrue,'k')

    for m in range(nrw):

        A2=np.dot(wt,np.diag(A))
        r0=y-np.dot(A2,x0)
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=np.dot(A2.conj(),r0)
            beta=np.dot(hulp1.T.conj(),hulp1)/np.dot(old.T.conj(),old)
            p1=(hulp1+beta*p0)
            
            hulp2=np.dot(A2,p1)
            alpha=np.dot(hulp1.T.conj(),hulp1)/np.dot(hulp2.T.conj(),hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            #check the error
            rr=mt.sqrt(np.dot(r1.T.conj(),r1)/np.dot(y.T.conj(),y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xx=np.fft.ifft(x0).real
            xx[xx<0.]=0.
            xx=np.fft.fft(xx)

            #xx=x0.copy()
            xx[len(xx)/2:len(xx)]=0.
            xx=2.*xx
            xx[0]=xx[0]/2.
            
            xit.append(xx)

        xtot.append(xit)
        x0=xx
        yest=A*x0
        yest=np.fft.ifft(yest).real
        

        
        wt0=1./np.sqrt(1.+np.power(yest-ytrue,2)/sigdat/sigdat)*wt0
        wt0[wt0<0.9]=0.
        print max(wt0),min(wt0)
        y=wt0*ytrue
        ax4_2.plot(yest,'r')
        y=np.fft.fft(y)
        wt=np.fft.ifft(np.fft.fft(np.diag(wt0),axis=0),axis=1)

        
        
            
            


    return xtot,np.fft.fftshift(wt0)

def CGNR(x0,A,y,nit,nrw,aan):

    wt=np.ones((len(x0),))
    xtot=[]

    for m in range(nrw):

        A2=np.dot(A,np.diag(wt))
        r0=y-np.dot(A2,x0)
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=np.dot(A2.T,r0)
            beta=np.dot(hulp1,hulp1)/np.dot(old,old)
            p1=(hulp1+beta*p0)
            
            hulp2=np.dot(A2,p1)
            alpha=np.dot(hulp1,hulp1)/np.dot(hulp2,hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            

            #check the error
            rr=mt.sqrt(np.dot(r1,r1)/np.dot(y,y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xit.append(x0*wt)

        xtot.append(xit)
        x0=x0*wt
        x0[x0<0.]=0.
        wt=penalty(x0,sig,norm)
            
            


    return xtot,wt
    
def CGNRNN(x0,Ain,yin,extrain,nit,nrw,sigdat,thresh,scale,dwt):

    wt=np.ones((len(x0),))
    #dwt=np.ones((len(yin),))
    xtot=[]
    y=dwt*yin
    A=np.dot(np.diag(dwt),Ain)

    for m in range(nrw):
        y=dwt*yin
        A=np.dot(np.diag(dwt),Ain)

        r0=np.dot(A.T,y-np.dot(A,x0*wt))*wt
        r=r0.copy()
        d0=r0.copy()
        #x0=np.zeros((len(d0),))

        chck=1
        it=0

        xit=[x0,]

        while chck>0:
            
            it=it+1
            
            gamma=np.dot(A,d0*wt)
            alpha=np.dot(r0*wt,r0)/np.dot(gamma,gamma)
            x1   =(x0+alpha*d0)*wt
            r1   =(r0-alpha*np.dot(A.T,gamma))*wt
            beta =np.dot(r1*wt,r1)/np.dot(r0*wt,r0)
            d1   =(r1+beta*d0)*wt

            rr=mt.sqrt(np.dot(r1,r1)/np.dot(r0,r0))

            if rr<thresh or it==nit:
                chck=-1
            d0=d1.copy()
            r0=r1.copy()
            x0=x1.copy()
            xdum=x1.copy()
            #xdum[xdum<0.]=0.
            xit.append(xdum*wt)

        yest=np.dot(Ain,xdum*wt)
        xtot.append(xit)
        err=(yest-yin)*dwt
        sigdat=err.std()
        #dwt=1./np.sqrt(1.+np.power(err/sigdat,2))*dwt
        #dwt=outliers(err,extrain,.01)
        #dwt[dwt<0.9]=0.
        #wt[x0<0.]=0.
        #x0[x0<0.]=0.
        dummy=np.dot(x0,scale)
        trunc=sum(dummy)/sum(wt)*.01
        wt[dummy<=trunc]=0.
        x0[dummy<=trunc]=0.

    extra=extrain+err
        

        

    return xtot,dwt,extra,wt
def CGNRNN_NN(x0,Ain,yin,extrain,nit,nrw,sigdat,thresh,aan):

    wt=np.ones((len(x0),))
    dwt=np.ones((len(yin),))
    xtot=[]
    y=dwt*yin
    A=np.dot(np.diag(dwt),Ain)

    aset=np.ones((len(x0),))

    for m in range(nrw):
        y=dwt*yin
        A=np.dot(np.diag(dwt),Ain)

        r0=np.dot(A.T,y-np.dot(A,x0*wt))*wt
        r=np.dot(A.T,y-np.dot(A,x0*wt))*wt
        d0=np.dot(A.T,y-np.dot(A,x0*wt))*wt
        #x0=np.zeros((len(d0),))

        chck=1
        it=0

        xit=[x0,]

        while chck>0:
            
            it=it+1
            
            gamma=np.dot(A,d0*wt)
            alpha=np.dot(r0*wt,r0)/np.dot(gamma,gamma)
            x1   =(x0+alpha*d0)*wt
            imin=np.argmin(x1)
            if x1[imin]<0.:
                aset[imin]=0.
                dx=x0[imin]
                alpha=-dx/d0[imin]
                x1   =(x0+alpha*d0)*wt
                
            r1   =(r0-alpha*np.dot(A.T,gamma))*wt
            beta =np.dot(r1*wt,r1)/np.dot(r0*wt,r0)
            d1   =(r1+beta*d0)*wt

            rr=mt.sqrt(np.dot(r1,r1)/np.dot(r0,r0))

            if rr<thresh or it==nit:
                chck=-1
            d0=d1.copy()
            r0=r1.copy()
            x0=x1.copy()
            xdum=x1.copy()
            #xdum[xdum<0.]=0.
            xit.append(xdum*wt)
            wt=aset.copy()

        yest=np.dot(A,xdum*wt)
        xtot.append(xit)
        err=yest-y
        sigdat=err.std()
        #dwt=1./np.sqrt(1.+np.power(err/sigdat,2))*dwt
        dwt=outliers(err,extrain,.1)
        #dwt[dwt<0.9]=0.
        #wt[x0<0.]=0.
        #x0[x0<0.]=0.

    extra=extrain+err
        

        

    return xtot,dwt,extra

def CGNRC_SP(x0,A,y,nit,nrw,aan):

    wt=np.ones((len(y),))
    xtot=[]

    for m in range(nrw):

        
        r0=y-A.mult(x0)
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=A.Tmult(r0)
            beta=np.dot(hulp1.T.conj(),hulp1)/np.dot(old.T.conj(),old)
            p1=(hulp1+beta*p0)
            
            hulp2=A.mult(p1)
            alpha=np.dot(hulp1.T.conj(),hulp1)/np.dot(hulp2.T.conj(),hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            #check the error
            rr=mt.sqrt(np.dot(r1.T.conj(),r1)/np.dot(y.T.conj(),y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xx=np.fft.ifft(x0).real
            xx[xx<0.]=0.
            xx=np.fft.fft(xx)

            #xx=x0.copy()
            xx[len(xx)/2:len(xx)]=0.
            xx=2.*xx
            xx[0]=xx[0]/2.
            
            xit.append(xx)

        xtot.append(xit)
        x0=xx
        
            
            


    return xtot,wt

def CGNRC2_SP(x0,A,yin,nit,nrw,aan):

    y=yin.copy()
    ytrue=np.fft.ifft(yin).real
    wt=np.diag(np.ones((len(y),)))
    wt0=np.ones((len(y),))
    xtot=[]

    ax4_2.plot(ytrue,'k')

    for m in range(nrw):

        A2=Csparse(np.dot(wt,np.diag(A)))
        r0=y-A2.mult(x0)
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=A2.Tmult(r0)
            beta=np.dot(hulp1.T.conj(),hulp1)/np.dot(old.T.conj(),old)
            p1=(hulp1+beta*p0)
            
            hulp2=np.dot(A2,p1)
            alpha=np.dot(hulp1.T.conj(),hulp1)/np.dot(hulp2.T.conj(),hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            #check the error
            rr=mt.sqrt(np.dot(r1.T.conj(),r1)/np.dot(y.T.conj(),y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xx=np.fft.ifft(x0).real
            xx[xx<0.]=0.
            xx=np.fft.fft(xx)

            #xx=x0.copy()
            xx[len(xx)/2:len(xx)]=0.
            xx=2.*xx
            xx[0]=xx[0]/2.
            
            xit.append(xx)

        xtot.append(xit)
        x0=xx
        yest=A*x0
        yest=np.fft.ifft(yest).real
        

        
        wt0=1./np.sqrt(1.+np.power(yest-ytrue,2)/sigdat/sigdat)*wt0
        wt0[wt0<0.9]=0.
        print max(wt0),min(wt0)
        y=wt0*ytrue
        ax4_2.plot(yest,'r')
        y=np.fft.fft(y)
        wt=np.fft.ifft(np.fft.fft(np.diag(wt0),axis=0),axis=1)

        
        
            
            


    return xtot,np.fft.fftshift(wt0)

def CGNR_SP(x0,A,y,nit,nrw,aan):

    wt=np.ones((len(x0),))
    xtot=[]

    for m in range(nrw):

        A2=sp.diag(A,wt)
        r0=y-A2.mult(x0)
        p0=0.
        old=np.array([1,])
        k=0
        chck=-1

        # iteration scheme
        xit=[x0,]
        while chck<0:
            k=k+1
            
            hulp1=A2.Tmult(r0)
            beta=np.dot(hulp1,hulp1)/np.dot(old,old)
            p1=(hulp1+beta*p0)
            
            hulp2=A2.mult(p1)
            alpha=np.dot(hulp1,hulp1)/np.dot(hulp2,hulp2)
            x1=(x0+alpha*p1)
            r1=r0-alpha*hulp2
            

            

            #check the error
            rr=mt.sqrt(np.dot(r1,r1)/np.dot(y,y))

            if rr<thresh or k==nit:
                chck=1

            # update the vectors
            old=hulp1.copy()
            r0=r1.copy()
            x0=x1.copy()
            p0=p1.copy()
            xit.append(x0*wt)

        xtot.append(xit)
        x0=x0*wt
        x0[x0<0.]=0.
        wt=penalty(x0,sig,norm)
            
            


    return xtot,wt
    
def CGNRNN_SP(x0,Ain,yin,nit,nrw,sigdat,thresh,aan):

    wt=np.ones((len(x0),))
    dwt=np.ones((len(yin),))
    xtot=[]
    y=dwt*yin

    print len(dwt)
    print Ain.nr,Ain.nc
    A=sp.Tdiag(Ain,dwt)

    for m in range(nrw):
        y=dwt*yin
        A=sp.Tdiag(Ain,dwt)

        print len(x0*wt),A.nc,A.nr,len(A.I)

        r0=A.Tmult(y-A.mult(x0*wt))*wt
        r= A.Tmult(y-A.mult(x0*wt))*wt
        d0=A.Tmult(y-A.mult(x0*wt))*wt
        #x0=np.zeros((len(d0),))

        chck=1
        it=0

        xit=[x0,]

        while chck>0:
            
            it=it+1
            
            gamma=A.mult(d0*wt)
            alpha=np.dot(r0*wt,r0)/np.dot(gamma,gamma)
            x1   =(x0+alpha*d0)*wt
            r1   =(r0-alpha*A.Tmult(gamma))*wt
            beta =np.dot(r1*wt,r1)/np.dot(r0*wt,r0)
            d1   =(r1+beta*d0)*wt

            rr=mt.sqrt(np.dot(r1,r1)/np.dot(r0,r0))

            if rr<thresh or it==nit:
                chck=-1
            d0=d1.copy()
            r0=r1.copy()
            x0=x1.copy()
            xdum=x1.copy()
            xdum[xdum<0.]=0.
            xit.append(xdum*wt)

        yest=A.mult(xdum*wt)
        xtot.append(xit)
        dwt=1./np.sqrt(1.+np.power(yest-y,2)/sigdat/sigdat)
        dwt=outliers(err,nn)
        #dwt[dwt<0.9]=0.
        wt[x0<0.]=0.
        x0[x0<0.]=0.
        

        

    return xtot,dwt


def lsqr(x0,Ain,subsin,extrain,nit,nrw,sigdat,thresh,scale,dwt):
	'''
    lsqr
    input :  x0         initial guess
             Ain        forward operator
             subsin     measurement vector
             extrain    dummy array
             nit        number of (internal) iterations
             nrw        number of external iterations to assure non-negativity
             sigdat     dummy scalar (used as stdv of error for L1 inversion in otther schemes)
             thresh     cuttoff for error norm minimalization
             scale      dummy vector
             dwt        data weight vector (exported fro instance from external outlier detection algorithm)

    export : xtot       matrix with all solution vectors (nrw*nit vectors)
             restot     matrix with accompanying resolution vectors (nrw*nit vectors)
             atot       matrix with accompanying ritz values
             btot       matrix with accompanying ritz values
	'''
	
	try:
	
		wt=np.ones((len(x0),))

		A=np.dot(np.diag(dwt),Ain)
	
		subs=subsin*dwt

	

		for ll in range(nrw):
			x0=0.
			
			atot=[]
			btot=[]
			
			b1=np.linalg.norm(subs)
			u1=subs/b1
			v1=np.dot(A.T,u1)*wt
			a1=np.linalg.norm(v1)
			v1=v1/a1
			w1=v1
			
			barp1=b1
			barr1=a1
			D=0.
			B=0.

			vtot=[]
			utot=[]
			xtot=[]
			restot=[]
			resolution=0.

			for m in range(nit):
				u2=np.dot(A,v1)-a1*u1
				b2=np.linalg.norm(u2)
				u2=u2/b2

				v2=(np.dot(A.T,u2)-b2*v1)*wt
				a2=np.linalg.norm(v2)
				v2=v2/a2

				r1    = mt.sqrt(mt.pow(barr1,2)+mt.pow(b2,2))
				c1    = barr1/r1
				s1    = b2/r1
				angle2= s1*a2
				barr2 =-c1*a2
				p1    = c1*barp1
				barp2 = s1*barp1

				t1= p1/r1
				t2= -angle2/r1
				d1=w1/r1

				x1=x0+t1*w1*wt
				w2=v2+t2*w1*wt

				atot.append(a1)
				btot.append(b2)

				#D=D+np.linalg.norm(d1)
				#B=B+a1*a1+b2*b2

				# prepare next iteration
				u1=u2
				v1=v2
				barr1=barr2
				barp1=barp2
				w1=w2
				x0=x1
				a1=a2

				vtot.append(v1)
				utot.append(u1)

				dumu=np.zeros((len(u1),))
				dumv=np.zeros((len(v1),))
				#reorthogonalization
				if m<5:
					for n1 in range(m-1):
						dumu=dumu+np.dot(utot[n1],u1)*utot[n1]
						dumv=dumv+np.dot(vtot[n1],v1)*vtot[n1]

				u1=u1-dumu
				v1=v1-dumv

				resolution=resolution+np.outer(v1,v1)
				restot.append(np.diag(resolution))

				xtot.append(x1)

			dummy=np.dot(x1,scale)
			trunc=sum(dummy)/sum(wt)*.01
			wt[dummy<=trunc]=0.
			#x0[dummy<=trunc]=0.
			
		vtot=np.array(vtot)
		utot=np.array(utot)
		atot=np.array(atot)
		btot=np.array(btot)

		

		return xtot,restot,atot,btot
	except:
		helper.handle_exception()





