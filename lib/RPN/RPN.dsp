# Microsoft Developer Studio Project File - Name="RPN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=RPN - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RPN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RPN.mak" CFG="RPN - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RPN - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "RPN - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "RPN - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(LBSP)/GuiFrame/include" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Tensor/include" /I "$(LBSP)/Tensor/include" /I "..\FemApp" /I "../OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "..\Quantity\include" /I "..\DianaCore" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../MfcGuiFrame/include" /I "$(LBOBJ)\include30" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "RPN - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../ModelFrame/include" /I "$(LBSP)/ModelFrame/include" /I "$(LBSP)/GuiFrame/include" /I "../Geometry/include" /I "$(LBSP)/Geometry/include" /I "../Tensor/include" /I "$(LBSP)/Tensor/include" /I "..\FemApp" /I "../OpenGLFrame/include" /I "$(LBSP)/OpenGLFrame/include" /I "..\Quantity\include" /I "..\DianaCore" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "../MfcGuiFrame/include" /I "$(LBOBJ)\include30" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "RPN - Win32 Release"
# Name "RPN - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\rpnbinaryoperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnConstant.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnconstantoperand.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\rpndlgbase.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnobject.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnOperand.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnstack.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnTernaryOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnToken.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnunaryoperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\shunt.cpp
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\rpnbinaryoperation.h
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnConstant.h
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnconstantoperand.h
# End Source File
# Begin Source File

SOURCE=.\SRC\rpndlgbase.h
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnobject.h
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnOperand.h
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnstack.h
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnTernaryOperation.h
# End Source File
# Begin Source File

SOURCE=.\SRC\Rpntoken.h
# End Source File
# Begin Source File

SOURCE=.\SRC\rpnunaryoperation.h
# End Source File
# Begin Source File

SOURCE=.\SRC\RpnValueSet.h
# End Source File
# Begin Source File

SOURCE=.\SRC\shunt.h
# End Source File
# Begin Source File

SOURCE=.\SRC\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Doxyfile
# End Source File
# Begin Source File

SOURCE=.\RPN_INC.H
# End Source File
# End Target
# End Project
