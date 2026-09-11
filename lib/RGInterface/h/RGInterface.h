// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#ifndef RG_INTERFACE_H
#define RG_INTERFACE_H

#include "GMCommand.h"
#include "RGDepletionStage.h"
#include "RGElement.h"
#include "RGFailure.h"
#include "RGGeneralProperty.h"
#include "RGInterfaceExport.h"
#include "RGMonitorValues.h"
#include "RGNode.h"
#include "RGProperty.h"
#include "RGSurface.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// @brief Class handling data communication between RockMech and GEOMECH
/// Error handling via throwing exceptions
///////////////////////////////////////////////////////////////////////////////
class INTERFACE_RGINTERFACE RGInterface {
public:
  struct Versioning {
    char *name;
    int major; // incompatible changes in API
    int minor; // constant API, but non backwards-compatible change

    std::string toString() const;
  };
  static const Versioning VERSION;

  static std::string getVersionInfo();

  /// @brief RGInterface constructor. Open and load data file if it is exists or
  /// create new for saving if it doesn't exist.
  /// @param modelName name of the model. It is used as prefix for storage file
  /// @param saveAsTxt save model file in text format, usefull for debugging purpose, also add more log prints
  RGInterface(const std::string &modelName, bool saveAsTxt = false);

  /// @brief RGInterface copy constructor. (For testing purpose)
  /// @param modelName name of the model. It is used as prefix for storage file
  /// @param toCp object of RGInterface to be copied into current
  /// @param saveAsTxt save model file in text format, usefull for debugging purpose, also add more log prints
  RGInterface(const std::string &modelName, const RGInterface &toCp, bool saveAsTxt = false);

  /// @brief Destructor
  ~RGInterface();

  /// @brief Actual model writing is doing here.
  /// Can throw exceptions if any problems with dumping model.
  void dumpModel() const;

  // Get/Set current depletion stage. This method must be called by RockMech only
  /// @brief Set current depletion stage.
  /// @param curStage reference to current depletion stage
  void setCurrentDepletionStage(const RGDepletionStage &curStage);

  /// @brief Get current depletion stage
  /// @return current depletion stage
  RGDepletionStage getCurrentDepletionStage() const;

  /// @brief Get number of depletion stages in model. Model keeps one set of geometry data and
  /// set of properties for each depletion stage.
  /// @return number of available depletion stage
  size_t getDepletionStagesNumber() const;

  /// @brief Get n-th depletion stage, numbering starts from zero. If num greater than
  /// available number of depletion stages invalid depletion stage is returned. This method must be called by RockMech
  /// only
  /// @param num
  /// @return n-th depletion stage info
  RGDepletionStage getNthDeplethionStage(size_t num) const;

  /// @brief Add new node, there is no any checking for coincident nodes.
  /// @param nd reference to node object
  /// @return node ID
  RGNodeId addNode(const RGNode &nd);

  /// @brief Add new element as set of nodes and element type
  /// @param el reference to RGElement object
  /// @return element ID
  RGElementId addElement(const RGElement &el);

  /// @brief Add new polygon as set of nodes
  /// @param polygon reference to RGPolygon object
  /// @return added polygon ID
  RGPolygonId addPolygon(const RGPolygon &polygon);

  /// @brief Get total nodes number
  /// @return total nodes number in grid
  size_t getNumNodes() const;

  /// @brief Get n-th node
  /// @parm n node number
  /// @return node for given position
  RGNode getNodeN(const RGNodeId &n) const;

  /// @brief Get total elements number
  /// @return total elements number in grid
  size_t getNumElements() const;

  /// @brief Get n-th element
  /// @param n element number
  /// @return element for given position
  RGElement getElementN(const size_t &n) const;

  /// @brief Get total polygons number
  /// @return total polygons number in grid
  size_t getNumPolygons() const;

  /// @brief Get n-th polygon
  /// @parm n polygon id
  /// @return polygon for given position
  const RGPolygon &getPolygonN(const RGPolygonId &n) const;

  // Surfaces
  size_t getNumHorizons() const;
  size_t getNumFaults() const;

  const RGSurface &getHorizonN(const size_t) const;
  const RGSurface &getFaultFrontN(const size_t) const;
  const RGSurface &getFaultBackN(const size_t) const;

  void addHorizon(const RGSurface &);
  void addFault(const RGSurface &, const RGSurface &); // front and back

  /// the failure mechanism set and get methods
  const std::vector<RGFailureMechanism> &getAvailableFailureMechanismsInModel() const;
  void setAvailableFailureMechanismsInModel(const std::vector<RGFailureMechanism> &);

  ///  the LSF evaluation set and get methods
  const std::vector<RGLimitStateFunctionEvaluate> &getLSFEvaluationsInModel() const;
  void setLSFEvaluationsInModel(const std::vector<RGLimitStateFunctionEvaluate> &);

  // GEOMEC publishes list of properties that are available for monitoring,
  // This is triggered by the command typeCommandListMonitorableProperties.
  // Flow reads it
  std::vector<RGGeneralProperty> GetAvailableMonitorableProperties() const;
  void SetAvailableMonitorableProperties(const std::vector<RGGeneralProperty> &props);

  // Flow publishes list of point set and properties that the user has selected.
  // GEOMEC reads them, this is triggered by the command typeCommandLoadMonitorPointSets
  RGMonitorPointSets GetSelectedMonitoringPointSets() const;
  void SetSelectedMonitoringPointSets(const RGMonitorPointSets &);

  // GEOMEC publishes the interpolated properties at a given depletion stage,
  // This is triggered by the command typeCommandEvaluateMonitorValues.
  // Flow reads them
  void SetMonitorValues(const RGMonitorValues &monitorValues);
  RGMonitorValues GetMonitorValues() const;

  /// @brief Load property. Property values for irrelevant elements are defined as null (see RGUtils::isNull)
  /// type are defined as UNDEF_PROP_VALUE
  /// @param prop property description
  /// @param vals array for property values which will be resized to fit properties values for all elements
  void loadProperty(const RGProperty &property, std::vector<double> &vals); // typeCommandLoadProperty,
  void loadProperty(const RGProperty &property, std::vector<int> &vals);    // typeCommandLoadProperty,

  /// @brief Save property. Property values for irrelevant elements must be defined as null (see RGUtils::isNull)
  /// @param prop property description
  /// @param vals array of property values with size of elements number
  void saveProperty(const RGProperty &prop, const std::vector<double> &vals); // typeCommandSaveProperty,
  void saveProperty(const RGProperty &prop, const std::vector<int> &vals);    // typeCommandSaveProperty,

  /// @brief Get properties number for the current depletion stage
  /// @return properties number
  size_t getPropertiesNumber();

  /// @brief Get n-th property for the current depletion stage
  /// @param p property number
  /// @return n-th property
  RGProperty getNthProperty(size_t p);

  /// @brief Delete all properties for given depletion stage number
  /// @param stageNum depletion stage number
  void deletePropertiesForDepletionStage(size_t stageNum);

  void deletePropertyForDepletionStage(size_t stageNum, const RGPropertyType::Type &propertType);

  /// @brief Add commands, last command must always be typeCommandCalculate or typeCommandHibernate.
  /// Commands can be added to new created model only, and can't to the existing model.
  /// @param command command to be added
  void addCommand(const GMCommand &command);

  // Load commands
  /// @brief Get number of commands in model
  /// @return number of available commands
  size_t getNumCommands() const;
  /// @brief Get command for given position
  /// @param n position nubmer
  /// @return command for given position or undefined command if this position does not exists
  GMCommand getCommandN(const size_t &n) const;
  /// @brief Drop all commands which are in list
  void clearCommandList();

  // Error messages handling
  /// @brief Get number of error messages for current depletion stage.
  ///        When new depleting stage has being defined, this error messages list is reseted.
  /// @return number of error messages
  size_t getNumErrorMsgs() const;
  /// @brief Get Nth error mesage
  /// @param n message position in list
  /// @return n-th error message
  std::string getNthErrorMsg(size_t n) const;
  /// @brief Add new error message
  /// @param msg error message as string
  void addErrorMsg(const std::string &msg);

  // Formations are defined as an integer array on the elements.
  // GEOMEC derives the formation boundaries from this list.
  // Notice that the formation index for non-volume elements is irrelevant.
  // In addition a relation between the formation index and the name under which the user knows the formation could will
  // be provided

  /// @brief Get the user name associated with the specified formation index
  /// @param formationIndex value in array propElementFormation
  /// @return user name
  std::string getFormationName(int formationIndex) const;

  /// @brief Get the number of formations
  int GetNumFormations();

  /// @brief Get the formation information given the formation index
  /// @param formationIndex the formation index
  /// @param formationName the formation name
  /// @param formationId the int key-value from the formation map
  void GetFormationInfo(int formationIndex, std::string &formationName, int &formationId);

  /// @brief Set the user name associated with the specified formation index
  /// @param formationIndex value in array propElementFormation
  /// @param formationName name associated with ID
  void setFormationName(int formationIndex, const std::string &formationName);

  /// @brief Compare two RGInterface objects fo full equality. (For testing purpose)
  /// @param tcm object to compare with current
  /// @param eps precision for float number comparison
  /// @return true if all values in tcm object are equal to values in current object
  bool isEqual(const RGInterface &tcm, double eps = 1e-4) const;

  /// @brief Complete the interface
  void commandsCompleted();

  /// @brief Complete the interface
  bool isCommandsCompleted() const;

  class Exception : public std::runtime_error {
  public:
    explicit Exception(const std::string &);
    Exception(const std::string &, const GMCommandType &);
    ~Exception() throw();

    GMCommandType m_commandType;
  };

  typedef Exception VersionError;

private:
  // Protection for copy constructor and copy operator
  RGInterface(const RGInterface &);
  RGInterface &operator=(const RGInterface &);

  // Implementation is hidden
  class RGInterfaceImpl;
  RGInterfaceImpl *m_pimpl;
};

#endif
