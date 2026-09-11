#ifndef _StatusMessage_h_
#define _StatusMessage_h_

class QString;

/*
 * This file defines the interface for a function to display a message in
 * the statusbar of the application.
 * The implementation is dependent on the platform and user interface used and
 * stored with the application code.
 */

void statusMessageW(const QString& message);

#endif  // _StatusMessage_h_
