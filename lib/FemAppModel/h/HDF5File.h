#ifndef _HDF5_FILE_
#define _HDF5_FILE_

/*

HDF5 (.gm5) is a container format for .gm4 data.
Currently the data is spread out over different groups/datasets:

  /META/Version
  = 5.0.0, no checks are done for this yet
  ModelGroup(CModelBase::Index)/ModelStream
  = most of the old .gm4 data
  ModelGroup(CModelBase::Index)/Scenes
  = the scene info written by the GUI
  ResultGroupStream(CModelBase::Index, analysisType, depletionStage, resultType)
  = cells as in the MeshDataCacher

During loading (ModelIO.cpp/LoadModel) the results are skipped. The file remains open,
read-only, so that results can be read on demand.

When saving (ModelIO.cpp/SaveModel) special care is taken to close the file and re-open
the saved one read-only.

Access to the database is handled by the (singleton-ish) ConsistencyGuard.


In Load/SaveStream functions, use constructs like:

  // preferred if we just save the data in a different dataset
  if (stream.PushDataSet(ModelGroup(model.Index()) + "dataset")
  {
  }
  stream.PopDataSet();

or

  // this one
  if (stream.IsHDF5())
  {
  }

or

  // and this one allow us to skip loading altogether; usually we need to set up some proxy so that Geomec still thinks
  // that the data is loaded; and of course the proxy must load on demand.
  if (stream.DataSetExists(ModelGroup(model.Index()) + "/group/" [ + "dataset" ])
  {
  }

Take care that .gm5 remains backward compatible with .gm4: a .gm5 file can be "save as"'d
as a .gm4 file, without loss of information. Changes in Load/SaveStream that require an
update of the CStreamVersion used should continue to update the 4.1.X revisions.


Compression of the stream is de-activated in .gm5 and replaced by a HDF5 compression filter.
The filter used is based on the group: Models = zlib, Results = LZ4.


*/


#include <QIODevice>
#include <stack>

namespace H5
{
class H5File;
class DataSpace;
class DataSet;
class DSetCreatPropList;
class DataType;
}


class CHDF5File : public QIODevice
{
  Q_OBJECT
public:
  CHDF5File(QIODevice *file, QObject *parent = 0);
  ~CHDF5File();

  static bool IsHDF5File(QIODevice *device);

  QString FileName() const;

  bool CanRead() const;
  bool CanWrite() const;

  static QString ModelGroup(int modelIndex);
  static QString ResultGroup(int modelIndex);
  static QString ResultGroupStream(int modelIndex, int type, int stage, int result);

  virtual bool open(OpenMode mode);
  virtual void close();

  bool openStream(const QString& name, size_t chunkSize = 0x10000);
  void closeStream();

  // we need to override pos, because the base gives us the position in the file, but we want the position in the stream, which could have blocks all over the place
  // WARNING: comments in Qt suggest that this interface will change in Qt5
  virtual qint64 pos() const;
  virtual bool seek(qint64 pos);

  typedef enum { DEFAULT = 0, TINY, SMALL } THint;

  bool PushDataSet(const QString& name, int hint = DEFAULT);
  bool PopDataSet();

  bool DataSetExists(const QString& name) const;

protected:
  virtual qint64 readData(char *data, qint64 maxSize);
  virtual qint64 writeData(const char *data, qint64 maxSize);

private:

  class CHDF5Stream
  {
  public:
  CHDF5Stream(H5::H5File *file, const QString& name, size_t chunkSize);
  ~CHDF5Stream();

  bool open(unsigned int mode);
  void close();

  qint64 pos() const;
  bool seek(qint64 pos);

  qint64 bytesRead() const;
  void bytesRead(qint64 addBytes);

  qint64 readData(char *data, qint64 maxSize);
  qint64 writeData(const char *data, qint64 maxSize);

  private:
  H5::H5File    *m_file;
  unsigned int   m_mode;
  const QString  m_name;
  const size_t   m_chunkSize;

  size_t         m_index;
  unsigned char *m_buffer;
  size_t         m_processed;

  qint64         m_read;

  H5::DataSpace *m_dataBuffer;
  H5::DataSpace *m_dataSpace;
  H5::DataSet   *m_dataSet;

  H5::DSetCreatPropList *m_prop;
  H5::DataType  *m_type;

  bool m_flush;

  bool readBuffer(bool bForce = false);
  bool writeBuffer(bool bForce = false);
  };

  QIODevice  *m_file;
  H5::H5File *m_h5file;

  unsigned int m_mode;


  typedef std::stack<CHDF5Stream *> TStreamStack;
  CHDF5Stream *m_stream;
  TStreamStack m_stack;

  Q_DISABLE_COPY(CHDF5File)
};


#endif