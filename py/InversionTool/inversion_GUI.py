# Imports for GUI and basic functionality

import os;
LOG_FILE=os.path.dirname(os.path.abspath(__file__)) + "\..\IT_py.log"

try:
	from Tkinter import Frame
	import Tkinter as tk
	import ttk
	import tkFileDialog as tkF
	import numpy as np
	import os
	import nucleus_unstructured as nu # nucleus of strain - Kees
	import trimesh as trim # trimesh functions of Kees
	import cgschemes as cg # conjugate gradient scheme of Kees

	# Imports for plotting:
	import matplotlib
	from matplotlib.patches import Polygon
	from matplotlib.collections import PatchCollection
	from matplotlib import rcParams
	from matplotlib.figure import Figure
	from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg #, NavigationToolbar2TkAgg

	# Imports for the Geomec interface
	import logging
	import helper
	from workflow_IT1 import Workflow_IT1
	from workflow_IT2 import Workflow_IT2
	from geomec_interface import GeomecInterface as GI
	from widget import Widget
	from control import Sender
	import settings1
	from interface import Controls		

	from shutil import copyfile
	
	try:
		#dst = 'E:\IT_py_{}.log'.format( time.strftime('%Y-%m-%d_%H-%M') )
		#creation=time.ctime(os.path.getctime( LOG_FILE ) )
		f = open( LOG_FILE )
		timestamp = f.readline()[0:16].replace(':','-').replace(' ','_')
		dst = '{}_{}.log'.format( LOG_FILE[0:-4], timestamp )
		copyfile( LOG_FILE, dst )
	except:
		pass
	logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG, filemode='w' )
	logging.getLogger().setLevel( settings1.settings1['loggging_level'] ) # getLogger('logger_id_string')
	logging.info('previous execution: {}'.format( dst ) )
	LOGGING_LEVEL_DICT={ logging.CRITICAL : 'CRITICAL', logging.ERROR : 'ERROR', logging.WARNING : 'WARNING', logging.INFO : 'INFO', logging.DEBUG : 'DEBUG' }
	logging.info('current logging level: {}'.format( LOGGING_LEVEL_DICT[ settings1.settings1['loggging_level'] ] ) )
	#logging.info('creation:{}'.format( creation ) )
except:
	import logging
	logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG )
	import helper
	helper.handle_exception()

'''
This module contains a GUI for subsidence inversion

Kees Hindriks,  v1 2015
				v2 2016
            TO DO:
                - remove the checkboxes from the GUI
                - ensure that all changes from Manel are OK (especially depletion)
'''

def notdone():
	pass

def masked(x,mask):
	'''
	function to compute mean and standard deviation from a masked vector
	'''
	mean=sum(x*mask)/sum(mask)
	stdv=np.sqrt(sum(np.power(mask*(x-mean),2)/sum(mask)))

	return mean,stdv

def outlierdetect(recon,orig,frac):
	'''
	function for outlier detection
	input
		recon reconstructed/forward modeled observations
		orig  actual/real observations
		frac  percentage of expected outliers

	Some room for improvement is possible (i.e. use standard deviation information
	to define number of outliers, based on a confidence interval)
	'''
	try:
		#compute error
		err=orig-recon
		# define number of outliers
		nn=int(frac*float(len(err)))

		# initiate sample mask and value vector
		masker=np.ones((len(err),))
		vals=np.zeros((len(err),))

		#iterate over the number of desired outliers
		for m in range(nn):
			# compute mean and standard deviation of the error at current non-outlier locations
			mean,stdv=masked(err,masker)
			# make list of relative errors, in comparison to mean error, using the current non-outliers
			test=abs(err-mean)*masker/stdv
			# select the sample with the highest relative error
			outl=np.argmax(test)
			# set the mask for this sample to zero such that it will be excluded in the next iteration
			masker[outl]=0.
			# and make a complimentay mask
			vals[outl]=test[outl]
			vals[outl]=1.
		masker=1.-vals/max(vals)
		return masker
	except:
		helper.handle_exception()

rcParams['font.size']=6

class subsinversion(Frame):
	'''
	class for handling and displaying subsidence inversion through a GUI
	'''
	def __init__(self,parent=None):

		try:
			Frame.__init__(self,parent) # FIXME: don't do the work when app is launched sevreral times; tkk app needs to be closed properly
			self.pack(expand=tk.YES,fill=tk.BOTH)

			self.master.title("Unstructured inversion")
			self.directory='.'

			self.LOG_FILE=LOG_FILE

			self.combos={}
			self.checks={}
			self.radios={}
			self.labels={}

			#self.workflow = Workflow_IT1( self )
			self.workflow = Workflow_IT2( self )
			self.widget = Widget( self, self.workflow ) 

	
			rij = self.widget.create() # 'invert for' | 'New depletion' | Buttons | Combos
			logging.debug('widget : create : last row:{}'.format( rij))

			self.subsgrid=False
			self.resgrid=False
			self.enable=False
			self.cultural=False
			self.inverted=False
			self.setrec_parlimits = False # flag to help fix color limits through iterations 

			self.nit=3
			self.nrw=5
			self.mc=1
			self.cult=[]
			self.culttype=[]
			#self.readcultural()

			# ================== GRAPHICS ==================

			# set-up a canvas
			self.fig=Figure(figsize=(12.5,8),dpi=100,frameon=True)
			self.display=FigureCanvasTkAgg(self.fig,master=self )

			r, c, r_span, c_span = ( 0, 4, rij, 10 )
			self.display.get_tk_widget().grid( row=r,column=c, rowspan=r_span, columnspan=c_span, sticky=tk.W+tk.N+tk.E )

			self.minx=-1.
			self.miny=-1.
			self.scale=2.
			self.rec_subs=np.array([-1.,1.])

			self.sub1=self.fig.add_subplot(334)
			self.sub2=self.fig.add_subplot(333)
			self.sub3=self.fig.add_subplot(332)
			self.sub4=self.fig.add_subplot(335)
			self.sub5=self.fig.add_subplot(331)
			self.sub7=self.fig.add_subplot(336)
			self.sub8=self.fig.add_subplot(337)
			self.sub9=self.fig.add_subplot(338)
			self.ax1=[.295,.055,.01,.4]
			self.ax2=[.64,.055,.01,.4]
			#self.ax3=[.83,.78,.01,.173]
			self.ax4=[.295,.55,.01,.4]
			self.ax5=[.64,.55,.01,.4]
			ax1=self.fig.add_axes(self.ax1)
			ax2=self.fig.add_axes(self.ax2)
			#ax3=self.fig.add_axes(self.ax3)
			ax4=self.fig.add_axes(self.ax4)
			ax5=self.fig.add_axes(self.ax5)
			scat=self.sub4.scatter([0.,],[0.,],s=1.,c=0.)
			self.cbar1=self.fig.colorbar(scat, cax=ax1)
			self.cbar2=self.fig.colorbar(scat, cax=ax2)
			#self.cbar3=self.fig.colorbar(scat, cax=ax3)
			self.cbar4=self.fig.colorbar(scat, cax=ax4)
			self.cbar5=self.fig.colorbar(scat, cax=ax5)
			self.setplotpars()

			control_wnd = settings1.settings1['control_wnd']
			if control_wnd:
				self.control=Sender( self ) # IMPORTANT
			else:
				self.control=None

		except:
			helper.handle_exception()
	
	def calc_triprop(self, prop, p):
		'''
		For each triangle in the surface mesh trim.trimesh, calculate its given
		property from the node values of the triangle.
		'''
		p1 = p[0]
		p2 = p[1]
		p3 = p[2]
		dum=0.
		n=0
		if prop[p1-1] >0:
			dum=dum+prop[p1-1]
			n=n+1
		else:
			prop[p1-1]=-1.
		if prop[p2-1] >0:
			dum=dum+prop[p2-1]
			n=n+1
		else:
			prop[p2-1]=-1.
		if prop[p3-1] >0:
			dum=dum+prop[p3-1]
			n=n+1
		else:
			prop[p3-1]=-1.
		avg = dum/(float(n)+1.e-15)
		return prop, avg

	def update_mat(self):
		logging.debug("executing self.update_mat")


	def load_operator( self ):

		FILE_A='E:\IT_A.dat'
		FILE_A2='E:\IT_A2.dat'
		FILE_scale2='E:\IT_scale2.dat'
		FILE_scale_disp='E:\IT_scale_disp.dat'

		pointset_points=len(self.xirr) #448
		reservoir_elems=len(self.reservoirtriangles) #2618

		shape=(pointset_points, reservoir_elems)
		shape_scale2=(reservoir_elems, reservoir_elems)
		shape_scale_disp=(reservoir_elems,)

		logging.info('GUI : load_operator : shape: {}'.format( shape ) )

		self.A = np.ndarray( shape=shape, buffer=np.fromfile( FILE_A ) ) #(file, dtype=float, count=-1, sep='')
		self.A2 = np.ndarray( shape=shape, buffer=np.fromfile( FILE_A2 ) ) #(file, dtype=float, count=-1, sep='')

		self.scale2 = np.ndarray( shape=shape_scale2, buffer=np.fromfile( FILE_scale2 ) ) #(file, dtype=float, count=-1, sep='')
		self.scale_disp = np.ndarray( shape=shape_scale_disp, buffer=np.fromfile( FILE_scale_disp ) ) #(file, dtype=float, count=-1, sep='')

		logging.info('GUI : load_operator : A: {}'.format( self.A ) )
		logging.info('GUI : load_operator : A2: {}'.format( self.A2 ) )
		logging.info('GUI : load_operator : scale2: {}'.format( self.scale2 ) )
		logging.info('GUI : load_operator : scale_disp: {}'.format( self.scale_disp ) )

		self.enable=True # DON'T MISS IT : allows inversion process to be run


	def reservoir(self):
		'''
		Take reservoir data from geomec (self.reservoirpoints, self.reservoirtriangles
		and self.reservoirpars), generate a surface trimesh (trim.trimesh) and 
		calculate reservoir attributes for each triangle in the trimesh. Plot results.
		'''
		try:
			self.var_thickness='dz' #FIXME: is it a literal or an index?
			# KB: Geomec returns N, E, z, not x, y, z. Change it here.
			self.reservoirpoints1 = np.array( self.reservoirpoints)
			self.reservoirpoints[:,0] = self.reservoirpoints1[:,1]
			self.reservoirpoints[:,1] = self.reservoirpoints1[:,0]

			self.resgrid=True
			self.inverted=False

			#if self.subsgrid:
			#	self.b_construct['state'] = 'normal'
			zco=self.reservoirpoints[:,2]

			thick=self.th
			pr = self.pr
			ym = self.ym
			pres=self.pp
			#logging.debug("thick: {}, len(thick): {}, thick[0]: {}".format(thick, len(thick), thick[0]))
			#logging.debug("type(thick): {}, type(self.reservoirpars): {}".format(type(thick), type(self.reservoirpars)))
			self.shiftxres=min(self.reservoirpoints[:,0])
			self.shiftyres=min(self.reservoirpoints[:,1])
			self.scalexres=max(self.reservoirpoints[:,0])-min(self.reservoirpoints[:,0])
			self.scaleyres=max(self.reservoirpoints[:,1])-min(self.reservoirpoints[:,1])
			self.scaleres=max([self.scalexres,self.scaleyres])
			lowest=1000
			highest=0
			for tri in self.reservoirtriangles:
				for p in tri:
					if p > highest:
						highest=p
					if p < lowest:
						lowest=p

			trim.trimesh.initiate(self.reservoirpoints,self.reservoirtriangles)

			self.x0=np.zeros((trim.trimesh.nTri,))
			nt = trim.trimesh.nTri
			avdepth = np.zeros(nt)
			avthick= np.zeros(nt)
			avpres = np.zeros(nt)
			avpr = np.zeros(nt)
			avym = np.zeros(nt)
			avcm = np.zeros(nt)
			for tri in range(trim.trimesh.nTri):
				p1=trim.trimesh.pTris[tri,0]
				p2=trim.trimesh.pTris[tri,1]
				p3=trim.trimesh.pTris[tri,2]
				p = [p1, p2, p3]
				
				# Assign average depth per triangle
				avdepth[tri] = (zco[p1-1]+zco[p2-1]+zco[p3-1])/3.

				# Assign average thickness per triangle
				thick, _avg = self.calc_triprop(thick, p)
				avthick[tri] = _avg

				pres, _avg = self.calc_triprop(pres, p)
				avpres[tri] = _avg

				pr, _avgpr = self.calc_triprop(pr, p)
				avpr[tri] = _avgpr

				ym, _avgym = self.calc_triprop(ym, p)
				avym[tri] = _avgym

				_avgcm = ((1.-2.*_avgpr) * (1.+_avgpr)) / ( _avgym * (1.-_avgpr) )   
				avcm[tri] = _avgcm

			avH=sum(avthick)/len(avthick)
			thick[thick<0.]=avH		   
			avP=sum(avpres)/len(avpres)
			pres[pres<0.]=avP

			avPR_=sum(avpr)/len(avpr)
			pr[pr<0.]=avPR_

			avYM_=sum(avym)/len(avym)
			ym[ym<0.]=avYM_

			self.avthick=np.array(avthick)
			self.avdepth=np.array(avdepth)
			self.avpres=np.array(avpres)
			self.prevpres = np.ones(len(self.avpres)) * self.avpres
			self.prevstrain = np.zeros(len(self.avpres))         
			self.prevcm = np.zeros(len(self.avpres))
			self.avpr=np.array(avpr)
			self.avym=np.array(avym)
			self.avcm=np.array(avcm)
			self.zco=np.array(zco)
			self.reservoirpatches=[]
			for tri in range(trim.trimesh.nTri):
			   node1=trim.trimesh.pTris[tri,0]
			   node2=trim.trimesh.pTris[tri,1]
			   node3=trim.trimesh.pTris[tri,2]
			   poly=Polygon(np.array([[trim.trimesh.pnodes[node1-1,0],trim.trimesh.pnodes[node1-1,1]],
						   [trim.trimesh.pnodes[node2-1,0],trim.trimesh.pnodes[node2-1,1]],
						   [trim.trimesh.pnodes[node3-1,0],trim.trimesh.pnodes[node3-1,1]]]), True)
			   self.reservoirpatches.append(poly)
			#self.textU.insert(1.0,'Reservoir loaded -- Next step: "Fill" Meas. Disp.\n','OK')
			self.setlimits()
			#self.plotreservoir()
			return True

		except:
			helper.handle_exception()
			return False

	def subsidence(self):
		''' 

     '''
		try:

			gm_results = True if self.workflow.cmds_queue.cmds_done[0] == GI.Cmd.IT1.Get.Displacement else False
			logging.info( 'gm_results: {}, self.subs ({}): {}'.format( gm_results, type(self.subs), self.subs ) )

			# Inverted subsidence data, set subsgrid=True
			self.subsgrid=True
			# New subsidence data, set inverted=False
			self.inverted=False

			# KB: This was initially in construct(), but needs to be updated every time
			# new subsidence data is loaded. Can also be moved to workflow.py.
			if self.usex:
				if self.usey:
					if self.usez:
						self.subs=np.concatenate((self.subs[:,0],self.subs[:,1],self.subs[:,2]))
					else:
						self.subs=np.concatenate((self.subs[:,0],self.subs[:,1]))
				else:
					if self.usez:
						self.subs=np.concatenate((self.subs[:,0],self.subs[:,2]))
					else:
						self.subs=self.subs[:,0]
			else:
				if self.usey:
					if self.usez:
						self.subs=np.concatenate((self.subs[:,1],self.subs[:,2]))
					else:
						self.subs=self.subs[:,1]
				else:
					if self.usez:
						self.subs=self.subs[:,2]


			self.subsign=self.inputs['Subsidence sign']

       # If this is loading of measured displacement:
			if not gm_results: 
				# Loading measured displacement
				if self.subsign=='+':
					self.sign=1.
				# Geomec negative means exhumation, so strain = negative
				elif self.subsign=='-':
					self.sign=-1.
				else:
					self.sign=0.
				# Store measured displacement to calculate residual later on:
				self.subs_initial = np.array(self.subs) 
				# Keep track of average vertical displacement to see improvements per iteration
				self.avs = [np.average(self.subs_initial)]
				# Define self.dsubs for plotting (residual) surface displacement. 

				self.dsubs = np.array(self.subs_initial)
				# Fix the colorbar for displacement to allow comparing different iterations.
				self.dd_min = min(self.dsubs)
				self.dd_max = max(self.dsubs)
				# If all displacement is negative, fix the upper limit to 0:
				if self.dd_max < 0.:
					self.dd_max = 0.
			else:
				self.sign = 1.
				# Calculate the residual between modelled (self.subs) and meausured
				# displacement (self.subs_initial) and use this as new subs for next iteration
				# Ensure copy is independent.
				self.subs = np.array(self.subs_initial - self.subs)

				# Independent copy for plotting
				self.dsubs = np.array(self.subs) # self.dsubs_initial - self.zsubs_modelled_cumulative
				self.avs.append(np.average(abs(self.subs)))

			self.shiftxsurf=min(self.xirr)
			self.shiftysurf=min(self.yirr)
			self.scalexsurf=max(self.xirr)-min(self.xirr)
			self.scaleysurf=max(self.yirr)-min(self.yirr)
			self.scalesurf=max([self.scalexsurf,self.scaleysurf])
			#logging.debug("@@@end of subsidence, loaded sub data (gm is {}): {}".format(self.gm_results, self.subs))
			if self.usex or self.usey or self.usez:
				
				self.setlimits()
				#self.plotobservation()
				#self.b_construct['state'] = 'normal'
				return True
				
			else:
				return False
				#self.textU.insert(1.0,'ERROR: indicate at least 1 displacement\n','ERR')
				#self.textU.tag_configure('ERR', background='red', font='helvetica 6 bold', relief='raised')
				#self.subsgrid=False

		except:
			helper.handle_exception()
			return False

	def construct(self):
		'''
		Check if displacement and reservoir data are loaded. If yes, construct 
		the forward operator (makesystem())
		MCR 2017-12-27: do NOT update widgets from this routine : thread conflict
		'''
		try:
			logging.debug('BTN: Construct')
			if self.subsgrid:
				if self.resgrid:
					self.makesystem()
					self.enable=True
					return True
				else:
					return False
			else:
				return False
		except:
			helper.handle_exception()
			return False

	def makesystem(self):
		'''
		Construct the forward operator:
		As part of the inversion workflow, take the trim.trimesh generated in the
		reservoir() function and the displacement data (subs). 
		Uses:
		- Average reservoir thickness (self.avthick)
		- Displacement data coordinates (xirr, yirr, zirr)
		- Reservoir data z values (zco)
		- Reservoir trimesh
		Only needs to run if model geometry is changed.
     
		Note that I have split the generation of Az, Ax, Ay (only based on geometry)
		from the concatenate subs, as the latter needs to be done every time the subs 
		values change, but the A values stay constant.

		MCR 2017-12-27: do NOT update widgets from this routine : thread conflict
		'''

		try:
			Az,Ax,Ay=nu.quadr(trim.trimesh,self.zco,self.avthick,self.xirr,self.yirr,self.zirr,4,2)

			# use this to get volumetric strain:
			self.scale1=np.diag(1./np.array(self.avthick)*0.+1.)
			# use this to get reservoir compaction/square meter:
			#self.scale1=np.diag(1./np.array(self.avthick)*1.+0.)

			self.scale_disp=self.avthick*1.
		
			self.scale2=np.diag(1./np.sqrt(trim.trimesh.area))
			#logging.info('GUI : makesystem : self.scale2: {} : type: {} shape: {}'.format( self.scale2, type( self.scale2 ), self.scale2.shape ))
			#self.scale2=np.diag(1./trim.trimesh.area)

			if self.usex:
				if self.usey:
					if self.usez:
						A=np.concatenate((Ax,Ay,Az),axis=0)
					else:
						A=np.concatenate((Ax,Ay),axis=0)
				else:
					if self.usez:
						A=np.concatenate((Ax,Az),axis=0)
					else:
						A=Ax
			else:
				if self.usey:
					if self.usez:
						A=np.concatenate((Ay,Az),axis=0)
					else:
						A=Ay
				else:
					if self.usez:
						A=Az
			# Sometimes A is nested in another array.
			if len(A) == 1:
				A = np.array(A[0])
			logging.debug("Az isnan check: {}".format(sum(sum(np.isnan(A)))))           

			self.A=np.dot(A,self.scale1)

			self.A2=np.dot(self.A,self.scale2)	


		except:
			helper.handle_exception()

	def inversion(self):
		'''
		Do the inversion, using output from makesystem() and the displacement
		data.
		MCR 2017-12-27: do NOT update widgets from this routine : thread conflict
		'''

		try:
			#logging.info("GUI : inversion")
			self.inverted=True
			frac=float(self.inputs['reduction factor'])/100.
			self.nit=int(self.inputs['internal iterations'])
			#logging.debug("frac in def inverison: {}".format(frac))
			# External iterations:
			self.nrw=int(self.inputs['external iterations'])
			self.reweight=int(self.inputs['reweight iterations\noutlier detection'])
			self.mc=1.        

			if self.enable:

				wdat=np.zeros((len(self.subs),))+1.
				for nnn in range(self.reweight):
					#print nnn
				
					extra=np.zeros((len(self.subs),))
			
					# KB This is the inversion
					rec,wt,extra,wtt=cg.lsqr(self.x0,self.A2,self.subs,np.zeros((len(self.subs),)),self.nit,self.nrw,0.1,1e-2,self.scale2,np.sqrt(wdat))
					# self.x0 = array of zeros with the length of trimesh
					# self.A2 = self.A2=np.dot(self.A,self.scale2)
					# where self.A = quadratic forward operator
					# self.scale2 = scale factor to calculate volume weighting: self.scale2=np.diag(1./np.sqrt(trim.trimesh.area))
					# self.subs = displacement values
					# self.nit = internal iterations
					# self.nrw = external iterations
				
					self.resolution=wt[len(rec)-1]

					rec1=rec[len(rec)-1]

					# scale back the inverted parameters
					rec_par=np.dot(self.scale2,rec1)

					self.rec_par=1.*rec_par/float(self.mc)

					# KB: if this is the first run, store the 
					#     color range limits for next runs (reservoir strain).
					if not self.setrec_parlimits:
						self.rec_par_min = min(self.rec_par)
						self.rec_par_max = max(self.rec_par)
						self.setrec_parlimits = True

					self.rec_subs=np.dot(self.A,self.rec_par)
					self.outliers=wdat
					wdat=outlierdetect(self.rec_subs,self.subs,frac)
					self.errtot=[np.sqrt(sum(self.subs*self.subs*self.outliers)/sum(self.outliers)),]
					self.indxtot=[0,]
					self.energy=[]
					for lll in range(len(rec)):
						self.indxtot.append(lll+1)
						rec0=rec[lll]
						rec_par=np.dot(self.scale2,rec0)
						energy=rec_par*rec_par
						self.energy.append(energy)
						err=np.dot(self.A,rec_par)-self.subs
						self.errtot.append(np.sqrt(sum(err*err*self.outliers)/sum(self.outliers)))


				logging.info("GUI : inversion : self.rec_par: {}, sum(self.outliers): {}".format(self.rec_par, sum(self.outliers)))

				return True

			else:
				return False
		except:
			helper.handle_exception()
			return False

	def plotresults(self):
		'''
		Plot inversion results:
		sub1 = inverted strain/compaction
		sub2 = geomec output
		sub3 = displacement error
		sub7 = detected outliers
		sub8 = RMS error
		'''
		try:

			ss=np.ones((len(self.xirr),))*20.

			self.sub1.clear()
			self.sub2.clear()
			self.sub3.clear()
			self.sub7.clear()
			self.sub8.clear()
			try:
				self.cbar1.remove()
			except:
				pass
			try:
				self.cbar2.remove()
			except:
				pass
			#try:
			#	self.cbar3.remove()
			#except:
			#	pass

			if Controls.value_cb( self.combos[ 'invert_for'] ) == GI.InvertFor.Compaction:
			#if Controls.value_rb( self.radios[ 'invert_for'] ) == GI.InvertFor.Compaction:
				#logging.debug('len: {}; scale_disp: {}'.format(len(self.scale_disp),self.scale_disp))
				#logging.debug('len: {}; self.avpr: {}'.format(len(self.avpr),self.avpr))
				weight=self.scale_disp*np.pi/(1.-self.avpr)
				self.sub1.set_title('Inverted reservoir compaction [m]',fontsize=12)
			else:
				weight=1.
				self.sub1.set_title('Inverted reservoir strain [-]',fontsize=12)

			# Plot reservoir deformation
			p2=PatchCollection(self.reservoirpatches,cmap=matplotlib.cm.gist_rainbow,edgecolor='none')
			p2.set_array(np.array(self.rec_par)*weight)
			# KB: keep colors fixed based on the first iteration
			p2.set_clim([self.rec_par_min, self.rec_par_max])
			bla=self.sub1.add_collection(p2)

			# Plotting Geomec data to sub2
			p2=PatchCollection(self.reservoirpatches,cmap=matplotlib.cm.gist_rainbow,edgecolor='none')

			# Based on selected output format, plot Geomec data:
			if Controls.value_cb( self.combos[ 'dep_type'] ) == GI.Depletion.Pore_Pressure:
			#if Controls.value_rb( self.radios[ 'dep_type'] ) == GI.Depletion.Pore_Pressure:
				self.sub2.set_title('$\Delta$P for GEOMEC [MPa]',fontsize=12)
				_plotsub2 = (self.rec_par / self.avcm)
			elif Controls.value_cb( self.combos[ 'dep_type'] ) == GI.Depletion.Compressibility:
			#elif Controls.value_rb( self.radios[ 'dep_type'] ) == GI.Depletion.Compressibility:
				self.sub2.set_title('Cm for GEOMEC [1/MPa]',fontsize=12)  
				_plotsub2 = (self.rec_par / self.prevpres) #+ self.avcm
			else:
				self.sub2.set_title('Strain for GEOMEC [-]',fontsize=12)
				_plotsub2 = self.rec_par

			# Plot parameter resolution
			p2=PatchCollection(self.reservoirpatches,cmap=matplotlib.cm.gist_rainbow,edgecolor='none')
			p2.set_array(_plotsub2)
			bla2=self.sub2.add_collection(p2)

			nn=len(self.outliers)/len(self.xirr)
			nx=len(self.xirr)
			wdat=np.ones((nx,))
			for m in range(nn):
				wdat=wdat*self.outliers[m*nx:(m+1)*nx]
			wdat2=wdat*1.
			self.mask=wdat2
			wdat[wdat>0]=np.nan
			self.sub7.scatter(self.xirr,self.yirr,s=ss,c=wdat,edgecolor='k',vmin=0.,vmax=1)
			self.sub8.plot(self.indxtot,self.errtot)
			self.sub3.scatter(self.subs,self.rec_subs,s=ss,c=wdat2,vmin=0.,vmax=1.)
			ax1=self.fig.add_axes(self.ax1)
			ax2=self.fig.add_axes(self.ax2)

			#self.cbar1=self.fig.colorbar(bla, cax=ax1,format='%.0e')
			self.cbar1=self.fig.colorbar(bla, cax=ax1)
			self.cbar2=self.fig.colorbar(bla2, cax=ax2)

			self.plotobservation()

			self.setplotpars()

			self.display.show()
		except:
			helper.handle_exception()

	def plotreservoir(self):
		'''
		Plot the loaded reservoir surface with its attribute (e.g. thickness, depth)
		'''
		try:
			self.sub4.clear()
			try:
				self.cbar4.remove()
			except:
				pass

			# KB 21-7: avthick and avdepth have large # of decimals, that impacts plotting.
			# round those for plotting only.
			# ['Depth','Thickness','Poisson\'s ratio','Young\'s modulus','Initial pore pressure']    

			
			plot_id=Controls.value_cb( self.combos['plot'] )
			#plot_id=Controls.value_rb( self.radios['plot'] )
			#logging.info('GUI : plotreservoir : plot_id: {}'.format( plot_id ))
			
			if   plot_id==0:	_prop = np.around(self.avdepth,1)
			elif plot_id==1:	_prop = np.around(self.avthick,1)
			elif plot_id==2:	_prop = np.around(self.avpr,3)
			elif plot_id==3:	_prop = np.around(self.avym,3)
			elif plot_id==4:    _prop = np.around(self.avpres,1)
			elif plot_id==5:    _prop = np.around(self.avcm,1)
			else:				_prop = np.around(self.avdepth,1) # show depth by default

			#_title = str( Controls.value_cb_text( self.combos['plot'] ) )
			#logging.info('GUI : plotreservoir : plot name: {}'.format( GI.Plot.List[ plot_id ] ))
			_title = str( GI.Plot.List[ plot_id ] )

			p1=PatchCollection(self.reservoirpatches,cmap=matplotlib.cm.gist_rainbow,edgecolor='none')
			p1.set_array(_prop)
			bla1=self.sub4.add_collection(p1)

			ax4=self.fig.add_axes(self.ax4)
			self.sub4.set_title(_title,fontsize=12)

			self.cbar4=self.fig.colorbar(bla1, cax=ax4)

			self.setplotpars()
		except:
			helper.handle_exception()

	def plotobservation(self):
		'''
		Plot the measured input displacement or the modelled residual displacement.
		'''
		try:
			if self.inverted:
				mask=self.mask
			else:
				mask=np.ones((len(self.xirr),))

			ss=np.ones((len(self.xirr),))*20.

			self.sub5.clear()
			try:
				self.cbar5.remove()
			except:
				pass  
			# In case outliers have been detected in the inversion, take out these outliers for plotting (using mask), but do not change original data. 
			# The step of making another independent copy of self.dsubs is not needed, but just an extra assurance to make sure 
			# the original displacement data is not affected by the masking.
			dd = np.array(self.dsubs)
			dd[mask<1]=np.nan

			scats=self.sub5.scatter(self.xirr,self.yirr,s=ss,c=dd,vmin=self.dd_min,vmax=self.dd_max,edgecolor='none',cmap=matplotlib.cm.gist_rainbow_r)

			ax5=self.fig.add_axes(self.ax5)

			self.cbar5=self.fig.colorbar(scats, cax=ax5)
			self.sub5.set_title('Displacement for inversion [m]',fontsize=12)

			self.setplotpars()
		except:
			helper.handle_exception()

	def setlimits(self):
		'''
		Helper function to find the lateral extents of the plotted data.
		'''
		
		try:
			if self.subsgrid:
				minxsurf=self.shiftxsurf
				minysurf=self.shiftysurf
				maxxsurf=minxsurf+self.scalexsurf
				maxysurf=minysurf+self.scaleysurf
			else:
				minxsurf=self.shiftxres
				minysurf=self.shiftyres
				maxxsurf=minxsurf+self.scalexres
				maxysurf=minysurf+self.scaleyres

			if self.resgrid:
				minxres=self.shiftxres
				minyres=self.shiftyres
				maxxres=minxres+self.scalexres
				maxyres=minyres+self.scaleyres
			else:
				minxres=self.shiftxsurf
				minyres=self.shiftysurf
				maxxres=minxres+self.scalexsurf
				maxyres=minyres+self.scaleysurf

			self.minx=min(minxsurf,minxres)
			maxx=max(maxxsurf,maxxres)
			self.miny=min(minysurf,minyres)
			maxy=max(maxysurf,maxyres)
			scalex=maxx-self.minx
			scaley=maxy-self.miny
			self.scale=max(scalex,scaley)
		except:
			helper.handle_exception()

	def setplotpars(self):
		'''
		Helper function to set position, title and limits of plots:
		sub1 = inverted strain/compaction
		sub2 = geomec output
		sub3 = displacement error
		sub4 = reservoir properties
		sub5 = displacement
		sub6 = [removed]
		sub7 = detected outliers
		sub8 = RMS error
		sub9 = Mean absolute error
		'''
		try:
			self.sub1.set_xlim((self.minx,self.minx+self.scale))
			self.sub1.set_ylim((self.miny,self.miny+self.scale))
			self.sub2.set_xlim((self.minx,self.minx+self.scale))
			self.sub2.set_ylim((self.miny,self.miny+self.scale))
			self.sub3.set_xlim((min(self.rec_subs),max(self.rec_subs)))
			self.sub3.set_ylim((min(self.rec_subs),max(self.rec_subs)))
			self.sub4.set_xlim((self.minx,self.minx+self.scale))
			self.sub4.set_ylim((self.miny,self.miny+self.scale))
			self.sub5.set_xlim((self.minx,self.minx+self.scale))
			self.sub5.set_ylim((self.miny,self.miny+self.scale))
			self.sub7.set_xlim((self.minx,self.minx+self.scale))
			self.sub7.set_ylim((self.miny,self.miny+self.scale))
		
			self.sub3.set_title('Displacement error',fontsize=10)
			self.sub7.set_title('Detected outliers',fontsize=10)
			self.sub8.set_title('RMS error evolution',fontsize=10)
			self.sub8.set_xlabel('Internal iterations')
			self.sub8.set_ylabel('RMS error')
			self.sub9.set_title('Residual error [m]',fontsize=10)
			self.sub9.set_xlabel('Geomec runs')
			self.sub9.set_ylabel('Mean Absolute Error [m]')

			self.sub1.set_position([.04,.055,.25,.4])
			self.sub2.set_position([.385,.055,.25,.4])
			self.sub4.set_position([.04,.55,.25,.4])
			self.sub5.set_position([.385,.55,.25,.4])

			self.sub3.set_position([.72,.78,.25*3./7.,.4*3./7.])
			self.sub7.set_position([.72,.055,.25,.4])
			self.sub9.set_position([.883,.78,.09,.4*3./7.])
			self.sub8.set_position([.72,.55,.29*6/7,.4*3./7.])
			# [x (from left), y (from bottom), width, height]

			if self.cultural:
				for m in range(len(self.cult)):
					coor=self.cult[m]
					ptype=self.culttype[m]
					if ptype=='line':
						self.sub5.plot(coor[:,0],coor[:,1],'k')
						self.sub1.plot(coor[:,0],coor[:,1],'k')
						self.sub2.plot(coor[:,0],coor[:,1],'k')
						self.sub7.plot(coor[:,0],coor[:,1],'k')
						self.sub4.plot(coor[:,0],coor[:,1],'k')
					else:
						self.sub5.plot(coor[:,0],coor[:,1],'ok')
						self.sub1.plot(coor[:,0],coor[:,1],'ok')
						self.sub2.plot(coor[:,0],coor[:,1],'ok')
						self.sub7.plot(coor[:,0],coor[:,1],'ok')
						self.sub4.plot(coor[:,0],coor[:,1],'ok')
			self.display.show()
		except:
			helper.handle_exception()

	def readcultural(self): pass


	def saveresults( self ):	pass

def main():

	try:
		#logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG )
		logging.debug("------------------------------------------------------------------------------------------- IT : START")

		root = tk.Tk()
		w = subsinversion( root )

		root.bind("<KeyRelease>", w.workflow.on_key_release)

		w.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir )

		w.mainloop()  
		w.quit()
		root.quit()

		#del tk
		#del np

		#GI.send_cmd( GI.Cmd.IT1.Quit ) #do it this way and not like this: w.send_req( GI.Cmd.IT1.Quit )
		logging.debug("/////////////////////////////////////////////////////////////////////////////////////////// IT : END ")
	except:
		helper.handle_exception()

if __name__=='__main__':

	main()
