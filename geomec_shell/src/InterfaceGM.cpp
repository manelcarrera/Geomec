#include "InterfaceGM.h"
#include "ArchiveQDataStream.h"
#include "ArgumentParser.h"
#include "BaseEntryTypes.h"
#include "GeomecShellUtils.h"
#include "GeomecUtils.h"
#include "InterfaceContext.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "ResultCache.h"
#include "ResultRegister.h"
#include "WellZoomInModel.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"
#include "geomec_shell.h"

#include <QTextStream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ModelDiagnostics.h"

#include "Printer.h"

namespace {
Printer *printer = Printer::instance(Printer::Shell);
}

namespace gm_shell {

bool CInterfaceGM::Expose() const { return true; }

const char *CInterfaceGM::ShortDescription() const { return "Geomec"; }

bool CInterfaceGM::AcceptParameters(CArgumentParser &argParser, bool bForced) {
  if (bForced) {
    std::string key;
    std::string value;

    m_argParser = &argParser;

    int nFound = 0;

    for (size_t i = 0; i < argParser.ArgumentSize(); ++i) {
      argParser.GetArgument(i, key, value);
      if (value == "list") {
        m_commands.push(GM_LIST);
      } else if (value == "run") {
        m_commands.push(GM_RUN);
      }
      /*else if (value == "end")
      {
    m_commands.push(GM_END);
      }*/
      else if (value == "linear" || value == "nonlin" || value == "heat" || value == "mixture" ||
               value == "containment" || value == "FF") // FIXME
      {
        m_type = value.c_str();
      } else if (value.length() > 4) {
        size_t len = value.length();
        size_t pos = value.rfind('.');

        if (pos != std::string::npos) {
          std::string ext = value.substr(pos, len - pos);

          if (ext == ".gm5") {
            if (!nFound)
              m_model = value.c_str();
            ++nFound;
          } else if (ext == ".so") {
            m_import = value.c_str();
            m_commands.push(GM_IMPORT);
          }
        }
      } else {
        char *s = const_cast<char *>(value.c_str());
        int v = strtol(value.c_str(), &s, 10);
        if (s != value.c_str())
          m_indices.push_back(v);
      }
    }

    if (m_commands.size() == 1 && nFound == 1) {
      return true;
    } else {
      m_error = 1;
      if (m_commands.size() == 1)
        m_commands.pop();
      m_commands.push(IInterfaceBase::USAGE);
      m_commands.push(IInterfaceBase::ABORT);
    }
  }

  return false;
}

void CInterfaceGM::PrintUsage() const {
  std::cerr << std::endl
            << "geomec_shell /interface gm list <model>" << std::endl
            << "                           run [linear|nonlin|heat|mixture|containment] <model> [<child index> [<child "
               "index> ...]]"
            << std::endl
            << "                           <skua file> <output file>" << std::endl
            << "  only gm5 models are supported" << std::endl
            << "  this feature is EXPERIMENTAL" << std::endl
            << std::endl;
}

void CInterfaceGM::Startup() {
  printer->info("using interface CORA");

  std::cout << "using interface GM" << std::endl << std::endl;

  if (IsSlave()) {
    m_pModelContext = m_channel->Master()->GetModelContext();
  } else if (!m_initialized) {
    m_pModelContext = new CInterfaceModelContext(AppVersion());

    if (!m_model.isEmpty()) {
      gm_shell::MakeAbsolutePath(m_model);

      if (m_import.isEmpty()) {
        try {
          m_pModelContext->Load(m_model);
        } catch (std::exception const &e) {
          std::cerr << "caught exception: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        if (!m_pModelContext->Model()) {
          std::cerr << "Failed to load model. Please make sure it exists. Exiting..." << std::endl;
          m_commands.push(IInterfaceBase::ABORT);
        }

        m_pModelContext->AutoExport(true);
      }
    }
  }

  m_initialized = true;
}

void printWells(const CModelBase &model, std::string indent) {
  CNewWellPathEntry::TNodeSet wps =
      static_cast<const CNewWellPathEntry *>(model.GraphEntry(MD_NEW_WELLPATH))->EntryNodes();
  for (CNewWellPathEntry::TNodeSet::iterator it = wps.begin(); it != wps.end(); ++it) {
    if (!(*it)->CanZoomIn()) {
      CWellZoomInModel &child = (*it)->ZoomInModel();

      std::cout << indent << child.Index() << "\twell\t" << child.Name().toStdString() << std::endl;

      if (child.childModelSize() > 0) {
        std::cout << indent << child.childModel(0).Index() << "\tcasing\t" << child.childModel(0).Name().toStdString()
                  << std::endl;
      }
    }
  }
}

void CInterfaceGM::HandleLastCommand() {
  int command = m_commands.front();
  m_commands.pop();

  int retval = 1;

  switch (command) {
  case GM_LIST: {
    std::cout << "LISTING:" << std::endl;

    if (m_pModelContext->Model()) {
      std::cout << "0\tmain\t" << m_pModelContext->Model()->Name().toStdString() << std::endl;

      printWells(*m_pModelContext->Model(), "");

      CZoominModelEntry *pZIEntry =
          static_cast<CZoominModelEntry *>(m_pModelContext->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL));
      CZoominModelEntry::TNodeSet zoominNodes = pZIEntry->EntryNodes();
      for (CZoominModelEntry::TNodeSet::iterator it = zoominNodes.begin(); it != zoominNodes.end(); ++it) {
        CModelBase &model = (*it)->ChildModel();
        std::cout << model.Index() << "\tzoomin" << model.Name().toStdString() << std::endl;
        printWells(model, "\t");
      }
    }
    m_commands.push(IInterfaceBase::QUIT);
  } break;
  case GM_RUN: {
    for (size_t i = 0; i < m_indices.size(); ++i) {
      int index = m_indices[i];

      bool bSwitched = false;

      if (index > 0) {
        CNewWellPathEntry::TNodeSet wps =
            static_cast<const CNewWellPathEntry *>(m_pModelContext->Model()->GraphEntry(MD_NEW_WELLPATH))->EntryNodes();
        for (CNewWellPathEntry::TNodeSet::iterator it = wps.begin(); it != wps.end(); ++it) {
          if (!(*it)->CanZoomIn()) {
            CWellZoomInModel &child = (*it)->ZoomInModel();
            if (child.Index() == index) {
              m_pModelContext->SwitchTo(&child);
              bSwitched = true;
              break;
            }

            if (child.childModelSize() > 0) {
              if (child.childModel(0).Index() == index) {
                m_pModelContext->SwitchTo(&child.childModel(0));
                bSwitched = true;
                break;
              }
            }
          }
        }

        if (!bSwitched) {
          CZoominModelEntry *pZIEntry =
              static_cast<CZoominModelEntry *>(m_pModelContext->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL));
          CZoominModelEntry::TNodeSet zoominNodes = pZIEntry->EntryNodes();
          for (CZoominModelEntry::TNodeSet::iterator it = zoominNodes.begin(); it != zoominNodes.end(); ++it) {
            CModelBase &model = (*it)->ChildModel();
            if (model.Index() == index) {
              m_pModelContext->SwitchTo(&model);
              break;
            }
          }
        }
      }
    }
    m_pModelContext->Type(m_type);

    m_commands.push(IInterfaceBase::CALCULATE);
    // m_commands.push(IInterfaceBase::END);
    m_commands.push(GM_SAVE);
    m_commands.push(IInterfaceBase::QUIT);
  } break;
  case GM_SAVE: {
    QString base = m_model.left(m_model.length() - 4);
    QString ext = m_model.right(4);

    QString model = base + "_results" + ext;
    int i = 0;
    while (QFile(model).exists())
      model = base + QString("_results%1").arg(i++) + ext;

    m_pModelContext->SaveAs(model);

    std::cout << "Saved as '" << model.toStdString() << "'" << std::endl;
  } break;
  case GM_IMPORT: {
    m_pModelContext->Create(m_model, m_import);
    m_pModelContext->SaveAs(m_model);
    m_commands.push(IInterfaceBase::QUIT);
  }
  }

  if (retval != 0) {
    m_error = retval;
    m_commands.push(IInterfaceBase::USAGE);
    m_commands.push(IInterfaceBase::ABORT);
  }
}

CInterfaceModelContext *CInterfaceGM::GetModelContext() { return m_pModelContext; }

} // namespace gm_shell