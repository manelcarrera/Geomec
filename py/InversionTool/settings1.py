import logging
from geomec_interface import GeomecInterface as GI

settings1 = {
	'force_can' : True,
	'LINES' : 1, # key sentive so it must be in capital letters
	'loggging_level' : logging.DEBUG, # logging.CRITICAL logging.ERROR logging.WARNING logging.INFO logging.DEBUG

	'component'		: GI.Component.Z,
	'composite'		: GI.Composite.Displacement,
	'dep'			: GI.Depletion.Id.Last,
	'dep_output'	: GI.Depletion.Output.Branch,
	'prop_form'		: GI.Prop.Form.Thickness,
	'prop_mat'		: GI.Prop.Mat.PoissonsRatio,
	'prop_dep'		: GI.Prop.Dep.PorePressure,
	'ps_id'			: 0,
	'list'			: GI.List.Reservoir,
	'composite_idx'	: -1, # case: set_depletion with action == GI.Depletion.Action.Replace
	'mat_idx'		: -1,
	'mat_type'		: GI.Material.Model.Linear,
	'param_idx'		: 0,
	'param_value'	: 0,
	'control_wnd'	: False,
	'next'			: { GI.Cmd.IT1.List.Reservoir:				GI.Cmd.IT1.List.Surface,
						#GI.Cmd.IT1.List.Surface:				GI.Cmd.IT1.Get.Reservoir,
						GI.Cmd.Operation.Reservoir:				GI.Cmd.IT1.List.Measured_Displacement,
						#GI.Cmd.IT1.List.Measured_Displacement:	GI.Cmd.IT1.Get.Measured_Displacement,
						GI.Cmd.Operation.Subsidence:			GI.Cmd.Operation.Construct,
						#GI.Cmd.Operation.Construct:				GI.Cmd.Operation.Invert,
						#GI.Cmd.Operation.Invert:				GI.Cmd.IT1.Set.Depletion,
						GI.Cmd.IT1.Exe.Run_Geomec:				GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,
						GI.Cmd.IT1.Exe.Wait_End_Run_Geomec:		GI.Cmd.Res.Available,
						GI.Cmd.Res.Available:					GI.Cmd.IT1.Get.Displacement 
						}
}
