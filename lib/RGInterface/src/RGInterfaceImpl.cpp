// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGInterfaceImpl.h"
#include "FilesystemHelper.h"
#include "RGInterface.h"
#include "RGSurface.h"
#include "RGUtils.h"

#include <climits>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <RescueArrayFloat.h>
#include <RescueArrayInt.h>
#include <RescueBlock.h>
#include <RescueCoordinateSystem.h>
#include <RescueDataContainer.h>
#include <RescueHistory.h>
#include <RescueHorizon.h>
#include <RescueLogicalOrder.h>
#include <RescueLogicalOrderEntry.h> // has to be included before RescueLogicalOrder.h
#include <RescueLookup.h>
#include <RescueLookupString.h>
#include <RescueLookupTable.h>
#include <RescueModel.h>
#include <RescueUnstructuredGrid.h>
#include <RescueVertex.h>
#include <cNameValuePair.h>
#include <cSetRescueDataContainer.h>

///////////////////////////////////////////////////////////////////////////////
/// @brief Default constructor.
/// @param modelName name of the model. It is used as prefix for storage file
/// @param saveAsTxt save model file in text format, usefull for debugging purpose
///////////////////////////////////////////////////////////////////////////////
RGInterface::RGInterfaceImpl::RGInterfaceImpl(const std::string &modelName, bool saveAsTxt)
    : m_modelName(modelName), m_nbPolygonCorner(4), // Tiangles and Quadrilateral only
      m_dstage(-1), m_isTxtFormat(saveAsTxt) {
  m_isReadOnly = false;

  m_rescueFileName = FilesystemHelper::RGGetFileName(m_modelName, m_isTxtFormat);

  auto pContext = std::make_unique<RescueContext>();
  std::unique_ptr<RescueModel> pModel(
      RescueModel::UnarchiveModel(pContext.get(), const_cast<char *>(m_rescueFileName.c_str())));
  if (pModel.get()) // loading existing Rescue model
  {
    checkRgiVersion(pModel);

    loadGeometry(pModel);
    loadCommands(pModel);
    loadProperties(pModel);
    loadErrorMsgs(pModel);
    loadFormations(pModel);
    loadSurfaces(pModel);

    loadAvailableFailureMechanismsInModel(pModel);
    loadLSFEvaluationsInModel(pModel);

    // load Monitorable properties from standalone
    loadAvailableMonitorableProperties(pModel);
    loadMonitorablePointSets(pModel);
    loadMonitorableValues(pModel);

    m_isReadOnly = m_nodes.size() || !m_cmdSet.size(); // only set of commands, no geometry given
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Copy constructor. (For testing purpose)
/// @param modelName name of the model. It is used as prefix for storage file
/// @param toCp object of RGInterface to be copied into current
/// @param saveAsTxt save model file in text format, usefull for debugging purpose
///////////////////////////////////////////////////////////////////////////////
RGInterface::RGInterfaceImpl::RGInterfaceImpl(const std::string &modelName, const RGInterface::RGInterfaceImpl *toCp,
                                              bool saveAsTxt)
    : m_modelName(modelName), m_dstage(-1), m_isReadOnly(false), m_isTxtFormat(saveAsTxt) {
  assert(toCp);
  m_nbPolygonCorner = toCp->m_nbPolygonCorner;
  m_rescueFileName = FilesystemHelper::RGGetFileName(m_modelName, m_isTxtFormat);

  m_nodes = toCp->m_nodes;
  m_elements = toCp->m_elements;
  m_dstage = toCp->m_dstage;
  m_timeSteps = toCp->m_timeSteps;
  m_propTable = toCp->m_propTable;
  m_propIntTable = toCp->m_propIntTable;
  m_cmdSet = toCp->m_cmdSet;
  m_polygons = toCp->m_polygons;
  m_monitorableProperties = toCp->m_monitorableProperties;
  m_monitorPointSets = toCp->m_monitorPointSets;
  m_monitorValues = toCp->m_monitorValues;

  std::copy(toCp->m_surfaces, toCp->m_surfaces + RGSurface::NumTypes, m_surfaces);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Actual model writing is doing here.
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::dumpModel() const {
  try {
    // to preven memleak reports should delete those objecto on return
    std::unique_ptr<RescueCoordinateSystem> pLocCoordSys;
    std::unique_ptr<RescueVertex> pVertex;
    std::unique_ptr<RescueCoordinateSystem> pGlbCoordSys;

    auto pContext = std::make_unique<RescueContext>();
    // create Rescue model
    std::unique_ptr<RescueModel> pModel;

    // at first make an attempt to open existing model with only commands set
    pModel.reset(RescueModel::UnarchiveModel(pContext.get(), const_cast<char *>(m_rescueFileName.c_str())));
    bool newModel = false;
    // if failed - create new model
    if (!pModel.get()) {
      const std::string &dirName = FilesystemHelper::RGGetDirName(m_rescueFileName);
      FilesystemHelper::mkdir(dirName);
      pModel.reset(new RescueModel(pContext.get(), const_cast<char *>(dirName.c_str()))); // create new model
      newModel = true;
      setRgiVersionInModel(pModel);
    }
    // setup geometry
    dumpGeometry(pModel);
    if (newModel && pModel->CoordinateSystem()) {
      pLocCoordSys.reset(pModel->CoordinateSystem());      // should care about local coordinate system
      pVertex.reset(pModel->CoordinateSystem()->Vertex()); // should care about Vertex
      pGlbCoordSys.reset(
          pModel->CoordinateSystem()->Vertex()->CoordinateSystem()); // should care about global coordinate system alse
    }

    setMetaData(pModel);

    // dump the failure mechanisms and lsf
    dumpAvailableFailureMechanismsInModel(pModel);
    dumpLSFEvaluationsInModel(pModel);
    dumpAvailableMonitorablePropertiesInModel(pModel);
    dumpMonitorablePointSetsInModel(pModel);
    dumpMonitorableValuesInModel(pModel);

    bool propSaved = false;
    propSaved |= dumpProperties(pModel);
    propSaved |= dumpCommands(pModel);
    propSaved |= dumpErrorMsgs(pModel);
    propSaved |= dumpFormations(pModel);
    propSaved |= dumpSurfaces(pModel);

    if (!m_isReadOnly || propSaved) {
      // actual dump model to files
      if (m_isTxtFormat)
        std::cout << "Archiving the model..." << std::endl;
      if (!pModel->ArchiveModel(const_cast<char *>(FilesystemHelper::RGGetDirName(m_rescueFileName).c_str()),
                                !m_isTxtFormat, 37, true)) {
        std::ostringstream out;
        out << "An error occurred while saving the RESCUE model.";
        if (pModel->Context()->LastError()) {
          out << " " << pModel->Context()->LastError();
        }
        pModel->Context()->ClearError();
        throw Exception(out.str());
      }
    }
  } catch (std::runtime_error &ex) // process any Rescue errors in very simple way
  {
    throw Exception(std::string("RGInterface library error. ") + ex.what());
  } catch (char const *msg) {
    throw Exception(std::string("RGInterface library error. ") + msg);
  }
}

/// @brief Get number of available depletion stages
/// @return number of depletion stages
size_t RGInterface::RGInterfaceImpl::getDepletionStagesNumber() const { return m_timeSteps.size(); }

/// @breif Get n-th depletion stage
/// @param num zero based depletion stage number
/// @return n-th depletion stage if exist of invalid depletion stage otherwise
RGDepletionStage RGInterface::RGInterfaceImpl::getNthDeplethionStage(size_t num) const {
  return num < m_timeSteps.size() ? m_timeSteps[num] : RGDepletionStage();
}

/// @brief Get total nodes number
size_t RGInterface::RGInterfaceImpl::getNumNodes() const { return m_nodes.size(); }
const RGNode &RGInterface::RGInterfaceImpl::getNodeN(const size_t &n) const { return m_nodes.at(n); }

/// get total elements number
size_t RGInterface::RGInterfaceImpl::getNumElements() const { return m_elements.size(); }
const RGElement &RGInterface::RGInterfaceImpl::getElementN(const size_t &n) const { return m_elements.at(n); }

/// get total polygons number
size_t RGInterface::RGInterfaceImpl::getNumPolygons() const { return m_polygons.size(); }
const RGPolygon &RGInterface::RGInterfaceImpl::getPolygonN(const RGPolygonId &n) const { return m_polygons.at(n); }

/// @brief Get properties number for the current depletion stage
/// @return properties number
size_t RGInterface::RGInterfaceImpl::getPropertiesNumber() const {
  return m_dstage < 0 ? 0 : m_propTable[m_dstage].size() + m_propIntTable[m_dstage].size();
}

/// @brief Get n-th property for the current depletion stage
/// @param p property number
/// @return n-th property
RGProperty RGInterface::RGInterfaceImpl::getNthProperty(size_t p) const {
  if (m_dstage < 0)
    return RGProperty();

  if (p < m_propTable[m_dstage].size()) {
    std::map<RGPropertyType::Type, std::vector<double>>::const_iterator it = m_propTable[m_dstage].begin();
    std::advance(it, p);
    return it->first;
  }

  p -= m_propTable[m_dstage].size();

  if (p < m_propIntTable[m_dstage].size()) {
    std::map<RGPropertyType::Type, std::vector<int>>::const_iterator it = m_propIntTable[m_dstage].begin();
    std::advance(it, p);
    return it->first;
  }
  return RGProperty();
}

size_t RGInterface::RGInterfaceImpl::getNumCommands() const { return m_cmdSet.size(); }

/// @brief Get command for given position
/// @param n position nubmer
/// @return command for given position or undefined command if this position does not exists
GMCommand RGInterface::RGInterfaceImpl::getCommandN(const size_t &n) const {
  return n < m_cmdSet.size() ? m_cmdSet[n] : GMCommand(typeCommandLast);
}

/// @brief Drop all commands which are in list
void RGInterface::RGInterfaceImpl::clearCommandList() { m_cmdSet.clear(); }

/// @brief Get number of error messages for current depletion stage.
///        When new depleting stage has being defined, this error messages list is reseted.
/// @return number of error messages
size_t RGInterface::RGInterfaceImpl::getNumErrorMsgs() const { return m_errMsgs.size(); }

/// @brief Get Nth error mesage
/// @param n message position in list
/// @return n-th error message
std::string RGInterface::RGInterfaceImpl::getNthErrorMsg(size_t n) const { return m_errMsgs.at(n); }

/// @brief Add new error message
/// @param msg error message as string
void RGInterface::RGInterfaceImpl::addErrorMsg(const std::string &msg) {
  if (!msg.empty())
    m_errMsgs.push_back(msg);
}

bool RGInterface::RGInterfaceImpl::isReadOnly() const { return m_isReadOnly; }

// Dealing with formations
/// @brief Get the user name associated with the specified formation index
/// @param formationID value in array propElementFormation
/// @return user name
std::string RGInterface::RGInterfaceImpl::getFormationName(int formationID) const {
  return m_formationMap.count(formationID) ? m_formationMap.find(formationID)->second : "";
}

int RGInterface::RGInterfaceImpl::GetNumFormations() { return m_formationMap.size(); }

void RGInterface::RGInterfaceImpl::GetFormationInfo(int formationIndex, std::string &formationName, int &formationId) {
  if (formationIndex < m_formationMap.size()) {
    std::map<int, std::string>::iterator it = m_formationMap.begin();
    std::advance(it, formationIndex);
    formationName = it->second;
    formationId = it->first;
  } else {
    formationName = std::string("");
    formationId = -1;
  }
}

// Surfaces
size_t RGInterface::RGInterfaceImpl::getNumHorizons() const { return m_surfaces[RGSurface::Horizon].size(); }
size_t RGInterface::RGInterfaceImpl::getNumFaults() const { return m_surfaces[RGSurface::FaultFront].size(); }

const RGSurface &RGInterface::RGInterfaceImpl::getHorizonN(const size_t &n) const {
  return m_surfaces[RGSurface::Horizon].at(n);
}
const RGSurface &RGInterface::RGInterfaceImpl::getFaultFrontN(const size_t &n) const {
  return m_surfaces[RGSurface::FaultFront].at(n);
}
const RGSurface &RGInterface::RGInterfaceImpl::getFaultBackN(const size_t &n) const {
  return m_surfaces[RGSurface::FaultBack].at(n);
}

void RGInterface::RGInterfaceImpl::addHorizon(const RGSurface &surface) {
  m_surfaces[RGSurface::Horizon].push_back(surface);
}

void RGInterface::RGInterfaceImpl::addFault(const RGSurface &faultF, const RGSurface &faultB) {
  m_surfaces[RGSurface::FaultFront].push_back(faultF);
  m_surfaces[RGSurface::FaultBack].push_back(faultB);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set current depletion stage. This method must be called by RockMech only
/// @param curStage reference to current depletion stage
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setCurrentDepletionStage(const RGDepletionStage &curStage) {
  assert(curStage.isValid());

  if (m_timeSteps.size()) {
    if (m_timeSteps.back() > curStage) // attept to set time step early than last one, check history
    {
      std::vector<RGDepletionStage>::iterator dst = std::find(m_timeSteps.begin(), m_timeSteps.end(), curStage);
      if (dst ==
          m_timeSteps.end()) // can't find time step in history, inserting in history is forbidden, fire up exception
      {
        throw Exception("setCurrentDepletionStage(): There is no given depletion stage in history");
      } else {
        m_dstage = dst - m_timeSteps.begin(); // find position in history, can switch to given
      }
    } else if (curStage >
               m_timeSteps.back()) // append at the end of time steps array and create empty group of properties
    {
      m_dstage = m_timeSteps.size();
      m_timeSteps.push_back(curStage);
      m_propTable.push_back(std::map<RGPropertyType::Type, std::vector<double>>());
      m_propIntTable.push_back(std::map<RGPropertyType::Type, std::vector<int>>());

      if (!m_errMsgs.empty()) // reset the list of errro messages
      {
        m_errMsgs.clear();
      }
    } else // curStage == m_timeSteps.back()
    {
      m_dstage = m_timeSteps.size() - 1;
    }
  } else {
    m_dstage = m_timeSteps.size();
    m_timeSteps.push_back(curStage);
    m_propTable.push_back(std::map<RGPropertyType::Type, std::vector<double>>());
    m_propIntTable.push_back(std::map<RGPropertyType::Type, std::vector<int>>());

    if (!m_errMsgs.empty()) // reset the list of errro messages
    {
      m_errMsgs.clear();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get current depletion stage
/// @return current depletion stage
///////////////////////////////////////////////////////////////////////////////
RGDepletionStage RGInterface::RGInterfaceImpl::getCurrentDepletionStage() const {
  assert(m_dstage >= 0);
  return m_timeSteps[m_dstage];
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Add new node
/// @param nd reference to node object
/// @return node ID
///////////////////////////////////////////////////////////////////////////////
RGNodeId RGInterface::RGInterfaceImpl::addNode(const RGNode &nd) {
  assert(nd[0] < 1e10);
  assert(nd[1] < 1e10);
  assert(nd[2] < 1e10);

  m_nodes.push_back(nd);
  return m_nodes.size() - 1;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Add new element as set of nodes and element type. Interface elements
/// are stored separate from cell elements because of cell properties array should
/// be the same size as m_elemenents
/// @param element reference to RGElemnt object
/// @return added element ID
///////////////////////////////////////////////////////////////////////////////
RGElementId RGInterface::RGInterfaceImpl::addElement(const RGElement &element) {
  switch (element.getType()) {
    // Tetrahedron
  case RGElementType::typeTE12L:
  case RGElementType::typeCTE30:
    // Hexahedron
  case RGElementType::typeHX24L:
  case RGElementType::typeCHX60:
    // Interface
  case RGElementType::typeT18IF:
  case RGElementType::typeCT36I:
  case RGElementType::typeQ24IF:
  case RGElementType::typeCQ48I:
    m_elements.push_back(element);
    break;

  default:
    assert(0);
  }
  return m_elements.size() - 1;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Add new polygon as vector of nodes
/// @param polygon reference to RGPolygon object
/// @return added polygon ID
///////////////////////////////////////////////////////////////////////////////
RGPolygonId RGInterface::RGInterfaceImpl::addPolygon(const RGPolygon &polygon) {
  assert(polygon.size() > 2);
  m_polygons.push_back(polygon);
  return m_polygons.size() - 1;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Calculate total faces number
/// @return number of faces in all elements
///////////////////////////////////////////////////////////////////////////////
size_t RGInterface::RGInterfaceImpl::getNumFaces() const {
  size_t faceNum = 0;
  for (size_t i = 0; i < m_elements.size(); ++i) {
    switch (m_elements[i].getType()) {
      // Tetrahedron
    case RGElementType::typeTE12L: // Linear
    case RGElementType::typeCTE30: // Quadratic
      faceNum += 4;
      break;

      // Hexahedron
    case RGElementType::typeHX24L: // Linear
    case RGElementType::typeCHX60: // Quadratic
      faceNum += 6;
      break;

      // Interface
    case RGElementType::typeT18IF: // 3-noded-tri-interface element
    case RGElementType::typeCT36I: // 6-noded-tri-interface element
    case RGElementType::typeQ24IF: // 4-noded-quad-interface element
    case RGElementType::typeCQ48I: // 8-noded-quad-interface element
      faceNum += 1;
      break;

    default:
      assert(0);
    }
  }
  return faceNum;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Write metas data into model
/// @param pModel Rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setMetaData(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  char datetime[50];

  /* Get current time and date */
  time_t tt = time(NULL);
  struct tm *t = localtime(&tt);

  /* Create time/date string */
  /* Year is displayed with century (e.g 1998 ) */
  strftime(datetime, sizeof(datetime), "%a %d/%m/%Y %H:%M", t);

  pModel->SetWriter("RGInterface", RGInterface::VERSION.toString().c_str(), datetime,
                    FilesystemHelper::getProcessName().c_str());
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Check version consistency between library and model, throw if conflict
/// @param pModel Rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::checkRgiVersion(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Get old version
  RGInterface::Versioning modelVersion = getRgiVersionInModel(pModel);

  // Throw if inconsistent
  if (RGInterface::VERSION.major != modelVersion.major) {
    std::ostringstream msg;
    msg << "writeRgiVersion(): RGI version conflict between Geomec and Rockmech. \n";
    msg << "Geomec RGI version: " << RGInterface::VERSION.toString() << "\n";
    msg << "Rockmech RGI version: " << modelVersion.toString() << "\n";

    // Write error and throw exception
    addErrorMsg(msg.str());
    dumpModel();

    throw VersionError(msg.str());
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set RGI version into model
/// @param an existing model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setRgiVersionInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());
  RescueLookup *versionTable = pModel->LookupNamed(const_cast<char *>((RGInterface::VERSION.name)));
  if (versionTable) {
    pModel->DropRescueLookup(versionTable);
  }

  versionTable = new RescueLookup(const_cast<char *>(RGInterface::VERSION.name), 1, pModel.get());
  if (versionTable) {
    RESCUEFLOAT *versionArray = new RESCUEFLOAT[2];
    versionArray[0] = RGInterface::VERSION.major;
    versionArray[1] = RGInterface::VERSION.minor;
    versionTable->SetNthItem(0, new RescueLookupTable(pModel->Context(), 1, versionArray));
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get RGI version from model
/// @return RGI version or empty string if no version could be retrieved
///////////////////////////////////////////////////////////////////////////////
RGInterface::Versioning RGInterface::RGInterfaceImpl::getRgiVersionInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());
  RescueLookup *versionTable = pModel->LookupNamed(const_cast<char *>(RGInterface::VERSION.name));
  if (versionTable) {
    // If existing table, check current version
    RescueLookupTable *versionArray = static_cast<RescueLookupTable *>(versionTable->NthItem(0));
    if (versionArray && versionArray->TranslationLength64() == 2) {
      RESCUEFLOAT *versionPtr = versionArray->Translation();
      RGInterface::Versioning version = {RGInterface::VERSION.name, versionPtr[0], versionPtr[1]};
      return version;
    }
  }
  RGInterface::Versioning noVersion = {"", 0, 0};
  return noVersion;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load property. Property values for elements which can't keep given property
/// type are defined as UNDEF_PROP_VALUE. If there is no such property for current depletion stage
/// return empty values array
/// @param prop property description
/// @param vals array for property values which will be resized to fit properties values for all elements
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadProperty(const RGProperty &property, std::vector<double> &vals) {
  vals.clear();
  if (m_dstage < 0) {
    return;
  }
  // check if given property exist in the table
  std::map<RGPropertyType::Type, std::vector<double>>::iterator ppos = m_propTable[m_dstage].find(property.getType());
  if (ppos != m_propTable[m_dstage].end()) // found one
  {
    vals.insert(vals.end(), ppos->second.begin(), ppos->second.end());
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load property. Intgers property values for elements which can't keep given property
/// type are defined as UNDEF_PROP_VALUE. If there is no such property for current depletion stage
/// return empty values array
/// @param prop property description
/// @param vals array for property values which will be resized to fit properties values for all elements
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadProperty(const RGProperty &property, std::vector<int> &vals) {
  vals.clear();
  if (m_dstage < 0) {
    return;
  }
  // check if given property exist in the table
  std::map<RGPropertyType::Type, std::vector<int>>::iterator ppos = m_propIntTable[m_dstage].find(property.getType());
  if (ppos != m_propIntTable[m_dstage].end()) // found one
  {
    vals.insert(vals.end(), ppos->second.begin(), ppos->second.end());
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Check if saving of supplied array of values is correct, throw an exeption on error
/// @param property property description
/// @parm vals the set of the property values
///////////////////////////////////////////////////////////////////////////////
template <class T>
void RGInterface::RGInterfaceImpl::CheckSaveProperty(const RGProperty &property, const std::vector<T> &vals) {
  if (m_dstage < 0) {
    throw Exception("saveProperty(): It is necessary to set depletion stage before adding property");
  }

  if (vals.size() != m_elements.size()) {
    std::ostringstream msg;
    msg << "saveProperty(): the number of values (" << vals.size() << ") provided for property " << property.toString()
        << " does not match the number of elements (" << m_elements.size() << ")" << std::endl;
    throw Exception(msg.str());
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Insert property value array into properties table for current
/// depletion stage position
/// @param property property description
/// @parm vals the set of the property values
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::saveProperty(const RGProperty &property, const std::vector<double> &vals) {
  CheckSaveProperty(property, vals);

  // Create or update the property
  m_propTable[m_dstage][property.getType()] = vals;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Insert property integer values array into properties table for current
/// depletion stage position
/// @param property property description
/// @parm vals the set of the property values
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::saveProperty(const RGProperty &property, const std::vector<int> &vals) {
  CheckSaveProperty(property, vals);

  // Create or update the property
  m_propIntTable[m_dstage][property.getType()] = vals;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Delete all properties for given depletion stage number
/// @param stageNum depletion stage number
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::deletePropertiesForDepletionStage(size_t stageNum) {
  assert(stageNum < m_propTable.size());
  assert(stageNum < m_propIntTable.size());

  if (stageNum < m_propTable.size()) {
    m_propTable[stageNum].clear();
  }

  if (stageNum < m_propIntTable.size()) {
    m_propIntTable[stageNum].clear();
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Add commands, last command must always be typeCommandCalculate or typeCommandHibernate.
/// Commands can be added to new created model only, and can't to the existing model.
/// @param command command to be added
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::addCommand(const GMCommand &command) {
  if (command.isValid()) {
    m_cmdSet.push_back(command);
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the user name associated with the specified formation index
/// @param formationID value in array propElementFormation
/// @param formationName name associated with ID
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setFormationName(int formationID, const std::string &formationName) {
  if (m_formationMap.count(formationID)) {
    if (m_formationMap[formationID] != formationName) {
      std::stringstream out;
      out << "setFormationName(): can not set different formation name for " << formationID << " formation ID. ";
      out << "it is already defined as " << m_formationMap[formationID];
      throw Exception(out.str());
    }
  } else {
    m_formationMap[formationID] = formationName;
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Compare two RGInterfaceImpl objects for full equality. (For testing purpose)
/// @param tcm object to compare with current
/// @param eps precision for float number comparison
/// @return true if all values in tcm object are equal to values in current object
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::isEqual(const RGInterface::RGInterfaceImpl *tcm, double eps) const {
  if (!tcm)
    return false;

  // Checking geometry
  // Checking nodes
  if (m_nodes.size() != tcm->m_nodes.size()) {
    assert(0);
    return false;
  }
  for (size_t i = 0; i < m_nodes.size(); ++i) {
    if (!m_nodes[i].isEqual(tcm->m_nodes[i], eps)) {
      assert(0);
      return false;
    }
  }
  // checking elements
  if (m_elements.size() != tcm->m_elements.size()) {
    assert(0);
    return false;
  }
  for (size_t i = 0; i < m_elements.size(); ++i) {
    if (m_elements[i] != tcm->m_elements[i]) {
      assert(0);
      return false;
    }
  }

  // Checking time steps
  if (m_dstage != (tcm->m_dstage)) {
    assert(0);
    return false;
  }
  if (m_timeSteps.size() != tcm->m_timeSteps.size()) {
    for (size_t i = 0; i < m_timeSteps.size(); ++i) {
      if (m_timeSteps[i] != (tcm->m_timeSteps[i])) {
        assert(0);
        return false;
      }
      // check properties tables
      if (m_propTable[i].size() != tcm->m_propTable[i].size() ||
          m_propIntTable[i].size() != tcm->m_propIntTable[i].size()) {
        assert(0);
        return false;
      }
      // compare properties sets for one time step
      for (std::map<RGPropertyType::Type, std::vector<double>>::const_iterator it = m_propTable[i].begin();
           it != m_propTable[i].end(); ++it) {
        std::map<RGPropertyType::Type, std::vector<double>>::const_iterator itt = tcm->m_propTable[i].find(it->first);
        if (itt == tcm->m_propTable[i].end()) {
          assert(0);
          return false; // maps keep different set of properties
        }
        // compare values in property arrays
        if (it->second.size() != itt->second.size()) {
          assert(0);
          return false;
        }
        for (size_t j = 0; j < it->second.size(); ++j) {
          if (std::fabs(static_cast<float>(it->second[j]) - static_cast<float>(itt->second[j])) > eps) {
            assert(0);
            return false;
          }
        }
      }

      // compare integer properties sets for one time step
      for (std::map<RGPropertyType::Type, std::vector<int>>::const_iterator it = m_propIntTable[i].begin();
           it != m_propIntTable[i].end(); ++it) {
        std::map<RGPropertyType::Type, std::vector<int>>::const_iterator itt = tcm->m_propIntTable[i].find(it->first);
        if (itt == tcm->m_propIntTable[i].end()) {
          assert(0);
          return false; // maps keep different set of properties
        }
        // compare values in property arrays
        if (it->second.size() != itt->second.size()) {
          assert(0);
          return false;
        }
        for (size_t j = 0; j < it->second.size(); ++j) {
          if (it->second[j] != itt->second[j]) {
            assert(0);
            return false;
          }
        }
      }
    }
  }

  // Check command list
  if (m_cmdSet.size() != tcm->m_cmdSet.size()) {
    assert(0);
    return false;
  }

  for (size_t i = 0; i < m_cmdSet.size(); ++i) {
    if (m_cmdSet[i] != tcm->m_cmdSet[i]) {
      assert(0);
      return false;
    }
  }

  return true;
}

static int hexFace2nodePerm[6][4] = {
    {1, 3, 7, 5}, // back face   +DIMX
    {2, 6, 7, 3}, // right face  +DIMY
    {0, 2, 3, 1}, // bottom face +DIMZ
    {0, 4, 6, 2}, // front face  -DIMX
    {0, 1, 5, 4}, // left face   -DIMY
    {4, 5, 7, 6}  // top face    -DIMZ
};

static int tetFace2nodePerm[4][3] = {
    {0, 1, 2},
    {2, 1, 3},
    {0, 3, 1},
    {0, 2, 3},
};

///////////////////////////////////////////////////////////////////////////////
/// @brief write model geometry data into rescue file format. unstructured grid
/// exported as lgr for 1x1x1 cpg grid
/// @param pModel rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::dumpGeometry(std::unique_ptr<RescueModel> &pModel) const {
  if (!pModel->CoordinateSystem()) {
    if (m_isTxtFormat)
      std::cout << "Defining coordinate system as LUF..." << std::endl;
    // Create the global coordinate system.
    RescueCoordinateSystem *pGlobalCoordinateSystem =
        new RescueCoordinateSystem(pModel->Context(), const_cast<char *>("Global Coordinate System"),
                                   RescueCoordinateSystem::LUF, 0, "northing", "m", "easting", "m", "depth", "m");

    // Create the model vertex.
    RescueVertex *pModelVertex = new RescueVertex(const_cast<char *>("Model Vertex"), pGlobalCoordinateSystem, 0, 0, 0);
    pModel->SetRotation(0.0f);

    // Create the local coordinate system.
    RescueCoordinateSystem *pLocalCoordinateSystem = new RescueCoordinateSystem(
        pModel->Context(), const_cast<char *>("Local Coordinate System"), RescueCoordinateSystem::LUF, pModelVertex,
        "northing", "m", "easting", "m", "depth", "m");

    pModel->SetCoordinateSystem(pLocalCoordinateSystem);
  }

  if (!m_nodes.size())
    return;

  // set up geometry
  if (m_isTxtFormat)
    std::cout << "Setting up geometry..." << std::endl;
  RescueGrid *pRescueGrid = new RescueGrid(pModel->Context(), RescueCoordinateSystem::LUF, 0, 2, 0, 2, 0, 2);
  RescueGeometry *pGeometry = new RescueGeometry(pModel.get(), pRescueGrid, RGUtils::nullReal());
  pModel->AddGeometry(pGeometry);

  // setup logical structure
  if (m_isTxtFormat)
    std::cout << "Setting up logical structure..." << std::endl;
  RescueBlock *pBlock(new RescueBlock(const_cast<char *>("Global Block"), pModel.get()));
  RescueLogicalOrder *pOrder(pModel->LogicalOrder());

  pOrder->InsertAtBase(new RescueHorizon(const_cast<char *>("Base"), pModel.get()));

  RescueUnit *pUnit(new RescueUnit(const_cast<char *>("The only zone"), pModel.get()));
  new RescueBlockUnit(pBlock, pUnit);
  pOrder->InsertAtBase(pUnit);
  new RescueGeometryUnit(pGeometry, pUnit, 1, 0);

  pOrder->InsertAtBase(new RescueHorizon(const_cast<char *>("Top"), pModel.get()));

  // create 1x1x1 CPG grid
  if (m_isTxtFormat)
    std::cout << "Creating 1x1x1 CPG grid..." << std::endl;
  RESCUEFLOAT minVals[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
  RESCUEFLOAT maxVals[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

  for (std::vector<RGNode>::const_iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    for (int i = 0; i < 3; ++i) {
      if ((*it)[i] < minVals[i])
        minVals[i] = (*it)[i];
      if ((*it)[i] > maxVals[i])
        maxVals[i] = (*it)[i];
    }
  }
  pGeometry->SetCornerNode(0, 0, 0, 0, minVals[0], minVals[1], minVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 1, maxVals[0], minVals[1], minVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 3, minVals[0], maxVals[1], minVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 2, maxVals[0], maxVals[1], minVals[2]);

  pGeometry->SetCornerNode(0, 0, 0, 4, minVals[0], minVals[1], maxVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 5, maxVals[0], minVals[1], maxVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 7, minVals[0], maxVals[1], maxVals[2]);
  pGeometry->SetCornerNode(0, 0, 0, 6, maxVals[0], maxVals[1], maxVals[2]);

  // Create unstructured grid as a LGR for CPG grid
  if (m_isTxtFormat)
    std::cout << "Dumping unstructured grid as refinement to 1x1x1 CPG..." << std::endl;
  RescueProperty *pProperty =
      new RescueProperty(pGeometry, const_cast<char *>("LGRID"), const_cast<char *>("General Discrete"),
                         const_cast<char *>(""), RGUtils::nullInteger());
  RescueArrayInt *pArray = static_cast<RescueArrayInt *>(pProperty->Data());
  pArray->AcceptValue(RGUtils::nullInteger(), new RESCUEINT32[pGeometry->Grid()->NodeCount(true)]);

  const RESCUEINT64 lgrID = 777; // nice number, just for checking in debugger

  pArray->Value()[0] = lgrID;
  pArray->MarkChanged();

  size_t numNodes = getNumNodes();

  // Expect only tetrahedra, so each cell has 4 faces, each face has 3 vertices
  const size_t reallocSize = getNumElements();
  RescueGeometry *pUGeometry =
      new RescueGeometry(pGeometry, 0, 1, 0, 1, 0, 1, lgrID, RescueGeometry::R_UNSTRUCTURED_3D_POLYHEDRAL,
                         getNumNodes(), getNumFaces(), 3 * getNumFaces(), getNumElements(), 4 * getNumElements(),
                         reallocSize, reallocSize, reallocSize, reallocSize, reallocSize);

  RescueProperty *pElTypeProperty =
      new RescueProperty(pUGeometry, const_cast<char *>("ElementType"), const_cast<char *>("General Discrete"),
                         const_cast<char *>(""), RGUtils::nullInteger());

  RescueArrayInt *pElTypeArray = static_cast<RescueArrayInt *>(pElTypeProperty->Data());
  pElTypeArray->AcceptValue(RGUtils::nullInteger(), new RESCUEINT32[getNumElements()]);

  RescueUnstructuredGrid *pUGrid = pUGeometry->UnstructuredGrid();
  if (numNodes) {
    for (size_t i = 0; i < numNodes; ++i) {
      RESCUEFLOAT x = static_cast<RESCUEFLOAT>((m_nodes[i])[0]);
      RESCUEFLOAT y = static_cast<RESCUEFLOAT>((m_nodes[i])[1]);
      RESCUEFLOAT z = static_cast<RESCUEFLOAT>((m_nodes[i])[2]);
      pUGrid->AddVertices(1, &x, &y, &z);
    }
  }
  if (m_isTxtFormat)
    std::cout << "Dumped " << numNodes << " nodes." << std::endl;

  // dumping non interface elements
  size_t faceId = 0;
  size_t elementId = 0;

  for (size_t i = 0; i < m_elements.size(); ++i, ++elementId) {
    // store element type as int property
    pElTypeArray->Value()[elementId] = m_elements[i].getType();
    pElTypeArray->MarkChanged();

    int k;
    RESCUEINT64 faceIds[6];
    RESCUEINT64 face[16];

    // add faces
    switch (m_elements[i].getType()) {
    case RGElementType::typeTE12L:
    case RGElementType::typeCTE30:
      for (k = 0; k < 4; ++k, ++faceId) {
        for (int j = 0; j < 3; ++j) {
          face[j] = static_cast<RESCUEINT64>(m_elements[i].getNodeN(tetFace2nodePerm[k][j]));
        }
        pUGrid->AddFace(3, face);
        faceIds[k] = faceId;
      }
      break;

    case RGElementType::typeHX24L:
    case RGElementType::typeCHX60:
      for (k = 0; k < 6; ++k, ++faceId) {
        for (int j = 0; j < 4; ++j) {
          face[j] = static_cast<RESCUEINT64>(m_elements[i].getNodeN(hexFace2nodePerm[k][j]));
        }
        pUGrid->AddFace(4, face);
        faceIds[k] = faceId;
      }
      break;

    case RGElementType::typeT18IF:
    case RGElementType::typeCT36I:
    case RGElementType::typeQ24IF:
    case RGElementType::typeCQ48I:
      k = 1;
      assert(m_elements[i].getNumNodes() < 16);
      for (size_t j = 0; j < m_elements[i].getNumNodes() && j < sizeof(face) / sizeof(RESCUEINT64); ++j) {
        face[j] = static_cast<RESCUEINT64>(m_elements[i].getNodeN(j));
      }
      pUGrid->AddFace(m_elements[i].getNumNodes(), face);
      faceIds[0] = faceId++;
      break;

    default:
      assert(0);
    }
    // fake cell center as first node
    pUGrid->AddCell(m_elements[i].getNodeN(0), 0, k, faceIds);
  }
  if (m_isTxtFormat)
    std::cout << "Dumped " << faceId << " faces for " << elementId << " elements." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Read model geometry data from rescue file
/// @param pmode rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadGeometry(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());
  if (!pModel->GridGeometry(0))
    return; // nothing to load

  if (!pModel->GridGeometry(0)->IsLoaded()) {
    pModel->GridGeometry(0)->Load();
  }

  RescueGeometry *pUGeometry = pModel->GridGeometry(0)->ProgenyHavingID(777);
  if (!pUGeometry)
    return; // no UGrid to load;

  if (!pUGeometry->IsLoaded()) {
    pUGeometry->Load();
  }

  RescueUnstructuredGrid *pUGrid = pUGeometry->UnstructuredGrid();
  if (!pUGrid) {
    throw Exception("loadGeometry(): wrong file format. Can't get unstructured grid object");
  }

  RescueProperty *pElTypeProperty = pUGeometry->PropertyNamed("ElementType");
  if (!pElTypeProperty) {
    throw Exception("loadGeometry(): wrong file format. Can't get elements type property");
  }

  if (!pElTypeProperty->Data()->IsLoaded() && !pElTypeProperty->Data()->Load()) {
    throw Exception("loadGeometry(): wrong file format. Can't get elements type property array");
  }
  RescueArrayInt *pElTypeArray = static_cast<RescueArrayInt *>(pElTypeProperty->Data());

  if (m_isTxtFormat)
    std::cout << "Loading geometry..." << std::endl;

  RESCUEINT64 numNodes = pUGrid->VertexCount64();
  for (RESCUEINT64 i = 0; i < numNodes; ++i) {
    RESCUEFLOAT x = pUGrid->NthVertexX(i);
    RESCUEFLOAT y = pUGrid->NthVertexY(i);
    RESCUEFLOAT z = pUGrid->NthVertexZ(i);
    addNode(RGNode(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)));
  }
  if (m_isTxtFormat)
    std::cout << "Loaded " << numNodes << " nodes." << std::endl;

  RESCUEINT64 numCells = pUGrid->CellCount64();

  // Get faces arrays - they looks like CRS matrix:
  // Array with number of faces for correspondent cell
  RESCUEINT64 *faceCellCntArr = pUGrid->FaceCellCount();
  // Array with offsets for cell faces set
  RESCUEINT64 *faceCellNdxArr = pUGrid->FaceCellNdx();
  // Array of faces sets
  RESCUEINT64 *faceListArr = pUGrid->FaceList();

  assert(pUGrid->FaceCellCountLength64() == numCells);

  m_elements.clear();
  m_elements.reserve(numCells);
  for (RESCUEINT64 i = 0; i < numCells; ++i) {
    size_t numFaces = faceCellCntArr[i];
    RESCUEINT64 *facesIds = faceListArr + faceCellNdxArr[i];
    RESCUEINT64 nodesIds[16];
    std::vector<RGNodeId> nodes(16);

    // Check does the number of element faces matchs to the element type
    RGElementType::Type elType = static_cast<RGElementType::Type>(pElTypeArray->Value()[i]);
    if (numFaces != RGElement::getNumFaces(elType)) {
      throw Exception("loadGeometry(): wrong file format. Faces number doesn't match to element type");
    }
    // restore RGElement object
    switch (elType) {
    case RGElementType::typeTE12L:
    case RGElementType::typeCTE30:
      nodes.resize(4);
      // 1st and 2nd faces are enough to restore nodes:
      numNodes = pUGrid->NthFace(facesIds[0], sizeof(nodesIds) / sizeof(RESCUEINT64), nodesIds);
      assert(numNodes == 3);
      nodes[tetFace2nodePerm[0][0]] = static_cast<RGNodeId>(nodesIds[0]);
      nodes[tetFace2nodePerm[0][1]] = static_cast<RGNodeId>(nodesIds[1]);
      nodes[tetFace2nodePerm[0][2]] = static_cast<RGNodeId>(nodesIds[2]);

      numNodes = pUGrid->NthFace(facesIds[1], sizeof(nodesIds) / sizeof(RESCUEINT64), nodesIds);
      assert(numNodes == 3);
      nodes[tetFace2nodePerm[1][2]] = static_cast<RGNodeId>(nodesIds[2]);
      break;

    case RGElementType::typeHX24L:
    case RGElementType::typeCHX60:
      nodes.resize(8);
      for (int f = 2; f < 6; f += 3) // +DIMZ (3th) & -DIMZ (6th) faces are enough to restore nodes:
      {
        numNodes = pUGrid->NthFace(facesIds[f], sizeof(nodesIds) / sizeof(RESCUEINT64), nodesIds);
        assert(numNodes == 4);

        for (RESCUEINT64 j = 0; j < numNodes; ++j) {
          nodes[hexFace2nodePerm[f][j]] = static_cast<RGNodeId>(nodesIds[j]);
        }
      }
      break;

    case RGElementType::typeT18IF:
    case RGElementType::typeCT36I:
    case RGElementType::typeQ24IF:
    case RGElementType::typeCQ48I:
      nodes.clear();
      // only 1 face up to 16 nodes
      numNodes = pUGrid->NthFace(facesIds[0], sizeof(nodesIds) / sizeof(RESCUEINT64), nodesIds);
      for (RESCUEINT64 j = 0; j < numNodes; ++j) {
        nodes.push_back(nodesIds[j]);
      }
      break;

    default:
      assert(0);
    }
    m_elements.push_back(RGElement(elType, nodes));
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Write properties data into rescue file format.
/// @param pModel rescue model
/// @return true if properties were updated in Rescue file
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpProperties(std::unique_ptr<RescueModel> &pModel) const {
  bool retVal = false;
  if (!m_timeSteps.size())
    return false; // no times stapping - no properties

  assert(pModel.get());

  RescueGeometry *pUGeometry = NULL;
  if (pModel->GridGeometry(0)) {
    pUGeometry = pModel->GridGeometry(0)->ProgenyHavingID(777);
  }

  // Run over all available timesteps
  for (size_t ids = 0; ids != m_timeSteps.size(); ++ids) {
    bool newGroup = false;
    RescueTimeStepGroup *pTimeStepGroup = getTimeStepGroup(pModel, m_timeSteps[ids], newGroup);
    retVal |= newGroup;

    // do not dump properties if there is no UGrid, just store time steps as RescueTimeStep groups
    if (!pUGeometry)
      continue;

    for (std::map<RGPropertyType::Type, std::vector<double>>::const_iterator pit = m_propTable[ids].begin();
         pit != m_propTable[ids].end(); ++pit) {
      assert(pit->second.size() == m_elements.size());
      RGProperty rgProp(pit->first);
      RescueProperty *prop = pTimeStepGroup->PropertyNamed(const_cast<char *>(rgProp.toString().c_str()));
      if (!prop) // if didn't find property in file, dump it
      {
        prop =
            new RescueProperty(pUGeometry, const_cast<char *>(rgProp.toString().c_str()), const_cast<char *>("Float"),
                               const_cast<char *>("Float"), RESCUEFLOAT(RGUtils::nullReal()));
        // Initialize the array.
        RescueArrayFloat *pArray = static_cast<RescueArrayFloat *>(prop->Data());
        pArray->AcceptValue(RGUtils::nullReal(), new RESCUEFLOAT[m_elements.size()]);
        assert(pArray);
        for (size_t i = 0; i < pit->second.size(); ++i) {
          pArray->Value()[i] = static_cast<RESCUEFLOAT>((pit->second)[i]);
        }
        pTimeStepGroup->AddProperty(prop);
        retVal = true;
      }
    }

    for (std::map<RGPropertyType::Type, std::vector<int>>::const_iterator pit = m_propIntTable[ids].begin();
         pit != m_propIntTable[ids].end(); ++pit) {
      assert(pit->second.size() == m_elements.size());
      RGProperty rgProp(pit->first);
      RescueProperty *prop = pTimeStepGroup->PropertyNamed(const_cast<char *>(rgProp.toString().c_str()));
      if (!prop) // if didn't find property in file, dump it
      {
        prop =
            new RescueProperty(pUGeometry, const_cast<char *>(rgProp.toString().c_str()), const_cast<char *>("Integer"),
                               const_cast<char *>("Integer"), RESCUEINT32(RGUtils::nullInteger()));
        // Initialize the array.
        RescueArrayInt *pArray = static_cast<RescueArrayInt *>(prop->Data());
        pArray->AcceptValue(RGUtils::nullInteger(), new RESCUEINT32[m_elements.size()]);
        assert(pArray);
        for (size_t i = 0; i < pit->second.size(); ++i) {
          pArray->Value()[i] = static_cast<RESCUEINT64>((pit->second)[i]);
        }
        pTimeStepGroup->AddProperty(prop);
        retVal = true;
      }
    }

    // Delete properties from timestep group which doesn't exist in properties list
    RESCUEINT64 numProps = pTimeStepGroup->RescuePropertyCount64();

    std::vector<RescueProperty *> toDrop; // container for properties which will be dropped

    for (RESCUEINT64 i = 0; i < numProps; ++i) {
      RescueProperty *prop = pTimeStepGroup->NthRescueProperty(i);
      assert(prop);
      if (!prop->Data()->IsLoaded() && !prop->Data()->Load())
        continue; // ignore property which can't be loaded

      // convert property name into RGProperty enum
      RGPropertyType::Type ptype =
          RGProperty::fromString(static_cast<const char *>(prop->Data()->PropertyName()->NonNullString()));

      if (RGPropertyType::propLast == ptype)
        continue; // skip unknown property

      std::map<RGPropertyType::Type, std::vector<double>>::const_iterator ppos = m_propTable[ids].find(ptype);
      std::map<RGPropertyType::Type, std::vector<int>>::const_iterator ippos = m_propIntTable[ids].find(ptype);

      if ((ppos == m_propTable[ids].end()) && (ippos == m_propIntTable[ids].end())) // do not find
      {
        toDrop.push_back(prop); // can't change model before loop finished
        retVal = true;
      }
    }
    // do real properties deleting
    for (std::vector<RescueProperty *>::iterator it = toDrop.begin(); it != toDrop.end(); ++it) {
      // here is a bug in RescueGeometry::DropRescueProperty(). When property object deleted it doesn't clean
      // RescueModel property timestep group container
      pTimeStepGroup->RemoveRescueProperty(*it);
      (*it)->Geometry()->DropRescueProperty(*it);
    }
  }
  return retVal;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load properties data from rescue file format with timesteps information. Set up depletion stage
//  to the latest available in Rescue file. Should be called only in constructor
/// @param pModel rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadProperties(std::unique_ptr<RescueModel> &pModel) {
  assert(m_timeSteps.size() == 0); // must be called in constructor only
  assert(pModel.get());

  RescuePropertyGroup *pPropertyGroup = pModel->NthRescuePropertyGroup(0);

  // global properties container which keeps all timestep groups
  if (pPropertyGroup) {
    RESCUEINT32 tgNum = pPropertyGroup->TimeStepCount();

    // loop over all available time steps and put it in sorted order in m_timeSteps array filling m_propTable
    // simultaneously
    for (RESCUEINT32 tg = 0; tg < tgNum; ++tg) {
      // restore set of depletion stages from names of timestep groups
      RescueTimeStepGroup *pTimeStepGroup = pPropertyGroup->NthTimeStepGroup(tg);
      const RGDepletionStage &ds = RGDepletionStage::fromString(pTimeStepGroup->TimeStepName()->NonNullString());
      std::vector<RGDepletionStage>::iterator it = std::upper_bound(m_timeSteps.begin(), m_timeSteps.end(), ds);
      size_t pos = it - m_timeSteps.begin();

      // insert time step info and empty properties table
      m_timeSteps.insert(it, ds);

      std::vector<std::map<RGPropertyType::Type, std::vector<double>>>::iterator pt =
          m_propTable.insert(m_propTable.begin() + pos, std::map<RGPropertyType::Type, std::vector<double>>());

      std::vector<std::map<RGPropertyType::Type, std::vector<int>>>::iterator ipt =
          m_propIntTable.insert(m_propIntTable.begin() + pos, std::map<RGPropertyType::Type, std::vector<int>>());

      RESCUEINT64 propNums = pTimeStepGroup->RescuePropertyCount64();

      // fill properties table
      for (RESCUEINT64 p = 0; p < propNums; ++p) {
        RescueProperty *prop = pTimeStepGroup->NthRescueProperty(p);
        if (!prop->Data()->IsLoaded() && !prop->Data()->Load())
          continue; // ignore property which can't be loaded

        // convert property name into RGProperty enum
        RGPropertyType::Type ptype =
            RGProperty::fromString(static_cast<const char *>(prop->Data()->PropertyName()->NonNullString()));

        if (RGPropertyType::propLast == ptype)
          continue; // skip unknown property

        // get property data
        switch (prop->Data()->IsA()) {
        case R_RescueArrayFloat: {
          RescueArrayFloat *pArr = static_cast<RescueArrayFloat *>(prop->Data());
          assert(pArr);
          // get cells number from array size
          RESCUEINT64 numCells = pArr->ValueLength64();

          std::vector<double> vals(numCells, RGUtils::nullReal());
          for (RESCUEINT64 i = 0; i < numCells; ++i) {
            vals[i] = static_cast<double>(pArr->Value()[i]);
          }
          // insert property array into properties table
          (*pt).insert(std::pair<RGPropertyType::Type, std::vector<double>>(ptype, vals));
        } break;

        case R_RescueArrayInt: {
          RescueArrayInt *pArr = static_cast<RescueArrayInt *>(prop->Data());
          assert(pArr);
          // get cells number from array size
          RESCUEINT64 numCells = pArr->ValueLength64();

          std::vector<int> vals(numCells, RGUtils::nullInteger());
          for (RESCUEINT64 i = 0; i < numCells; ++i) {
            vals[i] = pArr->Value()[i];
          }
          // insert property array into properties table
          (*ipt).insert(std::pair<RGPropertyType::Type, std::vector<int>>(ptype, vals));
        } break;

        default:
          assert(0);
          throw Exception("loadProperties(): Unsupported property type in rescue model");
        }
      }
    }
  }
  if (m_timeSteps.size()) {
    m_dstage = m_timeSteps.size() - 1;
  }
}

static const char CmdListLookupID[] = "CommandsList";
static const char CmdPrmsListLookupID[] = "CommandsPrmsList";
///////////////////////////////////////////////////////////////////////////////
/// @brief Save commands set and their parameters to Rescue file as 2 RescueLookup objects
///
/// @full Save commands set to Rescue file. If Rescue file already has any commands, it
/// compares the commands set from Rescue and from current RGInterface object. If they are the
/// same - it will do nothing. Otherwise, it will drop object and recreate it from scratch
///
/// @param pModel Rescue model
/// @return true if model was changed and requires archiving, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpCommands(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get()); // check is model created/loaded correctly

  // Search for corresponded RescueLookup objects
  RescueLookup *cmdList = pModel->LookupNamed(const_cast<char *>(CmdListLookupID));
  RescueLookup *cmdPrmsList = pModel->LookupNamed(const_cast<char *>(CmdPrmsListLookupID));

  bool result = false;

  if (!m_cmdSet.empty()) {
    if (cmdList && cmdPrmsList) {
      if (m_cmdSet.size() != cmdList->Count64() || m_cmdSet.size() != cmdPrmsList->Count64()) {
        pModel->DropRescueLookup(cmdList);
        pModel->DropRescueLookup(cmdPrmsList);
        cmdList = cmdPrmsList = NULL;
      } else // table has the same size, compare items then
      {
        bool theSame = true;

        for (RESCUEINT64 i = 0; i < cmdList->Count64(); ++i) {
          RescueLookupString *lkStr = static_cast<RescueLookupString *>(cmdList->NthItem(i));
          if (!lkStr) // not the same number of formation names, obviously
          {
            theSame = false;
            break;
          }

          std::string cmd(lkStr->Translation()->NonNullString());

          lkStr = static_cast<RescueLookupString *>(cmdPrmsList->NthItem(i));
          std::string prms(lkStr->Translation()->NonNullString());

          if (m_cmdSet[i].toStringCmd() != cmd || m_cmdSet[i].toStringPrms() != prms) {
            theSame = false;
            break;
          }
        }
        if (!theSame) {
          pModel->DropRescueLookup(cmdList);
          pModel->DropRescueLookup(cmdPrmsList);
          cmdList = cmdPrmsList = NULL;
        }
      }
    } else // only one is defined - something wrong, drop it
    {
      pModel->DropRescueLookup(cmdList ? cmdList : cmdPrmsList);
    }

    // if there is no commands list or in list in Rescue is differ from RGInterface list, recreate it
    if (!cmdList) {
      cmdList = new RescueLookup(const_cast<char *>(CmdListLookupID), m_cmdSet.size(), pModel.get());
      cmdPrmsList = new RescueLookup(const_cast<char *>(CmdPrmsListLookupID), m_cmdSet.size(), pModel.get());
      int i = 0;
      for (std::vector<GMCommand>::const_iterator it = m_cmdSet.begin(); it != m_cmdSet.end(); ++it) {
        cmdList->SetNthItem(i,
                            new RescueLookupString(pModel->Context(), const_cast<char *>(it->toStringCmd().c_str())));
        cmdPrmsList->SetNthItem(
            i, new RescueLookupString(pModel->Context(), const_cast<char *>(it->toStringPrms().c_str())));
        ++i;
      }
      result = true;
    }
  } else {
    result = (cmdList || cmdPrmsList) ? true : false;

    if (cmdList)
      pModel->DropRescueLookup(cmdList);
    if (cmdPrmsList)
      pModel->DropRescueLookup(cmdPrmsList);
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load commands set from Rescue file as cNameValuePairs
///
/// @full Load commands set from Rescue file. Due to impossibility of deleting Name - Value pairs
/// from Rescue container, commands are being written by chunks divided by "N_Separator" command.
/// The only last chunk is loaded
///
/// @param pModel Rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadCommands(std::unique_ptr<RescueModel> &pModel) {
  m_cmdSet.clear(); // should be called from constructor only!

  assert(pModel.get()); // check is model loaded correctly

  // Obtain Rescue container for commands set
  RescueLookup *cmdList = pModel->LookupNamed(const_cast<char *>(CmdListLookupID));
  RescueLookup *cmdPrmsList = pModel->LookupNamed(const_cast<char *>(CmdPrmsListLookupID));

  if (cmdList && cmdPrmsList) {
    assert(cmdList->Count64() == cmdPrmsList->Count64());

    for (RESCUEINT64 i = 0; i < cmdList->Count64(); ++i) {
      RescueLookupString *lkStr = static_cast<RescueLookupString *>(cmdList->NthItem(i));
      if (!lkStr)
        continue;

      std::string cmd(lkStr->Translation()->NonNullString());

      lkStr = static_cast<RescueLookupString *>(cmdPrmsList->NthItem(i));
      if (!lkStr)
        continue;

      std::string prms(lkStr->Translation()->NonNullString());

      m_cmdSet.push_back(GMCommand::fromStrings(cmd, prms));

      if (!m_cmdSet.back().isValid()) {
        throw Exception("loadCommands(): wrong file format. Unknown command");
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Save list of error messages as RescueHistory for current depletion stage
/// @param pModel Rescue model
/// @return true if model was changed and requires archiving, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpErrorMsgs(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  bool result = false;

  if (!m_errMsgs.empty()) {
    // Search for corresponded  RescueHistory object
    RescueHistory *curHistObj = NULL;

    RCHString signStr;
    signStr << "RGInterface errors list for " << m_dstage << " depletion stage";

    RESCUEINT64 histSize = pModel->HistoryCount64();
    for (RESCUEINT64 hi = 0; hi < histSize; ++hi) {
      RescueHistory *candidate = pModel->NthRescueHistory(hi);
      if (candidate && !strcmp(candidate->ChangeDescription()->NonNullString(), signStr.NonNullString())) {
        curHistObj = candidate;
        break;
      }
    }
    if (!curHistObj) // not added before
    {
      curHistObj = new RescueHistory(pModel.get(), signStr.NonNullString());
    }

    RCHString errList;
    for (std::vector<std::string>::const_iterator msg = m_errMsgs.begin(); msg != m_errMsgs.end(); ++msg) {
      errList << (*msg).c_str() << "\n";
    }
    curHistObj->SetParsableDescription(errList.NonNullString());
    result = true;
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load list of error messages as RescueHistory for current depletion stage
/// @param pModel Rescue model
/// @return true if model was changed and requires archiving, false otherwise
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadErrorMsgs(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Search for corresponded  RescueHistory object
  RescueHistory *curHistObj = NULL;

  RCHString signStr;
  signStr << "RGInterface errors list for " << m_dstage << " depletion stage";

  RESCUEINT64 histSize = pModel->HistoryCount64();
  for (RESCUEINT64 hi = 0; hi < histSize; ++hi) {
    RescueHistory *candidate = pModel->NthRescueHistory(hi);
    if (candidate && !strcmp(candidate->ChangeDescription()->NonNullString(), signStr.NonNullString())) {
      curHistObj = candidate;
      break;
    }
  }
  if (curHistObj) {
    m_errMsgs.clear();
    RCHString *msgLstStr = curHistObj->ParsableDescription();
    msgLstStr->tokenize("\n");
    RCHString lineAtATime;
    while ((*msgLstStr) >> lineAtATime) {
      m_errMsgs.push_back(lineAtATime.NonNullString());
    }
  }
}

static const char FormationLookupID[] = "FormationNames";
///////////////////////////////////////////////////////////////////////////////
/// @brief Save list of formations names as RescueLookup object
/// @param pModel Rescue model
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpFormations(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  bool result = false;

  if (!m_formationMap.empty()) {
    // Search for corresponded  RescueLookup object
    RescueLookup *formationTable = pModel->LookupNamed(const_cast<char *>(FormationLookupID));

    if (formationTable) {
      if (m_formationMap.size() != formationTable->Count64()) {
        pModel->DropRescueLookup(formationTable);
        formationTable = NULL;
      } else // table has the same size, compare items then
      {
        bool theSame = true;
        for (RESCUEINT64 i = 0; i < formationTable->Count64(); ++i) {
          RescueLookupString *lkStr = static_cast<RescueLookupString *>(formationTable->NthItem(i));
          if (!lkStr) // not the same number of formation names, obviously
          {
            theSame = false;
            break;
          }

          std::string val(lkStr->Translation()->NonNullString());
          size_t pos_ = val.find("_");
          if (std::string::npos == pos_) {
            theSame = false;
            break;
          }
          int formId;
          std::istringstream(val.substr(0, pos_)) >> formId; // convert string to number
          // no such ID or different Name
          if (!m_formationMap.count(formId) || m_formationMap.find(formId)->second != val.substr(pos_ + 1)) {
            theSame = false;
            break;
          }
        }
        if (!theSame) {
          pModel->DropRescueLookup(formationTable);
          formationTable = NULL;
        }
      }
    }

    // if there is no table of formation names or in Rescue table is differ from RGInterface table, recreate such table
    if (!formationTable) {
      formationTable = new RescueLookup(const_cast<char *>(FormationLookupID), m_formationMap.size(), pModel.get());
      int i = 0;
      for (std::map<int, std::string>::const_iterator it = m_formationMap.begin(); it != m_formationMap.end(); ++it) {
        std::ostringstream oss;
        oss << it->first << "_" << it->second;
        formationTable->SetNthItem(i, new RescueLookupString(pModel->Context(), const_cast<char *>(oss.str().c_str())));
        ++i;
      }
      result = true;
    }
  } else {
    // Search for corresponded  RescueLookup object
    RescueLookup *formationTable = pModel->LookupNamed(const_cast<char *>(FormationLookupID));
    if (formationTable) {
      pModel->DropRescueLookup(formationTable);
      result = true;
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Load list of formations names as RescueLookup
/// @param pModel Rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadFormations(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  m_formationMap.clear();
  // Search for corresponded  RescueLookup object
  RescueLookup *formationTable = pModel->LookupNamed(const_cast<char *>(FormationLookupID));
  if (formationTable) {
    for (RESCUEINT64 i = 0; i < formationTable->Count64(); ++i) {
      RescueLookupString *lkStr = static_cast<RescueLookupString *>(formationTable->NthItem(i));
      if (!lkStr)
        continue;

      std::string val(lkStr->Translation()->NonNullString());
      size_t pos_ = val.find("_");
      if (std::string::npos == pos_)
        continue;

      int formId;
      std::istringstream(val.substr(0, pos_)) >> formId; // convert string to number
      m_formationMap.insert(std::pair<int, std::string>(formId, val.substr(pos_ + 1)));
    }
  }

  // Add the whole grid formation name
  m_formationMap.insert(std::pair<int, std::string>(-1, std::string("WholeGrid")));
}

static const char SurfaceNameLookupID[] = "SurfaceNames";
static const char SurfaceTypesLookupID[] = "SurfaceTypes";
static const char SurfaceAttributesLookupID[] = "SurfaceAttributes";
static const char SurfacePropertyName[] = "Surface_0";
///////////////////////////////////////////////////////////////////////////////
/// @brief write all surfaces and polygons that compose them into rescue file, and list the surfaces in a Rescue lookup
/// table
/// @param pModel rescue model
/// @return true if model was updated, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpSurfaces(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());
  bool retVal = false;

  size_t nSurfaces = getNumHorizons() + getNumFaults() * 2;
  size_t nPolygonsMax = m_polygons.size();

  RescueGeometry *pGrid = RGUtils::getRescueGridByProperty(pModel, SurfacePropertyName);
  if (nSurfaces) {
    // Delete existing grid
    if (pGrid) {
      pModel->DropGeometry(pGrid);
      pGrid = NULL;
    }
    if (!pGrid) {
      retVal = true;

      RescueGrid *pSurfaceGrid =
          new RescueGrid(pModel->Context(), RescueCoordinateSystem::LUF, 0, nPolygonsMax + 2, 0, 2, 0, 1);
      pGrid = new RescueGeometry(pModel.get(), pSurfaceGrid, RGUtils::nullInteger());
      pModel->AddGeometry(pGrid);

      assert(pModel->GeometryCount() >= 2);

      // Store the names && properties && attributes
      RescueLookup *nameTable = new RescueLookup(const_cast<char *>(SurfaceNameLookupID), nSurfaces, pModel.get());
      RescueLookup *attributeTable =
          new RescueLookup(const_cast<char *>(SurfaceAttributesLookupID), nSurfaces, pModel.get());
      RescueLookup *typeTable = new RescueLookup(const_cast<char *>(SurfaceTypesLookupID), nSurfaces, pModel.get());

      size_t surfaceId = 0;

      for (size_t iType = 0; iType < RGSurface::NumTypes; ++iType) {
        for (std::vector<RGSurface>::const_iterator it = m_surfaces[iType].begin();
             it < m_surfaces[iType].end() && retVal; ++it) {
          // Each surface content is stored in a table labelled by its surfaceId
          retVal = writeSurfaceContent(pGrid, surfaceId, *it);

          nameTable->SetNthItem(surfaceId,
                                new RescueLookupString(pModel->Context(), const_cast<char *>(it->getName().c_str())));

          // Note that a RescueLookup table can contain either strings or array of float.
          // Thus enums, like attribute or type, are stored in string form
          attributeTable->SetNthItem(
              surfaceId, new RescueLookupString(pModel->Context(),
                                                const_cast<char *>(RGSurface::toString(it->getAttribute()).c_str())));
          typeTable->SetNthItem(surfaceId,
                                new RescueLookupString(pModel->Context(),
                                                       const_cast<char *>(RGSurface::toString(it->getType()).c_str())));

          ++surfaceId;
        }
      }
    }
    if (retVal) {
      retVal = dumpPolygons(pModel);
    }
  } else if (pGrid) {
    pModel->DropGeometry(pGrid);
    retVal = true;
  }

  return retVal;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief load surfaces listed in rescue file
/// @param pModel rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadSurfaces(std::unique_ptr<RescueModel> &pModel) {
  m_polygons.clear();
  for (size_t iType = 0; iType < RGSurface::NumTypes; ++iType) {
    m_surfaces[iType].clear();
  }

  RescueGeometry *pGrid = RGUtils::getRescueGridByProperty(pModel, SurfacePropertyName);
  if (pGrid) {
    RescueLookup *nameTable = pModel->LookupNamed(const_cast<char *>(SurfaceNameLookupID));
    RescueLookup *typeTable = pModel->LookupNamed(const_cast<char *>(SurfaceTypesLookupID));
    RescueLookup *attributeTable = pModel->LookupNamed(const_cast<char *>(SurfaceAttributesLookupID));

    if (nameTable && typeTable && attributeTable) {
      assert(nameTable->Count64() == typeTable->Count64());
      assert(nameTable->Count64() == attributeTable->Count64());

      for (RESCUEINT64 surfaceId = 0; surfaceId < nameTable->Count64(); ++surfaceId) {
        RescueLookupString *name = static_cast<RescueLookupString *>(nameTable->NthItem(surfaceId));
        RescueLookupString *sType = static_cast<RescueLookupString *>(typeTable->NthItem(surfaceId));
        RescueLookupString *sAttribute = static_cast<RescueLookupString *>(attributeTable->NthItem(surfaceId));

        RGSurface::Type type = RGSurface::typeFromString(sType->Translation()->NonNullString());
        RGSurface::Attribute attribute = RGSurface::attributeFromString(sAttribute->Translation()->NonNullString());
        assert(type != RGSurface::UndefinedType);

        RGSurface surface(name->Translation()->NonNullString(), type, attribute);

        // Retrieve the table containing its list of polygons
        readSurfaceContent(pGrid, surfaceId, surface);

        m_surfaces[type].push_back(surface);
      }
    }

    // Load the polygon list
    loadPolygons(pModel);
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief write a surface with a specific name into rescue file
/// @param pSurfaceDataContainer the rescue container which contains the surfaces
/// @param surfaceName the surface name
/// @param surface to be written
/// @return true if model was updated, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::writeSurfaceContent(RescueGeometry *pGrid, size_t surfaceId,
                                                       const RGSurface &surface) const {
  bool isOK = false;
  // Store polygons ID for each surface
  std::ostringstream sSurfaceId;
  sSurfaceId << "Surface_" << surfaceId;
  RescueProperty *pSurfaceProp = new RescueProperty(pGrid, const_cast<char *>(sSurfaceId.str().c_str()),
                                                    "General Discrete", "", RGUtils::nullInteger());
  if (pSurfaceProp) {
    isOK = true;
    size_t surfaceSize = surface.getSize(); // Surface effective size
    size_t maxSize = m_polygons.size() + 1; // Stored array size

    RescueArrayInt *pSurfacePolygonsIdArray = static_cast<RescueArrayInt *>(pSurfaceProp->Data());
    RESCUEINT32 *surfacePolygonIds = new RESCUEINT32[maxSize];

    surfacePolygonIds[0] = surfaceSize;
    for (size_t i = 0; i < surfaceSize; ++i) {
      surfacePolygonIds[i + 1] = surface.getPolygonN(i);
    }
    for (size_t i = surfaceSize + 1; i < maxSize; ++i) {
      surfacePolygonIds[i] = RGUtils::nullInteger();
    }

    pSurfacePolygonsIdArray->AcceptValue(RGUtils::nullInteger(), surfacePolygonIds);
  }
  return isOK;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief read a surface with a specific name from the rescue file
/// @param pSurfaceDataContainer the rescue container which contains the surfaces
/// @param surfaceName the surface name
/// @param surface
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::readSurfaceContent(RescueGeometry *pGrid, size_t surfaceId, RGSurface &surface) {
  bool isOK = false;

  std::ostringstream sSurfaceId;
  sSurfaceId << "Surface_" << surfaceId;
  RescueProperty *pSurfaceProp = pGrid->PropertyNamed(const_cast<char *>(sSurfaceId.str().c_str()));
  if (pSurfaceProp) {
    if (!pSurfaceProp->Data()->IsLoaded())
      pSurfaceProp->Data()->Load();
    if (pSurfaceProp->Data()->IsLoaded()) {
      isOK = true;
      RescueArrayInt *pSurfaceArray = static_cast<RescueArrayInt *>(pSurfaceProp->Data());
      assert(pSurfaceArray);

      size_t surfaceSize = pSurfaceArray->Value()[0]; // First value is the number of polygon for this surface
      for (size_t i = 0; i < surfaceSize; ++i) {
        surface.addPolygon(pSurfaceArray->Value()[i + 1]);
      }
    }
  }
  // if something wrong with loading issue error
  if (!isOK) {
    throw Exception("readSurface(): Can't read a surface from Rescue");
  }
}

static const char PolygonsPropertyName[] = "PolygonsNodes";
///////////////////////////////////////////////////////////////////////////////
/// @brief write the list of polygons into rescue file
/// @param pModel rescue model
/// @return true if model was updated, false otherwise
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::dumpPolygons(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  bool retVal = false;

  RescueGeometry *pGrid = RGUtils::getRescueGridByProperty(pModel, PolygonsPropertyName);
  if (m_polygons.size()) {
    // check is something was changed?
    if (pGrid) {
      // here we check equality just by comparing size, because there is no
      // any RGInterface methods to replace or delete support element! They can
      // be only added.
      if (pGrid->Grid() && pGrid->Grid()->NodeCount() != (m_polygons.size() + 1)) {
        pModel->DropGeometry(pGrid);
        pGrid = NULL;
      }
    }
    if (!pGrid) {
      RescueGrid *pPolygonGrid = new RescueGrid(pModel->Context(), RescueCoordinateSystem::LUF, 0,
                                                m_polygons.size() + 1, 0, m_nbPolygonCorner + 1, 0, 1);
      pGrid = new RescueGeometry(pModel.get(), pPolygonGrid, RGUtils::nullInteger());
      pModel->AddGeometry(pGrid);

      assert(pModel->GeometryCount() >= 2);

      RescueProperty *pPolyProp = new RescueProperty(pGrid, PolygonsPropertyName, const_cast<char *>("General"),
                                                     const_cast<char *>(""), RGUtils::nullInteger());
      RescueArrayInt *pPolyArray = static_cast<RescueArrayInt *>(pPolyProp->Data());
      assert(pPolyArray);

      RESCUEINT32 *polygons = new RESCUEINT32[m_polygons.size() * m_nbPolygonCorner];
      for (size_t ipolygon = 0; ipolygon < m_polygons.size(); ++ipolygon) {
        RESCUEINT32 *current(&polygons[ipolygon * m_nbPolygonCorner]);
        for (size_t inode = 0; inode < m_polygons[ipolygon].size(); ++inode) // Now fill the polygon with nodesId
        {
          current[inode] = m_polygons[ipolygon][inode];
        }
        for (size_t inode = m_polygons[ipolygon].size(); inode < m_nbPolygonCorner;
             ++inode) // Other corners should have missing value
        {
          current[inode] = RGUtils::nullInteger();
        }
      }
      pPolyArray->AcceptValue(RGUtils::nullInteger(), polygons);
      retVal = true;
    }
  } else if (pGrid) {
    pModel->DropGeometry(pGrid);
    retVal = true;
  }

  return retVal;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief load the polygon list used by the surfaces from the rescue file
/// @param pModel rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadPolygons(std::unique_ptr<RescueModel> &pModel) {
  m_polygons.clear();
  bool isOK = true;

  RescueGeometry *pGrid = RGUtils::getRescueGridByProperty(pModel, PolygonsPropertyName);
  if (pGrid) {
    RescueProperty *pPolyProp = pGrid->PropertyNamed(const_cast<char *>(PolygonsPropertyName));

    if (pPolyProp) {
      if (!pPolyProp->Data()->IsLoaded())
        pPolyProp->Data()->Load();

      if (pPolyProp->Data()->IsLoaded()) {
        RescueArrayInt *pPolyArray = static_cast<RescueArrayInt *>(pPolyProp->Data());
        assert(pPolyArray);

        size_t polySize = pPolyArray->ValueLength64() / m_nbPolygonCorner;
        for (size_t ipolygon = 0; ipolygon < polySize; ++ipolygon) {
          RGPolygon polygon;
          int *current = &pPolyArray->Value()[ipolygon * m_nbPolygonCorner];
          for (size_t inode = 0; inode < m_nbPolygonCorner && current[inode] != RGUtils::nullInteger(); ++inode) {
            polygon.push_back(current[inode]);
          }
          m_polygons.push_back(polygon);
        }
      } else {
        isOK = false;
      }
    }
  }

  // if something wrong with loading issue error
  if (!isOK) {
    throw Exception("loadPolygons(): Can't load polygons from Rescue");
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Look for corresponded time step group in Rescue file, and create new one if it wasn't found
/// @param pModel Rescue model
/// @param depStage time step info
/// @return pointer to found timestep group or new created time step group
///////////////////////////////////////////////////////////////////////////////
RescueTimeStepGroup *RGInterface::RGInterfaceImpl::getTimeStepGroup(std::unique_ptr<RescueModel> &pModel,
                                                                    const RGDepletionStage &depStage,
                                                                    bool &fileChanged) const {
  assert(depStage.isValid());

  fileChanged = true;

  RescuePropertyGroup *pPropertyGroup = pModel->NthRescuePropertyGroup(0);
  if (pPropertyGroup) {
    RESCUEINT32 tgNum = pPropertyGroup->TimeStepCount();

    for (RESCUEINT32 tg = 0; tg < tgNum; ++tg) {
      RescueTimeStepGroup *pTimeStepGroup = pPropertyGroup->NthTimeStepGroup(tg);
      if (RGDepletionStage::fromString(pTimeStepGroup->TimeStepName()->NonNullString()) == depStage) {
        fileChanged = false;
        return pTimeStepGroup;
      }
    }
  } else {
    pPropertyGroup = RescuePropertyGroup::MakeRescuePropertyGroup("Default Property Group", pModel.get());
    assert(pPropertyGroup);
  }
  return new RescueTimeStepGroup(depStage.toString().c_str(), pPropertyGroup);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Mark the commands of this model as completed
///        used by Geomec to confirm the commands had all been completed without error
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::commandsCompleted() { m_cmdSet.clear(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Indicate if the commands had been completed
///////////////////////////////////////////////////////////////////////////////
bool RGInterface::RGInterfaceImpl::isCommandsCompleted() const { return m_cmdSet.empty(); }

///////////////////////////////////////////////////////////////////////////////
/// @brief Get available failure mechanisms
/// @return A vector containing the failure mechanisms
///////////////////////////////////////////////////////////////////////////////
const std::vector<RGFailureMechanism> &RGInterface::RGInterfaceImpl::getAvailableFailureMechanismsInModel() const {
  return m_failureMechanisms;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set available failure mechanisms
/// @param failureMechanisms vector containing available failure mechanism objects
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setAvailableFailureMechanismsInModel(
    const std::vector<RGFailureMechanism> &failureMechanisms) {
  // we wipe
  m_failureMechanisms.clear();
  // we copy. the RGInterface implementation has to own
  // its own copy of the failure mechanisms vector to avoid
  // a situation where the that vector is cleared
  // before the rescue file is written (dumped)
  m_failureMechanisms = failureMechanisms;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Get available lsf evaluations
/// @return lsfEvauations vector containing available limit state functions
///////////////////////////////////////////////////////////////////////////////
const std::vector<RGLimitStateFunctionEvaluate> &RGInterface::RGInterfaceImpl::getLSFEvaluationsInModel() const {
  return m_limitStateFunctions;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Set available LSF evaluations
/// @param limitStateFunctions vector containing available limit state functions
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::setLSFEvaluationsInModel(
    const std::vector<RGLimitStateFunctionEvaluate> &limitStateFunctions) {
  // we wipe
  m_limitStateFunctions.clear();
  // we copy. the RGInterface implementation has to own
  // its own copy of the LSF vector to avoid
  // a situation where the that vector is cleared
  // before the rescue file is written (dumped)
  m_limitStateFunctions = limitStateFunctions;
}

// Reliability analysis table names for failure mechanisms
static const std::string FailureMechanismObjectNameTable = "FailureMechanismObjectNameTable";
static const std::string FailureMechanismObjectTable = "FailureMechanismObjectTable";
static const std::string FailureMechanismFailureTypesTable = "FailureMechanismFailureTypesTable";
static const std::string FailureMechanismLsfTypeTable = "FailureMechanismLsfTypeTable";
static const std::string FailureMechanismValueTable = "FailureMechanismValueTable";

// Reliability analysis table names for LSF evaluations
static const std::string LimitStateFunctionEvalObjectNameTable = "LimitStateFunctionEvalObjectNameTable";
static const std::string LimitStateFunctionEvalObjectTable = "LimitStateFunctionEvalObject";
static const std::string LimitStateFunctionEvalFailureTypeTable = "LimitStateFunctionEvalFailureTypeTable";
static const std::string LimitStateFunctionEvalLSFTypeTable = "LimitStateFunctionEvalLSFTypeTable";
static const std::string LimitStateFunctionEvalValueTable = "LimitStateFunctionEvalValueTable";
static const std::string LimitStateFunctionEvalDepletionStageTable = "LimitStateFunctionEvalDepletionStageTable";
static const std::string LimitStateFunctionEvalThresholdTable = "LimitStateFunctionEvalThresholdTable";
static const std::string LimitStateFunctionEvalLSFValueTable = "LimitStateFunctionEvalLSFValueTable";

///////////////////////////////////////////////////////////////////////////////
/// @brief load available failure mechanisms
/// @param pModel the rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadAvailableFailureMechanismsInModel(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Lookup the tables
  RescueLookup *failureMechanismObjectNameTable =
      pModel->LookupNamed(const_cast<char *>(FailureMechanismObjectNameTable.c_str()));
  RescueLookup *failureMechanismObjectTable =
      pModel->LookupNamed(const_cast<char *>(FailureMechanismObjectTable.c_str()));
  RescueLookup *failureMechanismFailureTypesTable =
      pModel->LookupNamed(const_cast<char *>(FailureMechanismFailureTypesTable.c_str()));
  RescueLookup *failureMechanismLsfTypeTable =
      pModel->LookupNamed(const_cast<char *>(FailureMechanismLsfTypeTable.c_str()));
  RescueLookup *failureMechanismValueTable =
      pModel->LookupNamed(const_cast<char *>(FailureMechanismValueTable.c_str()));

  if (failureMechanismObjectNameTable->Count64() == 0)
    return;

  // confirm the tables are consistent
  assert(failureMechanismObjectNameTable->Count64() > 0);
  assert(failureMechanismObjectNameTable->Count64() == failureMechanismObjectTable->Count64());
  assert(failureMechanismObjectNameTable->Count64() == failureMechanismFailureTypesTable->Count64());
  assert(failureMechanismObjectNameTable->Count64() == failureMechanismLsfTypeTable->Count64());
  assert(failureMechanismObjectNameTable->Count64() == failureMechanismValueTable->Count64());

  m_failureMechanisms.clear();

  for (size_t i = 0; i < failureMechanismObjectNameTable->Count64(); i++) {
    // add the new failure mechanism element
    m_failureMechanisms.push_back(RGFailureMechanism());

    RescueLookupString *objName = static_cast<RescueLookupString *>(failureMechanismObjectNameTable->NthItem(i));
    RescueLookupString *obj = static_cast<RescueLookupString *>(failureMechanismObjectTable->NthItem(i));
    RescueLookupString *failureType = static_cast<RescueLookupString *>(failureMechanismFailureTypesTable->NthItem(i));
    RescueLookupString *lsfType = static_cast<RescueLookupString *>(failureMechanismLsfTypeTable->NthItem(i));
    RescueLookupTable *value = static_cast<RescueLookupTable *>(failureMechanismValueTable->NthItem(i));

    const std::string objNameStr(objName->Translation()->NonNullString());
    m_failureMechanisms[i].objectName = objNameStr;

    const std::string objStr(obj->Translation()->NonNullString());
    m_failureMechanisms[i].object = FailureMechanismObjectFromString(objStr);

    const std::string failureTypeStr(failureType->Translation()->NonNullString());
    m_failureMechanisms[i].failureType = FailureMechanismFailureTypeFromString(failureTypeStr);

    const std::string lsfTypeStr(lsfType->Translation()->NonNullString());
    m_failureMechanisms[i].lsfType = RGFailureMechanismLsfTypeFromString(lsfTypeStr);

    // first clear the failure mechanism values
    m_failureMechanisms[i].value.clear();
    for (int j = 0; j < value->Rows(); ++j) {
      m_failureMechanisms[i].value.push_back(value->Translation()[j]);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief write available failure mechanisms
/// @param pModel  the rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::dumpAvailableFailureMechanismsInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  // create the tables
  const int num_available_failure_mechanisms = m_failureMechanisms.size();
  RescueLookup *failureMechanismObjectNameTable = new RescueLookup(
      const_cast<char *>(FailureMechanismObjectNameTable.c_str()), num_available_failure_mechanisms, pModel.get());
  RescueLookup *failureMechanismObjectTable = new RescueLookup(const_cast<char *>(FailureMechanismObjectTable.c_str()),
                                                               num_available_failure_mechanisms, pModel.get());
  RescueLookup *failureMechanismFailureTypesTable = new RescueLookup(
      const_cast<char *>(FailureMechanismFailureTypesTable.c_str()), num_available_failure_mechanisms, pModel.get());
  RescueLookup *failureMechanismLsfTypeTable = new RescueLookup(
      const_cast<char *>(FailureMechanismLsfTypeTable.c_str()), num_available_failure_mechanisms, pModel.get());
  RescueLookup *failureMechanismValueTable = new RescueLookup(const_cast<char *>(FailureMechanismValueTable.c_str()),
                                                              num_available_failure_mechanisms, pModel.get());

  // iterate over all the failure mechanism objects and save them
  for (int i = 0; i < num_available_failure_mechanisms; i++) {
    // add the object name
    failureMechanismObjectNameTable->SetNthItem(
        i, new RescueLookupString(pModel->Context(), const_cast<char *>(m_failureMechanisms[i].objectName.c_str())));
    // add the object type
    failureMechanismObjectTable->SetNthItem(
        i, new RescueLookupString(
               pModel->Context(),
               const_cast<char *>(FailureMechanismObjectToString(m_failureMechanisms[i].object).c_str())));
    // add the failure type
    failureMechanismFailureTypesTable->SetNthItem(
        i, new RescueLookupString(
               pModel->Context(),
               const_cast<char *>(FailureMechanismFailureTypeToString(m_failureMechanisms[i].failureType).c_str())));
    // add the lsf type
    failureMechanismLsfTypeTable->SetNthItem(
        i, new RescueLookupString(
               pModel->Context(),
               const_cast<char *>(FailureMechanismLsfTypeToString(m_failureMechanisms[i].lsfType).c_str())));

    // add the values. Note that rescue lookup tables are function values
    // so the storage scheme is meant for x-y pairs as rows. We therefore have to
    // allocate 2 * number of rows
    RESCUEFLOAT *valPtr = new RESCUEFLOAT[2 * m_failureMechanisms[i].value.size()];
    for (int j = 0; j < m_failureMechanisms[i].value.size(); ++j) {
      valPtr[j] = m_failureMechanisms[i].value[j];
    }

    failureMechanismValueTable->SetNthItem(
        i, new RescueLookupTable(pModel->Context(), m_failureMechanisms[i].value.size(), valPtr));
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief load the limit state functions
/// @param pModel the rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::loadLSFEvaluationsInModel(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Lookup the tables
  RescueLookup *limitStateFunctionEvalObjectNameTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalObjectNameTable.c_str()));
  RescueLookup *limitStateFunctionEvalObjectTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalObjectTable.c_str()));
  RescueLookup *limitStateFunctionEvalFailureTypeTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalFailureTypeTable.c_str()));
  RescueLookup *limitStateFunctionEvalLSFTypeTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalLSFTypeTable.c_str()));
  RescueLookup *limitStateFunctionEvalValueTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalValueTable.c_str()));
  RescueLookup *limitStateFunctionEvalDepletionStageTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalDepletionStageTable.c_str()));
  RescueLookup *limitStateFunctionEvalThresholdTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalThresholdTable.c_str()));
  RescueLookup *limitStateFunctionEvalLSFValueTable =
      pModel->LookupNamed(const_cast<char *>(LimitStateFunctionEvalLSFValueTable.c_str()));

  if (limitStateFunctionEvalObjectNameTable->Count64() == 0)
    return;

  // confirm the tables are consistent
  assert(limitStateFunctionEvalObjectNameTable->Count64() > 0);
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalObjectTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalFailureTypeTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalLSFTypeTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalDepletionStageTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalValueTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalThresholdTable->Count64());
  assert(limitStateFunctionEvalObjectNameTable->Count64() == limitStateFunctionEvalLSFValueTable->Count64());

  // Clear the lSF's
  m_limitStateFunctions.clear();

  for (int i = 0; i < limitStateFunctionEvalObjectNameTable->Count64(); i++) {
    // Create the failure mechanism associated with the LSF evaluation
    m_limitStateFunctions.push_back(RGLimitStateFunctionEvaluate());

    RescueLookupString *objName = static_cast<RescueLookupString *>(limitStateFunctionEvalObjectNameTable->NthItem(i));
    RescueLookupString *obj = static_cast<RescueLookupString *>(limitStateFunctionEvalObjectTable->NthItem(i));
    RescueLookupString *failureType =
        static_cast<RescueLookupString *>(limitStateFunctionEvalFailureTypeTable->NthItem(i));
    RescueLookupString *lsfType = static_cast<RescueLookupString *>(limitStateFunctionEvalLSFTypeTable->NthItem(i));
    RescueLookupTable *value = static_cast<RescueLookupTable *>(limitStateFunctionEvalValueTable->NthItem(i));
    RescueLookupString *depletionStage =
        static_cast<RescueLookupString *>(limitStateFunctionEvalDepletionStageTable->NthItem(i));
    RescueLookupTable *threshold = static_cast<RescueLookupTable *>(limitStateFunctionEvalThresholdTable->NthItem(i));
    RescueLookupTable *lsfValue = static_cast<RescueLookupTable *>(limitStateFunctionEvalLSFValueTable->NthItem(i));

    const std::string objNameStr(objName->Translation()->NonNullString());
    m_limitStateFunctions[i].failureMechanism->objectName = objNameStr;

    const std::string objStr(obj->Translation()->NonNullString());
    m_limitStateFunctions[i].failureMechanism->object = FailureMechanismObjectFromString(objStr);

    const std::string failureTypeStr(failureType->Translation()->NonNullString());
    m_limitStateFunctions[i].failureMechanism->failureType = FailureMechanismFailureTypeFromString(failureTypeStr);

    const std::string lsfTypeStr(lsfType->Translation()->NonNullString());
    m_limitStateFunctions[i].failureMechanism->lsfType = RGFailureMechanismLsfTypeFromString(lsfTypeStr);

    // first clear the failure mechanism values
    m_limitStateFunctions[i].failureMechanism->value.clear();
    for (int j = 0; j < value->Rows(); ++j) {
      m_limitStateFunctions[i].failureMechanism->value.push_back(value->Translation()[j]);
    }

    const std::string depletionStageStr(depletionStage->Translation()->NonNullString());
#ifndef _WIN32
    std::istringstream(depletionStageStr) >> m_limitStateFunctions[i].depletionStage;
#else
    m_limitStateFunctions[i].depletionStage = std::stoi(depletionStageStr);
#endif

    m_limitStateFunctions[i].threshold = threshold->Translation()[0];

    m_limitStateFunctions[i].limitStateFunctionValue = lsfValue->Translation()[0];
  }
}

///////////////////////////////////////////////////////////////////////////////
/// @brief write LSF evaluations
/// @param pModel the rescue model
///////////////////////////////////////////////////////////////////////////////
void RGInterface::RGInterfaceImpl::dumpLSFEvaluationsInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  // create the tables
  const int numlsf_evaluations = m_limitStateFunctions.size();
  RescueLookup *limitStateFunctionEvalObjectNameTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalObjectNameTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalObjectTable =
      new RescueLookup(const_cast<char *>(LimitStateFunctionEvalObjectTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalFailureTypeTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalFailureTypeTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalLSFTypeTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalLSFTypeTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalValueTable =
      new RescueLookup(const_cast<char *>(LimitStateFunctionEvalValueTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalDepletionStageTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalDepletionStageTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalThresholdTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalThresholdTable.c_str()), numlsf_evaluations, pModel.get());
  RescueLookup *limitStateFunctionEvalLSFValueTable = new RescueLookup(
      const_cast<char *>(LimitStateFunctionEvalLSFValueTable.c_str()), numlsf_evaluations, pModel.get());

  // iterate over all the failure mechanism objects and save them
  for (int i = 0; i < numlsf_evaluations; i++) {
    // add the object name
    limitStateFunctionEvalObjectNameTable->SetNthItem(
        i, new RescueLookupString(pModel->Context(),
                                  const_cast<char *>(m_limitStateFunctions[i].failureMechanism->objectName.c_str())));
    // add the object type
    limitStateFunctionEvalObjectTable->SetNthItem(
        i, new RescueLookupString(
               pModel->Context(),
               const_cast<char *>(
                   FailureMechanismObjectToString(m_limitStateFunctions[i].failureMechanism->object).c_str())));
    // add the failure type
    limitStateFunctionEvalFailureTypeTable->SetNthItem(
        i,
        new RescueLookupString(
            pModel->Context(),
            const_cast<char *>(
                FailureMechanismFailureTypeToString(m_limitStateFunctions[i].failureMechanism->failureType).c_str())));
    // add the lsf type
    limitStateFunctionEvalLSFTypeTable->SetNthItem(
        i, new RescueLookupString(
               pModel->Context(),
               const_cast<char *>(
                   FailureMechanismLsfTypeToString(m_limitStateFunctions[i].failureMechanism->lsfType).c_str())));

    // add the values. Note that rescue lookup tables are function values
    // so the storage scheme is meant for x-y pairs as rows. We therefore have to
    // allocate 2 * number of rows
    RESCUEFLOAT *valPtr = new RESCUEFLOAT[2 * m_limitStateFunctions[i].failureMechanism->value.size()];
    for (int j = 0; j < m_limitStateFunctions[i].failureMechanism->value.size(); ++j) {
      valPtr[j] = m_limitStateFunctions[i].failureMechanism->value[j];
    }

    limitStateFunctionEvalValueTable->SetNthItem(
        i, new RescueLookupTable(pModel->Context(), m_limitStateFunctions[i].failureMechanism->value.size(), valPtr));

    // add the depletion stage
#ifndef _WIN32
    std::ostringstream conv;
    conv << m_limitStateFunctions[i].depletionStage;

    limitStateFunctionEvalDepletionStageTable->SetNthItem(
        i, new RescueLookupString(pModel->Context(), const_cast<char *>(conv.str().c_str())));
#else
    limitStateFunctionEvalDepletionStageTable->SetNthItem(
        i, new RescueLookupString(pModel->Context(),
                                  const_cast<char *>(std::to_string(m_limitStateFunctions[i].depletionStage).c_str())));
#endif

    // add the threshold
    RESCUEFLOAT *thresholdPtr = new RESCUEFLOAT[2];
    thresholdPtr[0] = m_limitStateFunctions[i].threshold;
    thresholdPtr[1] = 0.0;
    limitStateFunctionEvalThresholdTable->SetNthItem(i, new RescueLookupTable(pModel->Context(), 1, thresholdPtr));

    // add the lsf value
    RESCUEFLOAT *lsfValuePtr = new RESCUEFLOAT[2];
    lsfValuePtr[0] = m_limitStateFunctions[i].limitStateFunctionValue;
    lsfValuePtr[1] = 0.0;
    limitStateFunctionEvalLSFValueTable->SetNthItem(i, new RescueLookupTable(pModel->Context(), 1, lsfValuePtr));
  }
}

std::vector<RGGeneralProperty> RGInterface::RGInterfaceImpl::GetAvailableMonitorableProperties() const {
  return m_monitorableProperties;
}

void RGInterface::RGInterfaceImpl::SetAvailableMonitorableProperties(const std::vector<RGGeneralProperty> &props) {
  // Let's clear the existing data
  m_monitorableProperties.clear();
  m_monitorableProperties = props;
}

RGMonitorPointSets RGInterface::RGInterfaceImpl::GetSelectedMonitoringPointSets() const { return m_monitorPointSets; }

void RGInterface::RGInterfaceImpl::SetSelectedMonitoringPointSets(const RGMonitorPointSets &pointSet) {
  // Let's clear the exisitng value
  m_monitorPointSets.clear();
  m_monitorPointSets = pointSet;
}

RGMonitorValues RGInterface::RGInterfaceImpl::GetMonitorValues() const { return m_monitorValues.at(m_dstage); }

void RGInterface::RGInterfaceImpl::SetMonitorValues(const RGMonitorValues &monitorValues) {
  if (m_dstage < 0)
    return;

  m_monitorValues[m_dstage].clear();
  m_monitorValues[m_dstage] = monitorValues;
}

// GEOMEC publishes list of properties that are available for monitoring,
// This is triggered by the command typeCommandListMonitorableProperties.
// Flow reads it

static const std::string MonitorablePropertyNameTable = "MonitorablePropertyNameTable";
static const std::string MonitorablePropertyQuantityTable = "MonitorablePropertyQuantityTable";
static const std::string MonitorablePropertySupportTable = "MonitorablePropertySupportTable";

void RGInterface::RGInterfaceImpl::loadAvailableMonitorableProperties(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Lookup the table
  RescueLookup *monitorPropertiesObjectPropertyNameTable = pModel->LookupNamed(MonitorablePropertyNameTable.c_str());
  RescueLookup *monitorPropertiesObjectQuantityTable = pModel->LookupNamed(MonitorablePropertyQuantityTable.c_str());
  RescueLookup *monitorPropertiesObjectSupportTable = pModel->LookupNamed(MonitorablePropertySupportTable.c_str());

  if (nullptr == monitorPropertiesObjectPropertyNameTable || monitorPropertiesObjectPropertyNameTable->Count64() == 0) {
    return;
  }

  assert(monitorPropertiesObjectPropertyNameTable->Count64() > 0);
  assert(monitorPropertiesObjectPropertyNameTable->Count64() == monitorPropertiesObjectQuantityTable->Count64());
  assert(monitorPropertiesObjectPropertyNameTable->Count64() == monitorPropertiesObjectSupportTable->Count64());

  m_monitorableProperties.clear();

  for (int propIdx = 0; propIdx < monitorPropertiesObjectPropertyNameTable->Count64(); propIdx++) {
    RescueLookupString *name =
        static_cast<RescueLookupString *>(monitorPropertiesObjectPropertyNameTable->NthItem(propIdx));
    RescueLookupString *quantity =
        static_cast<RescueLookupString *>(monitorPropertiesObjectQuantityTable->NthItem(propIdx));
    RescueLookupString *support =
        static_cast<RescueLookupString *>(monitorPropertiesObjectSupportTable->NthItem(propIdx));

    std::string propName(name->Translation()->NonNullString());
    std::string propQuant(quantity->Translation()->NonNullString());
    std::string propSupport(support->Translation()->NonNullString());

    m_monitorableProperties.emplace_back(propName, propQuant, RGSupport::toEnum(propSupport));
  }
}

void RGInterface::RGInterfaceImpl::dumpAvailableMonitorablePropertiesInModel(
    std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  {
    RescueLookup *monitorPropertiesObjectPropertyNameTable = pModel->LookupNamed(MonitorablePropertyNameTable.c_str());
    RescueLookup *monitorPropertiesObjectQuantityTable = pModel->LookupNamed(MonitorablePropertyQuantityTable.c_str());
    RescueLookup *monitorPropertiesObjectSupportTable = pModel->LookupNamed(MonitorablePropertySupportTable.c_str());

    if (monitorPropertiesObjectPropertyNameTable) {
      pModel->DropRescueLookup(monitorPropertiesObjectPropertyNameTable);
      pModel->DropRescueLookup(monitorPropertiesObjectQuantityTable);
      pModel->DropRescueLookup(monitorPropertiesObjectSupportTable);
    }
  }

  // create the tables
  const size_t num_available_monitorable_properties = m_monitorableProperties.size();

  if (num_available_monitorable_properties <= 0)
    return;

  RescueLookup *monitorPropertiesObjectPropertyNameTable =
      new RescueLookup(MonitorablePropertyNameTable.c_str(), num_available_monitorable_properties, pModel.get());
  RescueLookup *monitorPropertiesObjectQuantityTable =
      new RescueLookup(MonitorablePropertyQuantityTable.c_str(), num_available_monitorable_properties, pModel.get());
  RescueLookup *monitorPropertiesObjectSupportTable =
      new RescueLookup(MonitorablePropertySupportTable.c_str(), num_available_monitorable_properties, pModel.get());

  // iterate over all the Monitorable properties and save them
  for (int propIdx = 0; propIdx < num_available_monitorable_properties; propIdx++) {
    monitorPropertiesObjectPropertyNameTable->SetNthItem(
        propIdx, new RescueLookupString(pModel->Context(), m_monitorableProperties[propIdx].GetProperty().c_str()));
    monitorPropertiesObjectQuantityTable->SetNthItem(
        propIdx, new RescueLookupString(pModel->Context(), m_monitorableProperties[propIdx].GetQuantity().c_str()));
    monitorPropertiesObjectSupportTable->SetNthItem(
        propIdx, new RescueLookupString(pModel->Context(),
                                        RGSupport::toString(m_monitorableProperties[propIdx].GetSupport()).c_str()));
  }
}

static const std::string MonitorPointSetsTable = "MonitorPointSetsTable";
static const std::string MonitorPointSetsPointNameTable = "MonitorPointSetsPointNameTable";
static const std::string MonitorPointSetsPointNorthingTable = "MonitorPointSetsPointNorthingTable";
static const std::string MonitorPointSetsPointEastingTable = "MonitorPointSetsPointEastingTable";
static const std::string MonitorPointSetsPointDepthTable = "MonitorPointSetsPointDepthTable";
static const std::string MonitorPointSetsPropertyNameTable = "MonitorPointSetsPropertyNameTable";
static const std::string MonitorPointSetsPropertyQuantityTable = "MonitorPointSetsPropertyQuantityTable";
static const std::string MonitorPointSetsPropertySupportTable = "MonitorPointSetsPropertySupportTable";
static const std::string MonitorPointSetsPropertiesCountTable = "MonitorPointSetsPropertiesCountTable";

void RGInterface::RGInterfaceImpl::loadMonitorablePointSets(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  // Lookup the table
  RescueLookup *monitorPointSetsTable = pModel->LookupNamed(MonitorPointSetsTable.c_str());
  RescueLookup *monitorPointsObjectNameTable = pModel->LookupNamed(MonitorPointSetsPointNameTable.c_str());
  RescueLookup *monitorPointsObjectNorthingTable = pModel->LookupNamed(MonitorPointSetsPointNorthingTable.c_str());
  RescueLookup *monitorPointsObjectEastingTable = pModel->LookupNamed(MonitorPointSetsPointEastingTable.c_str());
  RescueLookup *monitorPointsObjectDepthTable = pModel->LookupNamed(MonitorPointSetsPointDepthTable.c_str());
  RescueLookup *monitorPropertyObjectNameTable = pModel->LookupNamed(MonitorPointSetsPropertyNameTable.c_str());
  RescueLookup *monitorPropertyObjectQuantityTable = pModel->LookupNamed(MonitorPointSetsPropertyQuantityTable.c_str());
  RescueLookup *monitorPropertyObjectSupportTable = pModel->LookupNamed(MonitorPointSetsPropertySupportTable.c_str());
  RescueLookup *monitorPointSetsPropertiesCountTable =
      pModel->LookupNamed(MonitorPointSetsPropertiesCountTable.c_str());

  if (monitorPointSetsTable == nullptr || monitorPointSetsTable->Count64() == 0) {
    return;
  }

  assert(monitorPointSetsTable->Count64() > 0);
  assert(monitorPointSetsTable->Count64() == monitorPointsObjectNameTable->Count64());
  assert(monitorPointSetsTable->Count64() == monitorPointsObjectNorthingTable->Count64());
  assert(monitorPointSetsTable->Count64() == monitorPointsObjectEastingTable->Count64());
  assert(monitorPointSetsTable->Count64() == monitorPointsObjectDepthTable->Count64());

  m_monitorPointSets.clear();

  int counter = 0;

  for (int pointSetIdx = 0; pointSetIdx < monitorPointSetsTable->Count64(); pointSetIdx++) {
    // Points
    RescueLookupString *namePointSet =
        static_cast<RescueLookupString *>(monitorPointsObjectNameTable->NthItem(pointSetIdx));
    RescueLookupTable *northing =
        static_cast<RescueLookupTable *>(monitorPointsObjectNorthingTable->NthItem(pointSetIdx));
    RescueLookupTable *easting =
        static_cast<RescueLookupTable *>(monitorPointsObjectEastingTable->NthItem(pointSetIdx));
    RescueLookupTable *depth = static_cast<RescueLookupTable *>(monitorPointsObjectDepthTable->NthItem(pointSetIdx));

    std::vector<double> northingVec;
    std::vector<double> eastingVec;
    std::vector<double> depthVec;
    for (int ptIndex = 0; ptIndex < northing->Rows(); ptIndex++) {
      northingVec.push_back(northing->Translation()[ptIndex]);
      eastingVec.push_back(easting->Translation()[ptIndex]);
      depthVec.push_back(depth->Translation()[ptIndex]);
    }

    const std::string strName(namePointSet->Translation()->NonNullString());

    RGPointSet pointSet(strName, northingVec, eastingVec, depthVec);

    // Props
    RescueLookupTable *pCount =
        static_cast<RescueLookupTable *>(monitorPointSetsPropertiesCountTable->NthItem(pointSetIdx));
    int props_count = pCount->Translation()[0];

    std::vector<RGGeneralProperty> props;

    for (int propIdx = 0; propIdx < props_count; propIdx++) {
      RescueLookupString *name = static_cast<RescueLookupString *>(monitorPropertyObjectNameTable->NthItem(counter));
      RescueLookupString *quant =
          static_cast<RescueLookupString *>(monitorPropertyObjectSupportTable->NthItem(counter));
      RescueLookupString *support =
          static_cast<RescueLookupString *>(monitorPropertyObjectQuantityTable->NthItem(counter));

      std::string pName = name->Translation()->NonNullString();
      std::string pQuant = quant->Translation()->NonNullString();
      std::string pSupport = support->Translation()->NonNullString();

      props.emplace_back(pName, pQuant, RGSupport::toEnum(pSupport));
      counter++;
    }

    m_monitorPointSets.Add(pointSet, props);
  }
}

void RGInterface::RGInterfaceImpl::dumpMonitorablePointSetsInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  {
    RescueLookup *monitorPointSetsTable = pModel->LookupNamed(MonitorPointSetsTable.c_str());
    RescueLookup *monitorPointsObjectNameTable = pModel->LookupNamed(MonitorPointSetsPointNameTable.c_str());
    RescueLookup *monitorPointsObjectNorthingTable = pModel->LookupNamed(MonitorPointSetsPointNorthingTable.c_str());
    RescueLookup *monitorPointsObjectEastingTable = pModel->LookupNamed(MonitorPointSetsPointEastingTable.c_str());
    RescueLookup *monitorPointsObjectDepthTable = pModel->LookupNamed(MonitorPointSetsPointDepthTable.c_str());
    RescueLookup *monitorPropertyObjectNameTable = pModel->LookupNamed(MonitorPointSetsPropertyNameTable.c_str());
    RescueLookup *monitorPropertyObjectQuantityTable =
        pModel->LookupNamed(MonitorPointSetsPropertyQuantityTable.c_str());
    RescueLookup *monitorPropertyObjectSupportTable = pModel->LookupNamed(MonitorPointSetsPropertySupportTable.c_str());
    RescueLookup *monitorPointSetsPropertiesCountTable =
        pModel->LookupNamed(MonitorPointSetsPropertiesCountTable.c_str());

    if (monitorPointsObjectNameTable) {
      pModel->DropRescueLookup(monitorPointSetsTable);
      pModel->DropRescueLookup(monitorPointsObjectNameTable);
      pModel->DropRescueLookup(monitorPointsObjectNorthingTable);
      pModel->DropRescueLookup(monitorPointsObjectEastingTable);
      pModel->DropRescueLookup(monitorPointsObjectDepthTable);
      pModel->DropRescueLookup(monitorPropertyObjectNameTable);
      pModel->DropRescueLookup(monitorPropertyObjectQuantityTable);
      pModel->DropRescueLookup(monitorPropertyObjectSupportTable);
      pModel->DropRescueLookup(monitorPointSetsPropertiesCountTable);
    }
  }

  // create the tables
  size_t num_available_monitorable_points_sets = m_monitorPointSets.GetNumPointSets();

  if (num_available_monitorable_points_sets <= 0)
    return;

  size_t totalCount = m_monitorPointSets.GetTotalCount();

  RescueLookup *monitorPointSetsTable =
      new RescueLookup(MonitorPointSetsTable.c_str(), num_available_monitorable_points_sets, pModel.get());
  RescueLookup *monitorPointsObjectNameTable =
      new RescueLookup(MonitorPointSetsPointNameTable.c_str(), num_available_monitorable_points_sets, pModel.get());
  RescueLookup *monitorPointsObjectNorthingTable =
      new RescueLookup(MonitorPointSetsPointNorthingTable.c_str(), num_available_monitorable_points_sets, pModel.get());
  RescueLookup *monitorPointsObjectEastingTable =
      new RescueLookup(MonitorPointSetsPointEastingTable.c_str(), num_available_monitorable_points_sets, pModel.get());
  RescueLookup *monitorPointsObjectDepthTable =
      new RescueLookup(MonitorPointSetsPointDepthTable.c_str(), num_available_monitorable_points_sets, pModel.get());
  RescueLookup *monitorPropertyObjectNameTable =
      new RescueLookup(MonitorPointSetsPropertyNameTable.c_str(), totalCount, pModel.get());
  RescueLookup *monitorPropertyObjectQuantityTable =
      new RescueLookup(MonitorPointSetsPropertyQuantityTable.c_str(), totalCount, pModel.get());
  RescueLookup *monitorPropertyObjectSupportTable =
      new RescueLookup(MonitorPointSetsPropertySupportTable.c_str(), totalCount, pModel.get());
  RescueLookup *monitorPointSetsPropertiesCountTable = new RescueLookup(
      MonitorPointSetsPropertiesCountTable.c_str(), num_available_monitorable_points_sets, pModel.get());

  RESCUEFLOAT *monitorPointSetCount = new RESCUEFLOAT[1];
  monitorPointSetCount[0] = num_available_monitorable_points_sets;
  monitorPointSetsTable->SetNthItem(0, new RescueLookupTable(pModel->Context(), 1, monitorPointSetCount));

  size_t counter = 0;

  for (int pointSetIdx = 0; pointSetIdx < num_available_monitorable_points_sets; pointSetIdx++) {
    const RGPointSet point = m_monitorPointSets.GetPointSet(pointSetIdx);
    size_t point_count = point.GetNumPoints();

    RESCUEFLOAT *northingPtr = new RESCUEFLOAT[point_count];
    RESCUEFLOAT *eastingPtr = new RESCUEFLOAT[point_count];
    RESCUEFLOAT *depthPtr = new RESCUEFLOAT[point_count];

    for (int pointIdx = 0; pointIdx < point_count; pointIdx++) {
      double n;
      double e;
      double d;

      point.GetNthPoint(pointIdx, n, e, d);
      northingPtr[pointIdx] = n;
      eastingPtr[pointIdx] = e;
      depthPtr[pointIdx] = d;
    }

    const std::vector<RGGeneralProperty> props = m_monitorPointSets.GetProperties(pointSetIdx);
    const size_t props_count = props.size();

    RESCUEFLOAT *propsCount = new RESCUEFLOAT[1];
    propsCount[0] = props_count;

    for (int propsIdx = 0; propsIdx < props_count; propsIdx++) {
      monitorPropertyObjectNameTable->SetNthItem(
          counter, new RescueLookupString(pModel->Context(), props[propsIdx].GetProperty().c_str()));
      monitorPropertyObjectSupportTable->SetNthItem(
          counter, new RescueLookupString(pModel->Context(), props[propsIdx].GetQuantity().c_str()));
      monitorPropertyObjectQuantityTable->SetNthItem(
          counter,
          new RescueLookupString(pModel->Context(), RGSupport::toString(props[propsIdx].GetSupport()).c_str()));
      counter++;
    }

    monitorPointSetsPropertiesCountTable->SetNthItem(pointSetIdx,
                                                     new RescueLookupTable(pModel->Context(), props_count, propsCount));
    monitorPointsObjectNameTable->SetNthItem(pointSetIdx,
                                             new RescueLookupString(pModel->Context(), point.GetName().c_str()));
    monitorPointsObjectNorthingTable->SetNthItem(pointSetIdx,
                                                 new RescueLookupTable(pModel->Context(), point_count, northingPtr));
    monitorPointsObjectEastingTable->SetNthItem(pointSetIdx,
                                                new RescueLookupTable(pModel->Context(), point_count, eastingPtr));
    monitorPointsObjectDepthTable->SetNthItem(pointSetIdx,
                                              new RescueLookupTable(pModel->Context(), point_count, depthPtr));
  }
}

static const std::string MonitorablePropertyValueTable = "MonitorablePropertyValueTable";

void RGInterface::RGInterfaceImpl::loadMonitorableValues(std::unique_ptr<RescueModel> &pModel) {
  assert(pModel.get());

  if (m_dstage < 0)
    return;

  RescueLookup *monitorablePropertyValueTable = pModel->LookupNamed(MonitorablePropertyValueTable.c_str());

  if (!monitorablePropertyValueTable || monitorablePropertyValueTable->Count64() == 0)
    return;

  size_t pointSetCount = m_monitorPointSets.GetNumPointSets();

  int counter = 0;

  m_monitorValues[m_dstage] = RGMonitorValues(getCurrentDepletionStage(), m_monitorPointSets);

  // There is no need to Send Pointset to GEOMEC

  for (int pointSetIdx = 0; pointSetIdx < pointSetCount; pointSetIdx++) {
    size_t pointCount = m_monitorPointSets.GetPointSet(pointSetIdx).GetNumPoints();
    size_t propertyCount = m_monitorPointSets.GetProperties(pointSetIdx).size();

    for (int pointIdx = 0; pointIdx < pointCount; pointIdx++) {
      for (int propertyIdx = 0; propertyIdx < propertyCount; propertyIdx++) {
        RescueLookupTable *propValuePtr =
            static_cast<RescueLookupTable *>(monitorablePropertyValueTable->NthItem(counter));
        counter++;
        double propertyValue = propValuePtr->Translation()[0];
        m_monitorValues[m_dstage].Set(pointSetIdx, pointIdx, propertyIdx, propertyValue);
      }
    }
  }
}

void RGInterface::RGInterfaceImpl::dumpMonitorableValuesInModel(std::unique_ptr<RescueModel> &pModel) const {
  assert(pModel.get());

  {
    RescueLookup *monitorablePropertyValueTable = pModel->LookupNamed(MonitorablePropertyValueTable.c_str());

    if (monitorablePropertyValueTable) {
      pModel->DropRescueLookup(monitorablePropertyValueTable);
    }
  }

  if (m_dstage < 0 || m_monitorValues.size() == 0 || m_monitorValues.find(m_dstage) == m_monitorValues.end())
    return;

  RGMonitorValues rgMonitorValue = m_monitorValues.at(m_dstage);
  RGMonitorPointSets pointSets = rgMonitorValue.GetPointSets();

  size_t numPointSets = pointSets.GetNumPointSets();
  size_t totalSizeOfValueTable = pointSets.GetTotalCount();

  RescueLookup *monitorablePropertyValueTable =
      new RescueLookup(MonitorablePropertyValueTable.c_str(), totalSizeOfValueTable, pModel.get());

  double value;
  int count = 0;

  for (int pointSetIdx = 0; pointSetIdx < numPointSets; pointSetIdx++) {
    // Properties will be same for all the points of this pointset
    size_t numPoints = pointSets.GetPointSet(pointSetIdx).GetNumPoints();
    size_t numProps = pointSets.GetProperties(pointSetIdx).size();

    for (int pointIdx = 0; pointIdx < numPoints; pointIdx++) {
      for (int propertyIdx = 0; propertyIdx < numProps; propertyIdx++) {
        RESCUEFLOAT *prop_Value = new RESCUEFLOAT[1];
        value = m_monitorValues.at(m_dstage).Get(pointSetIdx, pointIdx, propertyIdx);
        prop_Value[0] = value;
        monitorablePropertyValueTable->SetNthItem(count, new RescueLookupTable(pModel->Context(), 1, prop_Value));
        count++;
      }
    }
  }
}
