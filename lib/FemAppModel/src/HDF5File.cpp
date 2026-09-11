#include "HDF5File.h"


#include <QFile>
#include <QStringList>
#include <cstring>
#include <cassert>

#include "H5Cpp.h"
#include "blosc_filter.h"

#include "AnalysisType.h"


static void register_blosc_once()
{
  static int registered = 0;
  if (!registered)
  {
  char *version, *date;
  register_blosc(&version, &date);

  registered = 1;
  }
}


CHDF5File::CHDF5Stream::CHDF5Stream(H5::H5File *file, const QString& name, size_t chunkSize)
  : m_file(file)
  , m_mode(0)
  , m_name(name)
  , m_chunkSize(chunkSize)
  , m_index(0)
  , m_buffer(0)
  , m_processed(0)
  , m_read(0)
  , m_dataBuffer(0)
  , m_dataSpace(0)
  , m_dataSet(0)
  , m_prop(0)
  , m_type(0)
  , m_flush(false)
{
}

CHDF5File::CHDF5Stream::~CHDF5Stream()
{
  close();
}

bool CHDF5File::CHDF5Stream::open(unsigned int mode)
{
  bool retval = false;

  register_blosc_once();

  m_index = 0;
  m_buffer = new unsigned char[m_chunkSize];
  memset(m_buffer, 0, m_chunkSize);
  m_processed = 0;

  m_mode = mode;

  H5::Group *group = 0;

  try
  {
  int     rnk = 1;
  hsize_t dim[1] = { m_chunkSize };
  hsize_t max[1] = { H5S_UNLIMITED };

  m_dataBuffer = new H5::DataSpace(rnk, dim, NULL);

  m_type = new H5::DataType(H5T_OPAQUE, 1);

  int compressionType = 0;

  if (mode == H5F_ACC_TRUNC)
  {
      QStringList l = m_name.split('/', QString::SkipEmptyParts);

      if (l.size() > 0)
      {
    if (l[0] == "Models")
          compressionType = 1;
    else if (l[0] == "Results")
          compressionType = 2;
      }

      QString grpPath;
      for (int g = 0; g < l.size() - 1; ++g)
      {
    grpPath += "/" + l[g];

    try
    {
          m_file->createGroup(grpPath.toStdString());
    }
    catch (...)
    {
          m_file->openGroup(grpPath.toStdString());
    }
      }

      m_dataSpace = new H5::DataSpace(rnk, dim, max);

      m_prop = new H5::DSetCreatPropList;
      m_prop->setChunk(rnk, dim);

      unsigned int cd_values[7]; // 0-3 reserved, 4 compression level, 5 shuffle active flag (0/1), 6 compression library
      switch (compressionType)
      {
      case 1:
    {
          cd_values[4] = 3; // seems to be a sweet spot
          cd_values[5] = 0;
          cd_values[6] = BLOSC_ZLIB;

          m_prop->setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);
    }
    break;
      case 2:
    {
          cd_values[4] = 9;
          cd_values[5] = 0;
          cd_values[6] = BLOSC_BLOSCLZ;

          m_prop->setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);
    }
    break;
      }
  
      m_dataSet = new H5::DataSet(m_file->createDataSet(m_name.toStdString(), *m_type, *m_dataSpace, *m_prop));
  }
  else
  {
      QStringList l = m_name.split('/', QString::SkipEmptyParts);
      for (int g = 0; g < l.size() - 1; ++g)
      {
    try
    {
          if (group)
          {
      H5::Group *newGroup = new H5::Group(group->openGroup(l[g].toStdString()));
      delete group;
      group = newGroup;
          }
          else
          {
      group = new H5::Group(m_file->openGroup(l[g].toStdString()));
          }
    }
    catch (...)
    {
    }
      }
      if (group)
      {
    m_dataSet = new H5::DataSet(group->openDataSet(l.back().toStdString()));
    delete group;
    group = 0;
      }
      else
    m_dataSet = new H5::DataSet(m_file->openDataSet(l.back().toStdString()));
  }

  m_mode = mode;

  retval = true;

  }
  catch (...)
  {
  delete group;
  }

  return retval;
}

void CHDF5File::CHDF5Stream::close()
{
  if (m_buffer)
  {
  if (m_mode == H5F_ACC_TRUNC && m_index != 0)
      writeBuffer(true);

  m_index = 0;
  delete[] m_buffer;
  m_buffer = 0;

  try { delete m_type; } catch (...) {}
  try { if (m_prop) m_prop->close(); } catch (...) {}
  try { delete m_prop; } catch (...) {}
  try { delete m_dataSpace; } catch (...) {}
  try { delete m_dataSet; } catch (...) {}
  try { delete m_dataBuffer; } catch (...) {}
  }
}

qint64 CHDF5File::CHDF5Stream::pos() const
{
  qint64 position = m_processed + m_index;

  if (m_mode == H5F_ACC_RDONLY)
  position -= m_chunkSize;

  return position;
}

bool CHDF5File::CHDF5Stream::seek(qint64 pos)
{
  if (m_mode == H5F_ACC_RDONLY)
  {
  m_index     = pos % m_chunkSize;
  m_processed = pos - m_index;
  return readBuffer(true);
  }
  else
  {
  if (m_index != 0)
      writeBuffer(true);

  m_index     = pos % m_chunkSize;
  m_processed = pos - m_index;
  readBuffer(true);
  m_processed -= m_chunkSize;

  return true;
  }
}

qint64 CHDF5File::CHDF5Stream::bytesRead() const
{
  return m_read;
}

void CHDF5File::CHDF5Stream::bytesRead(qint64 addBytes)
{
  m_read += addBytes;
}


qint64 CHDF5File::CHDF5Stream::readData(char *data, qint64 maxSize)
{
  for (qint64 i = 0; i < maxSize; ++i)
  {
  if (!readBuffer())
      return i;

  *(data + i) = m_buffer[m_index++];
  }

  return maxSize;
}


qint64 CHDF5File::CHDF5Stream::writeData(const char *data, qint64 maxSize)
{
  for (qint64 i = 0; i < maxSize; ++i)
  {
  m_buffer[m_index++] = *(data + i);

  if (!writeBuffer())
      return i;
  }

  return maxSize;
}


bool CHDF5File::CHDF5Stream::readBuffer(bool bForce)
{
  if (m_index == m_chunkSize)
  m_index = 0;
  if (bForce || m_index == 0)
  {
  try
  {
      H5::DataSpace *fileSpace = new H5::DataSpace(m_dataSet->getSpace());
      hsize_t offset[1] = { m_processed };
      hsize_t chunk[1] = { m_chunkSize };
      fileSpace->selectHyperslab(H5S_SELECT_SET, chunk, offset);

      m_dataSet->read(m_buffer, *m_type, *m_dataBuffer, *fileSpace);

      m_processed += m_chunkSize;

      delete fileSpace;
  }
  catch (...)
  {
      return false;
  }
  }

  return true;
}

bool CHDF5File::CHDF5Stream::writeBuffer(bool bForce)
{
  if (bForce || m_index == m_chunkSize)
  {
  try
  {
      hsize_t dim[2];
      m_dataSet->getSpace().getSimpleExtentDims(dim);

      if (m_processed + m_chunkSize > dim[0])
      {
    hsize_t new_dim[1] = { m_processed + m_chunkSize };
    m_dataSet->extend(new_dim);
      }

      H5::DataSpace *fileSpace = new H5::DataSpace(m_dataSet->getSpace());
      hsize_t offset[1] = { m_processed };
      hsize_t chunk[1] = { m_chunkSize };
      fileSpace->selectHyperslab(H5S_SELECT_SET, chunk, offset);

      m_dataSet->write(m_buffer, *m_type, *m_dataBuffer, *fileSpace);

      m_processed += m_chunkSize;

      delete fileSpace;

      m_index = 0;
      memset(m_buffer, 0, m_chunkSize);
  }
  catch (...)
  {
      return false;
  }
  }

  return true;
}




CHDF5File::CHDF5File(QIODevice *file, QObject *parent)
  : QIODevice(parent)
  , m_file(file)
  , m_h5file(0)
  , m_mode(0)
  , m_stream(0)
{
  try
  {
  H5::Exception::dontPrint();
  }
  catch (...)
  {
  }
}

CHDF5File::~CHDF5File()
{
  close();
}


bool CHDF5File::IsHDF5File(QIODevice *device)
{
  QFile *file = dynamic_cast<QFile *>(device);

  return file && file->exists() && H5::H5File::isHdf5(file->fileName().toStdString());
}

QString CHDF5File::FileName() const
{
  return static_cast<QFile *>(m_file)->fileName();
}

// For now we only support either read-only or write-only
bool CHDF5File::CanRead() const
{
  return m_mode == H5F_ACC_RDONLY;
}

bool CHDF5File::CanWrite() const
{
  return m_mode == H5F_ACC_TRUNC;
}

QString CHDF5File::ModelGroup(int modelIndex)
{
  return QString("/Models/%1/").arg(modelIndex);
}

QString CHDF5File::ResultGroup(int modelIndex)
{
  return QString("/Results/%1/").arg(modelIndex);
}

QString CHDF5File::ResultGroupStream(int modelIndex, int type, int stage, int column)
{
  QChar cType = '?';
  switch (type)
  {
  case 0:
  cType = 'N';
  break;
  case 1:
  cType = 'L';
  break;
  case 2:
  cType = 'H';
  break;
  case 3:
  cType = 'M';
  break;
  case 4:
  cType = 'C';
  break;
  }
  assert(cType != '?');

  return ResultGroup(modelIndex) + QString("%1/D%2/%3").arg(cType).arg(stage).arg(column);
}


bool CHDF5File::open(OpenMode mode)
{
  QFile *file = static_cast<QFile *>(m_file);

  try
  {
  m_mode = mode == QIODevice::ReadOnly ? H5F_ACC_RDONLY : H5F_ACC_TRUNC;

  m_h5file = new H5::H5File(file->fileName().toStdString(), m_mode);

  setOpenMode(mode);

  if (CanRead())
  {
      /* We're currently not checking for version, but we might in the future
      char version[64];
      if (PushDataSet("/META/Version"))
      {
    m_stream->readData(version, 64);
      }
      PopDataSet();
      */
  }
  else if (CanWrite())
  {
      if (PushDataSet("/META/Version", TINY))
      {
    QString version = "5.0.0";
    m_stream->writeData(version.toStdString().c_str(), version.length());
      }
      PopDataSet();
  }

  return true;
  }
  catch (...)
  {
  m_h5file = 0;
  return false;
  }
}

void CHDF5File::close()
{
  closeStream();
  try
  {
  if (m_h5file)
      m_h5file->close();
  delete m_h5file;
  m_h5file = 0;

  if (m_file)
      m_file->close();
  delete m_file;
  m_file = 0;
  }
  catch (...)
  {
  }
  setOpenMode(NotOpen);
}

bool CHDF5File::openStream(const QString& name, size_t chunkSize)
{
  assert(m_h5file);
  assert(!m_stream);

  m_stream = new CHDF5Stream(m_h5file, name, chunkSize);

  if (!m_stream->open(m_mode))
  return false;

  QIODevice::seek(0);

  return true;
}

void CHDF5File::closeStream()
{
  delete m_stream;
  m_stream = 0;
}

qint64 CHDF5File::pos() const
{
  if (!m_stream)
  return -1;

  return m_stream->pos();
}

bool CHDF5File::seek(qint64 pos)
{
  if (!m_stream)
  return false;

  return m_stream->seek(pos);
}

bool CHDF5File::PushDataSet(const QString& dataSet, int hint)
{
  if (m_stream)
  {
  if (CanRead())
      m_stream->bytesRead(QIODevice::pos());

  m_stack.push(m_stream);
  m_stream = 0;
  }

  bool retval = false;

  switch (hint)
  {
  case TINY:
  retval = openStream(dataSet, 32);
  break;
  case SMALL:
  retval = openStream(dataSet, 2048);
  break;
  default:
  retval = openStream(dataSet);
  }

  return retval;
}

bool CHDF5File::PopDataSet()
{
  if (m_stream)
  closeStream();

  if (m_stack.empty())
  return false;

  m_stream = m_stack.top();

  QIODevice::seek(0);

  if (CanRead())
  m_stream->seek(m_stream->bytesRead());

  m_stack.pop();


  return true;
}


bool CHDF5File::DataSetExists(const QString& name) const
{
  H5::Group *group = 0;
  H5::DataSet *dataSet = 0;

  bool isGroup = name.endsWith("/");

  QStringList l = name.split('/', QString::SkipEmptyParts);

  int size = isGroup ? l.size() : l.size() - 1;

  for (int g = 0; g < size; ++g)
  {
  try
  {
      if (group)
      {
    H5::Group *newGroup = new H5::Group(group->openGroup(l[g].toStdString()));
    delete group;
    group = newGroup;
      }
      else
      {
    group = new H5::Group(m_h5file->openGroup(l[g].toStdString()));
      }
  }
  catch (...)
  {
  }
  }
  if (!isGroup)
  {
  try
  {
      if (group)
      {
    dataSet = new H5::DataSet(group->openDataSet(l.back().toStdString()));
    delete group;
    group = 0;
      }
      else
    dataSet = new H5::DataSet(m_h5file->openDataSet(l.back().toStdString()));
  }
  catch (...)
  {
      delete group;
  }

  if (dataSet)
  {
      delete dataSet;
      return true;
  }
  }
  else
  {
  if (group)
  {
      delete group;
      return true;
  }
  }

  return false;
}


qint64 CHDF5File::readData(char *data, qint64 maxSize)
{
  if (!m_stream || !CanRead())
  return -1;

  return m_stream->readData(data, maxSize);
}

qint64 CHDF5File::writeData(const char *data, qint64 maxSize)
{
  if (!m_stream || !CanWrite())
  return -1;

  return m_stream->writeData(data, maxSize);
}


