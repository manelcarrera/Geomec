//////////////////////////////////////////////////////////////////////
//
// Definition of the tables in the GM3 database
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLEDEF_H__DC291152_F928_4BAD_ACE5_20104E85B879__INCLUDED_)
#define AFX_TABLEDEF_H__DC291152_F928_4BAD_ACE5_20104E85B879__INCLUDED_

#define TB_GLOBAL_SETTINGS _T("GlobalSettings")   // Table name of global settings
#define FD_ECLIPSE_IMPORT _T("ImportingEclipse")  // Import Eclipse field ...
#define FD_TOT_VERT_STRESS_GRAD _T("TotVertGrad") // Total vertical stress gradient field
#define FD_VERT_STRESS_EXP _T("VertStressExp")    // Vertical stress exponent field
#define FD_WATER_COL_GRAD _T("WaterColGrad")      // Water Column Stress Gradient field
#define FD_NR_OF_TIMESTEPS _T("NrOfTimeSteps")    // Number of timesteps field
#define FD_PROJECT_INFO _T("ProjectInfo")         // Project info field
#define FD_VERSION_DB _T("Version")               // Database version
#define FD_TIME_ANALYSIS _T("TimeAnalysis")       // Time Analysis for non lin settings

#define TB_TIMESTEP _T("TimeStep")      // Table name of timesteps
#define FD_TIMESTEP_NR _T("TimeStepNr") // Timestep nr field
#define FD_TIMESTEP_NAME _T("Name")     // Name of the timestep
#define FD_YEAR _T("Year")              // Year of the timestep
#define FD_YEAROK _T("YearOk")          // Field Year OK flag

#define TB_NONLIN _T("NonlinParams")                // Table name for non lin params
#define FD_NONLIN_LOADSTEPS _T("NumLoadsteps")      // Loadsteps field
#define FD_NONLIN_CONVNORMVAL _T("ConvNormValue")   // Convergence Norm value
#define FD_NONLIN_MAXNUMITER _T("MaxNumIterations") // Maximum nr of iterations
#define FD_NONLIN_ITERSCHEME _T("IterationScheme")  // Iteration scheme
#define FD_NONLIN_CONVTYPE _T("ConvNormType")       // Convergence type

#define TB_HORIZON _T("Seismic")                   // Table name of horizon ...
#define FD_HORIZON_NAME _T("Name")                 // Name field name of horizon
#define FD_HORIZON_COLOR _T("ColorCode")           // Color code of horizon
#define FD_HORIZON_NR _T("SeismicNr")              // Index of horizon
#define FD_HORIZON_X_MAX _T("XMax")                // Not used
#define FD_HORIZON_Y_MAX _T("YMax")                // Not used
#define FD_HORIZON_Z_MAX _T("ZMax")                // Not used
#define FD_HORIZON_X_MIN _T("XMin")                // Not used
#define FD_HORIZON_Y_MIN _T("YMin")                // Not used
#define FD_HORIZON_Z_MIN _T("ZMin")                // Not used
#define FD_HORIZON_CONST_DEPTH _T("ConstantDepth") // constant depth or not ...
#define FD_HORIZON_PT_X _T("X")                    // X
#define FD_HORIZON_PT_Y _T("Y")                    // Y
#define FD_HORIZON_PT_Z _T("Z")                    // Z
#define TB_HORIZON_PT _T("SeismicPoint")           // Table of horizon pt's
#define FD_HORIZON_INDEX _T("InSeismic")           // Index of horizon in pt table...
#define FD_HORIZON_TOP_FOR _T("TopFormation")      // Top Formation ...

#define TB_FORMATION _T("Formation")
#define FD_FORMATION_INDEX _T("FormationNr")
#define FD_FORMATION_NAME _T("Name")
#define FD_FORMATION_COLOR _T("ColorCode")
#define FD_FORMATION_ELEMENT _T("ElementCount")
#define FD_FORMATION_RESERVOIR _T("IsReservoir")
#define FD_MATERIAL_NAME _T("MaterialName")
#define FD_FORMATION_GWC _T("IsGWC")
#define FD_FOR_CONT_DEPTH _T("InsituReservoirContactDepth")
#define FD_FOR_CONT_PRESS _T("InsituReservoirContactPressure")
#define FD_FOR_GRAD_UP _T("InsituReservoirGradientAbove")
#define FD_FOR_GRAD_DOWN _T("InsituReservoirGradientBelow")
#define FD_FOR_INI_PORE_PRESS _T("InSituPorePressure")
#define FD_FOR_SINGLE_INSITU _T("SingleInSituPressure")
#define FD_FOR_PORE_PRESS _T("PorePressure")
#define FD_FOR_PORE_GRAD _T("PorePressureGradient")
#define FD_FOR_INI_PORE_GRAD_OK _T("InSituPressureGradientOK")
#define FD_FOR_INI_REF_DEPTH_OK _T("InSituReferenceDepthOK")
#define FD_FOR_INI_PORE_GRAD _T("InSituPressureGradient")
#define FD_FOR_INI_REF_DEPTH _T("InSituReferenceDepth")

#define TB_PRESSURE _T("Pressure")
#define FD_PRESSURE_FOR_INDEX _T("InFormation")
#define FD_PRESSURE_TIME_INDEX _T("TimeStep")
#define FD_PRESSURE_MAX_X _T("XMaxAll")
#define FD_PRESSURE_MAX_Y _T("YMaxAll")
#define FD_PRESSURE_MIN_X _T("XMinAll")
#define FD_PRESSURE_MIN_Y _T("YMinAll")
#define FD_PRESSURE_MAX_Z _T("ZMax")
#define FD_PRESSURE_MIN_Z _T("ZMin")
#define FD_PRESSURE_GWC _T("IsGWC")
#define FD_PRESSURE_FILE_NAME _T("Name")
#define FD_PRESSURE_GWC_PRESS _T("GWCPressure")
#define FD_PRESSURE_GWC_DEPTH _T("GWCDepth")
#define FD_PRESSURE_GWC_PRESS_R _T("ReservoirContactPressure")
#define FD_PRESSURE_GWC_DEPTH_R _T("ReservoirContactDepth")
#define FD_PRESSURE_GWC_GRAD_UP _T("ReservoirGradientAbove")
#define FD_PRESSURE_GWC_GRAD_DO _T("ReservoirGradientBelow")
#define FD_PRESSURE_PT_INDEX _T("PressureNr")

#define TB_PRESSURE_PT _T("PressurePoint")
#define FD_PRESSURE_PT_X _T("X")
#define FD_PRESSURE_PT_Y _T("Y")
#define FD_PRESSURE_PT_Z _T("Z")
#define FD_PRESSURE_INDEX _T("InPressure")

#define TB_MESH _T("Mesh")
#define FD_MESH_DELTA_XY _T("DeltaXY")
#define FD_MESH_MIN_X _T("XMin")
#define FD_MESH_MIN_Y _T("YMin")
#define FD_MESH_MAX_X _T("XMax")
#define FD_MESH_MAX_Y _T("YMax")

#define TB_MESH_ZONE _T("Zone")
#define FD_MESH_ZONE_ID _T("ZoneNr")
#define FD_MESH_ZONE_EXP_X _T("ExponentX")
#define FD_MESH_ZONE_EXP_Y _T("ExponentY")
#define FD_MESH_ZONE_INDEX _T("BinaryID")

#define TB_MESH_ZONE_NODE _T("ZoneNode")
#define FD_MESH_ZONE_NODE_ID _T("NodeNr")
#define FD_MESH_ZONE_NR _T("ZoneNr")
#define FD_MESH_ZONE_NODE_X _T("X")
#define FD_MESH_ZONE_NODE_Y _T("Y")

#define TB_XSECTION _T("XSection")
#define FD_XSECTION_INDEX _T("XSectionNr")
#define FD_XSECTION_NAME _T("Name")
#define FD_XSECTION_FIRST_X _T("XPoint1")
#define FD_XSECTION_FIRST_Y _T("YPoint1")
#define FD_XSECTION_SECOND_X _T("XPoint2")
#define FD_XSECTION_SECOND_Y _T("YPoint2")

#define TB_LINER _T("Liner")
#define FD_LINER_INDEX _T("LinerNr")
#define FD_LINER_NAME _T("Name")
#define FD_LINER_XPOINT _T("XPoint") // Reference point, x-ccor.
#define FD_LINER_YPOINT _T("YPoint") // Reference point, y-coor.
#define FD_LINER_ISVERTICAL _T("IsVertical")
#define FD_LINER_ELEVATION _T("Elevation") // e.g. Derrick elevation

#define TB_LINERPOINT _T("LinerPoint")
#define FD_INLINER_INDEX _T("InLiner")
#define FD_NORTHING _T("Northing")
#define FD_EASTING _T("Easting")
#define FD_TVD _T("Tvd")
#define FD_AHD _T("Ahd")
#define FD_AZIMUTH _T("Azimuth")
#define FD_INCLINATION _T("Inclination")
#define FD_POINTTYPE _T("PointType")

#endif // !defined(AFX_TABLEDEF_H__DC291152_F928_4BAD_ACE5_20104E85B879__INCLUDED_)
