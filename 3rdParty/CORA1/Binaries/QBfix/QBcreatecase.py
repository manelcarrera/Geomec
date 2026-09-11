#
# QBcreatecase
#
# by Famke Kraaijeveld
#
#
# FK20131105 Unneccessary imports deleted as it clashed with QB UI
# FK20121201 logging added
# FK20120819 Fault evaluation moved to QBevalcase.py; GoCad file is created instead
# FK20120817 Switch response type moved to GUI or QBbatchcall
# FK20120728 Enforcing integers for keywords ending with x.n, y.n, segH, segV
# FK20120530 timestamp bug solved
# FK20120530 respparam replaced by "fileprops[loiinfo[0]+'_failureMechanism'"
# FK20120530 Output resposes are: "Stress", "Strain", "VirginStress", "Displacement"
#

"""
-----------------------
By Famke Kraaijeveld 2011 - 2012

""In:"" ascii file with basic settings, basecase file or template with basecase parameters, case specific parameters

""Out:"" QuickBlocks input file; updated ascii file

Program creates a single scenario case for QuickBlocks. It assumes a geometry built in Quickblocks with
	- constant rock properties
	- in situ conditions or incremental change
	- either constant depletion properties or a file
	- one or multiple reservoirs

CORA will supply a file with basic settings, base case parameters and case specific paramaters. 
Depending on the choice of response parameter, output parameter is set on the next possible locations in global coordinates (Northing, Easting, Depth)
	- A well (line): a starting point, direction (azimuth, dip), a length (from point in direction Depth); or ps file
	- A fault (plane): a corner node, two directions, width & height (assumed is normal in N,E plane of start coordinate); or ps file
	- A formation (volume): formation name; or a corner node, two widths & height
	- A layer (depth plane): array of depths for output 
	
"""
# FK20131105 Unneccessary imports deleted as it clashed with QB UI
import sys
import os
import logging

#----------------------------------------------------------------------
# Intermediate file reader
# Later xml
#----------------------------------------------------------------------
def read_intermed_file(filename, start_tag=None, end_tag=None):    
    """   
    Reads a standard property file using # as comment and key value definition
    separated by =. Quotes are trimmed out when reading.
    
    :param filename: the file to read from
    :type filename: str
    :param start_tag: tag where starting to read the file
    :type start_tag: str
    :param end_tag: tag where to stop reading the file
    :type end_tag: str
    :rtype: property dictionnary
	
	Written by Frank van der Heyden.
    """

    props = dict()
    try:
        with open(filename, 'r') as fh:
            logging.debug("Reading %s from %s to %s" % (filename, start_tag, end_tag))
            if start_tag is not None:
                # find the stag
                for line in fh:
                    if line.startswith(end_tag):
                        break
                else:
                    # not found, use all the file
                    raise ValueError("%s was not found in your input file."  % start_tag)
            # parse the values
            for line in fh:
                if end_tag is not None and line.startswith(end_tag):
                    return props
                if line.startswith("#") or len(line.strip()) == 0:
					continue
					
                tokens = line.split('=')                
                if len(tokens) >=  2:
					key=tokens[0].strip().strip('"')
					val=tokens[1].strip().strip('"')
					# FK20120728 Enforcing integers for keywords ending with x.n, y.n, segH, segV
					if (key.endswith('x.n') or key.endswith('y.n')):
						newval = int(val)
					elif (key.endswith('_segV') or key.endswith('_segH')):
						newval = int(float(val))
					else:
						try: 
							newval = float(val)
						except:
							newval = str(val)

					if key not in props:
						props[key] = newval

						
    except IOError as (errno, strerror):
        logging.error("I/O error({0}): {1}. Could not open file {2}".format(errno, strerror, filename))      
    
    return props

#------------------------------------
# START LOGGING
#
#------------------------------------
# create logger
logger = logging.getLogger('cora_create')
logger.setLevel(logging.INFO)

#create formatter and add it to the handlers
formatter = \
   logging.Formatter("%(asctime)s - %(filename)s - %(levelname)s - %(message)s")

#set the root logger
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
ch.setFormatter(formatter)
logger.addHandler(ch)

#----------------------------------------------------------------------
# IMPORT INTERMEDIATE FILE INFORMATION
# USER DEFINITIONS
#----------------------------------------------------------------------
# read defaults
try:
	intermediatefile = sys.argv[1]
except:
	msg = 'No intermediate file name selected. Please retry running this script with name of intermediate file between quotes.'
	logger.critical(msg)
	raise ValueError(msg)

try:
	fileprops = read_intermed_file(intermediatefile)
except:
	msg = 'No such file or directory: '+intermediatefile

# FK20121201 Added logger
if fileprops.has_key('logfile'):
	logfile = fileprops['logfile']
	fh = logging.FileHandler(logfile,mode='a')
	fh.setFormatter(formatter)
	logger.addHandler(fh)
	
# set default paths
QBsetuppath=fileprops['QBpath']

fixedfilespath=fileprops['scriptpath']
if fixedfilespath not in sys.path: sys.path+=[fixedfilespath]
import QBmodI as QBi
#import QBmodO as QBo
#import remove_files as rmf

casefilespath=fileprops['inputfolder']
if not os.path.exists(casefilespath):
	os.mkdir(casefilespath)
	msg = casefilespath+' has been created'
	logger.info(msg)

resultspath=fileprops['outputfolder']
if not os.path.exists(resultspath):
	os.mkdir(resultspath)
	msg = resultspath+' has been created'
	logger.info(msg)

logfile = os.path.join(resultspath,'.\logCORAQB.txt') 

# set other defaults
modelinfo = [fileprops['model'],fileprops['modeltype']]
casename = fileprops['casename']
loiinfo = [fileprops['loiname'],fileprops['loitype']]

# FK20120817 Switch response type moved to GUI or QBbatchcall
if fileprops.has_key(loiinfo[0]+'_OutputParam'):
	QBOutputParam = fileprops[loiinfo[0]+'_OutputParam']
else:
	msg = 'ERROR Output parameter not understood for compontent '+loiinfo[0]+'. Please choose "Stress", "VirginStress", "Strain" or "Displacement".'
	logger.critical(msg)
	raise ValueError(msg)


# ________________________________________________
# MODEL INFO
# _________________________________________________
msg = 'Adding basic model information and settings to work dictionary'
logger.debug(msg)
baseQB = {}
baseQB = QBi.add_defaults(baseQB, modelinfo, fixedfilespath, casefilespath, resultspath)

# ________________________________________________
# RESPONSE 
# _________________________________________________
pressQB = {}
if fileprops.has_key('timestamp'):
	if not fileprops['timestamp'] == '':
		for resname in baseQB['resnames']:
			# Only for CORAv0; assuming colnames are in Excel sheet
			# FK20120530 update for QBv2.0
			pressQB['fw.res.'+resname+'.loading.dP.ps.name'] = fileprops['timestamp']
			
		if fileprops.has_key('casefullname'):
			casefullname = fileprops['casefullname']
		else:
			casename = casename+'_'+ fileprops['timestamp']
			fileprops['casename'] = casename

baseQB['OutputParam'] = QBOutputParam
baseQB.update(pressQB)

if baseQB['modelinfo'][1]=='template':
	baseQB['insitu'] = fileprops['insitu']

outQB = QBi.create_outputprop(baseQB)

# ________________________________________________
# SCENARIO
# _________________________________________________
# create QB input keys if not using a QB gui file
# Only if model is template, fft and other settings are adjusted
# Furthermore, only template allows for geometry change
msg = 'Adding case specific information to work dictionary'
logger.debug(msg)

caseQB = QBi.create_case(baseQB,fileprops)
caseQB.update(outQB)
caseQB.update(pressQB)

if caseQB['modelinfo'][1]=='template': 
	caseQBtemp = QBi.create_QBinput(caseQB,casename)
	caseQB.update(caseQBtemp)

# _________________________________________________
# LOCATIONS
# _________________________________________________
msg = 'Adding location specific information to work dictionary'
logger.debug(msg)

caseQB['loiinfo'] = loiinfo
if not caseQB['loiinfo'][0]=='':
	
	# get filename
	loi = caseQB['loiinfo'][0]
	if fileprops.has_key('casefullname'):
		casefullname = fileprops['casefullname']
	else:
		# casename already updated with timestamp if applicable
		casefullname = casename+'_'+loi
	
	# adding filenames to QB dictionary
	loiQB = QBi.create_singleLOI(caseQB,loiinfo,casefullname)
	caseQB.update(loiQB)
else:
	# defaults in file should be used
	if caseQB['modelinfo'][1]=='template':
		msg = 'No output locations defined'
		raise ValueError(msg) 
	else:
		# no output locations, so default in file are used
		if fileprops.has_key('casefullname'):
			casefullname = fileprops['casefullname']
		else:
			# casename already updated with timestamp if applicable
			casefullname = casename

# _________________________________________________
# CREATE QB INPUT FILES
# _________________________________________________
# create QB input files
msg = 'Creating new QuickBlocks input file '+ casefullname
logger.debug(msg)
basecase = caseQB['modelinfo'][0]
caseinputname = QBi.create_caseinputfile(casefullname,caseQB,basecase,casefilespath)

# _________________________________________________
# UPDATE INTERMEDIATE
# _________________________________________________
filename = intermediatefile
fileprops['caseinputname'] = caseinputname
ffile = open(filename, 'a') 
ffile.writelines('caseinputname = '+ caseinputname + '\n')
ffile.close()

# FK20120819 Fault evaluation moved to QBevalcase.py; GoCad file is created instead
# for loitypes = fault, add normal and heart to intermediate file
if loiinfo[1] == 'Fault' and (not caseQB.has_key(loiinfo[0]+'_file')):
	ffile = open(filename, 'a') 
	ffile.writelines(loiinfo[0]+'_GoCadFile = ' + caseQB[loiinfo[0]+'_GoCadFile'] + '\n')
	ffile.writelines(loiinfo[0]+'_GoCadFaultName = ' + caseQB[loiinfo[0]+'_GoCadFaultName'] + '\n')
	ffile.close()
 	
# for loitypes formations and wells
# FK20120530 arrays are written as comma separated lists
# FK20120530 respparam is replaced by loiinfo[0]+'_failureMechanism'
if (loiinfo[1] == 'Formation'): 
	if ((fileprops[loiinfo[0]+'_failureMechanism'] == 'Shear failure') or (fileprops[loiinfo[0]+'_failureMechanism'] == 'Virgin Shear failure')):
		properties = ['cohesion','fricangle']
		labels = ['_Coh','_phi']
		alabels = ['_acoh','_aphi']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Pore collapse'):
		properties = ['strength']
		labels = ['_S1lim']
		alabels = ['_aS1lim']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Tensile failure'):
		properties = ['strength']
		labels = ['_S3lim']
		alabels = ['_aS3lim']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Compression failure'):
		properties = ['strength']
		labels = ['_E1lim']
		alabels = ['_aE1lim']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Extension failure'):
		properties = ['strength']
		labels = ['_E3lim']
		alabels = ['_aE3lim']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Compaction failure'):
		properties = ['strength']
		labels = ['_Ezmax']
		alabels = ['_aEzmax']
	elif (fileprops[loiinfo[0]+'_failureMechanism'] == 'Elongation failure'):
		properties = ['strength']
		labels = ['_Ezmin']
		alabels = ['_aEzmin']
	
	faildict = QBi.get_PropertyPerDepth(fileprops,properties,labels)
	caseQB.update(faildict)
	
	ffile = open(filename, 'a') 
	for indexprop in range(0,len(properties)):
		ffile.writelines(loiinfo[0] + alabels[indexprop] + ' = ' + str(list(caseQB[properties[indexprop]])).strip('[]') + '\n')
	
	ffile.writelines(loiinfo[0]+'_adepth = ' + str(list(caseQB['depthsout'])).strip('[]')  + '\n')
	ffile.writelines(loiinfo[0]+'_afbounds = ' + str(list(caseQB['formbounds'])).strip('[]')  + '\n')
	ffile.close()
	
	
# ________________________________________________
# END LOGGING
# _________________________________________________
#
msg = 'New QuickBlocks input file '+caseinputname+' successfully created'
logger.info(msg)

if fileprops.has_key('logfile'):
	fh.close()