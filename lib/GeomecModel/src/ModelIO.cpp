#include <memory>

#include "AnalysisLogger.h"
#include "ArchiveQDataStream.h"
#include "BaseEntryTypes.h"
#include "ConsistencyGuard.h"
#include "EclipseModel.h"
#include "Global.h"
#include "GlobalMessage.h"
#include "GoCadModel.h"
#include "HDF5File.h"
#include "HexaModel.h"
#include "IProgressFactory.h"
#include "KeyFile.h"
#include "Settings.h"
#include "VersionManager.h"
#include "VersionNumbers.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"

#include "ModelLifetimeFacade.h"

#include <QDir>
#include <QFile>
#include <QObject>
#include <QScopedPointer>

namespace {
const QString COMPANY_NAME = "TNO";

const QString MODEL_DOES_NOT_EXIST = "model '%1' does not exist";
const QString MODEL_CANNOT_BE_OPENED = "model '%1' cannot be opened";

} // namespace

namespace privateModelIO {

CModelBase *LoadModel(const CModelBase::IModelLoadSave &modelLoadSave, const QString &strFileName,
                      CAnalysisLogger &logger, bool bMain, bool bSilent) {
  QFile *qFile = 0;
  QIODevice *pFile = 0;
  CHDF5File *h5File = 0;
  bool bIsHDF5 = false;
  bool bIsGMP = strFileName.endsWith(".gmp", Qt::CaseInsensitive);
  CModelBase *pModel = 0;
  CArchiveInterface *archive = 0;

  if (bIsGMP) {
    if (modelLoadSave.SupportGMP()) {
      archive = modelLoadSave.createCArchive(strFileName);
    }
  } else {
    qFile = new QFile(strFileName); // if we have an HDF5 file, then qFile will be owned by it
    bIsHDF5 = CHDF5File::IsHDF5File(qFile);

    if (bIsHDF5) {
      h5File = new CHDF5File(qFile);

      if (h5File && h5File->open(QIODevice::ReadOnly) && h5File->openStream(CHDF5File::ModelGroup(0) + "ModelStream"))
        pFile = h5File;
    } else {
      if (qFile->open(QIODevice::ReadOnly))
        pFile = qFile;
    }

    if (pFile)
      archive = new CArchiveQDataStream(pFile);
  }

  if (!archive || !archive->IsLoading()) {
    QString message =
        QString((!archive || !archive->Exists()) ? MODEL_DOES_NOT_EXIST : MODEL_CANNOT_BE_OPENED).arg(strFileName);
    logger.AddLine(message.toStdString(), true);

    delete h5File;
    delete archive;

    return 0;
  }

  try {
    QString Magic;

    *archive >> Magic;

    if (Magic == MAGIC) {
      // Read file header

      QString m_app_version;
      CVersionManager m_versionManager(m_app_version);
      int nMajor, nMinor, nRevision;

      *archive >> nMajor >> nMinor >> nRevision;
      m_versionManager.FileVersion(CStreamVersion(nMajor, nMinor, nRevision));

      QString applicationName = QString("GEOMEC %1.%2").arg(nMajor).arg(nMinor);

      // Check version ...

      if (m_versionManager.CheckVersion(m_versionManager.FileVersion())) {
        CKeyFile keyFile;
        int nModelType;

        *archive >> nModelType;

        switch (nModelType) {
        case HEXA_MODEL:
          pModel = bMain ? IModelLifetimeFacade::NewMain(HEXA_MODEL, logger, m_versionManager)
                         : IModelLifetimeFacade::NewChild(HEXA_MODEL, logger, m_versionManager);
          break;
        case GOCAD_MODEL:
#ifdef TETRA_KEYFILE
          if (keyFile.Unlocked()) {
#endif
            pModel = bMain ? IModelLifetimeFacade::NewMain(GOCAD_MODEL, logger, m_versionManager)
                           : IModelLifetimeFacade::NewChild(GOCAD_MODEL, logger, m_versionManager);
#ifdef TETRA_KEYFILE
          } else {
            _m()->msg("Tetrahedron models are locked in this version");
          }
#endif
          break;
        case ECLIPSE_MODEL:
#ifdef ECLIPSE_KEYFILE
          if (keyFile.Unlocked()) {
#endif
            pModel = bMain ? IModelLifetimeFacade::NewMain(ECLIPSE_MODEL, logger, m_versionManager)
                           : IModelLifetimeFacade::NewChild(ECLIPSE_MODEL, logger, m_versionManager);
#ifdef ECLIPSE_KEYFILE
          } else {
            _m()->msg("Eclipse models are locked in this version");
          }
#endif
          break;
        case G2D_MODEL:
          _m()->msg("2D models are no longer supported in this version");
          break;
        default:
          assert(false);
        }
      }

      if (pModel) {

        if (bIsHDF5)
          pModel->GetConsistencyGuard()->AttachHDF5File(archive->GetHDF5File());

        if (m_versionManager.FileVersion() > CStreamVersion(3, 0, 84)) {
          *archive >> m_app_version;
        } else {
          m_app_version = "unknown (3.4.1.1 or older)";
        }

        CModelBase::IModelLoadSave *pModelLoadSave = const_cast<CModelBase::IModelLoadSave *>(&modelLoadSave);

        pModelLoadSave->SetAppVersion(m_app_version);
        pModel->SetAppVersion(m_app_version);

        std::auto_ptr<IProgressBase> prog;

        if (bSilent)
          prog = std::auto_ptr<IProgressBase>(_g->prog()->create(eProgress::Silent));
        else
          prog = std::auto_ptr<IProgressBase>(_g->prog()->create(eProgress::Geo, "load"));

        pModelLoadSave->SetVersionManager(m_versionManager);
        if (!pModelLoadSave->LoadModel(*archive, m_versionManager.FileVersion(), *prog, *pModel))
          throw new CProgressCancel;
      }
    } else {
      _m()->error("File is not a valid GEOMEC model");
    }
  }

  catch (CProgressCancel *p) {
    delete p;

    if (pModel->isOpen()) {
      IModelLifetimeFacade::Close(pModel);
      pModel = 0;
    }
  }

  delete archive;

  if (!bIsHDF5)
    delete qFile;

  return pModel;
}

bool SaveModel(const CModelBase::IModelLoadSave &modelLoadSave, CModelBase &model, const QString &strFileName,
               bool bSwitch) {
  class CBackup {
  public:
    class CBackupError {
      QString m_msg;

    public:
      CBackupError(QString msg) : m_msg(msg) {}

      QString Message() { return m_msg; }
    };

  public:
    CBackup(const QString &strFileName, bool bGuard = true) {
      if (bGuard && QFile::exists(strFileName)) {
        QFile file(strFileName);

        QString strBackup = strFileName + ".bak";
        QString errorMsg = QObject::tr("Unable to create a backup copy of '%1'").arg(strFileName);

        if (QFile::exists(strBackup)) {
          if (!QFile::copy(strFileName, strBackup))
            throw CBackupError(errorMsg);
        } else {
          if (!QFile::rename(strFileName, strBackup))
            throw CBackupError(errorMsg);
        }

        m_strFileName = strFileName;
      }
    }

    ~CBackup() {
      if (!m_strFileName.isEmpty()) {
        // restore backup
        if (QFile::exists(m_strFileName))
          QFile::remove(m_strFileName);
        QFile::rename(m_strFileName + ".bak", m_strFileName);
      }
    }

    void Release() {
      if (!m_strFileName.isEmpty())
        QFile::remove(m_strFileName + ".bak");
      m_strFileName.clear();
    }

  private:
    QString m_strFileName;
  };

  bool retval = true;

  CConsistencyGuard *guard = model.GetConsistencyGuard();
  assert(guard);

  QString databaseHDF5File = guard->HDF5FileName();

  bool bNormalSave = QDir::toNativeSeparators(databaseHDF5File) == QDir::toNativeSeparators(strFileName);

  if (bNormalSave)
    guard->AttachHDF5File(
        0); // if we have an HDF5 file open as "database", this MAY close it, so that we can create a backup

  QFile *qFile = 0; // if we have an HDF5 file, then qFile will be owned by it
  CHDF5File *h5File = 0;

  bool bIsHDF5 = strFileName.right(4).toLower() == ".gm5";

  try {
    CBackup backup(strFileName);

    // If we are saving an HDF5 file, and we got to this point, we need to open the backup to gain access to the
    // database
    if (bNormalSave) {
      qFile = new QFile(databaseHDF5File + ".bak");
      h5File = new CHDF5File(qFile);

      if (h5File && h5File->open(QIODevice::ReadOnly) && h5File->openStream(CHDF5File::ModelGroup(0) + "ModelStream"))
        model.GetConsistencyGuard()->AttachHDF5File(h5File);
      else {
        delete h5File;
        throw CBackup::CBackupError(QString("Unable to use '%1.bak' as database, use 'Save As'").arg(databaseHDF5File));
      }
    }

    IProgressBase *pProg = _g->prog()->create(eProgress::Geo, "Saving ...");

    qFile = new QFile(strFileName);
    QIODevice *pFile = 0;

    bool bRet = false;

    if (bIsHDF5) {
      h5File = new CHDF5File(qFile);

      if (h5File && h5File->open(QIODevice::WriteOnly))
        pFile = h5File;
    } else {
      if (!qFile->open(QIODevice::WriteOnly)) {
        _m()->error(QObject::tr("Unable to open '%1' for writing").arg(strFileName));
        return false;
      }
      pFile = qFile;
    }

    if (pFile) {
      CArchiveQDataStream stream(pFile);

      model.NumberModels();

      bRet = const_cast<CModelBase::IModelLoadSave &>(modelLoadSave).SaveModel(stream, *pProg, model);

      stream.Close();
    }

    delete h5File;

    delete pProg;

    if (bRet) {
      if (bNormalSave)
        guard->AttachHDF5File(0); // release our backup database, so that we can release our backup

      backup.Release(); // success, don't restore backup file

      if (bSwitch)
        databaseHDF5File = strFileName; // the new file will be our database
    }

  } catch (CBackup::CBackupError &error) {
    _m()->error(error.Message());
    retval = false;
  }

  if (bIsHDF5 && !databaseHDF5File.isEmpty() && (bSwitch || bNormalSave)) {
    qFile = new QFile(databaseHDF5File);
    h5File = new CHDF5File(qFile);

    if (h5File && h5File->open(QIODevice::ReadOnly) && h5File->openStream(CHDF5File::ModelGroup(0) + "ModelStream"))
      model.GetConsistencyGuard()->AttachHDF5File(h5File, retval);
    else {
      _m()->error(QString("Unable to open '%1' for reading on demand, please check.").arg(databaseHDF5File));
      delete h5File;
    }
  }

  return retval;
}

} // namespace privateModelIO