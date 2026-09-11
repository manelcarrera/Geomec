#include "IT_Command.h"
#include "Printer.h"

#include "IT_Data.h"

Command::Command( eType _cmd, IT_Data* _data ) : m_cmd( _cmd ), m_data( _data ){}
Command::Command() : Command( Cmd_None ){}
Command::Command( const Command& _cmd ) : m_cmd( _cmd.m_cmd ), m_data( _cmd.m_data ){}

void Command::print(){ Printer::instance()->debug("cmd: %d req/res: %p", m_cmd, m_data ); }

const std::string Command::m_name[]=
{		
	"IT1_List_Reservoir",					// 0
	"IT1_List_Surface",						// 1
	"IT1_List_Depletion",					// 2
	"IT1_List_Measured_Displacement",		// 3
	"IT1_Get_Reservoir",					// 4
	"IT1_Get_Measured_Displacement",		// 5
	"IT1_Get_Displacement",					// 6
	"IT1_Get_Material_Parameter",			// 7
	"IT1_Set_Depletion",					// 8
	"IT1_Exe_Run_Geomec",					// 9
	"IT1_Exe_Run_Geomec_Step",				// 10
	"IT1_Exe_Wait_End_Run_Geomec",			// 11
	"IT1_Exe_Remove_Depletion",				// 12
	"IT1_Exe_Save_Displacement",			// 13
	"IT1_Control_Quit",						// 14
	// Phase II
	//Model
	"Model_Elems",							// 15
	"Model_Points",							// 16
		//Prop
	"Model_Prop_Form",						// 17
	"Model_Prop_Mat",						// 18
	"Model_Prop_Dep",						// 19
		//Dep
	"Model_Dep_Add",						// 20
	"Model_Dep_Remove",						// 21
	"Model_Dep_Output",						// 22
	//Res
	"Res_Available",						// 23
	"Res_Values",							// 24
	//BD
	"DB_Points",							// 25
	"DB_Values",							// 26
	"DB_Elems",								// 27
		//PointSet
	"DB_PointSet_Create",					// 28
	"DB_PointSet_Remove",					// 29
	"DB_PointSet_AddValues",				// 30
	"DB_PointSet_Link",						// 31
	"DB_PointSet_Unlink",					// 32

	"DB_PointSet_Values",					// 33
	"DB_PointSet_ValuesList",				// 34
	"DB_PointSet_RemoveValues",				// 35
	"DB_PointSet_ReplaceValues",			// 36
		//Material
	"DB_Material_List",						// 37
	"DB_Material_Create",					// 38
	"DB_Material_Remove",					// 39
	"DB_Material_GetValues",				// 40
	"DB_Material_SetValues",				// 41
	"DB_Material_Link",						// 42
	"DB_Material_Unlink",					// 43

	//Gen
	"Gen_List",								// 44
	//Control - IT Phase II	
	"Control_Ini",							// 45
	"Control_End",							// 46

	"Cmd_None"								// 47
};
