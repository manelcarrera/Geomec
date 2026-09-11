// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_INTERFACE_EXPORT_H
#define RG_INTERFACE_EXPORT_H

#ifdef WIN32
#ifdef RGINTERFACE_EXPORT
#define INTERFACE_RGINTERFACE __declspec( dllexport )
#else
#define INTERFACE_RGINTERFACE __declspec( dllimport )
#endif
#else
#define INTERFACE_RGINTERFACE
#endif
#endif

