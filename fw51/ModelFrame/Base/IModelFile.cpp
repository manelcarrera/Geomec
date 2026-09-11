/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IModelFile.h"
#include "lbcx.h"
#include <QTextStream>

/*!
  \class IModelFile
  \brief baseclass for read/write functionality.
*/

/*!
  Constructs a reader/writer without an object hierarchy yet.
  The root object is upon start of readModel() or writeModel().
*/
IModelFile::IModelFile() : m_succes(true), m_root(0), m_cancel(false), m_nFatal(0), m_maxFatal(10) {}

/*!
  \fn virtual bool IModelFile::canRead( const QString& fileName ) const =0

  Override to guess if the file has an acceptable format. Criteria could be
  - file extension,
  - some magic data at the start,
  - keywords encountered, and/or
  - file version.
*/

/*!
  \fn QString IModelFile::formatExtension() const = 0

  Override to return the default extension of files in this format.
*/

/*!
  \fn QString IModelFile::formatDescription() const = 0

  Override to return a description for the user of files in this format.
*/

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4290) // see comment in Geometry's ConvexHull.h
#endif
extern "C" void ModelFileAssertFunction(const char *expr, const char *file,
                                        unsigned int line) throw(DianaAssertMessage) {
  throw(DianaAssertMessage(expr, file, line));
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void IModelFile::assertHandler(const DianaAssertMessage &msg) {
  QString messageText = tr("Diana assertion '%1' failed").arg(msg.assertion);
  if (!msg.file.isEmpty())
    messageText += tr(", file %1").arg(msg.file);
  if (msg.line > 0)
    messageText += tr(", line %1").arg(msg.line);
  messageText += ".\n";
  messageText += get_diadate(NULL);
  messageText += "\n";
  messageText += get_os_info();
  messageText += "\n";
  TFileError message(messageText, 0);
  emit abortMessage(message);
  m_succes = false;
}

/*!
  Reads a file changing the object hierarchy root.
  Calls the specialized function read( fileName ).
  The signal done() is emitted at the end.
*/
bool IModelFile::readModel(IModelObject &root, const QString &fileName) {
  m_root = &root;
  m_nFatal = 0;
  m_cancel = false;
  DianaAssertFunction_t daf = SetDianaAssertFunction(ModelFileAssertFunction);
  bool result = false;
  try {
    init();
    result = read(fileName);
    clear();
  } catch (DianaAssertMessage msg) {
    assertHandler(msg);
  }
  SetDianaAssertFunction(daf);
  emit done();

  return result;
}

/*!
  Reads a device changing the object hierarchy root.
  Calls the specialized function read( file ).
  The signal done() is emitted at the end.
*/
bool IModelFile::readModel(IModelObject &root, FILE *file) {
  m_root = &root;
  DianaAssertFunction_t daf = SetDianaAssertFunction(ModelFileAssertFunction);
  bool result = false;
  try {
    init();
    result = read(file);
    clear();
  } catch (DianaAssertMessage msg) {
    assertHandler(msg);
  }
  SetDianaAssertFunction(daf);
  emit done();

  return result;
}

/*!
  Writes the object hierarchy root to a file named fileName.
  Calls the specialized function write( fileName ).
  The signal done() is emitted at the end.
*/
bool IModelFile::writeModel(IModelObject &root, const QString &fileName) {
  m_root = &root;
  DianaAssertFunction_t daf = SetDianaAssertFunction(ModelFileAssertFunction);
  bool result = false;
  try {
    init();
    result = write(fileName);
    clear();
  } catch (DianaAssertMessage msg) {
    assertHandler(msg);
  }
  SetDianaAssertFunction(daf);
  emit done();
  return result;
}

/*!
  Writes the object hierarchy root to a device.
  Calls the specialized function write( file ).
  The signal done() is emitted at the end.
*/
bool IModelFile::writeModel(IModelObject &root, FILE *file) {
  m_root = &root;
  DianaAssertFunction_t daf = SetDianaAssertFunction(ModelFileAssertFunction);
  bool result = false;
  try {
    init();
    result = write(file);
    clear();
  } catch (DianaAssertMessage msg) {
    assertHandler(msg);
  }
  SetDianaAssertFunction(daf);
  emit done();
  return result;
}

/*!
  Writes the object hierarchy root to a TextStream
  Calls the specialized function write( file ).
  The signal done() is emitted at the end.
*/
bool IModelFile::writeModel(IModelObject &root, QTextStream &stream) {
  m_root = &root;
  DianaAssertFunction_t daf = SetDianaAssertFunction(ModelFileAssertFunction);
  bool result = false;
  try {
    init();
    result = write(stream);
    clear();
  } catch (DianaAssertMessage msg) {
    assertHandler(msg);
  }
  SetDianaAssertFunction(daf);
  emit done();
  return result;
}

/*!
  Write not supported in all derived classes,
  so generate an error message if the baseclass function is used
 */
bool IModelFile::write(const QString & /*filename*/) {
  putAbortMessage(TFileError(tr("write to textstream not implemented"), 11));
  return false;
}

/*!
  Write not supported in all derived classes,
  so generate an error message if the baseclass function is used
 */
bool IModelFile::write(FILE * /*file*/) {
  putAbortMessage(TFileError(tr("write to textstream not implemented"), 12));
  return false;
}

/*!
  Write not supported in all derived classes,
  so generate an error message if the baseclass function is used
 */
bool IModelFile::write(QTextStream &) {
  putAbortMessage(TFileError(tr("write to textstream not implemented"), 13));
  return false;
}

/*!
  Returns the object hierarchy to write or to modify by reading.
  Must have been set by construction or setRootObject!
*/
IModelObject &IModelFile::rootObject() {
  assert(m_root);
  return *m_root;
}

/*!
  Returns the object hierarchy to write or to modify by reading.
  Must have been set by construction or setRootObject!
*/
const IModelObject &IModelFile::rootObject() const {
  assert(m_root);
  return *m_root;
}

/*!
  Connect to this slot to cancel an ongoing read/write process.
*/
void IModelFile::onCancel() { m_cancel = true; }

/*!
  \fn void IModelFile::init() = 0

  Override to initialize the reader/writer with the model that has been set
  at the start of reading or writing.

  Automatically called before each readModel() or writeModel().
*/

/*!
  \fn void IModelFile::clear() = 0

  Override to clear the state of the reader/writer and to undo any
  initialization doen at init().

  Automatically called after each readModel() or writeModel().
*/

/*!
  \fn bool IModelFile::read( const QString& fileName ) = 0

  Override to implement the reading from a file.
*/

/*!
  \fn bool IModelFile::read( FILE* file ) = 0

  Override to implement the reading from a device.
*/

/*!
  \fn bool IModelFile::write( const QString& fileName ) = 0

  Override to implement the writing to a file.
*/

/*!
  \fn bool IModelFile::write( FILE* file ) = 0

  Override to implement the writing to a device.
*/

/*!
  Emits a status message.
*/
void IModelFile::putStatusMessage(const TFileMessage &message) {
  emit statusMessage(message);
  checkCancel();
}

/*!
  Emits a warning message.
*/
void IModelFile::putWarningMessage(const TFileError &message) {
  emit warningMessage(message);
  checkCancel();
}

/*!
  Emits a fatal message.
  A TFatalException is thrown. It must be handled somewhere in the
  implementation of read() and write().
*/
void IModelFile::putFatalMessage(const TFileError &message) {
  m_nFatal++;
  emit fatalMessage(message);
  checkCancel();
  if (m_nFatal == m_maxFatal) { /* set <= 0 to show all */
    TFileError err(tr("%1 fatal errors seen").arg(m_nFatal), 1000);
    putAbortMessage(err);
  }
  m_succes = false;
  throw TFatalException(message);
}

/*!
  Emits an abort message.
  A TAbortException is thrown. It must be handled somewhere in the
  implementation of read() and write().
*/
void IModelFile::putAbortMessage(const TFileError &message) {
  emit abortMessage(message);
  m_succes = false;
  throw TAbortException(message);
}

/*!
  Emits progress information.
  \param step  The number of steps taken since the operation started.
  \param total The total number of steps needed to finish the operation.
               Passing a zero means the number of steps is unknown in advance;
         this facilitates a busy indicator.

  Call this member repeatedly to keep a client user interface 'alive' and
  facilitate the use of a progress bar.
*/
void IModelFile::putProgress(int step, int total) {
  emit progress(step, total);
  checkCancel();
}

/*!
  Checks if the operation has been cancelled. If so, an abort message is
  emitted, which throws a TAbortException, which aborts the ongoing operation.

  This member is called automatically when emitting messages and progress
  information.
*/
void IModelFile::checkCancel() {
  if (m_cancel) {
    m_cancel = false;
    TFileError cancelMsg(tr("Operation cancelled"), TFilePosition());
    putAbortMessage(cancelMsg);
  }
}

/*!
  Set the maximum number of fatal error messages.
  Immediately abort after the specified number of fatal messages
  A number lower or equal to zero, can be used to avoid abortion
 */
int IModelFile::setMaxFatal(int nFatal) {
  int rv = m_maxFatal;
  m_maxFatal = nFatal;
  return rv;
}

/*!
  \fn void IModelFile::statusMessage( const TFileMessage& message )

  Connect to this signal to pass status messages to the user.
*/

/*!
  \fn void IModelFile::warningMessage( const TFileError&   message )

  Connect to this signal to pass warning messages to the user.
*/

/*!
  \fn void IModelFile::fatalMessage( const TFileError&   message )

  Connect to this signal to pass fatal error messages to the user.
*/

/*!
  \fn void IModelFile::abortMessage( const TFileError&   message )

  Connect to this signal to pass abort messages to the user.
*/

/*!
  \fn void IModelFile::progress( int step, int total )
  \param step  The number of steps taken since the operation started.
  \param total The total number of steps needed to finish the operation.
               A zero means the number of steps is unknown; use a busy
         indicator only.

  Connect to this signal to provide progress information to the user.
*/

/*!
  \fn void IModelFile::done()

  Connect to this signal to stop observing the read/write process.
*/
