/*****************************************************************
 * FILENAME:
 *   $Workfile: LicenseData.cpp $
 *
 * PURPOSE:
 *   Contains the dialog used to show calculation progress
 *
 * COPYRIGHT 1999 Shell Internationale Research Mij. B.V.
 *
 * CONFIDENTIALITY:
 *   This is a CONFIDENTIAL document.
 *   Neither the  whole  or any  part of this document may
 *   be  reproduced,  stored in  any retrieval  system  or
 *   transmitted in any form or by any means  (electronic,
 *   mechanical,  reprographic,  recording  or  otherwise)
 *   without the  prior  consent of the  copyright  owner.
 *
 * VERSION INFORMATION:
 *   $Revision: 30 $
 *   $Author: Esseniuse $
 *   $Date: 9/18/06 3:57p $
 *
 * PART OF MODULE:
 *   License
 *
 * MODULE CONTROLLER:
 *   Logica
 *****************************************************************/
#include "stdafx.h"
#include "MessageReporter.h"
#include <windows.h>

MessageReporter::MessageReporter(void)
{
}

MessageReporter::~MessageReporter(void)
{
}

int MessageReporter::ShowMessageBox(const char *message, const char *caption)
{
	return MessageBox(0, message, caption, MB_OK | MB_TOPMOST);
}

int MessageReporter::ShowRetryBox(const char *message, const char *caption)
{
	return MessageBox(0, message, caption, MB_RETRYCANCEL | MB_ICONEXCLAMATION | MB_TOPMOST) != IDCANCEL;
}
