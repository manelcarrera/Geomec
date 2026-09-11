#
# QBmodI
#
# by Famke Kraaijeveld
#
# FK20131105 unnecessary import
# FK20130224 Bug fix: correct depths when formations when name is given
# FK20130215 Patch reservoir pointset
# FK20130131 Add surface depth and block information
# FK20121218 Distinction file or gridplanes in create_singleloi
# FK20121217 allow for 1 depth or multiple input depth
# FK20120909 Allow only 1 depth or horizontal well
# FK20120817 Clean up of write_ps module by adding generation modules 
# FK20120810 Virgin stress as output added
# FK20120716 Developer version pointset reader expects pressure files to start with POINTSET 
# FK20120716 Adding directories to qbc except for input (as pressure files etc couuld be nonlocal)
# FK20120603 Check added for existance model
# FK02052012 replaced call to quickblocks.qbio by QBpio
# FK02052012 replacing Quickblocks python routines by local copy
#
#
# Sample equals the number of data points 
# number of spacings, for segV and segH, i.e. datapoints = seg+1
#

#----------------------------------------------------------------------
# Import settings / modules
#
#----------------------------------------------------------------------
# FK20131105 Unneccessary imports deleted as it clashed with QB UI
import os
import numpy as np

#------------------------------------
# START LOGGING
#
#------------------------------------
import logging
# create logger
logger = logging.getLogger("cora.qbmodi")
if __file__.endswith('.py'):
    print 'cora.'+__file__[:-3]
	
# --------------------------------------------------------
# READING
#
# --------------------------------------------------------
	
def create_case(defaultprop, scenprop):
	""" 
	Merges Excel dictionaries for a case with base case
	
	""defaultprop"" Excel dictionary with base case
	""scenprop"" 	Name of table: with first column and row contain names
	
	"""
	
	caseprop = defaultprop.copy()
	caseprop.update(scenprop)
	
	return caseprop


	
def add_defaults(allprop, modelinfo,fixedfilespath='./QBfix',casefilespath='./QBin',resultspath='./QBout'):
	""" 
	Adds model name, layer names and reservoir names to the given dictionary allprop.
	The names of filepaths and QuickBlocks setup file are also included into this dictionary.
	
	""allprop"" 			Dictionary with dictionary of tags in Excel sheet
	""fixedfilespath""		Location of Python files which are not to be altered
	""casefilespath"" 		Destination of input files for QuickBlocks run
	""resultspath""			Destination of output files from QuickBlocks run
	""calcpath""			Location of CORA related files

	"""
	
	# FK02052012 removing QB
	import qbio as QBpio
	
	# 
	# get layer info
	fname = modelinfo[0]
	if modelinfo[1] == 'template':
		allprop['modelinfo'] = modelinfo
		msg =  'File '+fname+ ' is used as template.'
		logger.debug(msg)
		
	elif modelinfo[1] == 'model':
		allprop['modelinfo'] = modelinfo
		msg = 'File '+fname+ ' is used as basecase.'
		logger.debug(msg)
		
	else:
		# FK20120603 Check added for existance model
		msg = 'ERROR no model selected.'
		logger.critical(msg)
		raise ValueError(msg)
		
	# Change 16/1/2012: read_property_file disabled
	try:
		fileprops,ml = QBpio.read_propsection(fname)
	except:
		msg =  'File '+fname+ ' does not exist.'
		logger.critical(msg)
		raise ValueError(msg)
	
	# FK20130131 Add surface depth
	# ADD LAYER INFORMATION
	allprop['surfacedepth.datatype'] = fileprops['surfacedepth.datatype']
	if fileprops.has_key('surfacedepth.constant'):
			allprop['surfacedepth.constant'] = float(fileprops['surfacedepth.constant'])
	else: 
		msg = 'WARNING Not all formation have constant surfaces. Some features require this and might not be available'
		logger.warning(msg)
	
	layers = fileprops['layers'].replace(' ','').split(',')
	allprop['layernames'] = layers
	for layer in layers:
		allprop[layer+'.base.datatype'] = fileprops[layer+'.base.datatype']
		allprop[layer+'.base.type'] = fileprops[layer+'.base.type']
		if fileprops.has_key(layer+'.base.constant'):
			allprop[layer+'.base.constant'] = float(fileprops[layer+'.base.constant'])
		else: 
			msg = 'WARNING Not all formation have constant surfaces. Some features require this and might not be available'
			logger.warning(msg)
	# 
	# ADD RESERVOIR INFORMATION
	# add reservoirs with keywords QBv2.0
	# FK20120403 fix datatype/ key; allow dT as reservoir for nontemplates. 
	if fileprops.has_key('fw.res.groups'):
		resgroup = fileprops['fw.res.groups'].replace(' ','').split(',')
		if modelinfo[1]=='template':
			for res in resgroup:
				if fileprops['fw.res.'+res+'.depletionproperty']=='dT':
					msg = 'ERROR Uncertainty in temperature loading not supported for a template'
					logger.critical(msg)
					raise ValueError(msg)
					
	
	allprop['resnames'] = resgroup
	
	# FK20130131 Add block information
	for coord in ['x','y']:
		allprop['blocks.'+coord+'.firstcoord'] = float(fileprops['blocks.'+coord+'.firstcoord'])
		allprop['blocks.'+coord+'.size'] = float(fileprops['blocks.'+coord+'.size'])
		allprop['blocks.'+coord+'.n'] = int(fileprops['blocks.'+coord+'.n'])
	
	#
	# add paths
	allprop['fixedfilespath'] = fixedfilespath
	allprop['casefilespath'] = casefilespath
	allprop['resultspath'] = resultspath
	
	# FK20120716 Adding directories to qbc except for input (as pressure files etc couuld be nonlocal)
	allprop['dir_intermfiles'] = resultspath
	allprop['dir_fwresults'] = resultspath
	allprop['dir_invresults'] = resultspath

	return allprop


# --------------------------------------------------------
# Create model input for template with QuickBlocks v2.0
#
# --------------------------------------------------------
def create_geometry(geomprop):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to the field geometry, including output depths
	Only used for template. Only supports QuickBlocks v2.0
	
	**geomprop** 	Dictionary with Excel keywords for QuickBlocks model
	
	"""
	
	nsample = 4.

	geomdict = {}
	layers = geomprop['layernames']
	
	# get depths
	thickness = []
	if geomprop.has_key('surfacedepth.constant'):
		geomdict['surfacedepth.constant'] = geomprop['surfacedepth.constant']
	else:
		geomdict['surfacedepth.constant'] = 0.0

	thickness.append(geomprop['surfacedepth.constant'])
	for layer in layers:
		geomdict[layer+'.base.constant'] = geomprop[layer+'.base.constant'] ##2011-07-23## +'.T']
		thickness.append(geomprop[layer+'.base.constant']) ##2011-07-23## +'.T'])
	
	# 2012-01-30 FFT settings ok
	# add fft settings
	keywords = ['fft.x.n','fft.x.resolution','fft.y.n','fft.y.resolution']
	for key in keywords:
		if not geomprop.has_key(key):
			msg = 'ERROR FFT settings not found'
			logger.error(msg)
			raise ValueError(msg)
		else:
			geomdict['horzhet.'+key] = geomprop[key]
	
	return geomdict


def create_rockprop(rockprop):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to the rock properties
	Only used for template with QuickBlocks v2.0
	
	**rockprop** 	Dictionary with Excel keywords for QuickBlocks model
	
	"""

	# print 'READING FIELD PROPERTIES'
	rockdict = {}
	layers = rockprop['layernames']

	# Add elastic parameters
	# E wins over Cm
	for layer in layers:
		if rockprop.has_key(layer+'.nu.constant')&(rockprop.has_key(layer+'.E.constant')):
			rockdict[layer+'.E.constant'] = rockprop[layer+'.E.constant']
			rockdict[layer+'.nu.constant'] = rockprop[layer+'.nu.constant']
			
		elif rockprop.has_key(layer+'.Cm')&rockprop.has_key(layer+'.nu.constant'):
			Poisson = rockprop[layer+'.nu.constant']
			Cm = rockprop[layer+'.Cm']
			E =(1./Cm)*((1.+Poisson)*(1.-2.*Poisson)/(1.-Poisson))
			rockdict[layer+'.E.constant'] = E
			rockdict[layer+'.nu.constant'] = Poisson
		
		else:
			msg = 'WARNING Insufficient info to alter rock properties for layer '+layer
			logger.warning(msg)
		
	# Add in situ parameters
	sikeys = ['.si.density.constant','.si.Kh.constant','.si.KH.constant','.si.angleNH.constant','.si.Pref.constant','.si.zref.constant','.si.Pgrad.constant']
	if rockprop['insitu'] == 'yes':
		for layer in layers:
			for sikey in sikeys:
				if rockprop.has_key(layer+sikey):
					rockdict[layer+sikey] = rockprop[layer+sikey]
				else:
					msg = 'WARNING default value is used for '+layer+sikey
					logger.warning(msg)
		
	else:
		# ????? warning assuming output dse when insitu = no
		msg = 'WARNING only incremental stress changes might be calculated'
		logger.warning(msg)
	
	return rockdict	
	
	
def write_psreservoir(resprop, resname, value, colname, filename):
	""" 
	Creates pointset file with pressure data
	Only used for template
	
	**resprop** 	Dictionary with Excel keywords for QuickBlocks model
	**resname** 	Name of reservoir layer
	**value**		Output pressure value
	**colname** 	Output pressure column name
	**filename** 	Output file name
	
	"""
	
	# FK02052012 removing QB
	import qbio as QBpio
	
	# ensuring sampling of r_nblocksx x r_nblocksy in reservoir
	r1_nblocksE = resprop['blocks.x.n']
	r1_nblocksN = resprop['blocks.y.n']
	
	# get reservoir shape
	# shape: ellips/1 = circular; rect/2 = rectangular
	E0 = resprop['blocks.x.firstcoord']
	N0 = resprop['blocks.y.firstcoord']
	dE = resprop['blocks.x.size']*r1_nblocksE
	dN = resprop['blocks.y.size']*r1_nblocksN
	rshape = resprop[resname+'_shape']
	
	# Create pointset grid and values
	NresE = 3*r1_nblocksE
	NresN = 3*r1_nblocksN
	
	dx = np.linspace(E0,E0+dE,NresE+1)
	dy = np.linspace(N0,N0+dN,NresN+1)
	
	Easting, Northing = np.meshgrid(dx,dy)
	Pressure = value*np.ones(Easting.shape)
	
	# FK20130215 Patch reservoir pointset
	if rshape == 'ellips' or rshape == 1.0:	
		Emid = (dx.max()+dx.min())/2.
		Nmid = (dy.max()+dy.min())/2.
		aE = dE/2.
		bN = dN/2.
		radius = ((Easting-Emid)/aE)**2. + ((Northing-Nmid)/bN)**2.
		Pressure[np.where(radius > 1.0)] = 0.
	
	# create pressure table
	#20120217 Northin Easting changed
	colnames = ['Easting','Northing',colname]
	pscoords = np.vstack((Easting.ravel(),Northing.ravel(),Pressure.ravel())).T
	
	# write to file
	# FK20120716 Developer version pointset reader expects pressure files to start with POINTSET 
	# # FK20120705 remove write_ps and replace by write_simpleascii 
	# ffile = QBpio.write_simpleascii(filename,pscoords,colnames)
	ffile = open(filename, 'w') 
	ffile.writelines('POINTSET\n')
	ffile = QBpio.write_ps(ffile,pscoords,colnames)
	ffile.writelines('~POINTSET')
	ffile.close()
	
	msg = 'Pressure pointset has been created in '+ filename
	logger.debug(msg)
	
	return 1

	
def create_reservoirpressures(pressprop, casename):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to the depletion scenarios. 
	Pointset file with pressure data is created if neccessary
	Only used for template with QuickBlocks v2.0. Assumes only pressure change.
	
	**pressprop** 	Dictionary with Excel keywords for QuickBlocks model
	**casename** 	Name of the case	
	
	"""
	
	layers = pressprop['layernames']
	resnames = pressprop['resnames']
	casefilespath = pressprop['casefilespath']
	
	pressdict = {}
	
	# Create reservoir blocks 
	# ???? when c,y given this  is superfluous
	# 24/1/2012/ Streamlined module to calc with keys or fatal error
	keywords = ['blocks.x.firstcoord','blocks.x.n','blocks.x.size','blocks.y.firstcoord','blocks.y.n','blocks.y.size']
	for key in keywords:
		if not pressprop.has_key(key):
			msg = 'ERROR Reservoir related block properties are not all defined'
			logger.error(msg)
			raise ValueError(msg)
		else:
			pressdict[key] = pressprop[key]

	# QuickBlocks v2.0 support:
	# Here fw loading and loading.type dP is assumed
	kstart = 'fw.res.' 
	loadtype = '.loading.dP'

	# Set-up depletion pointset
	#????????? Here many assumptions on key names!
	for resname in resnames:
		if not pressprop[resname+loadtype+'.ps.filename'] == '':
			# Reservoir file is used
			pressdict[kstart+resname+loadtype+'.datatype']= 'ps'
			pressdict[kstart+resname+loadtype+'.ps.name']= pressprop[resname+loadtype+'.ps.name']
			filename = os.path.join(casefilespath+'\..',pressprop[resname+loadtype+'.ps.filename'])
			pressdict[kstart+resname+loadtype+'.ps.filename']= filename

		elif not pressprop[resname+loadtype] == '':
			# Reservoir file is created
			pressdict[kstart+resname+loadtype+'datatype']= 'ps'
			colname = 'press.' + resname
			pressdict[kstart+resname+loadtype+'.ps.name']= colname
			filename = os.path.join(casefilespath,casename+resname+'_press.dat')
			pressdict[kstart+resname+loadtype+'.ps.filename']= filename
			value = pressprop[resname+loadtype]
			pressdict[resname+'_shape'] = pressprop[resname+'_shape']
			write_psreservoir(pressdict, resname, value, colname, filename)
			
		else:
			# not a reservoir
			msg = 'WARNING '+resname+ ' is ignored as a reservoir'
			logger.warning(msg)
			pressdict[resname+'.dP.type']= 'No reservoir'
	
	return pressdict
	
	
	
def create_QBinput(allprop, casename):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to full model. 
	Pointset files are created if neccessary
	Only used for template, version ondependent
	
	**allprop** 	Dictionary with Excel keywords for QuickBlocks model
	**casename** 	Name of the case	
	
	"""
	modeldict = {}
	# set depths and fft
	geom = create_geometry(allprop)
	modeldict.update(geom)
	
	# set material parameters
	rock = create_rockprop(allprop)
	modeldict.update(rock)
	
	# set pressure parameters and block properties
	press = create_reservoirpressures(allprop, casename)
	modeldict.update(press)
	
	return modeldict
	
	
### --------------------------------------------------------    
### Set specific output parameters
### --------------------------------------------------------
def create_outputprop(outprop):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to the output properties
	This part will replace the old part in the model / template completely
	
	**outprop** 	Dictionary with Excel keywords for QuickBlocks model
	
	"""
	
	# print 'READING FIELD PROPERTIES'
	outdict = {}
	layers = outprop['layernames']

	# Keywords for both depths and export on a pointset 
	# Also depends on response parameter
	ukeys = ['uN','uE','uz']
	sefkeys = ['seEEfinal','seNNfinal','sezzfinal','seENfinal','seEzfinal','seNzfinal']#,'sepfinal','seqfinal']
	seikeys = ['seEEinit','seNNinit','sezzinit','seENinit','seEzinit','seNzinit']#,'sepfinal','seqfinal']
	dsekeys = ['dseEE','dseNN','dsezz','dseEN','dseEz','dseNz']
	dekeys = ['eEE','eNN','ezz','eEN','eEz','eNz','evol','eq']
	if outprop['OutputParam'] == 'All':
		# all fields are calculated
		okeys = ukeys + sefkeys + dsekeys + dekeys
	elif outprop['OutputParam'] == 'Strain':
		# strain field is calculated
		okeys = dekeys
	elif outprop['OutputParam'] == 'Stress':
		# stress field is calculated
		# if no insitu properties, then incr are auto calculated
		okeys = sefkeys
	# FK20120810 Virgin stress added
	elif outprop['OutputParam'] == 'VirginStress':
		# in situ stress field is calculated
		okeys = seikeys
	elif outprop['OutputParam'] == 'Displacement':
		# displacement field is calculated
		okeys = ukeys
	elif (outprop['OutputParam'] == ''):
		if (outprop['modelinfo'][1]=='template'):
			if outprop['insitu']=='yes':
				# total effective stress field is calculated
				okeys = ukeys + dekeys + sefkeys
			else:
				# effective stress change is calculated
				okeys = ukeys + dekeys + dsekeys
		else:
			# do nothing
			msg = 'WARNING Response parameter not set. Default of model will be used.'
			logger.warning(msg)
			okeys = []
	else:
		msg = 'ERROR Response parameter not set.'
		logger.critical(msg)
		raise ValueError(msg)
		
	# Add to properties
	# FK20120327 propout cancelled; CORAOUT will be added; only simple_ascii
	if not okeys == []:
		outdict['fw.data.coordgroups'] = 'CORA_OUT'
		outdict['fw.data.CORA_OUT.propgroups']=str(okeys).strip('[]').strip('\'').replace('\', \'',',')
		for okey in okeys:
			# first 3 are always the displacements
			outdict['fw.data.CORA_OUT.'+okey+'.props'] = okey
			outdict['fw.data.CORA_OUT.'+okey+'.propcoefs'] = '1.0'
			outdict['fw.data.CORA_OUT.'+okey+'.export.filetypes'] = 'simpleascii'
		
	return outdict
	
	
### --------------------------------------------------------    
### Create component specific input
### --------------------------------------------------------
def generate_line(dip, azi, length, segV, start=[0.,0.,0.]):
	
	# azi = angle with northing; clockwise with North
	# dip = angle with vertical, anti-clockwise with Depth
	lengthE=length*np.sin(dip)*np.sin(azi)
	lengthN=length*np.sin(dip)*np.cos(azi)
	lengthD=length*np.cos(dip)
	
	lengthtot = np.sqrt(lengthE*lengthE+lengthN*lengthN+lengthD*lengthD)
	
	Easting = np.linspace(start[0],start[0]+lengthE,segV+1)
	Northing = np.linspace(start[1],start[1]+lengthN,segV+1)
	Depth= np.linspace(start[2],start[2]+lengthD,segV+1)
	
	pscoords=np.array(zip(Easting, Northing, Depth))
	return pscoords
	
	
def generate_plane(dip, azi, width, segH, height, segV, start=[0.,0.,0.]):

	# azi = direction ball rolls; clockwise with North
	# dip = angle with horizontal, clockwise with Northing/Easting plane
	dx = np.linspace(0.0,width,segH+1)
	dy = np.linspace(0.0,height,segV+1)
	
	# 20120217 - Gridding Northing - Easting - Depth
	Egrid0, Ngrid0 = np.meshgrid(dx,dy)
	
	#Dgrid0 = Egrid0*0.
	
	### Step1: Rotation around Easting (dip, angle with horizontal, clockwise)
	Egrid1 = Egrid0
	Ngrid1 = Ngrid0*np.cos(dip)# - Dgrid0*np.sin(dip)
	Dgrid1 = Ngrid0*np.sin(dip)# + Dgrid0*np.cos(dip)

	### Step2: Rotation around Depthing
	Ngrid2 = Ngrid1*np.cos(azi) - Egrid1*np.sin(azi)
	Egrid2 = Ngrid1*np.sin(azi) + Egrid1*np.cos(azi)
	Dgrid2 = Dgrid1
	
	### Collected
	Easting = start[0] + Egrid2
	Northing = start[1] + Ngrid2
	Depth = start[2] + Dgrid2
	#Depth = start[3]+ dy*np.sin(dip)
	
	pscoords = np.vstack((Easting.ravel(),Northing.ravel(),Depth.ravel())).T
	
	return pscoords
		

def generate_volume(widthE, segHE, widthN, segHN, height, segV, start=[0.,0.,0.]):
	# Create grid from scratch or from data calculated above
	East = np.linspace(start[0],start[0]+widthE,segHE+1)
	North = np.linspace(start[1],start[1]+widthN,segHN+1)
	Depth = np.linspace(start[2],start[2]+height,segV+1)
	
	# 20120217 - grNorth, grEast = np.meshgrid(North, East)
	grEast, grNorth = np.meshgrid(East,North)
	Depthing = []
	Northing = []
	Easting = []
	for valdepth in Depth:
		Depthing.append(np.ones(grNorth.shape)*valdepth)
		Northing.append(grNorth)
		Easting.append(grEast)
	
		
	# 20120217 - pscoords = np.vstack((np.array(Northing).ravel(),np.array(Easting).ravel(),np.array(Depthing).ravel())).T
	pscoords = np.vstack((np.array(Easting).ravel(),np.array(Northing).ravel(),np.array(Depthing).ravel())).T
	return pscoords

	
def get_DictDepths(allprop):
	"""
	Derives the Depth coordinate of all formations in dictionary
	
	"""
	
	layers = allprop['layernames']
	
	# Get formation boundaries
	depths = [float(allprop['surfacedepth.constant'])]
	for layer in layers:
		# if thickness given
		if (allprop[layer+'.base.type'] == 'thickness'):
			newdepth = depths[-1] + float(allprop[layer+'.base.constant'])
			depths.append(newdepth) 
		
		# if base given
		elif (allprop[layer+'.base.type'] == 'base'):
			depths.append(float(allprop[layer+'.base.constant'])) 
		
		# other
		else:
			msg = 'Feature only available for models with constant formation layers (thickness or base).'
			logger.critical(msg)
			raise ValueError(msg)
		
	depthsarray = np.array(depths)
	
	return depthsarray
	
	
def get_formationinfo(allprop, loiname):
	"""
	Derives the output coordinates for given formation name. 
	It assumes sideburden equal to factor*reservoir size. Factor = 1.0
	
	"""
	
	formdict = {}
	factor = 1.0
	
	layername = allprop[loiname+'_name']
	layers = allprop['layernames']
	
	# FK20130224 Bug fix: correct depths when formations when name is given
	# get all depths = [surface, layer1_base, layer2_base ...]
	depthbounds = get_DictDepths(allprop)
	
	# Get index layername
	lindex = 999
	for i in np.arange(0,len(layers)):
		if layername==layers[i]:
			lindex = i
	
	if lindex == 999:
		msg = 'ERROR Layername for formation not recognized as layer in qbc.'
		logger.critical(msg)
		raise ValueError(msg)
		
	top = depthbounds[lindex] 
	bottom = depthbounds[lindex+1]
	
	formdict[loiname+'_D'] = top
	formdict[loiname+'_height'] = bottom-top
	
	widthE = allprop['blocks.x.n']*allprop['blocks.x.size']*(1+factor*2.)
	widthN = allprop['blocks.y.n']*allprop['blocks.y.size']*(1.+factor*2.)
	formdict[loiname+'_E'] = allprop['blocks.x.firstcoord']-widthE*0.5
	formdict[loiname+'_widthE'] = widthE
	formdict[loiname+'_N'] = allprop['blocks.y.firstcoord']-widthN*0.5
	formdict[loiname+'_widthN'] = widthN
	
	return formdict
	
	
			
def write_psloi(allprop, loiinfo, filename):
	""" 
	Creates pointset file with coordinates, i.e. location of interests
	
	""allprop"" 	Dictionary with Excel keywords for QuickBlocks model
	""loiname"" 	Name of component of interest
	""filename"" 	Output file name
	
	"""
	
	# # FK02052012 removing QB
	import qbio as QBpio
	
	geomdict = {}
	
	#............................................
	# Coordinates of interest
	#............................................
	loiname = loiinfo[0]
	
	#
	# FK20120817 Clean up of write_ps module by adding generation modules 
	# CREATE GRID FOR WELL PATH
	if loiinfo[1]=='Well':
		# azi = angle with northing; clockwise with North
		# dip = angle with vertical, anti-clockwise with Depth
		dip = allprop[loiname+'_dip']*np.pi/180.
		azi = allprop[loiname+'_azi']*np.pi/180.
		pscoords = generate_line(dip, azi, allprop[loiname+'_length'], allprop[loiname+'_segV'], [allprop[loiname+'_E'],allprop[loiname+'_N'],allprop[loiname+'_D']])
		
	# CREATE GRID FOR FAULT PLANES
	elif loiinfo[1]=='Fault':
		# azi = direction ball rolls; clockwise with North
		# dip = angle with horizontal, clockwise with Northing/Easting plane
		dip = allprop[loiname+'_dip']*np.pi/180.
		azi = allprop[loiname+'_azi']*np.pi/180.
		pscoords = generate_plane(dip, azi, allprop[loiname+'_width'], allprop[loiname+'_segH'], allprop[loiname+'_height'], allprop[loiname+'_segV'], [allprop[loiname+'_E'],allprop[loiname+'_N'],allprop[loiname+'_D']])

	# CREATE GRID FOR SEALS
	elif loiinfo[1]=='Formation':
		# For given layer/ formation in qbc
		if allprop.has_key(loiname+'_name'):
			formdict = get_formationinfo(allprop, loiname)
			allprop.update(formdict)
		pscoords = generate_volume(allprop[loiname+'_widthE'], allprop[loiname+'_segH'], allprop[loiname+'_widthN'], allprop[loiname+'_segH'], allprop[loiname+'_height'], allprop[loiname+'_segV'], [allprop[loiname+'_E'],allprop[loiname+'_N'],allprop[loiname+'_D']])
		
	else:
		msg = 'ERROR LOI type not understood.'
		logger.critical(msg)
		raise ValueError(msg)
		
	#............................................
	# Put in pointset file
	#............................................
	# 20120217 -  colnames=['Northing','Easting','Depth']
	colnames=['Easting','Northing','Depth']
	QBpio.write_simpleascii(filename,pscoords,colnames)
	msg = 'Pointset with locations has been created in '+ filename
	logger.debug(msg)

	# surfacedepth to be added
	Depth = pscoords[:,2]
	if allprop.has_key(loiname+'_depths'): 
		# FK20120816 create depths from input
		# FK20121217 allow for 1 depth or multiple input depths
		# geomdict['horzhet.depths'] = allprop[loiname+'_depths'].strip('[]')
		try:
			# if array
			geomdict['horzhet.depths'] = allprop[loiname+'_depths'].strip('[]')
		except:
			# if only 1 depth
			geomdict['horzhet.depths'] = allprop[loiname+'_depths']
		
	else:
		# create depths from sampling
		if allprop.has_key(loiname+'_sample'):
			nsample = int(allprop[loiname+'_sample'])
		else:
			nsample = 6

		# FK20120909 Allow only 1 depth or horizontal well
		if not Depth.min() == Depth.max():
			horzdepth = np.linspace(np.floor(Depth.min()),np.ceil(Depth.max()),nsample)		
			msg = 'Depth sampling resolution adjusted to '+ str(abs(horzdepth[1]-horzdepth[0]))+ 'm'
		else: 
			horzdepth = [Depth.min()]
			msg = 'WARNING Depth output adjusted to horzdepth = '+ str(horzdepth[0])+ 'm'

		geomdict['horzhet.depths'] = str(list(horzdepth)).strip('[]') 
		geomdict['depths'] = str(list(horzdepth)).strip('[]') 
		logger.info(msg)
		
	return geomdict
	
	
def create_singleLOI(loiprop, loiinfo, casefullname):
	""" 
	Returns a dictionary with QuickBlocks keywords and values related to response parameters. 
	Pointset file with locations of interest (LOI) is created if neccessary
		
	**loiprop** 		Dictionary with case specific keywords for QuickBlocks model
	**loiname** 		String containing name of LOI
	**casefullname**	String containing output name
	
	"""
	
	# FK02052012 removing QB
	import qbio as QBpio
	
	casefilespath = loiprop['casefilespath']
	loidict = {}
	loiname = loiinfo[0]
	# Possibilities: Formation; Fault; Well; Subsidence
	# file, gridplanes, create file
	
	# FK20121218 Distinction file or gridplanes in create_singleloi
	# FK20120327 c1 replaced by CORA_OUT
	if ( loiinfo[1]=='Subsidence' and loiprop.has_key(loiname+'_surface') ):
		#FK20121217 Allow pointset of surface and gridplanes
		datatype = 'gridplanes'
		loidict['horzhet.depths'] = loiprop[loiname+'_surface']
		
	elif ( loiinfo[1]=='Formation' and  loiprop.has_key(loiname+'_depths') ):
		# Layers, only depths and view points have to be altered
		datatype = 'gridplanes'
		try:
			# if array
			loidict['horzhet.depths'] = loiprop[loiname+'_depths'].strip('[]')
		except:
			# if only 1 depth
			loidict['horzhet.depths'] = loiprop[loiname+'_depths']
		
	# ALL TYPESWITH (CREATED) FILES
	else: 
		datatype = 'pointset'
		
		# file needs to be created:
		if not loiprop.has_key(loiname+'_file'):
			# note that here loidict gets overwritten
			filename = os.path.join(casefilespath, casefullname+'.dat')
			loidict = write_psloi(loiprop, loiinfo, filename)
			loidict['fw.data.CORA_OUT.datafile']=filename
			
			# Get fault properties for nonfiles
			if loiinfo[1]=='Fault': 
				writeGCname,faultname = generate_GCsurface(filename, loiname, loiprop[loiname+'_segH'])
				loidict[loiname+ '_GoCadFile'] = writeGCname
				loidict[loiname+ '_GoCadFaultName'] = faultname
				
		# file is presented:
		elif (loiprop.has_key(loiname+'_file') and not loiprop[loiname+'_file']==''):
			# use existing file
			filename = os.path.join(casefilespath+'\..', loiprop[loiname+'_file'])
			loidict['fw.data.CORA_OUT.datafile']=filename
			
			if loiprop.has_key(loiname+'_depths'): 
				try:
					loidict['horzhet.depths'] = loiprop[loiname+'_depths'].strip('[]')
				except:
					loidict['horzhet.depths'] = loiprop[loiname+'_depths']
			else:
				if loiprop.has_key(loiname+'_sample'):
					nsample = int(loiprop[loiname+'_sample'])
				else:
					nsample = 6
				ps,colh,ml=QBpio.read_pssection2(filename)
				Depth = ps['Depth']
				# FK20120909 Allow only 1 depth or horizontal well
				if not Depth.min() == Depth.max():
					horzdepth = np.linspace(np.floor(Depth.min()),np.ceil(Depth.max()),nsample)		
					msg = 'Depth sampling resolution adjusted to '+ str(abs(horzdepth[1]-horzdepth[0]))+ 'm'
				else: 
					horzdepth = [Depth.min()]
					msg = 'WARNING Depth output adjusted to horzdepth = '+ str(horzdepth[0])+ 'm'

				loidict['horzhet.depths'] = str(list(horzdepth)).strip('[]') 
				logger.info(msg)
		else:
			msg = 'ERROR key for file given but filename for loi '+loiname+' not understood.'
			logger.critical(msg)
			raise ValueError(msg)
			

	# FK20120327 For pointset colnames are added
	if datatype == 'gridplanes':
		loidict['fw.data.CORA_OUT.datadefinition']='gridplanes'
		loidict['fw.data.CORA_OUT.view.x.n'] = loiprop[loiname+'_view.x.n'] 
		loidict['fw.data.CORA_OUT.view.x.firstcoord'] = loiprop[loiname+'_view.x.firstcoord']
		loidict['fw.data.CORA_OUT.view.x.resolution'] = loiprop[loiname+'_view.x.resolution']
		loidict['fw.data.CORA_OUT.view.y.n'] = loiprop[loiname+'_view.y.n'] 
		loidict['fw.data.CORA_OUT.view.y.firstcoord'] = loiprop[loiname+'_view.y.firstcoord']
		loidict['fw.data.CORA_OUT.view.y.resolution'] = loiprop[loiname+'_view.y.resolution']

	elif datatype == 'pointset':
		loidict['fw.data.CORA_OUT.datadefinition']='pointset'
		loidict['fw.data.CORA_OUT.E.type']='colname'
		loidict['fw.data.CORA_OUT.E.colname']= 'Easting'
		loidict['fw.data.CORA_OUT.E.factor']=1.0
		loidict['fw.data.CORA_OUT.N.type']='colname'
		loidict['fw.data.CORA_OUT.N.colname']='Northing'
		loidict['fw.data.CORA_OUT.N.factor']=1.0
		loidict['fw.data.CORA_OUT.z.type']='colname'
		loidict['fw.data.CORA_OUT.z.colname']='Depth'
		loidict['fw.data.CORA_OUT.z.factor']=1.0
		
	return loidict
	
	
### --------------------------------------------------------    
### POST processing LOIs
### --------------------------------------------------------
def generate_GCsurface(filename, loiname, segH):
	
	# importing QBpio
	import qbio as QBpio
	
	# Regular surface in 3D is expected
	ps,colh,ml=QBpio.read_pssection2(filename)
	
	segV = (len(ps['Easting'])-1)/(segH+1)
	Easting = ps['Easting'].reshape((segH+1,segV+1))
	Northing = ps['Northing'].reshape((segH+1,segV+1))
	Depth = ps['Depth'].reshape((segH+1,segV+1))
	
	# CREATE CONNECTIVITY TABLE
	# for every 4 coordinates create 2 segments
	# connectivity = [nodenr1, nodenr2, nodenr3]
	# triangulation by diagonal from left top to right bottom
	segment = np.zeros((2*segH*segV, 3))
	for index in range(0,segH):
		for jndex in range(0,segV):
			segment[(jndex)*2*segH+2*index,:] = [index+1+jndex*(segH+1), index+2+jndex*(segH+1), index+2+(jndex+1)*(segH+1)]
			segment[(jndex)*2*segH+2*index+1,:] = [index+1+jndex*(segH+1), index+2+(jndex+1)*(segH+1), index+1+(jndex+1)*(segH+1)]
	
	# CREATE LINES
	writename = filename[0:-4]+'GC.mx'
	start_lines = 'GOCAD TSurf 1\n'+'HEADER {\n'+'name:'+loiname+'\n'+ '}\n'+'TFACE\n'
	
	node_lines = ''
	for index in range(0, len(ps['Easting'])):
		node_lines = node_lines + 'VRTX '+ str(index+1)+' '+str(ps['Easting'][index])+' '+str(ps['Northing'][index])+' '+str(ps['Depth'][index])+'\n'
	
	elem_lines = ''
	for index in range(0, 2*segH*segV):
		elem_lines = elem_lines + 'TRGL '+ str(int(segment[index,0]))+' '+str(int(segment[index,1]))+' '+str(int(segment[index,2]))+'\n'
	
	end_lines = 'END\n'
	
	# WRITE FILE
	fid = open(writename,'w')
	fid.writelines(start_lines)
	fid.writelines(node_lines)
	fid.writelines(elem_lines)
	fid.writelines(end_lines)
	fid.close()

	return writename, loiname
	

def get_Depths(filename):
	"""
	Derives the Depth coordinate 
	
	"""
	
	# FK02052012 removing QB
	import qbio as QBpio
	
	# Get properties
	allprop,ml = QBpio.read_propsection(filename)
	layers = allprop['layers'].replace(' ', '').split(',')
	
	# Get formation boundaries
	depths = [float(allprop['surfacedepth.constant'])]
	for layer in layers:
		# if thickness given
		if (allprop[layer+'.base.type'] == 'thickness'):
			newdepth = depths[-1] + float(allprop[layer+'.base.constant'])
			depths.append(newdepth) 
		
		# if base given
		elif (allprop[layer+'.base.type'] == 'base'):
			depths.append(float(allprop[layer+'.base.constant'])) 
		
		# other
		else:
			msg = 'Feature only available for models with constant formation layers (thickness or base).'
			logger.critical(msg)
			raise ValueError(msg)
		
	depthsarray = np.array(depths)
	
	return depthsarray, allprop

	
def get_PropertyPerDepth(intermedprops, properties, labels):
	"""
	Derives properties per coordinate
	
	"""
	faildict = {}
	
	# FK02052012 removing QB
	import qbio as QBpio
	
	# Get properties
	loiname = intermedprops['loiname']
	filename = intermedprops['caseinputname']
	# print filename 
	
	# get formation boundaries
	depthsbound, allprop = get_Depths(filename)
	layers = allprop['layers'].replace(' ', '').split(',')
	
	# Two cases: gridplanes or pointset
	# FK20120327 c1 replaced by CORA_OUT
	if allprop['fw.data.CORA_OUT.datadefinition'] == 'gridplanes':
		depthunsort = [float(d) for d in allprop['horzhet.depths'].strip(' ').split(',')]
	else:
		loifilename = allprop['fw.data.CORA_OUT.datafile']
		ps,colh,ml=QBpio.read_pssection2(loifilename)
		depthunsort = ps['Depth']
	
	# Depths in output
	depthsort = sorted(list(set(depthunsort)))
	
	# get properties
	if intermedprops.has_key(loiname+'_name'):
		# when formation has been selected, phi, coh are known
		layername = intermedprops[loiname+'_name']
		for indexprop in range(0,len(properties)):
			proplist = []
			[proplist.append(intermedprops[layername+labels[indexprop]]) for depth in depthsort]
			faildict[properties[indexprop]] = proplist
			
	else:
		# when volume over formations (also gridplanes)
		# formation 0 runs from surface.depth - next boundary
		for indexprop in range(0,len(properties)):
			proplist = []
			for loc in depthsort:
				index = max(np.where(loc <= depthsbound)[0].min()-1, 0)
				proplist.append(intermedprops[layers[index]+labels[indexprop]])
			faildict[properties[indexprop]] = proplist
			
	faildict['depthsout'] = depthsort
	faildict['formbounds'] = list(depthsbound)
	
	return faildict

	
### --------------------------------------------------------    
### Create scenarios/ generate engine input files
### --------------------------------------------------------
def change_property(lines,property,new_value):        
	""" 
	Replaces a property value by the new value for given lines
	
	""lines""		Array of QuickBlocks input line
	""property""	Array of properties to be renewed (start of lines)
	""new_value""	Array of new lines (to replace end of lines)
	
	"""	
	
	for line_no, line in enumerate(lines):
		if line.strip().startswith(property):
			prop = line.strip().split('=')
			prop[1] = new_value
			lines[line_no] = prop[0]+'='+str(prop[1])+'\n'
			
			break

	return lines

	
def changeQBpropfile(readname,writename,newprops,matprop):
	""" 
	Creates new model input file from an old one by replacing key values. 
	
	""readname""	Old QuickBlocks input file
	""writename""	New QuickBlocks input file
	""newprops""	Array of new properties to be replaced
	""matprop""		Dictionary of keys and values to be replaced
	
	"""
    
	success = False    
	fid = open(readname,'r')
	lines = fid.readlines()
	fid.close()
	illprop = []
		
	# FK 20120327 Cancel/comment output settings for when lois is chosen
	if not matprop['loiname'] == '': 
		for line_no,line in enumerate(lines):
			if line.startswith('fw.data.') or line.startswith('fw.file_'):
				lines[line_no] = '# '+line
	
	# replace a property
	for prop in set(newprops).difference(matprop.keys()):
		msg = 'WARNING: unknown property '+ prop+ '. File not changed'
		logger.warning(msg)
		
	for prop in set(newprops).intersection(matprop.keys()):
		if matprop[prop]=='' or matprop[prop]==None:
			illprop.append(prop)
		else:
			new_lines = change_property(lines,prop,matprop[prop])
			success = True
			if success:
				fid = open(writename,'w')
				fid.writelines(new_lines)
				fid.close()
				
	if not illprop==[]:
		msg = 'WARNING: properties ignored '+ str(illprop)
		logger.warning(msg)
	
	# FK 20120327 Add new output settings
	# ???? need to do something smart if pois iset, but respparam not
	if not matprop['loiname'] == '': 
		new_lines2 = []
		for fwkey in newprops:
			if fwkey.startswith('fw.data.'):
				new_lines2.append( fwkey + '=' +str(matprop[fwkey])+'\n')
		
		ffile = open(writename, 'a') 
		ffile.writelines(new_lines2)
		ffile.close()
	
	return success


def create_caseinputfile(casename, dict, fname, casefilespath=''):
	""" 
	Creates new case input file from an old one by replacing key values. 
	
	""casename""	Case name
	""dict""	Dictionary with properties and values to replace
	""fname""	Name of old WuickBlocks model input file
	""casefilespath""		Path where to put the new file
	
	"""
	# casedict contains dictionary of case 'Default'
	# scendict contains alternative dictionary of case 'Case0' etc
	
	oldname = fname
	newname = os.path.join(casefilespath,casename+'.qbc')

	# FK20120327 Changes to file wrt output added here
	msg = 'Adding output settings to work dictionary'
	logger.debug(msg)

	dict['file_interm']=casename+'.qbi'
	dict['export.filebase.simpleascii']=casename
	dict['export.filebase.geomec']=casename+'GEOM'
	dict['export.filebase.pngimage']=casename

	success = changeQBpropfile(oldname,newname,dict.keys(),dict)
	if success:
		msg = 'QuickBlocks input file '+ newname+ ' has been created.'
		logger.info(msg)
		
	return newname

