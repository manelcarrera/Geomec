from geomec_interface import GeomecInterface as GI

"""
Steps:
------
1. create 'cmds_xx'
2. add 'cmds_xx' to 'list'
3. select cmds to be run: id=xx
4. click on 'Settings' button in 'Control' window (push 'F8' to show 'Control' window)
"""

cmds_00 = [	GI.Cmd.IT1.List.Reservoir, 
			(GI.Cmd.IT1.List.Surface,{ 'res': 0 } ),
			GI.Cmd.IT1.Get.Reservoir,
			GI.Cmd.IT1.List.Measured_Displacement,
			(GI.Cmd.IT1.Get.Measured_Displacement,{'composite': 0, 'composite_idx': 0, 'component': 2}),

			GI.Cmd.Operation.LoadOperator, 
			#GI.Cmd.Operation.Construct, 
			GI.Cmd.Operation.Invert,

			GI.Cmd.IT1.Set.Depletion, # GI.Depletion.Strain

			#(GI.Cmd.DB.PointSet.AddValues,{'ps_id': 0,'dep_type': GI.Depletion.Compressibility}),
			#(GI.Cmd.DB.PointSet.AddValues,{'ps_id': 0,'dep_type': GI.Depletion.Pore_Pressure}),
			#(GI.Cmd.DB.PointSet.AddValues,{'ps_id': 0,'dep_type': GI.Depletion.Temperature}),

			#GI.Cmd.Operation.Pause,

			#(GI.Cmd.DB.PointSet.Unlink,{'ps_id': 0,'composite_idx': -1,'res':0,'dep':-1}),
			#(GI.Cmd.DB.PointSet.RemoveValues,{'ps_id': 0,'composite_idx': -1}), # -> 0: GI.Depletion.Strain , -1: Last

			#(GI.Cmd.DB.PointSet.Unlink,{'ps_id': 0,'composite_idx': -1,'res':0,'dep':-1}),
			#(GI.Cmd.DB.PointSet.RemoveValues,{'ps_id': 0,'composite_idx': -1}),

			#(GI.Cmd.DB.PointSet.Unlink,{'ps_id': 0,'composite_idx': -1,'res':0,'dep':-1}),
			#(GI.Cmd.DB.PointSet.RemoveValues,{'ps_id': 0,'composite_idx': -1}),

			#(GI.Cmd.DB.PointSet.Unlink,{'ps_id': 0,'composite_idx': -1,'res':0,'dep':-1}),
			#(GI.Cmd.DB.PointSet.RemoveValues,{'ps_id': 0,'composite_idx': -1}),

			#GI.Cmd.Operation.Pause,

			#(GI.Cmd.DB.PointSet.AddValues,{'ps_id': 0,'dep_type': GI.Depletion.Strain}),

			#GI.Cmd.Operation.Pause,
			#(GI.Cmd.DB.PointSet.ReplaceValues,{'ps_id': 0,'composite_idx': -1}), # -1: Last

			#GI.Cmd.Operation.Pause,
			(GI.Cmd.IT1.Set.Depletion,{'res': 0,'dep_type': GI.Depletion.Strain,'dep':-1,'dep_output': GI.Depletion.Output.Branch,'dep_action': GI.Depletion.Action.Current,'ps_id':0}),

			#GI.Cmd.Operation.Pause,
			(GI.Cmd.IT1.Set.Depletion,{'res': 0,'dep_type': GI.Depletion.Strain,'dep':-1,'dep_output': GI.Depletion.Output.Branch,'dep_action': GI.Depletion.Action.Replace,'ps_id':0, 'composite_idx':-1}),

			#GI.Cmd.Operation.Pause,
			(GI.Cmd.IT1.Set.Depletion,{'res': 0,'dep_type': GI.Depletion.Strain,'dep':-1,'dep_output': GI.Depletion.Output.Branch,'dep_action': GI.Depletion.Action.New,'ps_id':0}),

			GI.Cmd.Operation.Pause,
			GI.Cmd.IT1.Exe.Run_Geomec,

			GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,
			GI.Cmd.Res.Available,

			(GI.Cmd.IT1.Get.Displacement,{'composite': 0, 'composite_idx': 0, 'component': 2, 'dep': -1}) ]

cmds_01 = [	GI.Cmd.IT1.List.Reservoir, 
			GI.Cmd.IT1.List.Surface,
			GI.Cmd.IT1.Get.Reservoir ]

cmds_02 = [	GI.Cmd.IT1.Exe.Run_Geomec,

			GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,
			GI.Cmd.Res.Available,

			GI.Cmd.IT1.Get.Displacement ]

cmds_03 = [	GI.Cmd.IT1.List.Reservoir, 
			GI.Cmd.IT1.List.Surface,
			GI.Cmd.IT1.Get.Reservoir,
			GI.Cmd.IT1.List.Measured_Displacement,
			GI.Cmd.IT1.Get.Measured_Displacement,

			GI.Cmd.Operation.LoadOperator, 
			#GI.Cmd.Operation.Construct, 
			GI.Cmd.Operation.Invert,

			GI.Cmd.IT1.Set.Depletion ]

cmds_04 = [	GI.Cmd.Res.Available,
			GI.Cmd.IT1.Get.Displacement ]


cmds_05 = [	GI.Cmd.DB.Material.List,
			(GI.Cmd.DB.Material.Create,{ 'mat_type': GI.Material.Model.Linear }),	# Material
			(GI.Cmd.DB.Material.Link,{'res':0, 'mat_idx':-1}),
			#(GI.Cmd.DB.Material.GetValues,{'mat_idx':-1}),
			(GI.Cmd.DB.Material.SetValues,{'mat_idx':-1, 'mat_type':0, 'param_idx':[0,10], 'param_value':[10001,0.9]}), #GI.Material.Model.Linear -> (0,) ???
			GI.Cmd.Operation.Pause,
			(GI.Cmd.DB.Material.SetValues,{'mat_idx':-1, 'mat_type':0, 'param_idx':0, 'param_value':10007}),
			GI.Cmd.Operation.Pause,
			(GI.Cmd.DB.Material.Create,{ 'mat_type': GI.Material.Model.CamClay }),
			GI.Cmd.Operation.Pause,
			(GI.Cmd.DB.Material.Remove,{'mat_idx':-1}) ]

cmds_06 = [	GI.Cmd.IT1.List.Reservoir, 
			(GI.Cmd.IT1.List.Surface,{ 'res': 0 } ),
			GI.Cmd.IT1.Get.Reservoir,
			GI.Cmd.IT1.List.Measured_Displacement,
			(GI.Cmd.IT1.Get.Measured_Displacement,{'composite': 0, 'composite_idx': 0, 'component': 2}),
			GI.Cmd.Operation.LoadOperator, 
			GI.Cmd.Operation.Invert,
			GI.Cmd.IT1.Set.Depletion, # GI.Depletion.Strain
			GI.Cmd.IT1.Exe.Run_Geomec,
			GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,
			GI.Cmd.Res.Available,
			(GI.Cmd.IT1.Get.Displacement,{'composite': 0, 'composite_idx': 0, 'component': 2, 'dep': -1}) ]

cmds_07 = [ (GI.Cmd.DB.Material.GetValues,{'mat_idx':0}),
           (GI.Cmd.DB.Material.GetValues,{'mat_idx':1})]

list = [	cmds_00,	# 0
			cmds_01,	# 1
			cmds_02,	# 2
			cmds_03,	# 3
			cmds_04,	# 4
			cmds_05,	# 5
			cmds_06,
       cmds_07]	# 6

id=7
cmds = list[ id ]