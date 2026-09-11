# Microsoft Developer Studio Project File - Name="TSMesh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TSMesh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TSMesh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TSMesh.mak" CFG="TSMesh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TSMesh - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TSMesh - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "TSMesh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "./Gts" /I "./glib" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "$(DIAOBJ)/include30" /I "$(QTDIR)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "BUILDING_TSMESH" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "./Gts" /I "./glib" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "$(DIAOBJ)/include30" /I "$(QTDIR)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "BUILDING_TSMESH" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "TSMesh - Win32 Release"
# Name "TSMesh - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\MeshZone.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\src\TSNode.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\src\TSSurface.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\MeshZone.h
# End Source File
# Begin Source File

SOURCE=.\src\TSNode.h
# End Source File
# Begin Source File

SOURCE=.\src\TSProgressBase.h
# End Source File
# Begin Source File

SOURCE=.\src\TSSurface.h
# End Source File
# End Group
# Begin Group "Gts"

# PROP Default_Filter ""
# Begin Group "Gts Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Gts\bbtree.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\boolean.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\cdt.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\container.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\edge.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\eheap.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\face.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\fifo.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\graph.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\heap.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\hsurface.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\iso.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\kdtree.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\matrix.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\misc.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\named.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\object.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\oocs.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\partition.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\pgraph.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\point.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\predicates.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\predicates_init.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\psurface.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\refine.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\segment.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\split.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\stripe.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\surface.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\SurfaceDetail.c
# End Source File
# Begin Source File

SOURCE=.\Gts\triangle.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\vertex.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gts\vopt.c

!IF  "$(CFG)" == "TSMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "TSMesh - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# End Group
# Begin Group "Gts Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Gts\config.h
# End Source File
# Begin Source File

SOURCE=.\glib\glib.h
# End Source File
# Begin Source File

SOURCE=".\Gts\gts-private.h"
# End Source File
# Begin Source File

SOURCE=.\Gts\gts.h
# End Source File
# Begin Source File

SOURCE=.\Gts\predicates.h
# End Source File
# Begin Source File

SOURCE=.\Gts\predicates_init.h
# End Source File
# Begin Source File

SOURCE=.\Gts\rounding.h
# End Source File
# Begin Source File

SOURCE=.\Gts\SurfaceDetail.h
# End Source File
# End Group
# End Group
# Begin Group "gLib"

# PROP Default_Filter ""
# Begin Group "gLib Header files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\glib\acconfig.h
# End Source File
# Begin Source File

SOURCE=.\glib\galloca.h
# End Source File
# Begin Source File

SOURCE=.\glib\garray.h
# End Source File
# Begin Source File

SOURCE=.\glib\gasyncqueue.h
# End Source File
# Begin Source File

SOURCE=.\glib\gbacktrace.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gboxed.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gbsearcharray.h
# End Source File
# Begin Source File

SOURCE=.\glib\gcache.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gclosure.h
# End Source File
# Begin Source File

SOURCE=.\glib\gcompat.h
# End Source File
# Begin Source File

SOURCE=.\glib\gcompletion.h
# End Source File
# Begin Source File

SOURCE=.\glib\gconvert.h
# End Source File
# Begin Source File

SOURCE=.\glib\gdataset.h
# End Source File
# Begin Source File

SOURCE=.\glib\gdate.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\genums.h
# End Source File
# Begin Source File

SOURCE=.\glib\gerror.h
# End Source File
# Begin Source File

SOURCE=.\glib\gfileutils.h
# End Source File
# Begin Source File

SOURCE=.\glib\ghash.h
# End Source File
# Begin Source File

SOURCE=.\glib\ghook.h
# End Source File
# Begin Source File

SOURCE=.\glib\giochannel.h
# End Source File
# Begin Source File

SOURCE=".\glib\glib-object.h"
# End Source File
# Begin Source File

SOURCE=.\glib\glibconfig.h
# End Source File
# Begin Source File

SOURCE=.\glib\glist.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmacros.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmain.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmarkup.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gmarshal.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmem.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmessages.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmodule\gmodule.h
# End Source File
# Begin Source File

SOURCE=.\glib\gmodule\gmoduleconf.h
# End Source File
# Begin Source File

SOURCE=.\glib\gnode.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gobject.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gparam.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gparamspecs.h
# End Source File
# Begin Source File

SOURCE=.\glib\gprimes.h
# End Source File
# Begin Source File

SOURCE=.\glib\gqsort.h
# End Source File
# Begin Source File

SOURCE=.\glib\gquark.h
# End Source File
# Begin Source File

SOURCE=.\glib\gqueue.h
# End Source File
# Begin Source File

SOURCE=.\glib\grand.h
# End Source File
# Begin Source File

SOURCE=.\glib\grel.h
# End Source File
# Begin Source File

SOURCE=.\glib\gscanner.h
# End Source File
# Begin Source File

SOURCE=.\glib\gshell.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gsignal.h
# End Source File
# Begin Source File

SOURCE=.\glib\gslist.h
# End Source File
# Begin Source File

SOURCE=.\glib\gspawn.h
# End Source File
# Begin Source File

SOURCE=.\glib\gstrfuncs.h
# End Source File
# Begin Source File

SOURCE=.\glib\gstring.h
# End Source File
# Begin Source File

SOURCE=.\glib\gthread.h
# End Source File
# Begin Source File

SOURCE=.\glib\gthreadpool.h
# End Source File
# Begin Source File

SOURCE=.\glib\gtimer.h
# End Source File
# Begin Source File

SOURCE=.\glib\gtree.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gtype.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gtypemodule.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gtypeplugin.h
# End Source File
# Begin Source File

SOURCE=.\glib\gtypes.h
# End Source File
# Begin Source File

SOURCE=.\glib\gunibreak.h
# End Source File
# Begin Source File

SOURCE=.\glib\gunichartables.h
# End Source File
# Begin Source File

SOURCE=.\glib\gunicode.h
# End Source File
# Begin Source File

SOURCE=.\glib\gunidecomp.h
# End Source File
# Begin Source File

SOURCE=.\glib\gutils.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gvalue.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gvaluecollector.h
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gvaluetypes.h
# End Source File
# Begin Source File

SOURCE=.\glib\gwin32.h
# End Source File
# Begin Source File

SOURCE=".\glib\gobject\stamp-gmarshal.h"
# End Source File
# End Group
# Begin Group "gLib Lib"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=.\glib\gmodule\testgmodule.lib
# End Source File
# End Group
# Begin Group "gLib DLL"

# PROP Default_Filter "*.dll"
# Begin Source File

SOURCE=".\glib\glib-1.3.dll"
# End Source File
# Begin Source File

SOURCE=".\glib\gmodule\gmodule-1.3.dll"
# End Source File
# Begin Source File

SOURCE=".\glib\gobject\gobject-1.3.dll"
# End Source File
# Begin Source File

SOURCE=".\glib\gthread\gthread-1.3.dll"
# End Source File
# Begin Source File

SOURCE=.\glib\gmodule\libgplugin_a.dll
# End Source File
# Begin Source File

SOURCE=.\glib\gmodule\libgplugin_b.dll
# End Source File
# End Group
# Begin Group "DEF File"

# PROP Default_Filter "*.def"
# Begin Source File

SOURCE=.\glib\gmodule\gmodule.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\glib\gobject\gobject.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\glib\gthread\gthread.def
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\TSMesh.h
# End Source File
# End Target
# End Project
