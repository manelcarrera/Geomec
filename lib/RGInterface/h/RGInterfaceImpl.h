// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGInterface.h"

#include <map>

class RescueGeometry;
class RescueModel;
class RescueTimeStepGroup;

class RGInterface::RGInterfaceImpl
{
public:
   RGInterfaceImpl(  const std::string & modelName, bool saveAsTxt );
   RGInterfaceImpl(  const std::string & modelName, const RGInterfaceImpl * toCp, bool saveAsTxt );
   
   /// @brief Actual model writing is doing here.
   void dumpModel() const;

   template<class T> void CheckSaveProperty(const RGProperty& property, const std::vector<T>& vals);

   void setCurrentDepletionStage( const RGDepletionStage & curStage );
   RGDepletionStage getCurrentDepletionStage() const;
   RGDepletionStage getNthDeplethionStage( size_t num ) const;
   size_t getDepletionStagesNumber() const;

   RGNodeId    addNode( const RGNode & nd );
   RGElementId addElement( const RGElement & element );
   RGPolygonId addPolygon( const RGPolygon & polygon );

   size_t    getNumNodes()    const;
   const RGNode&    getNodeN( const size_t & n ) const;

   size_t    getNumElements() const;
   const RGElement& getElementN( const size_t & n ) const;

   size_t    getNumPolygons() const;
   const RGPolygon& getPolygonN( const RGPolygonId & n ) const;


   void loadProperty( const RGProperty & property, std::vector<double> & vals ); 
   void loadProperty( const RGProperty & property, std::vector<int> & vals ); 

   void saveProperty( const RGProperty & property, const std::vector<double> & vals );
   void saveProperty( const RGProperty & property, const std::vector<int> & vals );
   
   size_t getPropertiesNumber() const;
   RGProperty getNthProperty( size_t p ) const;
   void deletePropertiesForDepletionStage( size_t stageNum );   

   /// @brief Add commands, last command must always be typeCommandCalculate or typeCommandHibernate.
   /// Commands can be added to new created model only, and can't to the existing model.
   /// @param command command to be added
   void addCommand( const GMCommand & command );
   
   /// @brief Get number of commands in model
   /// @return number of available commands
   size_t     getNumCommands() const;
   
   /// @brief Get command for given position
   /// @param n position nubmer
   /// @return command for given position or undefined command if this position does not exists
   GMCommand  getCommandN( const size_t & n ) const;
   
   /// @brief Drop all commands which are in list
   void clearCommandList();

   /// @brief Get number of error messages for current depletion stage. 
   ///        When new depleting stage has being defined, this error messages list is reseted.
   /// @return number of error messages
   size_t getNumErrorMsgs() const;
   
   /// @brief Get Nth error mesage
   /// @param n message position in list
   /// @return n-th error message
   std::string getNthErrorMsg( size_t n ) const;
   
   /// @brief Add new error message
   /// @param msg error message as string
   void addErrorMsg( const std::string & msg );

   bool isReadOnly() const;

   // Dealing with formations
   /// @brief Get the user name associated with the specified formation index
   /// @param formationID value in array propElementFormation
   /// @return user name
   std::string getFormationName( int formationID ) const;

   void setFormationName( int formationID, const std::string& formationName );

   int GetNumFormations();

   void GetFormationInfo(int formationIndex, std::string& formationName, int& formationId);

   // Surfaces
   size_t getNumHorizons() const;
   size_t getNumFaults()   const;

   const RGSurface& getHorizonN(     const size_t & n) const;
   const RGSurface& getFaultFrontN(  const size_t & n) const;
   const RGSurface& getFaultBackN(   const size_t & n) const;

   void addHorizon( const RGSurface& surface);
   void addFault( const RGSurface& faultF, const RGSurface& faultB);

   bool isEqual( const RGInterfaceImpl * tcm, double eps ) const;

   void commandsCompleted();
   bool isCommandsCompleted() const;

   static std::string versioningToString( const RGInterface::Versioning& );

   // the failure mechanism set and get methods
   const std::vector<RGFailureMechanism>& getAvailableFailureMechanismsInModel() const;
   void setAvailableFailureMechanismsInModel(const std::vector<RGFailureMechanism> &);

   // the LSF evaluation set and get methods
   const std::vector<RGLimitStateFunctionEvaluate>& getLSFEvaluationsInModel() const;
   void setLSFEvaluationsInModel(const std::vector<RGLimitStateFunctionEvaluate> &);

   // GEOMEC publishes list of properties that are available for monitoring,
   // This is triggered by the command typeCommandListMonitorableProperties.
   // Flow reads it
   std::vector<RGGeneralProperty> GetAvailableMonitorableProperties() const;
   void SetAvailableMonitorableProperties(const std::vector<RGGeneralProperty> &props);

   RGMonitorPointSets GetSelectedMonitoringPointSets() const;
   void SetSelectedMonitoringPointSets(const RGMonitorPointSets&);

   void SetMonitorValues(const RGMonitorValues&);
   RGMonitorValues GetMonitorValues()const;

protected:
  
   // Metadata and version
   void setMetaData( std::unique_ptr<RescueModel> & pModel ) const;
   void setRgiVersionInModel( std::unique_ptr<RescueModel> & pModel ) const;
   RGInterface::Versioning getRgiVersionInModel( std::unique_ptr<RescueModel> & pModel ) const;
   void checkRgiVersion( std::unique_ptr<RescueModel> & pModel );

   // Axillary methods
   size_t getNumFaces() const;

   void dumpGeometry( std::unique_ptr<RescueModel> & pModel ) const;
   void loadGeometry( std::unique_ptr<RescueModel> & pModel );

   bool dumpProperties( std::unique_ptr<RescueModel> & pModel ) const;
   void loadProperties( std::unique_ptr<RescueModel> & pModel );
   
   bool dumpCommands( std::unique_ptr<RescueModel> & pModel ) const;
   void loadCommands( std::unique_ptr<RescueModel> & pModel );

   bool dumpErrorMsgs( std::unique_ptr<RescueModel> & pModel ) const;
   void loadErrorMsgs( std::unique_ptr<RescueModel> & pModel );

   bool dumpFormations( std::unique_ptr<RescueModel> & pModel ) const;
   void loadFormations( std::unique_ptr<RescueModel> & pModel );

   bool dumpSurfaces( std::unique_ptr<RescueModel> & pModel ) const;
   void loadSurfaces( std::unique_ptr<RescueModel> & pModel );
   
   bool writeSurfaceContent( RescueGeometry*, size_t surfaceId, const RGSurface& surface ) const;
   void readSurfaceContent ( RescueGeometry*, size_t surfaceId, RGSurface& surface);

   bool dumpPolygons( std::unique_ptr<RescueModel> & pModel) const;
   void loadPolygons( std::unique_ptr<RescueModel> & pModel);

   // the failure mechanism set and get methods
   void loadAvailableFailureMechanismsInModel(std::unique_ptr<RescueModel> & pModel);
   void dumpAvailableFailureMechanismsInModel(std::unique_ptr<RescueModel> & pModel) const;

   // the LSF evaluation set and get methods
   void loadLSFEvaluationsInModel(std::unique_ptr<RescueModel> & pModel);
   void dumpLSFEvaluationsInModel(std::unique_ptr<RescueModel> & pModel) const;

   //Get Monitorable properties
   void loadAvailableMonitorableProperties(std::unique_ptr<RescueModel> & pModel);
   void dumpAvailableMonitorablePropertiesInModel(std::unique_ptr<RescueModel> & pModel) const;

   //Monitorable points
   void loadMonitorablePointSets(std::unique_ptr<RescueModel> & pModel);
   void dumpMonitorablePointSetsInModel(std::unique_ptr<RescueModel> & pModel) const;

   //Get Monitorable properties values
   void loadMonitorableValues(std::unique_ptr<RescueModel> & pModel);
   void dumpMonitorableValuesInModel(std::unique_ptr<RescueModel> & pModel) const;

   RescueTimeStepGroup * getTimeStepGroup( std::unique_ptr<RescueModel> & pModel, const RGDepletionStage & depStage, bool & fileChanged ) const;


   // Data Members
   std::string                                        m_modelName;    // model file name
   std::vector<RGNode>                                m_nodes;        // array of nodes, nodes position in array defines it ID
   std::vector<RGElement>                             m_elements;     // array of cell elements
   std::map<int,std::string>                          m_formationMap; // table for Formation ID - Formation Name

   // Surfaces
   std::vector<RGPolygon>                             m_polygons;     // array of polygons listed in the surfaces (horizons, faults, slices)
   std::vector<RGSurface> m_surfaces[ RGSurface::NumTypes ];           // map of array of all surfaces (horizons, faults front, faults back, slices
   size_t                                             m_nbPolygonCorner; // number max of corner for a polygon
   int                    m_dstage;          // current depletion stage

   //////////////////////////////////////
   // Data member for keeping errror messages for current depletion stage
   std::vector<std::string>  m_errMsgs;

   //////////////////////////////////////
   // Data members for keeping properties
   std::vector<RGDepletionStage>  m_timeSteps; // time steps (depletion stages) info

   // table for keeping properties for each available time step - several properties for several depletion stages
   std::vector< std::map< RGPropertyType::Type, std::vector<double> > > m_propTable; 
   std::vector< std::map< RGPropertyType::Type, std::vector<int> > >    m_propIntTable; 
   
   /////////////////////////////////////
   // Data members for keeping commands
   std::vector< GMCommand > m_cmdSet;

   ////////////////////////
   // Axillary data members
   bool        m_isReadOnly;              // if it was created by file loading it is read only. In this case only properties
                                          // could be changed
   bool        m_isTxtFormat;             // save Rescue files in text or binary format
   std::string m_rescueFileName;          // full path rescue file name. 

   ////////////////////////
   // The failure mechanism data members
   std::vector<RGFailureMechanism> m_failureMechanisms;
   std::vector<RGLimitStateFunctionEvaluate> m_limitStateFunctions;

   //////////////////////////
   // Below are data members for handling Monitorable Properties
   std::vector<RGGeneralProperty> m_monitorableProperties; 
   RGMonitorPointSets m_monitorPointSets;
   std::map<int, RGMonitorValues> m_monitorValues;
};

