import logging
import helper #handle_exception
import numpy as np
import trimesh as trim # trimesh functions of Kees
from geomec_interface import GeomecInterface as GI
# Just frir infi -> trim.trimesh.initiate(self.reservoirpoints,self.reservoirtriangles)

class Depletion:
	'''
	
	'''

	class Pos:
		X=0
		Y=1
		Z=2
		Val=3

	@staticmethod
	def convert_to_cm( vals ):
		logging.debug("in convert_to_cm")
		return np.array( vals )

	@staticmethod
	def convert_to_pp( vals, init_vals, cm ):
		logging.debug("in convert_to_pp")
		return (np.array( vals ) / cm) + init_vals

	@staticmethod
	def points_and_values( vals ):
		'''
		For each triangle, get its center x, y and the value from array vals (1-D array with values)
		return list of [y, x, 0, val] for all triangles.
		'''        
		try:
			#logging.debug('depletion : points_and_values : len vals:{}'.format( len( vals ) ))
			#logging.debug('depletion : points_and_values : vals:{}'.format( vals ))
			#vals = np.array(vals)

			output = []
			# Here we take every triangle tri in trim.trimesh:
			for tri in range( trim.trimesh.nTri ):
				# The node id's belonging to this tri.s
				node1=trim.trimesh.pTris[tri,0]
				node2=trim.trimesh.pTris[tri,1]
				node3=trim.trimesh.pTris[tri,2]
				#this we can ignore for now
				#area=trim.trimesh.area[tri]
				#here we calculate the x and y coordinates of the center point of this triangle, based on the coords of the 3 nodes.
				x=(trim.trimesh.pnodes[node1-1,0]+trim.trimesh.pnodes[node2-1,0]+trim.trimesh.pnodes[node3-1,0])/3.
				y=(trim.trimesh.pnodes[node1-1,1]+trim.trimesh.pnodes[node2-1,1]+trim.trimesh.pnodes[node3-1,1])/3.
				# now get the specific strain (or pore pressure or compressibility) value for this triangle.
				val = vals[ tri ]
				# store x, y, value as N, E (i.e. reverse coords!)
				output.append( [ y, x, 0, val ] ) 
			logging.debug("output:{}", output)
			return np.array( output )
		except:
			helper.handle_exception()


	@staticmethod
	def points_fake():
		try:
			output=[] #[ x, y, z, val ]
			output.append( [ 1., 2., 3., 10. ] )
			output.append( [ 4., 5., 6., 20. ] )
			return np.array( output )
		except:
			helper.handle_exception()

	@staticmethod
	def values_a( dep_type, gui, update_vals = True ):
		'''
		Take the values from the inversion (gui) and calculate the Cm/PP/e for Geomec
		Return 1-D array vals with updated values.
		'''
		try:          
			# (Depletion.Type) : Strain / Compressibility / PorePressure 
			#data	= Depletion.points_fake() 
			# KB: Take the calculated change in pressure and add this to the initial pressure. 
			# Note that this needs to be different for compressibility and strain???
			if dep_type == GI.Depletion.Compressibility:
				# Invert for compressibility (Cm = e / deltaP).
				# Calculated Cm is deltaCm, so add original cm (self.avcm)
				# Does not work correctly because this needs a known dP, which needs to be calculated from the last 2 steps. 
				# Needs to start with assumption that there is no initial cm (gui.prevcm starts as 0)
				vals = (gui.rec_par / gui.prevpres) + gui.prevcm
				#logging.debug("avg strain: {}, avg pres: {}, avg prevcm: {}, avg result: {}".format(np.average(gui.rec_par), np.average(gui.prevpres), np.average(gui.prevcm), np.average(vals)))
				# Update 'initial' compressiblity to an independent copy of updated compressibility for next step.
				gui.prevcm = np.array(vals)
			elif dep_type == GI.Depletion.Pore_Pressure:
				# Invert for pore pressure (deltaP = e / Cm).
				# Calculated deltaP needs to be subtracted from previous absolute pressure.
				vals = gui.prevpres - (gui.rec_par / gui.avcm)
				# Update pore pressure for the next iteration step:
				gui.prevpres = np.array(vals)                 
			else: 
				# For strain, the first assumption is zero strain (defined in inversion_GUI). 
				# For the consecutive iterations, add new strain to previous strain results.
				vals = gui.rec_par + gui.prevstrain
				#logging.debug("------ STRAIN UPDATED: {}, original was: {}, added is: {}".format(vals, gui.prevstrain, gui.rec_par))
				gui.prevstrain = np.array(vals)
			#logging.debug('depletion : values_a : dep_type: {} : len vals:{} mean vals: {}'.format( GI.Depletion.List[ dep_type ], len( vals ), np.average(vals) ))
			return vals
		except:
			helper.handle_exception()

	@staticmethod
	def data( dep_type, gui ):
		try:
			# Convert inverted strain values from GUI to output required for Geomec:            
			values = Depletion.values_a( dep_type, gui )
			# Obtain for each mesh element in python the center coordinates and value:            
			data = Depletion.points_and_values( values )    

			# columns-> 0:x - 1:y - 2:z - 3:val
			points	= data[:,[Depletion.Pos.X,Depletion.Pos.Y,Depletion.Pos.Z]]
			vals	= data[:,Depletion.Pos.Val]

			return (points,vals)
		except:
			helper.handle_exception()

	@staticmethod
	def points( dep_type, gui ):
		try:
			# KB: This function should only give the coordinates, do not yet calculate the depletion values here.
			values	= gui.rec_par # Depletion.values_a( dep_type, gui )
			data	= Depletion.points_and_values( values )    # columns-> 0:x - 1:y - 2:z - 3:val
			points	= data[:,[Depletion.Pos.X,Depletion.Pos.Y,Depletion.Pos.Z]]
			return points
		except:
			helper.handle_exception()

	@staticmethod
	def values( dep_type, gui ):
		try:
			# KB: Only here, update the values:           
			values	= Depletion.values_a( dep_type, gui)
			data	= Depletion.points_and_values( values )    # columns-> 0:x - 1:y - 2:z - 3:val
			vals	= data[:,Depletion.Pos.Val]
			logging.debug( 'Depletion : values : vals:{}'.format( vals ) )
			return vals
		except:
			helper.handle_exception()
