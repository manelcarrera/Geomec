import numpy as np
'''
This module contains routines for reading data

Kees Hindriks, 2015

'''

import helper
import logging

'''
def readdata2(params):
	import logging
	logging.debug('params:{}'.format( params ) )
	return 1
'''

def readdata_temp():
    ''' read reservoir data without numpy arrays, store data as lists '''
    fin=open(filename,'r')
    
    return (coors,tris,pars,header)

def getmeasure_temp(filein):
    ''' get displacement data '''
    xirr=[]
    yirr=[]
    zirr=[]
    wdat=[]
    subs=[]
    

    fin=open(filein,'r')
    header=fin.readline().strip().split()
    coor={}
    count=0
    for m in header:
        coor[m]=count
        count+=1
    
    for m in fin.readlines():
        line=m.strip().split()
        xirr.append(float(line[coor['x']]))
        yirr.append(float(line[coor['y']]))
        zirr.append(float(line[coor['z']]))
        # Save all values (can be one or more columns)
        #val=np.array(line[3:len(line)])
        val=line[3:len(line)]
        # Convert all values to floats without using numpy
        val=[float(i) for i in val]
        subs.append(val)
        #subs.append(val.astype(np.float))
        #wdat.append(float(line[2]))
        wdat.append(4000.)
    fin.close()
    logging.debug('len(subs) in getmeasure_temp: {}'.format(len(subs)))	
    

	  #xirr=np.array(xirr)
    #yirr=np.array(yirr)
    #zirr=np.array(zirr)
    #wdat=np.array(wdat)
    #subs=np.array(subs)
    return (xirr,yirr,zirr,wdat,subs,header)

def readdata(filename):
	try:
		'''
		logging.debug('filename:{}'.format( filename ) )
		import os.path
		if os.path.exists(filename):
			logging.debug('Exists')
		else:
			logging.debug('Doesnt exists')
		'''
		fin=open(filename,'r')
		#logging.debug('fin:{}'.format( fin ) )
		#i=0
		#line=fin.readline()
		#logging.debug('line:{}'.format( line ) )
		''''
		for line in fin:
			if i < 10:
				logging.debug('i:{} line:{}'.format( i, line ) )
				i+=1
			else:
				break
		'''
		logging.debug('1')

		chck=0

		while (chck<1):
			line=fin.readline().strip().split()
			if line[0]=='PROPERTIES':
				header=line[1:len(line)]
			if line[0]=='TFACE':
				chck=10

		chck=0
		npar=len(header)
		pars=[]
		coors=[]
		line=fin.readline().strip().split()
		while (chck<1):
			coor=np.array(line[2:5])
			coors.append(coor.astype(np.float))
			val=np.array(line[5:len(line)])
			pars.append(val.astype(np.float))
			line=fin.readline().strip().split()
			if line[0]=='TRGL':
				chck=10

		coors=np.array(coors)
		pars=np.array(pars)

		chck=0

		tris=[]
		while (chck<1):
			#tri=np.array(line[1:4])
			try:
			   tris.append([int(line[1]),int(line[2]),int(line[3])])
			except:
				print line
			line=fin.readline().strip().split()
			if line[0]!='TRGL':
				chck=10


		fin.close()

		logging.debug('2')
		return coors,tris,pars,header
	except:
		helper.handle_exception()


def getmeasure(filein):
    xirr=[]
    yirr=[]
    zirr=[]
    wdat=[]
    subs=[]
    

    fin=open(filein,'r')
    header=fin.readline().strip().split()
    coor={}
    count=0
    for m in header:
        coor[m]=count
        count+=1
    
    for m in fin.readlines():
        line=m.strip().split()
        xirr.append(float(line[coor['x']]))
        yirr.append(float(line[coor['y']]))
        zirr.append(float(line[coor['z']]))
        val=np.array(line[3:len(line)])
        subs.append(val.astype(np.float))
        #wdat.append(float(line[2]))
        wdat.append(4000.)
    fin.close()
    xirr=np.array(xirr)
    yirr=np.array(yirr)
    zirr=np.array(zirr)
    wdat=np.array(wdat)
    subs=np.array(subs)
    # KB: Changed for Geomec [N, E] reading 
    return yirr,xirr,zirr,wdat,subs,header

def getmeasure2(filein):
   # KB: redundant? 
    xirr=[]
    yirr=[]
    zirr=[]
    wdat=[]
    subs=[]
    xsubs=[]
    ysubs=[]
    

    fin=open(filein,'r')
    header=fin.readline().strip().split()
    for m in fin.readlines():
        line=m.strip().split()
        xirr.append(float(line[0]))
        yirr.append(float(line[1]))
        zirr.append(float(line[2]))
        xsubs.append(float(line[3]))
        ysubs.append(float(line[4]))
        subs.append(float(line[5]))
        wdat.append(4000.)
    fin.close()
    xirr=np.array(xirr)
    yirr=np.array(yirr)
    zirr=np.array(zirr)
    wdat=np.array(wdat)
    subs=np.array(subs)
    xsubs=np.array(xsubs)
    ysubs=np.array(ysubs)
    

    return xirr,yirr,zirr,wdat,subs,xsubs,ysubs

#file_in='Reservoir_Map.ts'
#coors,tris,pars,header=readdata(file_in)
        
