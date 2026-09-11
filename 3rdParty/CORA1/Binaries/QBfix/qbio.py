# FK20131105 unnecessary import
# FK20120705: Adopted st modules are copied:
# read_propsection				: read property file
# read_pssection2(filename) 	: read results
# write_simpleascii				: write results file
# write_ps							: used by write_simpleascii

"""Input and output related methods

The io module groups all the reading and writing of files for the quickblocks
package.

It supports reading :
 * pointsets in simpleascii, ff2 and dat format
 * property files
 
FIXME : documentation should be improved
FIXME : most of the methods requires a global cleanup for documentation and
convention
"""
# The following line is required when running with Python 2.5; it's
# redundant for Python >= 2.6.
from __future__ import with_statement


import copy
import logging
import mmap
import os
import re

# FK20131105 unnecessary import
import numpy
from numpy import arange, array, NaN, unique, hsplit, where, vstack, \
     floor, savez, savetxt, isnan, ceil, zeros, hstack, linspace,\
     meshgrid, load, inf


	 
def read_propsection(filename, messagelist=None, lspac='[\n]', stag=None, etag=None):
    """
    This function opens a text file and interprets only lines that look like
    "key=val"

    Returns a dict properties[key]=val.

    Optially a start tag (stag) and end tag (stag) can be specified.
    In that case only the lines between the tags will be interpreted.

    The mmap-route was chose because the files can consist of a huge pointset
    section and a small properties section.
    Reading the whole file turns out to be unneccessarily slow.


    :param filename: the absolute or relative path to the file that needs to be
                      read
    """

    if messagelist is None:
        messagelist = []

    properties={}
    try:
        file=open(filename, 'r+b')
    except IOError as (errno, strerror):
        logging.error(
            "I/O error({0}): {1}. "
            "Could not open file {2}".format(errno, strerror, filename)
            )
        return properties,messagelist

    mfile = mmap.mmap(file.fileno(), 0)

    if stag is None:
        # No start tag specified: start reading at the
        # beginning of the file.
        start_pos = 0
    else:
        # Start tag specified;  search for point at which
        # to start reading.
        try:
            start_tag_pos = locate_tag(mfile, stag)
        except ValueError:
            # tag not found; start reading at the end of the file.
            start_pos = mfile.size()
        else:
            # tag found; start reading at the beginning of the
            # line following the tag.
            start_pos = mfile.find('\n', start_tag_pos) + 1

    if etag is None:
        # No end tag specified: stop reading at the end of the file.
        end_pos = mfile.size()
    else:
        # End tag specified; search for point at which to stop
        # reading.
        try:
            # End tag found;  beginning of the tag is the point
            # to stop reading.
            end_pos = locate_tag(mfile, etag, start=start_pos)
        except ValueError:
            # End tag not found: keep going until the end of the file.
            end_pos = mfile.size()

    section = mfile[start_pos:end_pos]
    file.close()

    for line in section.splitlines():
        # Ignore comment lines, blank lines.
        if line.startswith('#') or not line.strip():
            continue
        elif '=' in line:
            key, value = map(str.strip, line.split('='))
            if key in properties:
                logging.warning(
                    "Duplicate key in input file: {}. "
                    "Using the first value encountered: {}.".format(
                        key, properties[key]
                    )
                )
            else:
                properties[key] = value

    return properties,messagelist
   

   
def read_pssection2(filename,filetype='simpleascii',psnames=None,messagelist=None,lspac=None,elspac=None,stag=None,etag=None,chtrig=None,ssubtag=None,esubtag=None):
    """
    This function replaces an older one which has been deleted.

    It opens a text file and tries to distinguish a pointset inside of it.

    Only the text between the optional starttag and endtag is interpreted, and within this section only the text between the optional startsubtag and endsubtag.

    An optional columnheader-trigger (chtrig) can be specified.
    If exactly 1 line exists containing an element equal to chtrig, this line is taken as the column header line.
    Otherwise, the largest pointset in the section is taken (nr of lines containing i elements times i).
    If the first line contains only floats, the columns are numbered. Otherwise the first line defines the pointset names.

    Returns a dict columnlib[psname]=array([..]) with 1D arrays and the list of psnames.

    The mmap-route was chose because several tagged pointsets can exists inside one file.
    In this way, no time is wasted reading in unneccessary sections.

    """

    if messagelist is None:
        messagelist = []

    #define type-dependent defaults
    if filetype=='simpleascii':
        linespacer,elementspacer,starttag,endtag,colheadtrigger='[\n]',None,None,None,None#linespacer='[\r\n\f]+',elementspacer='[ \t\v]+'
    if filetype=='ff2':
        linespacer,elementspacer,starttag,endtag,colheadtrigger='[\n]',None,'POINTSET','~POINTSET','Northing'
    #if filetype=='ff5':
    #    if stag is None:
    #        print 'Warning: start tag was not defined for reading ff5 pointset section. Default "FORMATION AND DEPLETION POINTSETS" was chosen.'
    #        stag='FORMATION AND DEPLETION POINTSETS'
    #    linespacer,elementspacer,starttag,endtag,colheadtrigger='[\n]',None,stag,'~'+stag,None#for ff5 type, stag is mandatory input

    #overwrite None-values with type-dependent defaults
    if lspac is None: lspac=linespacer
    #if elspac is None: elspac=elementspacer
    if stag is None: stag=starttag
    if etag is None: etag=endtag
    if chtrig is None: chtrig=colheadtrigger

    
    try:
        file=open(filename, 'r+b')
    except IOError as (errno, strerror):
        logging.error("I/O error({0}): {1}. Could not open file {2}".format(errno, strerror, filename))      
        return {},[],messagelist

    map = mmap.mmap(file.fileno(), 0)
    file.close()

    foundanytag=False
    if stag is None or map.find(stag)==-1: i1=0#mapstr1='map[:'
    else:
        i1=map.find(stag)+len(stag)#mapstr1='map['+str(map.find(starttag))+':'
        foundanytag=True
    if etag is None or map.find(etag)==-1: i2=len(map)#mapstr2=']'
    else:
        i2=map.find(etag)#mapstr2=str(map.find(endtag))+']'
        foundanytag=True

    foundanysubtag=False
    if ssubtag is None or map[i1:i2].find(ssubtag)==-1: isub1=i1#mapstr1='map[:'
    else:
        isub1=i1+map[i1:i2].find('\n'+ssubtag)+len('\n'+ssubtag)#UPDATED #mapstr1='map['+str(map.find(starttag))+':'
        foundanysubtag=True
    if esubtag is None or map[i1:i2].find(esubtag)==-1: isub2=i2#mapstr2=']'
    else:
        isub2=i1+map[i1:i2].find(esubtag)#mapstr2=str(map.find(endtag))+']'
        foundanysubtag=True

    mapstr='map['+str(isub1)+':'+str(isub2)+']'#ERROR: isub1 points to subsections=GROUP c1_1_reduced,GROUP c1_2_reduced,GROUP c1_3_reduced

    if (not foundanytag and not (stag,etag)==(None,None)) or (not foundanysubtag and not (ssubtag,esubtag)==(None,None)):#if any tag was defined but none found: section does not exist. Return empty lib.
        return {},[],messagelist

    #if mapstr=='map[:]':
        #tm=time()
    #    els=[i.split() for i in re.split(lspac,map)]
        #print 'time2:',time()-tm
    #else: els=[i.split() for i in re.split(lspac,eval(mapstr))]
    lines=re.split(lspac,eval(mapstr))
    if elspac is None:
        els=[i.split() for i in lines]#split elements around any white space
    else:
        els=[re.split(elspac,i.strip()) for i in lines]#split elements around user-specified delimiter string. Strip needed to remove whitespace-like clutter around line delimiter. 

    while els.count([]): els.remove([])
    while els.count(['']): els.remove([''])#remove empty string elements. re.split leaves these which can cause errors.
    
    #els=[array(i)[array(i)!=''].tolist() for i in els]
    #tm=time()
    lenels=[len(i) for i in els]

    if not colheadtrigger is None:
        iscolumnheaderline=[colheadtrigger in i for i in re.split(lspac,eval(mapstr))]
        if iscolumnheaderline.count(True)==1:
            icolhead=iscolumnheaderline.index(True)
            colheaders=els[icolhead]
            nrels=len(colheaders)
            i1=icolhead+1+lenels[icolhead+1:].index(nrels)
            
            tmp=copy.copy(lenels)
            tmp.reverse()
            i2=len(lenels)-1-tmp.index(nrels)
            #print 'time3:',time()-tm

            if len(set(lenels[i1:i2+1]))!=1:
                messagelist+=['Error in pointset file '+filename+': number of element not the same for all lines.']
           
    else:
        i1,i2=geti1i2(lenels)
        if i1==i2:
            messagelist+=['Error in pointset file '+filename+': no 2 consecutive lines found with the same number of elements to define a pointset.']
            return {},[],messagelist

        for i in range(i1,i2+1):
            try:#walk through the lines of the pointset and break when the first 2 elements can be converted to floats
                tmp=[float(k) for k in els[i][:2]] 
                break
            except:
                pass

        if i==i1:
            colheaders=range(lenels[i1])
        else:
            colheaders=els[i-1]
            i1=i
    
    columnlib={}
    if psnames is None:#no list of psnames provided: all pointsets (columns) will be extracted
        doubles=[colheaders.count(i)>1 for i in colheaders]
        if doubles.count(True):
            validpsnames=[]
            doublenames=where(doubles,colheaders,None)
            for i in unique(doublenames):
                if i!=None:
                    messagelist+=['Error in pointset file '+filename+': column header '+i+' appears multiple times.']
        else:
            try:
                psarr=array(els[i1:i2+1],dtype=float)
            except:
                psarr=array(els[i1:i2+1])
                psarrflags=array([[isfloat(j) for j in i] for i in els[i1:i2+1]])
                psarr=where(psarrflags,psarr,'NaN')
                print 'Converting non-numerical values in pointsets '+css(array(colheaders)[~psarrflags.all(0)])+' to "NaN"'
                psarr=array(psarr,float)

            for key,val in zip(colheaders,hsplit(psarr,psarr.shape[1])):
                columnlib[key]=val.ravel()
            validpsnames=colheaders

    else:
        validpsnames=[]
        for psname in psnames:
            #if colheaders.count(i)==0: messagelist+=['Pointset '+psname+' could not be found in pointset file '+filename]
            if colheaders.count(psname)>1: messagelist+=['Error in pointset file '+filename+': pointset '+psname+' appears multiple times.']
            #else: validpsnames+=[psname]
            if colheaders.count(psname)==1: validpsnames+=[psname]
        #tm=time()
        if len(validpsnames):
            flatels=[]
            for i in els[i1:i2+1]: flatels+=i

        for psname in validpsnames:
            i=colheaders.index(psname)
            colvals=flatels[i::len(colheaders)]

            try: columnlib[psname]=array(colvals,float)
            except:
                colarr=array(colvals)
                colflags=array([isfloat(i) for i in colvals])
                colarr=where(colflags,colarr,'NaN')
                columnlib[psname]=array(colarr,float)
        #print 'time_fewps',time()-tm

    return columnlib,validpsnames,messagelist

	
	
def geti1i2(le):
    #le should be a list (type list) of the number of elements on each line
    #returns the indices of the first and last line of the largest pointset
    #the size of the pointset is defined as the total number of elements in an area where all consecutive lines have the same number of elements
    tmp=[0]
    for i in range(1,len(le)):
        if le[i]!=le[i-1]:
            tmp+=[0]
        else:
            tmp+=[tmp[i-1]+le[i]]
            
    if tmp.count(0)==len(tmp): return le.index(max(le)),le.index(max(le))
    
    i2=tmp.index(max(tmp))
    i1=i2-max(tmp)/le[i2]
    return i1,i2
    


def write_simpleascii(fn,array2D,colnames=None,delimiter=' ',eol='\n'):
    """
    Write a simple ascii file of a 2D pointset.

    IN:
    fn: filename (including path)
    array2D: a 2D array of values.
    colnames (optional): list of column header names. If specified, this is written before the array.
    
    """


    
    try:
        file=open(fn, 'w')
    except IOError as (errno, strerror):
        logging.error("I/O error({0}): {1}. Could not create file {2}".format(errno, strerror, fn))
        return

    
    logging.info('Writing simple ascii file '+fn+'...')
    file=write_ps(file,array2D,colnames,delimiter,eol)
    file.close()
    return

	
	
def write_ps(file,array2D,colnames=None,delimiter=' ',eol='\n'):
    """
    Appends a 2D array to an opened file instance.
    """

    
    if not colnames is None:
        #file.write('#')
        for cn in colnames[:-1]:
            file.write(cn+delimiter)
        file.write(colnames[-1]+eol)
                  
    for row in array2D:
        for el in row[:-1]:
            file.write(str(el).replace('nan','NaN') + delimiter)
        file.write(str(row[-1]).replace('nan','NaN')+eol)

    return file

