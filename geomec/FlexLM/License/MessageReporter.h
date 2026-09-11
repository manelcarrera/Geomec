/*****************************************************************
 * FILENAME:
 *   $Workfile: LicenseData.h $
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
 *   $Revision: 15 $
 *   $Author: Esseniuse $
 *   $Date: 9/18/06 3:57p $
 *
 * PART OF MODULE:
 *    License
 *
 * MODULE CONTROLLER:
 *    Logica
 *****************************************************************/
#pragma once

class MessageReporter
{
public:
	MessageReporter(void);
	~MessageReporter(void);

	virtual int ShowMessageBox(const char *message, const char *caption);
	virtual int ShowRetryBox(const char *message, const char *caption);
};

