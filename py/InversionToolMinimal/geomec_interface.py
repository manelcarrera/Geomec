#import sys
import geomec
import logging
import numpy as np #FIXME: remove

#GI = GeomecInterface

class GeomecInterface:

	class Prop:
		class Form:
			Thickness		= 0
			List			= [ 'Thickness' ]
			Name			= 'Prop.Form'
		class Mat:
			PoissonsRatio	= 0
			YoungsModulus	= 1
			Cm				= 2
			List			= [ 'PoissonsRatio', 'YoungsModulus', 'Cm' ]
			Name			= 'Prop.Mat'
		class Dep:
			PorePressure	= 0
			Strain			= 1
			List			= [ 'PorePressure', 'Strain' ]
			Name			= 'Prop.Dep'
	class Composite:
		Displacement				= 0
		TotalStress					= 1
		TotalStressChange			= 2
		TotalStrain					= 3
		PlasticStrain				= 4
		EffectiveStress				= 5
		EffectiveStressChange		= 6
		PorePressure				= 7
		Temperature					= 8
		Consolidation				= 9
		Porosity					= 10
		FaultEffectiveNormalStress	= 11
		FaultShearStress			= 12
		FaultDisplacement			= 13
		FaultDilatation				= 14
		FaultPlasticSlip			= 15
		Aperture					= 16
		ShearCapacity				= 17
		DerivedResults				= 18

		List						= [ 'Displacement', 'TotalStress', 'TotalStressChange', 'TotalStrain', 'PlasticStrain', 'EffectiveStress', 'EffectiveStressChange', 'PorePressure', 'Temperature', 'Consolidation', 
										'Porosity', 'FaultEffectiveNormalStress','FaultShearStress', 'FaultDisplacement', 'FaultDilatation', 'FaultPlasticSlip', 'Aperture', 'ShearCapacity', 'DerivedResults' ]
		Name						= 'Composite'
			
	class Component:
		X = 0
		Y = 1
		Z = 2
		List = [ 'X', 'Y', 'Z' ]
		Name = 'Component'
	class List:
		Surface			= 0
		Formation		= 1
		Reservoir		= 2
		Depletion		= 3
		Displacement	= 4
		DB_Composite	= 5
		List = [ 'Surface', 'Formation', 'Reservoir', 'Depletion', 'Displacement', 'DB_Composite' ]
		Name = 'List'
	class Depletion:
		Strain			= 0
		Compressibility	= 1
		Pore_Pressure	= 2
		Temperature		= 3
		List = [ 'Strain', 'Compressibility', 'Pore_Pressure', 'Temperature' ]
		List_Short = [ 'Strain', 'Cm', 'PP', 'Temp.' ]
		Name = 'Depletion'
		class Id:
			Last		= -1	# 0
			All			= -2	# 1
			New			= -3	# 2
			Other		= -4	# 3
			List		= [ 'Last', 'All', 'New', 'Other' ] # TODO: Take care !!!!!
			Name		= 'Dep.Id'
		class Output:
			Branch		= 1
			Results		= 2
			List		= [ 'Branch', 'Results' ] # TODO: Take care !!!!!
			Name		= 'Dep.Output'
		class Action:
			Current		= 0 # add new values to current depletion
			Replace		= 1	# overwrite current depletion values
			New			= 2 # create mew depletion an add new values to it
			List		= ['Current','Replace','New']
			Name		= 'Dep.Action'
	class Pointset:
			class Id:
				Default = 0
	class Material:
			class Model:
				Linear              =  0,
				CamClay             =  1,
				MohrCoulomb         =  2,
				ModifiedMohrCoulomb =  3,
				Creep               =  4,
				Undrained           =  5,
				MC_Cohesion_Hard1   =  6,
				MC_Cohesion_Hard2   =  7,
				MC_Cohesion_Hard3   =  8,
				MC_Friction_Hard1   =  9,
				MC_Friction_Hard2   = 10,
				MC_Friction_Hard3   = 11,
				Rigidity            = 13,
				DualCap             = 14,  #Not used anymore, maintained for preventing magic numbers.
				DualCap_Linela      = 15,
				Fracture_Anisotropy = 16,
				Upscaled_Anisotropy = 17,
				Fracture_Aperture   = 18,
				Anisotropic_CamClay = 19,
				Fracture_Aperture2  = 20,
				CamClay_Creep       = 21
				List				= ['Linear','CamClay','MohrCoulomb','ModifiedMohrCoulomb','Creep','Undrained','MC_Cohesion_Hard1','MC_Cohesion_Hard2','MC_Cohesion_Hard3','MC_Friction_Hard1','MC_Friction_Hard2','MC_Friction_Hard3',
										'Rigidity','DualCap','DualCap_Linela','Fracture_Anisotropy','Upscaled_Anisotropy','Fracture_Aperture','Anisotropic_CamClay','Fracture_Aperture2','CamClay_Creep' ]
				Name				= 'Material.Model'
				Ids					= []
			class Parameter:
				Youngs_Modulus = 0
				Poissons_Ratio = 1
				Cohesion = 2
				Friction_Angle = 3
				Porosity = 4
				Bulk_Stiffness = 5
				Shear_Modulus = 6
				Fluid_Bulk_Modulus = 7
				Density = 8
				Vp = 9
				Vs = 10
				Initial_Friction_Angle = 11
				Hardening_Parameter = 12
				Preconsolidation_Stress = 13
				Cap_Shape_Factor = 14
				Dilatation_Angle = 15
				SHtot_Svtot = 16
				Shtot_Svtot = 17
				SHtot_azimuth = 18
				Creep_strain_rate_coefficient_1 = 19
				Creep_strain_rate_coefficient_2 = 20
				Creep_power_coefficient_1 = 21
				Creep_power_coefficient_2 = 22
				Dynamic_Uniaxial_Stiffness = 23
				Dynamic_Shear_Stiffness = 24
				Undrained_Youngs_Modulus = 25
				Undrained_Poissons_Ratio = 26
				Hardening_Cohesion_1 = 27
				Hardening_Cohesion_2 = 28
				Hardening_Cohesion_3 = 29
				Equivalent_Plastic_Strain_1 = 30
				Equivalent_Plastic_Strain_2 = 31
				Equivalent_Plastic_Strain_3 = 32
				Hardening_Friction_Angle_1 = 33
				Hardening_Friction_Angle_2 = 34
				Hardening_Friction_Angle_3 = 35
				Activation_energy_Q1_R = 36
				Activation_energy_Q2_R = 37
				Reference_stress = 38
				KNNNN = 39
				KNNEE = 40
				KNNDD = 41
				KNNNE = 42
				KNNED = 43
				KNNDN = 44
				KEEEE = 45
				KEEDD = 46
				KEENE = 47
				KEEED = 48
				KEEDN = 49
				KDDDD = 50
				KDDNE = 51
				KDDED = 52
				KDDDN = 53
				KNENE = 54
				KNEED = 55
				KNEDN = 56
				KEDED = 57
				KEDDN = 58
				KDNDN = 59
				Tensile_stretch = 60
				Vol_Thermal_Expansion_Coeff = 61
				Secondary_Preconsolidation_Stress = 62
				Secondary_Hardening_Parameter = 63
				Uniaxial_Elastic_Compressibility = 64
				Uniaxial_Plastic_Compressibility = 65
				Youngs_Modulus_normal = 66
				Youngs_Modulus_transverse = 67
				Poissons_Ratio_normal = 68
				Poissons_Ratio_transverse = 69
				Layer_normal_dip = 70
				Layer_normal_azimuth = 71
				Fracture_stiffness_normal = 72
				Fracture_stiffness_shear = 73
				Highest_Fracture_Density = 74
				Highest_Fracture_Density_incl = 75
				Highest_Fracture_Density_azimuth = 76
				Lowest_Fracture_Density = 77
				Lowest_Fracture_Density_incl = 78
				Lowest_Fracture_Density_azimuth = 79
				Intermediate_Fracture_Density = 80
				Thermal_linear_expansion_normal = 81
				Thermal_linear_expansion_lateral = 82
				Thermal_Conductivity = 83
				Thermal_Capacity = 84
				Permeability = 85
				Youngs_Modulus_in_decompaction = 86
				Poissons_Ratio_in_decompaction = 87
				Viscosity = 88
				Fluid_Thermal_Expansion_Coeff = 89
				Fluid_Density = 90
				Reference_Stress = 91
				Aperture_Dependency = 92
				Grain_Stiffness = 93
				Youngs_Mod_normal_in_decompact = 94
				Youngs_Mod_transverse_in_decompact = 95
				Poissons_Ratio_normal_in_decompact = 96
				Poissons_Ratio_transverse_in_decompact = 97
				Anisotropic_Shear_Mod_in_decompact = 98
				Anisotropic_Shear_Modulus = 99
				Thomsen_epsilon = 100
				Thomsen_gamma = 101
				Thomsen_delta = 102
				V0_in_highest_fracture_density_dir = 103
				V0_in_intermediate_fracture_density_dir = 104
				V0_in_lowest_fracture_denstiy_dir = 105
				Undrained_Thermal_Expansion_Coeff = 106
				Reference_Aperture = 107
				Dummy = 108
				Name = 'Material.Parameter'

	class Value:
		NotDefined		= -1
		PollingInterval	= 100		# ms
		PollingInterval_Long	= 1000	# ms
		PollingInterval_Short	= 10	# ms
		ListDelimiter	= '*'
		class Default: # ???? needed?
			CompositeIdx=0
			ReservoirId	= 0
			SurfaceId	= 0
			PointsetId	= 0
	class InvertFor:
		Strain			= 0
		Compaction		= 1
		List			= [ 'Strain', 'Compaction' ]
		Name			= 'InvertFor'
	class Plot:
		Depth				= 0
		Thickness			= 1
		PoissonsRatio		= 2
		YoungsModulus		= 3
		InitialPorePressure = 4
		Cm					= 5
		List				= [ 'Depth', 'Thickness', 'PoissonsRatio', 'YoungsModulus', 'InitialPorePressure', 'Cm' ]
		List_Short			= [ 'Depth', 'Thick', 'PR', 'YM', 'Ini PP', 'Cm' ]
		Name				= 'Plot'
	class GUI:
		InvertFor			= 0
		DepType				= 1
		DepAction			= 2
		Plot				= 3
		List				= ['InvertFor','DepType','DepAction','Plot']

	class Cmd:

		class IT1: 
			class List:
				Reservoir				= 0		# Format : Ex.: "Item1*Item2*Item3"
				Surface					= 1
				Depletion				= 2
				Measured_Displacement	= 3
			class Get:
				Reservoir				= 4		# Elems (triangles)(p0,p1,p2) + Points (x,y,z) + Params (thickness,poisson's ratio,youngs modulus, pore pressure)
												# Elems : 3x points indices; indices start with '1'; points are the ones received in the command; indices are the order these points are received; NOT mesh indices 

				Measured_Displacement	= 5		# Elems id + subsidences + Z (point depth) + wdat (hard-coded: 4000.0)
												# Elems : poinset points subset; those that can be mapped in a given reservoir surface 
												# Subsidences : having elements above, the x,y and z sudsidence components of their mippoints; subsidences taken from the DB
												# Z : depth of points considered to get subsidences above

				Displacement			= 6		# Elems id + subsidences + Z (point depth) + wdat (hard-coded: 4000.0)
												# Elems : poinset points subset; those that can be mapped in a given reservoir surface 
												# Subsidences : having elements above, the x,y and z sudsidence components of their mippoints; subsidences taken from the results
												# Z : depth of points considered to get subsidences above
				Material_Parameter		= 7
			class Set:
				Depletion				= 8
			class Exe:
				Run_Geomec				= 9
				Run_Geomec_Step			= 10
				Wait_End_Run_Geomec		= 11
				Remove_Depletion		= 12
				Save_Displacement		= 13	# [NOT IMPLEMENTED]
			class Control:
				Quit					= 14

		# Phase II
		class Model: 
			Elems						= 15	# [IT1] as elems in IT1.Get.Reservoir
			Points						= 16	# All the points; not filetred
			class Prop:
				Form					= 17	# values: look at GI.Prop.Form
				Mat						= 18	# values: look at GI.Prop.Mat
				Dep						= 19	# values: look at GI.Prop.Dep
			class Depletion:
				Add						= 20	# Added at the end
				Remove					= 21	# pre-condition : output is 'Results' and not futher 'Branch' depletion stages
				Output					= 22	# values: look at GI.Depletion.Output
		class Res:
			Available					= 23
			Values						= 24	# [IT1] DB pointset points subset : those in the surface
		class DB:
			Points						= 25	# All the points; not filtered
			Values						= 26	#
			Elems						= 27	# [IT1] 
			class PointSet:
				Create					= 28
				Remove					= 29
				AddValues				= 30
				Link					= 31	# 
				Unlink					= 32
				Values					= 33
				ValuesList				= 34
				RemoveValues			= 35
				ReplaceValues			= 36
			class Material:
				List					= 37
				Create					= 38
				Remove					= 39
				GetValues				= 40
				SetValues				= 41
				Link					= 42
				Unlink					= 43
		class Gen:
			List						= 44
		class Control:
			Ini							= 45	# [INTERNAL USE]
			End							= 46	# [INTERNAL USE]
		NoCmd							= 47
		class Operation:
			Construct					= 48
			Invert						= 49
			Reservoir					= 50
			Subsidence					= 51
			Test						= 52
			Pause						= 53
			LoadOperator				= 54
			#List						= [ Operation.Construct, Operation.Invert, Operation.Reservoir, Operation.Test ]
			List						= [ 48, 49, 50, 51, 52, 53, 54 ] # FIXME
		class Type:
			Command						= 55
			Operation					= 56
			Other						= 57

		Name1							= 'Cmd_IT1'
		Name2							= 'Cmd_IT2'
		Name3							= 'Cmd_IT3'
		Name4							= 'Operations'

		name = (
			# [ IT1 ]
			'IT1.List.Reservoir',			# 0
			'IT1.List.Surface',				# 1
			'IT1.List.Depletion',			# 2
			'IT1.List.Measured_Displacement',	# 3

			'IT1.Get.Reservoir',			# 4
			'IT1.Get.Measured_Displacement',# 5
			'IT1.Get.Displacement',			# 6
			'IT1.Get.Material_Parameter',	# 7

			'IT1.Set.Depletion',			# 8

			'IT1.Exe.Run_Geomec',			# 9
			'IT1.Exe.Run_Geomec_Step',		# 10
			'IT1.Exe.Wait_End_Run_Geomec',	# 11
			'IT1.Exe.Remove_Depletion',		# 12
			'IT1.Exe.Save_Displacement',	# 13

			'IT1.Control.Quit',				# 14

			# [ Phase II ]
			# Model
			'Model.Elems',					# 15
			'Model.Points',					# 16
				# Prop
			'Model.Prop.Form',				# 17
			'Model.Prop.Mat',				# 18
			'Model.Prop.Dep',				# 19
				# Dep
			'Model.Depletion.Add',			# 20
			'Model.Depletion.Remove',		# 21
			'Model.Depletion.Output',		# 22

			#RES
			'Res.Available',				# 23
			'Res.Values',					# 24

			#DB
			'DB.Points',					# 25
			'DB.Values',					# 26
			'DB.Elems',						# 27
				#PS
			'DB.PointSet.Create',			# 28
			'DB.PointSet.Remove',			# 29
			'DB.PointSet.AddValues',		# 30
			'DB.PointSet.Link',				# 31
			'DB.PointSet.Unlink',			# 32
			'DB.PointSet.Values',			# 33
			'DB.PointSet.ValuesList',		# 34
			'DB.PointSet.RemoveValues',		# 35
			'DB.PointSet.ReplaceValues',	# 36

			'DB.Material.List',				# 37
			'DB.Material.Create',			# 38
			'DB.Material.Remove',			# 39
			'DB.Material.GetValues',		# 40
			'DB.Material.SetValues',		# 41
			'DB.Material.Link',				# 42
			'DB.Material.Unlink',			# 43

			#Gen
			'Gen.List',						# 44

			#Control - IT Phase II
			'Control.Ini',					# 45
			'Control.End',					# 46

			'NoCmd',						# 47

			# IT Internal operations
			'Operation.Construct',			# 48
			'Operation.Invert',				# 49
			'Operation.Reservoir',			# 50
			'Operation.Subsidence',			# 51
			'Operation.Test',				# 52
			'Operation.Pause',				# 53
			'Operation.LoadOperator',		# 54
		)

		List1 = (
			'IT1.List.Reservoir',			# 0
			'IT1.List.Surface',				# 1
			'IT1.List.Depletion',			# 2
			'IT1.List.Measured_Displacement',	# 3
			'IT1.Get.Reservoir',			# 4
			'IT1.Get.Measured_Displacement',# 5
			'IT1.Get.Displacement',			# 6
			'IT1.Get.Material_Parameter',	# 7
			'IT1.Set.Depletion',			# 8
			'IT1.Exe.Run_Geomec',			# 9
			'IT1.Exe.Run_Geomec_Step',		# 10
			'IT1.Exe.Wait_End_Run_Geomec',	# 11
			'IT1.Exe.Remove_Depletion',		# 12
			'IT1.Exe.Save_Displacement'	# 13
		)

		List2 = (
			'Model.Elems',					# 15
			'Model.Points',					# 16
			'Model.Prop.Form',				# 17
			'Model.Prop.Mat',				# 18
			'Model.Prop.Dep',				# 19
			'Model.Depletion.Add',			# 20
			'Model.Depletion.Remove',		# 21
			'Model.Depletion.Output',		# 22
			'Res.Available',				# 23
			'Res.Values',					# 24
			'DB.Points',					# 25
			'DB.Values',					# 26
			'DB.Elems',						# 27
			'DB.PointSet.Create',			# 28
			'DB.PointSet.Remove',			# 29
			'DB.PointSet.AddValues',		# 30
			'DB.PointSet.Link',				# 31
			'DB.PointSet.Unlink',			# 32
			'DB.PointSet.Values',			# 33
			'DB.PointSet.ValuesList',		# 34
			'DB.PointSet.RemoveValues',		# 35
			'DB.PointSet.ReplaceValues',	# 36
		)

		List3 = (
			'DB.Material.List',				# 37
			'DB.Material.Create',			# 38
			'DB.Material.Remove',			# 39
			'DB.Material.GetValues',		# 40
			'DB.Material.SetValues',		# 41
			'DB.Material.Link',				# 42
			'DB.Material.Unlink',			# 43
			#Gen
			'Gen.List',						# 44
		)

		List4 = (
			'Operation.Construct',			# 47
			'Operation.Invert',				# 48
			'Operation.Reservoir',			# 49
			'Operation.Subsidence',			# 50
			'Operation.Test',				# 51
			'Operation.Pause',				# 52
			'Operation.LoadOperator',		# 53
		)

		params = (
			# [ IT1 ]
			[],						#'IT1.List.Reservoir',				# 0 -> NO PARAMS
			['res'],							#'IT1.List.Surface',				# 1 
			[],						#'IT1.List.Depletion',				# 2 -> NO PARAMS
			['composite'],						#'IT1.List.Mesured_Displacement',	# 3	-> NO PARAMS

			['res','sur','dep'],				#'IT1.Get.Reservoir',				# 4
			['composite','component','composite_idx'],			#'IT1.Get.Measured_Displacement',	# 5
			['composite','component','composite_idx','dep'],				#'IT1.Get.Displacement',			# 6
			['res', 'type', 'param', 'sur'],	#'IT1.Get.Material_Parameter',		# 7

			['res','dep_type','dep','dep_output','dep_action','ps_id','composite_idx'],		#'IT1.Set.Depletion',				# 8 -> ???????

			[],									#'IT1.Exe.Run_Geomec',				# 9 -> NO PARAMS
			[],									#'IT1.Exe.Run_Geomec_Step',			# 10 -> NO PARAMS
			[],									#'IT1.Exe.Wait_End_Run_Geomec',		# 11 -> NO PARAMS
			['dep'],							#'IT1.Exe.Remove_Depletion',		# 12
			[],									#'IT1.Exe.Save_Displacement',		# 13 -> NOT IMPLEMENTED

			[],									#'IT1.Control.Quit',				# 14

			# [ Phase II ]
			# Model
			['res','sur'],						#'Model.Elems',						# 15
			['res','sur'],						#'Model.Points',					# 16
				# Prop
			['res','sur','prop_form'],				#'Model.Prop.Form',					# 17
			['res','sur','dep','prop_mat'],			#'Model.Prop.Mat',					# 18
			['res','sur','dep','prop_dep'],			#'Model.Prop.Dep',					# 19
				# Dep
			['dep_output'],							#'Model.Depletion.Add',				# 20
			['dep'],							#'Model.Depletion.Remove',			# 21
			['dep','dep_output'],					#'Model.Depletion.Output',			# 22

			#RES
			#['res','sur','dep','bd_composite_idx','composite','component'],#'Res.Values',		# 22	# points from PS mapped to the surface
			[],										#'Res.Available					# 23	# points from PS NOT!!! mapped to the surface
			['composite','component','composite_idx','dep'],	#'Res.Values',					# 24	# points from PS NOT!!! mapped to the surface

			#DB
			['composite','composite_idx'],				#'DB.Points',				# 25
			['composite','composite_idx','component'],	#'DB.Values',				# 26
			['res','sur','composite','composite_idx'],	#'DB.Elems',				# 27
				#PS
			['ps_id','dep_type'],			#'DB.PointSet.Create',				# 28
			['ps_id'],					#'DB.PointSet.Remove',				# 29
			['ps_id','dep_type'],								#'DB.PointSet.AddValues',			# 30
			['ps_id','composite_idx','res','dep'],					#'DB.PointSet.Link',				# 31
			['ps_id','composite_idx','res','dep'],		#'DB.PointSet.Unlink',				# 32

			['ps_id','composite_idx'],					#'DB.PointSet.Values',				# 33
			['ps_id'],							#'DB.PointSet.ValuesList',			# 34
			['ps_id','composite_idx'],					#'DB.PointSet.RemoveValues',		# 35
			['ps_id','composite_idx'],					#'DB.PointSet.ReplaceValues',		# 36

			[],#'DB.Material.List',					# 37	# FIXME: set params for all material cmds
			['mat_type'],#'DB.Material.Create',		# 38	
			['mat_idx'],#'DB.Material.Remove',		# 39	
			['mat_idx'],#'DB.Material.GetValues',	# 40
			['mat_idx','mat_type','param_idx','param_value'],#'DB.Material.SetValues',	# 41
			['mat_idx','res'],#'DB.Material.Link',	# 42
			['mat_idx','res'],#'DB.Material.Unlink',		# 43

			#Gen
			['list','param'],					#'Gen.List',						# 44

			#Control - IT Phase II
			[],						#'Control.Ini',						# 34
			[],						#'Control.End',						# 35
			[],						#'NoCmd'							# 36
		)

	#-------------------------- send: IT -> GM
	@staticmethod
	def send_cmd( cmd ):
		geomec.send_cmd( cmd )

	#-------------------------- get: II <- GM
	@staticmethod
	def get_result():
		return geomec.get_result()

	#-------------------------- show gui: GM -> IT
	@staticmethod
	def gui_it():
		import inversion_GUI
		inversion_GUI.main()